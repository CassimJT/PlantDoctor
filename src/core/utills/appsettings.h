#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QSettings>

class AppSettings : public QObject
{
    Q_OBJECT
public:
    static AppSettings& instance();

    // Survey methods
    Q_INVOKABLE QString varietyForClass(int classId) const;
    Q_INVOKABLE void setVarietyForClass(int classId, const QString &variety);
    Q_INVOKABLE bool hasVarietyForClass(int classId) const;

    // Language methods
    Q_INVOKABLE QString language() const;
    Q_INVOKABLE void setLanguage(const QString &newLanguage);

    // Inference counter methods
    Q_INVOKABLE int inferenceCounter() const;
    Q_INVOKABLE void setInferenceCounter(int newInferenceCounter);

    // Auth methods
    Q_INVOKABLE bool isLoggedIn() const;
    Q_INVOKABLE void setLoggedIn(bool loggedIn);
    Q_INVOKABLE QString authToken() const;
    Q_INVOKABLE void setAuthToken(const QString &token);

    // Survey completion methods
    Q_INVOKABLE bool hasSurveyCompleted(int classId) const;
    Q_INVOKABLE void setSurveyCompleted(int classId, bool completed);

    // User data methods
    Q_INVOKABLE void setUserData(const QString& phoneNumber, const QString& district);
    Q_INVOKABLE QString getUserPhone() const;
    Q_INVOKABLE QString getUserDistrict() const;
    Q_INVOKABLE bool hasUserData() const;

private:
    explicit AppSettings(QObject *parent = nullptr);
    QSettings settings;
};

#endif // APPSETTINGS_H