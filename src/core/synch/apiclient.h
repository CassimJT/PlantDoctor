#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QSslError>

class ApiClient : public QObject
{
    Q_OBJECT
public:
    explicit ApiClient(QObject *parent = nullptr);
    ~ApiClient();

    // Auth endpoints
    void registerUser(const QString &email, const QString &password, const QString &name);
    void login(const QString &email, const QString &password);
    void logout();

    // Inference endpoints
    void uploadInference(const QJsonObject &inferenceData);

    // Token management
    void setAccessToken(const QString &token);
    void setRefreshToken(const QString &token);
    QString getAccessToken() const;
    QString getRefreshToken() const;

signals:
    // Success signals
    void registerSuccess(const QJsonObject &response);
    void loginSuccess(const QJsonObject &response);
    void logoutSuccess();
    void uploadInferenceSuccess(const QJsonObject &response);

    // Error signals
    void errorOccurred(const QString &errorString, int statusCode = 0);
    void networkError(const QString &errorString);

    // Token expired signal
    void tokenExpired();

private slots:
    void onReplyFinished(QNetworkReply *reply);
    void onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_baseUrl;
    QString m_accessToken;
    QString m_refreshToken;

    // Helper methods
    QNetworkRequest createRequest(const QString &endpoint, bool requireAuth = false);
    void handleAuthError();
    void saveTokens(const QJsonObject &response);
};

#endif // APICLIENT_H