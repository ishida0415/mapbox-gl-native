#include "http_request.hpp"
#include "http_file_source.hpp"

#include <mbgl/storage/response.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/http_header.hpp>
#include <mbgl/util/string.hpp>

#include <QByteArray>
#include <QNetworkReply>
#include <QPair>

namespace mbgl {

HTTPRequest::HTTPRequest(HTTPFileSource::Impl* context, const Resource& resource, FileSource::Callback callback)
    : m_context(context)
    , m_resource(resource)
    , m_callback(callback)
{
    m_context->request(this);
}

HTTPRequest::~HTTPRequest()
{
    if (!m_handled) {
        m_context->cancel(this);
    }
}

QUrl HTTPRequest::requestUrl() const
{
    return QUrl::fromPercentEncoding(QByteArray(m_resource.url.data(), m_resource.url.size()));
}

QNetworkRequest HTTPRequest::networkRequest() const
{
    QNetworkRequest req = QNetworkRequest(requestUrl());
    req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    req.setRawHeader("User-Agent", "MapboxGL/1.0 [Qt]");

    if (m_resource.priorEtag) {
        const auto etag = m_resource.priorEtag;
        req.setRawHeader("If-None-Match", QByteArray(etag->data(), etag->size()));
    } else if (m_resource.priorModified) {
        req.setRawHeader("If-Modified-Since", util::rfc1123(*m_resource.priorModified).c_str());
    }

    return req;
}

void HTTPRequest::handleNetworkReply(QNetworkReply *reply)
{
    m_handled = true;

    // Calling `callback` may result in deleting `this`.
    // Copy data to temporaries first.
    auto callback = m_callback;
    mbgl::Response response;

    using Error = Response::Error;

    // Handle non-HTTP errors (i.e. like connection).
    if (reply->error() && reply->error() < 100) {
        response.error = std::make_unique<Error>(
            Error::Reason::Connection, reply->errorString().toStdString());
        callback(response);
        return;
    }

    QPair<QByteArray, QByteArray> line;
    foreach(line, reply->rawHeaderPairs()) {
        QString header = QString(line.first).toLower();

        if (header == "last-modified") {
            response.modified = util::parseTimestamp(line.second.constData());
        } else if (header == "etag") {
            response.etag = std::string(line.second.constData(), line.second.size());
        } else if (header == "cache-control") {
            response.expires = http::CacheControl::parse(line.second.constData()).toTimePoint();
        } else if (header == "expires") {
            response.expires = util::parseTimestamp(line.second.constData());
        }
    }

    int responseCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    switch(responseCode) {
    case 200: {
        QByteArray bytes = reply->readAll();
        if (bytes.isEmpty()) {
            response.data = std::make_shared<std::string>();
        } else {
            response.data = std::make_shared<std::string>(bytes.data(), bytes.size());
        }
        break;
    }
    case 204:
        response.noContent = true;
        break;
    case 304:
        response.notModified = true;
        break;
    case 404: {
        if (m_resource.kind == Resource::Kind::Tile) {
            response.noContent = true;
        } else {
            response.error = std::make_unique<Error>(
                Error::Reason::NotFound, "HTTP status code 404");
        }
        break;
    }
    default:
        Response::Error::Reason reason = (responseCode >= 500 && responseCode < 600) ?
            Error::Reason::Server : Error::Reason::Other;

        response.error = std::make_unique<Error>(
            reason, "HTTP status code " + util::toString(responseCode));
    }

    callback(response);
}

} // namespace mbgl
