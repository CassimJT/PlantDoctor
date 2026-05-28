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
    , m_baseUrl("https://plantdoctor-api.onrender.com/api")
    , m_isloading(false)
{
    connect(m_nam, &QNetworkAccessManager::finished, this, &APIClient::onReplyFinished);
    m_authToken = AppSettings::instance().authToken();
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

void APIClient::createBatchInferences(const QJsonArray &inferencesArray,
                                      std::function<void(bool, const QJsonObject&)> callback)
{
    QJsonObject data;
    data["inferences"] = inferencesArray;
    data["batchSize"] = inferencesArray.size();

    sendRequest("POST", "/inference/batch", data, callback);
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
            setIsloading(false);
        }
        );
}

void APIClient::getInferenceQml(const QString &inferenceId)
{
    setIsloading(true);
    getInference(inferenceId,
                 [this](bool success, const QJsonObject &response) {
                     emit getInferenceFinished(success, response);
                     setIsloading(false);
                 }
                 );
}

// ===== BATCH SYNC METHODS =====

// Helper to convert QVariantList to QJsonArray
QJsonArray APIClient::variantListToJsonArray(const QVariantList &list)
{
    QJsonArray jsonArray;

    for (const QVariant &item : list) {
        if (item.canConvert<QVariantMap>()) {
            QVariantMap map = item.toMap();
            QJsonObject obj;

            // Extract the 4 fields
            if (map.contains("diseaseName"))
                obj["diseasname"] = map["diseaseName"].toString();
            if (map.contains("confidence"))
                obj["confidence"] = map["confidence"].toDouble();
            if (map.contains("location"))
                obj["location"] = map["location"].toString();
            if (map.contains("variaty"))
                obj["variaty"] = map["variaty"].toString();

            // Alternative field names for flexibility
            if (map.contains("disease_name"))
                obj["diseasname"] = map["disease_name"].toString();
            if (map.contains("variety"))
                obj["variaty"] = map["variety"].toString();

            jsonArray.append(obj);
        } else if (item.canConvert<QJsonObject>()) {
            // Direct QJsonObject
            jsonArray.append(item.toJsonObject());
        }
    }

    return jsonArray;
}

// QML Batch Sync Method - accepts QVariantList
void APIClient::createBatchInferencesQml(const QVariantList &inferencesList)
{
    if (inferencesList.isEmpty()) {
        emit networkError("Cannot sync empty batch");
        emit batchCreateFinished(false, 0, 0, QJsonObject());
        return;
    }

    setIsloading(true);

    QJsonArray jsonArray = variantListToJsonArray(inferencesList);

    emit batchProgress(0, jsonArray.size(), "Starting batch sync...");

    createBatchInferences(jsonArray,
                          [this, totalCount = jsonArray.size()](bool success, const QJsonObject &response) {
                              int successCount = 0;
                              if (success && response.contains("successCount")) {
                                  successCount = response["successCount"].toInt();
                              } else if (success) {
                                  // If server doesn't return successCount, assume all succeeded
                                  successCount = totalCount;
                              }

                              emit batchCreateFinished(success, totalCount, successCount, response);
                              emit batchProgress(totalCount, totalCount, "Batch sync completed");
                              setIsloading(false);
                          }
                          );
}

// QML Batch Sync Method - accepts JSON string directly
void APIClient::createBatchInferencesFromJson(const QString &jsonArrayStr)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonArrayStr.toUtf8());
    if (!doc.isArray()) {
        emit networkError("Invalid JSON array string");
        emit batchCreateFinished(false, 0, 0, QJsonObject());
        return;
    }

    QJsonArray jsonArray = doc.array();

    if (jsonArray.isEmpty()) {
        emit networkError("Cannot sync empty batch");
        emit batchCreateFinished(false, 0, 0, QJsonObject());
        return;
    }

    setIsloading(true);

    emit batchProgress(0, jsonArray.size(), "Starting batch sync from JSON...");

    createBatchInferences(jsonArray,
                          [this, totalCount = jsonArray.size()](bool success, const QJsonObject &response) {
                              int successCount = 0;
                              if (success && response.contains("successCount")) {
                                  successCount = response["successCount"].toInt();
                              } else if (success) {
                                  successCount = totalCount;
                              }

                              emit batchCreateFinished(success, totalCount, successCount, response);
                              emit batchProgress(totalCount, totalCount, "Batch sync completed");
                              setIsloading(false);
                          }
                          );
}
//Auth
void APIClient::login(const QString &email,const QString &location){
    setIsloading(true);
    QJsonObject data;
    data["phone"] = email;
    data["location"] = location;
    sendRequest("POST","/auth/login",data,[this](bool success, const QJsonObject &response){
        if (success && response.contains("token")) {
            QString token =
                response["token"].toString();

            setAuthToken(token);

            AppSettings::instance()
                .setAuthToken(token);

            AppSettings::instance()
                .setLoggedIn(true);
        }

        emit loginFinished(success, response);

        setIsloading(false);
    }
                );
}
// Add this method to APIClient.cpp
void APIClient::registerUser(const QString &phoneNumber, const QString &district)
{
    setIsloading(true);
    QJsonObject data;
    data["phoneNumber"] = phoneNumber;
    data["district"] = district;

    sendRequest("POST", "/auth/login/register", data,
                [this, phoneNumber, district](bool success, const QJsonObject &response) {
                    if (success) {
                        // Just save locally - NO TOKENS
                        AppSettings::instance().setUserData(phoneNumber, district);
                        AppSettings::instance().setLoggedIn(true);
                        AppSettings::instance().setInferenceCounter(0);
                    }
                    emit loginFinished(success, response);
                    setIsloading(false);
                }
                );
}

void APIClient::logout(){
    m_authToken.clear();

    AppSettings::instance()
        .setAuthToken("");

    AppSettings::instance()
        .setLoggedIn(false);

    emit authTokenChanged();

    emit logoutFinished();
}