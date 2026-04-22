#include "apiclient.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QUrl>
#include <QDebug>

APIClient::APIClient(QObject *parent)
    : QObject(parent)
    , m_nam(new QNetworkAccessManager(this))
    , m_baseUrl("http://localhost:5000/api/inferences")
    ,m_isloading(false)
{
    connect(m_nam, &QNetworkAccessManager::finished, this, &APIClient::onReplyFinished);
}

APIClient::~APIClient()
{
}

void APIClient::setBaseUrl(const QString &url)
{
    if (m_baseUrl != url) {
        m_baseUrl = url;
        emit baseUrlChanged();
    }
}

void APIClient::setAuthToken(const QString &token)
{
    if (m_authToken != token) {
        m_authToken = token;
        emit authTokenChanged();
    }
}

void APIClient::createInference(const QString &location, const QString &diseaseName,
                                double confidence, const QString &variety,
                                std::function<void(bool, const QJsonObject&)> callback)
{
    QJsonObject data;
    data["location"] = location;
    data["diseasname"] = diseaseName;
    data["confidence"] = confidence;
    data["variaty"] = variety;

    sendRequest("POST", "", data, callback);
}

void APIClient::listInferences(std::function<void(bool, const QJsonArray&)> callback)
{
    sendRequest("GET", "", QJsonObject(), nullptr, callback);
}

void APIClient::getInference(const QString &inferenceId,
                             std::function<void(bool, const QJsonObject&)> callback)
{
    sendRequest("GET", "/" + inferenceId, QJsonObject(), callback);
}

void APIClient::sendRequest(const QString &method, const QString &endpoint,
                            const QJsonObject &data,
                            std::function<void(bool, const QJsonObject&)> objectCallback,
                            std::function<void(bool, const QJsonArray&)> arrayCallback)
{
    QUrl url(m_baseUrl + endpoint);
    QNetworkRequest request(url);
    setupRequestHeaders(request);

    QNetworkReply *reply = nullptr;

    if (method == "GET") {
        reply = m_nam->get(request);
    } else if (method == "POST") {
        QJsonDocument doc(data);
        QByteArray jsonData = doc.toJson();
        reply = m_nam->post(request, jsonData);
    } else if (method == "PUT") {
        QJsonDocument doc(data);
        QByteArray jsonData = doc.toJson();
        reply = m_nam->put(request, jsonData);
    } else if (method == "DELETE") {
        reply = m_nam->deleteResource(request);
    }

    if (reply) {
        PendingRequest pending;
        pending.objectCallback = objectCallback;
        pending.arrayCallback = arrayCallback;
        m_pendingRequests[reply] = pending;
    }
}

void APIClient::setupRequestHeaders(QNetworkRequest &request)
{
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (!m_authToken.isEmpty()) {
        request.setRawHeader("Authorization", QString("Bearer %1").arg(m_authToken).toUtf8());
    }
}

void APIClient::onReplyFinished(QNetworkReply *reply)
{
    if (!m_pendingRequests.contains(reply)) {
        reply->deleteLater();
        return;
    }

    PendingRequest pending = m_pendingRequests.take(reply);
    bool success = (reply->error() == QNetworkReply::NoError);

    if (!success) {
        emit networkError(reply->errorString());

        // Handle authentication errors
        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 401) {
            emit authenticationRequired();
        }

        if (pending.objectCallback) {
            pending.objectCallback(false, QJsonObject());
        } else if (pending.arrayCallback) {
            pending.arrayCallback(false, QJsonArray());
        }
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit networkError("JSON Parse Error: " + parseError.errorString());
        if (pending.objectCallback) {
            pending.objectCallback(false, QJsonObject());
        } else if (pending.arrayCallback) {
            pending.arrayCallback(false, QJsonArray());
        }
        reply->deleteLater();
        return;
    }

    if (pending.objectCallback) {
        pending.objectCallback(true, doc.object());
    } else if (pending.arrayCallback) {
        pending.arrayCallback(true, doc.array());
    }

    reply->deleteLater();
}

bool APIClient::isloading() const
{
    return m_isloading;
}

void APIClient::setIsloading(bool newIsloading)
{
    if (m_isloading == newIsloading)
        return;
    m_isloading = newIsloading;
    emit isloadingChanged();
}
// ===== QML WRAPPERS =====

void APIClient::createInferenceQml(const QString &location,
                                   const QString &diseaseName,
                                   double confidence,
                                   const QString &variety)
{
    setIsloading(true);
    createInference(location, diseaseName, confidence, variety,
                    [this](bool success, const QJsonObject &response) {
                        emit createInferenceFinished(success, response);
                        setIsloading(false);
                    }
                    );
}

void APIClient::listInferencesQml()
{
    setIsloading(true);
    listInferences(
        [this](bool success, const QJsonArray &response) {
            emit listInferencesFinished(success, response);
            setIsloading(true);
        }
        );
}

void APIClient::getInferenceQml(const QString &inferenceId)
{
    setIsloading(true);
    getInference(inferenceId,
                 [this](bool success, const QJsonObject &response) {
                     emit getInferenceFinished(success, response);
                     setIsloading(true);
                 }
                 );
}