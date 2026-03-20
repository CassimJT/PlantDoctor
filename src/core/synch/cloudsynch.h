#ifndef CLOUDSYNCH_H
#define CLOUDSYNCH_H

#include <QObject>

class CloudSynch : public QObject
{
    Q_OBJECT
public:
    explicit CloudSynch(QObject *parent = nullptr);

signals:
};

#endif // CLOUDSYNCH_H
