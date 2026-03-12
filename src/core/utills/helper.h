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
#endif



class Helper : public QObject
{
    Q_PROPERTY(bool isHompage READ getIsHompage WRITE setIsHompage NOTIFY isHompageChanged FINAL)
    Q_OBJECT
public:
    explicit Helper(QObject *parent = nullptr);
    bool getIsHompage() const;
    Q_INVOKABLE void setStatusBarAppearance(const QColor &backgroundColor, bool darkIcons);

public slots:
    void setIsHompage(bool newIsHompage);

    void imageToDataUrl(const QImage &image);

    void loadImageFromContentUri(const QString &uri);

    QString localFilePath();

    QString imagePreview();


signals:
    void imageReady();
    void isHompageChanged();

private:
    QString m_imagePath;
    QString m_localPath;
    bool isHompage;


};

#endif // HELPER_H
