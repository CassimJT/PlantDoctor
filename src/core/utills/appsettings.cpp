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
QString AppSettings::varietyForClass(int classId) const{
    return settings.value(
                       QString("variety/%1").arg(classId),
                       ""
                       ).toString();
}
void AppSettings::setVarietyForClass(int classId,const QString &variety){
    settings.setValue(
        QString("variety/%1").arg(classId),
        variety
        );
}
bool AppSettings::hasVarietyForClass(int classId) const{
    return !settings.value(
                        QString("variety/%1")
                            .arg(classId)
                        ).toString().isEmpty();
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

QString AppSettings::authToken() const
{
    return settings.value("auth/token", "").toString();
}

void AppSettings::setAuthToken(const QString &token)
{
    settings.setValue("auth/token", token);
}