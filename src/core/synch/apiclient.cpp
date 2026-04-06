#include "apiclient.h"
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>
#include <QDebug>

ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_baseUrl("http://localhost:3000/api") // Change to your actual API URL
{
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &ApiClient::onReplyFinished);

    // Connect SSL error handling
    connect(m_networkManager, &QNetworkAccessManager::sslErrors,
            this, &ApiClient::onSslErrors);
}

ApiClient::~ApiClient()
{
}

void ApiClient::registerUser(const QString &email, const QString &password, const QString &name)
{
    QNetworkRequest request = createRequest("/auth/register", false);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["email"] = email;
    json["password"] = password;
    json["name"] = name;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    m_networkManager->post(request, data);
}

void ApiClient::login(const QString &email, const QString &password)
{
    QNetworkRequest request = createRequest("/auth/login", false);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["email"] = email;
    json["password"] = password;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    m_networkManager->post(request, data);
}

void ApiClient::logout()
{
    if (m_accessToken.isEmpty()) {
        emit errorOccurred("No access token available");
        return;
    }

    QNetworkRequest request = createRequest("/auth/logout", true);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["refreshToken"] = m_refreshToken;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    m_networkManager->post(request, data);
}

void ApiClient::uploadInference(const QJsonObject &inferenceData)
{
    // No authentication required for upload inference
    QNetworkRequest request = createRequest("/inference", false);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc(inferenceData);
    QByteArray data = doc.toJson();

    m_networkManager->post(request, data);
}

void ApiClient::setAccessToken(const QString &token)
{
    m_accessToken = token;
}

void ApiClient::setRefreshToken(const QString &token)
{
    m_refreshToken = token;
}

QString ApiClient::getAccessToken() const
{
    return m_accessToken;
}

QString ApiClient::getRefreshToken() const
{
    return m_refreshToken;
}

void ApiClient::onReplyFinished(QNetworkReply *reply)
{
    if (!reply) return;

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QByteArray responseData = reply->readAll();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);

    QString endpoint = reply->url().toString();

    if (reply->error() == QNetworkReply::NoError) {
        // Success handling
        if (endpoint.contains("/auth/register")) {
            QJsonObject response = doc.object();
            emit registerSuccess(response);
        }
        else if (endpoint.contains("/auth/login")) {
            QJsonObject response = doc.object();
            saveTokens(response);
            emit loginSuccess(response);
        }
        else if (endpoint.contains("/auth/logout")) {
            m_accessToken.clear();
            m_refreshToken.clear();
            emit logoutSuccess();
        }
        else if (endpoint.contains("/inference")) {
            QJsonObject response = doc.object();
            emit uploadInferenceSuccess(response);
        }
    }
    else {
        // Error handling
        QString errorMessage;
        if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
            QJsonObject errorObj = doc.object();
            errorMessage = errorObj["message"].toString();
            if (errorMessage.isEmpty()) {
                errorMessage = errorObj["error"].toString();
            }
        }

        if (errorMessage.isEmpty()) {
            errorMessage = reply->errorString();
        }

        // Handle token expiration (401 Unauthorized) - only for auth endpoints
        if (statusCode == 401 && (endpoint.contains("/auth/logout"))) {
            handleAuthError();
        }

        emit errorOccurred(errorMessage, statusCode);
    }

    reply->deleteLater();
}

void ApiClient::onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    // For development only - ignore SSL errors
    // In production, you should handle this properly
    qWarning() << "SSL Errors occurred:" << errors;

    // Optionally, you can ignore SSL errors for development
    // reply->ignoreSslErrors();

    // Emit network error with details
    if (!errors.isEmpty()) {
        emit networkError("SSL Error: " + errors.first().errorString());
    }
}

QNetworkRequest ApiClient::createRequest(const QString &endpoint, bool requireAuth)
{
    QUrl url(m_baseUrl + endpoint);
    QNetworkRequest request(url);

    if (requireAuth && !m_accessToken.isEmpty()) {
        request.setRawHeader("Authorization", QString("Bearer %1").arg(m_accessToken).toUtf8());
    }

    return request;
}

void ApiClient::handleAuthError()
{
    // Token expired or invalid
    m_accessToken.clear();
    emit tokenExpired();
}

void ApiClient::saveTokens(const QJsonObject &response)
{
    if (response.contains("accessToken")) {
        m_accessToken = response["accessToken"].toString();
    }
    if (response.contains("refreshToken")) {
        m_refreshToken = response["refreshToken"].toString();
    }
}