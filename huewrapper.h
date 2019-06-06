#ifndef HUEWRAPPER_H
#define HUEWRAPPER_H

#include <QObject>
#include <QtNetwork>
#include <QSettings>
#include <QColor>

class HueWrapper : public QObject
{
    Q_OBJECT
public:
    explicit HueWrapper(QObject *parent = nullptr);

    void setColorLoop(int light, bool loop);
    void setLightColor(int light, QColor col);
    void setOn(int light, bool on);
    void setBri(int light, int bri);

    void ipRequest();

public slots:
    void sendUpdateSignal();
    void ipRequestFinished();

signals:
    void dataUpdated();
    void ipFound(QString ip);

public slots:

private:
    QSettings settings;
    QNetworkAccessManager nam;
};

#endif // HUEWRAPPER_H
