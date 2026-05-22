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

    bool hasTakenSurvey(int classId);
    void setSurveyTaken(int classId, bool taken);

    bool isLoggedIn() const;
    void setLoggedIn(bool newIsLogedIn);

    int inferenceCounter() const;
    void setInferenceCounter(int newInfarenceCounter);

    QString language() const;
    void setLanguage(const QString &newLanguage);

signals:
    void isLogedInChanged();

private:
    bool m_isLogedIn;
    int m_infarenceCounter;
    QString m_language;
    QSettings settings;


};

#endif // APPSETTINGS_H