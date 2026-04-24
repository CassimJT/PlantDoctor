#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <functional>
#include <QMap>
#include <QVariantList>

class APIClient : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString baseUrl READ baseUrl WRITE setBaseUrl NOTIFY baseUrlChanged)
    Q_PROPERTY(QString authToken READ authToken WRITE setAuthToken NOTIFY authTokenChanged)
    Q_PROPERTY(bool isloading READ isloading WRITE setIsloading NOTIFY isloadingChanged FINAL)

public:
    explicit APIClient(QObject *parent = nullptr);
    ~APIClient();

    // Properties
    QString baseUrl() const { return m_baseUrl; }
    void setBaseUrl(const QString &url);

    QString authToken() const { return m_authToken; }
    void setAuthToken(const QString &token);

    // C++ API
    void createInference(const QString &location, const QString &diseaseName,
                         double confidence, const QString &variety,
                         std::function<void(bool, const QJsonObject&)> callback);

    void listInferences(std::function<void(bool, const QJsonArray&)> callback);

    void getInference(const QString &inferenceId,
                      std::function<void(bool, const QJsonObject&)> callback);

    void createBatchInferences(const QJsonArray &inferencesArray,
                               std::function<void(bool, const QJsonObject&)> callback);

    // QML API
    Q_INVOKABLE void createInferenceQml(const QString &location,
                                        const QString &diseaseName,
                                        double confidence,
                                        const QString &variety);

    Q_INVOKABLE void listInferencesQml();

    Q_INVOKABLE void getInferenceQml(const QString &inferenceId);

    Q_INVOKABLE void createBatchInferencesQml(const QVariantList &inferencesList);
    Q_INVOKABLE void createBatchInferencesFromJson(const QString &jsonArray);

    bool isloading() const;
    void setIsloading(bool newIsloading);

signals:
    void baseUrlChanged();
    void authTokenChanged();
    void networkError(const QString &error);
    void authenticationRequired();
    void isloadingChanged();

    // Result signals
    void createInferenceFinished(bool success, QJsonObject response);
    void listInferencesFinished(bool success, QJsonArray response);
    void getInferenceFinished(bool success, QJsonObject response);

    // Batch result signals
    void batchCreateFinished(bool success, int totalCount, int successCount, QJsonObject response);
    void batchProgress(int current, int total, QString currentDisease);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_nam;
    QString m_baseUrl;
    QString m_authToken;
    bool m_isloading;

    struct PendingRequest {
        std::function<void(bool, const QJsonObject&)> objectCallback;
        std::function<void(bool, const QJsonArray&)> arrayCallback;
    };

    QMap<QNetworkReply*, PendingRequest> m_pendingRequests;

    void sendRequest(const QString &method, const QString &endpoint,
                     const QJsonObject &data = QJsonObject(),
                     std::function<void(bool, const QJsonObject&)> objectCallback = nullptr,
                     std::function<void(bool, const QJsonArray&)> arrayCallback = nullptr);

    void setupRequestHeaders(QNetworkRequest &request);

    QJsonArray variantListToJsonArray(const QVariantList &list);
};

#endif // APICLIENT_H