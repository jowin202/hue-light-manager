#include "huewrapper.h"

HueWrapper::HueWrapper(QObject *parent) : QObject(parent)
{
}

void HueWrapper::setColorLoop(int light, bool loop)
{
    QJsonObject obj;
    if (loop == true)
        obj.insert("effect", "colorloop");
    else
        obj.insert("effect", "none");

    QJsonDocument doc(obj);
    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/%2/lights/%3/state")
                                               .arg(settings.value("address").toString())
                                               .arg(settings.value("key").toString())
                                               .arg(light)));
    QNetworkReply *rep = nam.put(req,doc.toJson());
    connect(rep, SIGNAL(finished()), this, SLOT(sendUpdateSignal()));
}

void HueWrapper::setLightColor(int light, QColor col)
{
    QJsonObject obj;
    obj.insert("hue", (int)(col.hsvHue()/360.0*65535));
    obj.insert("sat", col.saturation());
    obj.insert("bri", col.value());
    QJsonDocument doc(obj);

    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/%2/lights/%3/state")
                                               .arg(settings.value("address").toString())
                                               .arg(settings.value("key").toString())
                                               .arg(light)));
    QNetworkReply *rep = nam.put(req,doc.toJson());
    connect(rep, SIGNAL(finished()), this, SLOT(sendUpdateSignal()));
}

void HueWrapper::setOn(int light, bool on)
{
    QJsonObject obj;
    obj.insert("on", on);
    QJsonDocument doc(obj);

    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/%2/lights/%3/state")
                                               .arg(settings.value("address").toString())
                                               .arg(settings.value("key").toString())
                                               .arg(light)));
    QNetworkReply *rep = nam.put(req,doc.toJson());
    connect(rep, SIGNAL(finished()), this, SLOT(sendUpdateSignal()));
}

void HueWrapper::setBri(int light, int bri)
{
    QJsonObject obj;
    obj.insert("bri", bri);
    QJsonDocument doc(obj);

    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/%2/lights/%3/state")
                                               .arg(settings.value("address").toString())
                                               .arg(settings.value("key").toString())
                                               .arg(light)));
    QNetworkReply *rep = nam.put(req,doc.toJson());
    connect(rep, SIGNAL(finished()), this, SLOT(sendUpdateSignal()));
}

void HueWrapper::ipRequest()
{
    QNetworkRequest req = QNetworkRequest(QUrl("https://www.meethue.com/api/nupnp"));
    QNetworkReply *rep = nam.get(req);
    connect(rep, SIGNAL(finished()), this, SLOT(ipRequestFinished()));
}

void HueWrapper::sendUpdateSignal()
{
    emit dataUpdated();
}

void HueWrapper::ipRequestFinished()
{
    QNetworkReply *rep = (QNetworkReply*)sender();
    if (rep != 0)
    {
        if (rep->error() == QNetworkReply::ProtocolUnknownError)
        {
            //SSL suckz doing nothing
            return;
        }
        else if (rep->error() == QNetworkReply::HostNotFoundError)
        {
            //can i haz internet
            return;
        }
        QByteArray json = rep->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(json);
        if (doc.array().count() > 0)
            emit ipFound(doc.array().first().toObject().value("internalipaddress").toString());
    }
}
