#include "appsettings.h"

AppSettings::AppSettings(QObject *parent)
    : QObject{parent}
{
    //constractore
}

AppSettings &AppSettings::instance()
{
    static AppSettings appSettings;
    return appSettings;
}

//Survey
bool AppSettings::hasTakenSurvey(int classId)
{
    return settings.value(
                       QString("survey/%1").arg(classId),
                       false
                       ).toBool();
}

void AppSettings::setSurveyTaken(int classId, bool taken)
{
    settings.setValue(
        QString("survey/%1").arg(classId),
        taken
        );
}
//language
QString AppSettings::language() const
{
    return settings.value("app/lan","en").toString();
}

void AppSettings::setLanguage(const QString &newLanguage)
{
    settings.setValue("app/lan", newLanguage);
}
//infarence counter
int AppSettings::inferenceCounter() const
{
    return settings.value("app/inferenceCounter", 0).toInt();
}

void AppSettings::setInferenceCounter(int newInferenceCounter)
{
    settings.setValue(
        "app/inferenceCounter",
        newInferenceCounter
        );
}
//auth
bool AppSettings::isLoggedIn() const
{
    return settings.value("auth/isLoggedIn", false).toBool();
}

void AppSettings::setLoggedIn(bool loggedIn)
{
    settings.setValue("auth/isLoggedIn", loggedIn);
}
