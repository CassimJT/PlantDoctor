#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QObject>
#include <QSettings>

class AppSettings : public QObject
{
    Q_OBJECT

public:
    explicit AppSettings(QObject *parent = nullptr);

    static AppSettings& instance();

    QString varietyForClass(int classId) const;

    void setVarietyForClass(int classId,const QString &variety);

    bool hasVarietyForClass(int classId) const;

    bool isLoggedIn() const;
    void setLoggedIn(bool newIsLogedIn);

    int inferenceCounter() const;
    void setInferenceCounter(int newInfarenceCounter);

    QString language() const;
    void setLanguage(const QString &newLanguage);

    QString authToken() const;
    void setAuthToken(const QString &token);

signals:
    void isLogedInChanged();

private:

    QSettings settings;


};

#endif // APPSETTINGS_H