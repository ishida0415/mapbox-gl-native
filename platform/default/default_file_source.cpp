#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/storage/asset_file_source.hpp>
#include <mbgl/storage/online_file_source.hpp>
#include <mbgl/storage/offline_database.hpp>
#include <mbgl/storage/offline_download.hpp>

#include <mbgl/platform/platform.hpp>
#include <mbgl/util/url.hpp>
#include <mbgl/util/thread.hpp>
#include <mbgl/util/work_request.hpp>

#include <cassert>

namespace {

const std::string assetProtocol = "asset://";

bool isAssetURL(const std::string& url) {
    return std::equal(assetProtocol.begin(), assetProtocol.end(), url.begin());
}

} // namespace

namespace mbgl {

class DefaultFileSource::Impl {
public:
    class Task {
    public:
        Task(Resource resource, FileSource::Callback callback, DefaultFileSource::Impl* impl) {
            auto offlineResponse = impl->offlineDatabase.get(resource);

            Resource revalidation = resource;

            if (offlineResponse) {
                revalidation.priorModified = offlineResponse->modified;
                revalidation.priorExpires = offlineResponse->expires;
                revalidation.priorEtag = offlineResponse->etag;
                callback(*offlineResponse);
            }

            if (!impl->offline) {
                onlineRequest = impl->onlineFileSource.request(revalidation, [=] (Response onlineResponse) {
                    impl->offlineDatabase.put(revalidation, onlineResponse);
                    callback(onlineResponse);
                });
            }
        }

        std::unique_ptr<FileRequest> onlineRequest;
    };

    Impl(const std::string& cachePath)
        : offlineDatabase(cachePath) {
    }

    void setAccessToken(const std::string& accessToken) {
        onlineFileSource.setAccessToken(accessToken);
    }

    std::string getAccessToken() const {
        return onlineFileSource.getAccessToken();
    }

    void listRegions(std::function<void (std::exception_ptr, optional<std::vector<OfflineRegion>>)> callback) {
        try {
            callback({}, offlineDatabase.listRegions());
        } catch (...) {
            callback(std::current_exception(), {});
        }
    }

    void createRegion(const OfflineRegionDefinition& definition,
                      const OfflineRegionMetadata& metadata,
                      std::function<void (std::exception_ptr, optional<OfflineRegion>)> callback) {
        try {
            callback({}, offlineDatabase.createRegion(definition, metadata));
        } catch (...) {
            callback(std::current_exception(), {});
        }
    }

    void getRegionStatus(int64_t regionID, std::function<void (std::exception_ptr, optional<OfflineRegionStatus>)> callback) {
        try {
            callback({}, getDownload(regionID).getStatus());
        } catch (...) {
            callback(std::current_exception(), {});
        }
    }

    void deleteRegion(OfflineRegion&& region, std::function<void (std::exception_ptr)> callback) {
        try {
            offlineDatabase.deleteRegion(std::move(region));
            callback({});
        } catch (...) {
            callback(std::current_exception());
        }
    }

    void removeUnusedOfflineResources(std::function<void (std::exception_ptr)> callback) {
        try {
            offlineDatabase.removeUnusedResources();
            callback({});
        } catch (...) {
            callback(std::current_exception());
        }
    }

    void setRegionObserver(int64_t regionID, std::unique_ptr<OfflineRegionObserver> observer) {
        getDownload(regionID).setObserver(std::move(observer));
    }

    void setRegionDownloadState(int64_t regionID, OfflineRegionDownloadState state) {
        getDownload(regionID).setState(state);
    }

    void add(FileRequest* req, Resource resource, Callback callback) {
        tasks[req] = std::make_unique<Task>(resource, callback, this);
    }

    void cancel(FileRequest* req) {
        tasks.erase(req);
    }

    void put(const Resource& resource, const Response& response) {
        offlineDatabase.put(resource, response);
    }

    void goOffline() {
        offline = true;
    }

private:
    OfflineDownload& getDownload(int64_t regionID) {
        auto it = downloads.find(regionID);
        if (it != downloads.end()) {
            return *it->second;
        }
        return *downloads.emplace(regionID,
            std::make_unique<OfflineDownload>(regionID, offlineDatabase.getRegionDefinition(regionID), offlineDatabase, onlineFileSource)).first->second;
    }

    OfflineDatabase offlineDatabase;
    OnlineFileSource onlineFileSource;
    std::unordered_map<FileRequest*, std::unique_ptr<Task>> tasks;
    std::unordered_map<int64_t, std::unique_ptr<OfflineDownload>> downloads;
    bool offline = false;
};

class DefaultFileRequest : public FileRequest {
public:
    DefaultFileRequest(Resource resource, FileSource::Callback callback, util::Thread<DefaultFileSource::Impl>& thread_)
        : thread(thread_),
          workRequest(thread.invokeWithCallback(&DefaultFileSource::Impl::add, callback, this, resource)) {
    }

    ~DefaultFileRequest() {
        thread.invoke(&DefaultFileSource::Impl::cancel, this);
    }

    util::Thread<DefaultFileSource::Impl>& thread;
    std::unique_ptr<WorkRequest> workRequest;
};

DefaultFileSource::DefaultFileSource(const std::string& cachePath, const std::string& assetRoot)
    : thread(std::make_unique<util::Thread<DefaultFileSource::Impl>>(util::ThreadContext{"DefaultFileSource", util::ThreadType::Unknown, util::ThreadPriority::Low}, cachePath)),
      assetFileSource(std::make_unique<AssetFileSource>(assetRoot)) {
}

DefaultFileSource::~DefaultFileSource() = default;

void DefaultFileSource::setAccessToken(const std::string& accessToken) {
    thread->invokeSync(&Impl::setAccessToken, accessToken);
}

std::string DefaultFileSource::getAccessToken() const {
    return thread->invokeSync<std::string>(&Impl::getAccessToken);
}

void DefaultFileSource::setMaximumCacheSize(uint64_t) {
    // TODO
}

void DefaultFileSource::setMaximumCacheEntrySize(uint64_t) {
    // TODO
}

std::unique_ptr<FileRequest> DefaultFileSource::request(const Resource& resource, Callback callback) {
    if (isAssetURL(resource.url)) {
        return assetFileSource->request(resource, callback);
    } else {
        return std::make_unique<DefaultFileRequest>(resource, callback, *thread);
    }
}

void DefaultFileSource::listOfflineRegions(std::function<void (std::exception_ptr, optional<std::vector<OfflineRegion>>)> callback) {
    thread->invoke(&Impl::listRegions, callback);
}

void DefaultFileSource::createOfflineRegion(const OfflineRegionDefinition& definition,
                                            const OfflineRegionMetadata& metadata,
                                            std::function<void (std::exception_ptr, optional<OfflineRegion>)> callback) {
    thread->invoke(&Impl::createRegion, definition, metadata, callback);
}

void DefaultFileSource::deleteOfflineRegion(OfflineRegion&& region, std::function<void (std::exception_ptr)> callback) {
    thread->invoke(&Impl::deleteRegion, std::move(region), callback);
}

void DefaultFileSource::setOfflineRegionObserver(OfflineRegion& region, std::unique_ptr<OfflineRegionObserver> observer) {
    thread->invoke(&Impl::setRegionObserver, region.getID(), std::move(observer));
}

void DefaultFileSource::setOfflineRegionDownloadState(OfflineRegion& region, OfflineRegionDownloadState state) {
    thread->invoke(&Impl::setRegionDownloadState, region.getID(), state);
}

void DefaultFileSource::getOfflineRegionStatus(OfflineRegion& region, std::function<void (std::exception_ptr, optional<OfflineRegionStatus>)> callback) const {
    thread->invoke(&Impl::getRegionStatus, region.getID(), callback);
}

void DefaultFileSource::removeUnusedOfflineResources(std::function<void (std::exception_ptr)> callback) {
    thread->invoke(&Impl::removeUnusedOfflineResources, callback);
}

// For testing only:

void DefaultFileSource::put(const Resource& resource, const Response& response) {
    thread->invokeSync(&Impl::put, resource, response);
}

void DefaultFileSource::goOffline() {
    thread->invokeSync(&Impl::goOffline);
}

} // namespace mbgl
