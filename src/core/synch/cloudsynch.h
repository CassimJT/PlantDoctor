#ifndef CLOUDSYNCH_H
#define CLOUDSYNCH_H

#include <QObject>
#include <QObject>
#include <QJsonObject>
#include <QCoreApplication>
#if defined(Q_OS_ANDROID)
#include <QtCore>
#include <jni.h>
#include <android/log.h>
#define LOG_TAG "NativeWorker"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#endif



class CloudSynch : public QObject
{
    Q_OBJECT
public:
    explicit CloudSynch(QObject *parent = nullptr);
    static CloudSynch* instance();
    void invoked() ;
   // void notifyQtReady();
signals:
    void workerInvoked();
private:
    void StartSchedua();
    void printLog();
    static CloudSynch * _instance;
    void showNotification();

};

#endif // CLOUDSYNCH_H
