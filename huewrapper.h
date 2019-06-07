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

    void setLightColorLoop(int light, bool loop);
    void setGroupColorLoop(int group, bool loop);
    void setLightColor(int light, QColor col);
    void setGroupColor(int group, QColor col);
    void setLightOn(int light, bool on);
    void setGroupOn(int group, bool on);
    void setLightBri(int light, int bri);
    void setGroupBri(int light, int bri);

    void ipRequest();

public slots:
    void doUpdateLights();
    void doUpdateGroups();
    void sendUpdateLightsSignal();
    void sendUpdateGroupsSignal();
    void ipRequestFinished();

signals:
    void dataLightsUpdated(QByteArray json);
    void dataGroupsUpdated(QByteArray json);
    void ipFound(QString ip);

public slots:

private:
    QSettings settings;
    QNetworkAccessManager nam;
};

#endif // HUEWRAPPER_H
