#ifndef HELPER_H
#define HELPER_H

#include <QObject>
#include <QImage>           // Add this for QImage
#include <QBuffer>
#include <QUrl>
#include <QFile>
#include <QDir>
#include <QUuid>
#include <QStandardPaths>
#include <QtConcurrent>
#include <QDebug>
#if defined(Q_OS_ANDROID)
#include <QJniObject>
#include <QJniEnvironment>
#include <QPermission>
#include <QImage>
#endif



class Helper : public QObject
{
    Q_PROPERTY(bool isHompage READ getIsHompage WRITE setIsHompage NOTIFY isHompageChanged FINAL)
    Q_OBJECT
public:
    explicit Helper(QObject *parent = nullptr);
    bool getIsHompage() const;
    Q_INVOKABLE void setStatusBarAppearance(const QColor &backgroundColor, bool darkIcons);
    Q_INVOKABLE void requestCameraPeremision();

    bool getIsCamera() const;

public slots:
    void setIsHompage(bool newIsHompage);

    void imageToDataUrl(const QImage &image);

    void loadImageFromContentUri(const QString &uri);
    void setIsCamera(bool newIsCamera);

    QString localFilePath();

    QString imagePreview();


signals:
    void imageReady();
    void isHompageChanged();

    void isCameraChanged(bool isCamera);

private:
    QString m_imagePath;
    QString m_localPath;
    bool isHompage;
    bool isCamera;
    void requestIgnoreBatteryOptimization();


    Q_PROPERTY(bool isCamera READ getIsCamera WRITE setIsCamera NOTIFY isCameraChanged FINAL)
};

#endif // HELPER_H
