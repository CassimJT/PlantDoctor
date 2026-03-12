#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "src/core/utills/helper.h"
#include <QQmlContext>



int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    qputenv("QT_ANDROID_NO_EXIT_CALL", "true");
    Helper helper;

    //Qsettings information
    QCoreApplication::setOrganizationName("PlantDoctor");
    QCoreApplication::setOrganizationDomain("PlantDoctor.com");
    QCoreApplication::setApplicationName("PlantDoctor");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("Helper", &helper);
    qmlRegisterSingletonType(QUrl("qrc:/ui/features/inference/model/HistoryModel.qml"), "HistoryModel", 1, 0, "HistoryModel");

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
                     Qt::QueuedConnection);
    engine.loadFromModule("PlantDoctor", "Main");

    return app.exec();
}
