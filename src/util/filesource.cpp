#include <mbgl/util/filesource.hpp>
#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/log.hpp>
#include <mbgl/util/compression.hpp>

#include <sqlite3.h>

#include <fstream>
#include <sstream>

namespace mbgl {

FileSource::FileSource() {
    const int err = sqlite3_open_v2("cache.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
    if (err != SQLITE_OK) {
        Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        db = nullptr;
    }

    createSchema();
}

FileSource::~FileSource() {
    closeDatabase();
}

void FileSource::createSchema() {
    if (db) {
        // Create schema
        const int err = sqlite3_exec(db,
            "CREATE TABLE IF NOT EXISTS `http_cache` ("
                "`url` TEXT PRIMARY KEY NOT NULL,"
                "`code` INTEGER NOT NULL,"
                "`type` INTEGER NOT NULL,"
                "`modified` INTEGER,"
                "`expires` INTEGER,"
                "`data` BLOB,"
                "`compressed` INTEGER NOT NULL DEFAULT 0"
            ");"
            "CREATE INDEX IF NOT EXISTS `http_cache_type_idx` ON `http_cache` (`type`);", nullptr, nullptr, nullptr);
        if (err != SQLITE_OK) {
            Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
            closeDatabase();
        }
    }
}

void FileSource::closeDatabase() {
    if (db) {
        const int err = sqlite3_close_v2(db);
        Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        db = nullptr;
    }
}

void FileSource::setBase(const std::string &value) {
    base = value;
}

const std::string &FileSource::getBase() const {
    return base;
}

std::string removeAccessTokenFromURL(const std::string &url) {
    const size_t token_start = url.find("access_token=");
    // Ensure that token exists, isn't at the front and is preceded by either & or ?.
    if (token_start == std::string::npos || token_start == 0 || !(url[token_start - 1] == '&' || url[token_start - 1] == '?')) {
        return url;
    }

    const size_t token_end = url.find_first_of('&', token_start);
    if (token_end == std::string::npos) {
        // The token is the last query argument. We slice away the "&access_token=..." part
        return url.substr(0, token_start - 1);
    } else {
        // We slice away the "access_token=...&" part.
        return url.substr(0, token_start) + url.substr(token_end + 1);
    }
}

bool FileSource::loadFile(ResourceType /*type*/, const std::string &url, std::function<void(platform::Response *)> callback) {
    if (!db) {
        return false;
    }

    sqlite3_stmt *stmt = nullptr;
    int err;

    err = sqlite3_prepare_v2(db, "SELECT `code`, `data`, `compressed` FROM `http_cache` WHERE `url` = ?", -1, &stmt, nullptr);
    if (err != SQLITE_OK) {
        Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        return false;
    }

    err = sqlite3_bind_text(stmt, 1, url.data(), url.size(), nullptr);
    if (err != SQLITE_OK) {
        Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        err = sqlite3_finalize(stmt);
        if (err != SQLITE_OK) {
            Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        }
        return false;
    }

    bool status = false;

    err = sqlite3_step(stmt);
    if (err == SQLITE_ROW) {
        // There is data.
        platform::Response res(callback);
        res.code = sqlite3_column_int(stmt, 0);
        const char *data = reinterpret_cast<const char *>(sqlite3_column_blob(stmt, 1));
        const size_t size = sqlite3_column_bytes(stmt, 1);
        const bool compressed = sqlite3_column_int(stmt, 2);

        if (compressed) {
            res.body = util::decompress({ data, size });
        } else {
            res.body = { data, size };
        }

        callback(&res);
        status = true;
    } else if (err == SQLITE_DONE) {
        // There is no data.
        // This is a noop.
    } else {
        // Another error occured.
        Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
    }

    err = sqlite3_finalize(stmt);
    if (err != SQLITE_OK) {
        Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
    }
    return status;
}

void FileSource::saveFile(ResourceType type, const std::string &url, platform::Response *const res) {
    if (!db) {
        return;
    }

    sqlite3_stmt *stmt = nullptr;
    int err;

    err = sqlite3_prepare_v2(db, "REPLACE INTO `http_cache` (`url`, `code`, `type`, `modified`, `expires`, `data`, `compressed`) VALUES(?, ?, ?, ?, ?, ?, ?)", -1, &stmt, nullptr);
    if (err != SQLITE_OK) {
        Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        return;
    }

    err = sqlite3_bind_text(stmt, 1, url.data(), url.size(), nullptr);
    if (err != SQLITE_OK) {
        Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        err = sqlite3_finalize(stmt);
        if (err != SQLITE_OK) {
            Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        }
        return;
    }

    err = sqlite3_bind_int(stmt, 2, res->code);
    if (err != SQLITE_OK) {
        Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        err = sqlite3_finalize(stmt);
        if (err != SQLITE_OK) {
            Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        }
        return;
    }

    err = sqlite3_bind_int(stmt, 3, int(type));
    if (err != SQLITE_OK) {
        Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        err = sqlite3_finalize(stmt);
        if (err != SQLITE_OK) {
            Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        }
        return;
    }

    err = sqlite3_bind_int(stmt, 4, res->modified);
    if (err != SQLITE_OK) {
        Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        err = sqlite3_finalize(stmt);
        if (err != SQLITE_OK) {
            Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        }
        return;
    }

    err = sqlite3_bind_int(stmt, 5, res->expires);
    if (err != SQLITE_OK) {
        Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        err = sqlite3_finalize(stmt);
        if (err != SQLITE_OK) {
            Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        }
        return;
    }

    bool compressed = false;
    switch (type) {
        case ResourceType::Image:
            err = sqlite3_bind_blob(stmt, 6, res->body.data(), res->body.size(), SQLITE_STATIC);
            break;
        default:
            const std::string *data = new std::string(std::move(util::compress(res->body)));
            compressed = true;
            err = sqlite3_bind_blob(stmt, 6, data->data(), data->size(), [](void *p) { delete (std::string *)p; });
            break;
    }
    if (err != SQLITE_OK) {
        Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        err = sqlite3_finalize(stmt);
        if (err != SQLITE_OK) {
            Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        }
        return;
    }

    err = sqlite3_bind_int(stmt, 7, compressed);
    if (err != SQLITE_OK) {
        Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        err = sqlite3_finalize(stmt);
        if (err != SQLITE_OK) {
            Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
        }
        return;
    }

    err = sqlite3_step(stmt);
    if (err != SQLITE_DONE) {
        // Another error occured.
        Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
    }

    err = sqlite3_finalize(stmt);
    if (err != SQLITE_OK) {
        Log::Warning(Event::Database, "%s: %s", sqlite3_errstr(err), sqlite3_errmsg(db));
    }
}


void FileSource::load(ResourceType type, const std::string &url, std::function<void(platform::Response *)> callback, const std::shared_ptr<uv::loop> loop) {
    // convert relative URLs to absolute URLs

    const std::string absoluteURL = [&]() -> std::string {
        const size_t separator = url.find("://");
        if (separator == std::string::npos) {
            // Relative URL.
            return base + url;
        } else {
            return url;
        }
    }();

    const size_t separator = absoluteURL.find("://");
    const std::string protocol = separator != std::string::npos ? absoluteURL.substr(0, separator) : "";

    if (protocol == "file") {
        // load from disk
        const std::string path = absoluteURL.substr(separator + 3);
        std::ifstream file(path);

        platform::Response response(callback);
        if (!file.good()) {
            response.error_message = "file not found (" + path + ")";
        } else {
            std::stringstream data;
            data << file.rdbuf();
            response.code = 200;
            response.body = data.str();
        }

        callback(&response);
    } else {
        // Don't use the access token when storing/retrieving URLs from the database
        const std::string cleanURL = removeAccessTokenFromURL(url);

        // load from the internet
        if (!loadFile(type, cleanURL, callback)) {
            const std::shared_ptr<FileSource> source = shared_from_this();
            platform::request_http(absoluteURL, [=](platform::Response *res) {
                source->saveFile(type, cleanURL, res);
                callback(res);
            }, loop);
        }
    }
}

}