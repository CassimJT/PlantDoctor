#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QLocale>
#include "src/core/utills/helper.h"
#include "src/core/infarence/infarencerunner.h"
#include "src/core/infarence/diseaseinfomanager.h"
#include "src/core/rtsp/rtsvideooutput.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    qputenv("QT_ANDROID_NO_EXIT_CALL", "true");

    // QSettings information
    QCoreApplication::setOrganizationName("PlantDoctor");
    QCoreApplication::setOrganizationDomain("PlantDoctor.com");
    QCoreApplication::setApplicationName("PlantDoctor");

    // Initialize resources
   // Q_INIT_RESOURCE(languages);

    // instances
    Helper helper;
    InfarenceRunner infarenceRunner;

    // Simple language detection - Chichewa for Malawi, English for everything else
    QString systemLang = QLocale::system().name().split("_").first();
    QString appLanguage = (systemLang == "ny" || systemLang == "mw") ? "ny" : "en";

    // Load the language
    DiseaseInfoManager::instance().loadLanguage(appLanguage);

    // Set up QML engine
    QQmlApplicationEngine engine;

    // Expose C++ objects to QML
    engine.rootContext()->setContextProperty("Helper", &helper);
    engine.rootContext()->setContextProperty("InfarenceRunner", &infarenceRunner);
    engine.rootContext()->setContextProperty("CurrentLanguage", appLanguage);
    qmlRegisterType<RTSVideoOutput>("RTSVideoOutput",1,0,"RTSVideoOutput");
    // Simple language names for UI
    QVariantMap languageNames;
    languageNames["en"] = "English";
    languageNames["ny"] = "Chichewa";
    engine.rootContext()->setContextProperty("LanguageNames", languageNames);

    qmlRegisterSingletonType(QUrl("qrc:/ui/features/inference/model/HistoryModel.qml"),
                             "HistoryModel", 1, 0, "HistoryModel");
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.loadFromModule("PlantDoctor", "Main");

    return app.exec();
}
