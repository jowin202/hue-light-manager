#include "huewrapper.h"

HueWrapper::HueWrapper(QObject *parent) : QObject(parent)
{
}

void HueWrapper::setLightColorLoop(int light, bool loop)
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
    /*QNetworkReply *rep = */nam.put(req,doc.toJson());
}


void HueWrapper::setGroupColorLoop(int group, bool loop)
{
    QJsonObject obj;
    if (loop == true)
        obj.insert("effect", "colorloop");
    else
        obj.insert("effect", "none");

    QJsonDocument doc(obj);
    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/%2/groups/%3/action")
                                               .arg(settings.value("address").toString())
                                               .arg(settings.value("key").toString())
                                               .arg(group)));
    /*QNetworkReply *rep = */nam.put(req,doc.toJson());
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
    /*QNetworkReply *rep = */nam.put(req,doc.toJson());
}

void HueWrapper::setGroupColor(int group, QColor col)
{
    QJsonObject obj;
    obj.insert("hue", (int)(col.hsvHue()/360.0*65535));
    obj.insert("sat", col.saturation());
    obj.insert("bri", col.value());
    QJsonDocument doc(obj);

    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/%2/groups/%3/action")
                                               .arg(settings.value("address").toString())
                                               .arg(settings.value("key").toString())
                                               .arg(group)));
    /*QNetworkReply *rep = */nam.put(req,doc.toJson());
}

void HueWrapper::setLightOn(int light, bool on)
{
    QJsonObject obj;
    obj.insert("on", on);
    QJsonDocument doc(obj);

    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/%2/lights/%3/state")
                                               .arg(settings.value("address").toString())
                                               .arg(settings.value("key").toString())
                                               .arg(light)));
    /*QNetworkReply *rep = */nam.put(req,doc.toJson());
}

void HueWrapper::setGroupOn(int group, bool on)
{
    QJsonObject obj;
    obj.insert("on", on);
    QJsonDocument doc(obj);

    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/%2/groups/%3/action")
                                               .arg(settings.value("address").toString())
                                               .arg(settings.value("key").toString())
                                               .arg(group)));
    /*QNetworkReply *rep = */nam.put(req,doc.toJson());
}

void HueWrapper::setLightBri(int light, int bri)
{
    QJsonObject obj;
    obj.insert("bri", bri);
    QJsonDocument doc(obj);

    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/%2/lights/%3/state")
                                               .arg(settings.value("address").toString())
                                               .arg(settings.value("key").toString())
                                               .arg(light)));
    /*QNetworkReply *rep = */nam.put(req,doc.toJson());
}

void HueWrapper::setGroupBri(int group, int bri)
{
    QJsonObject obj;
    obj.insert("bri", bri);
    QJsonDocument doc(obj);

    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/%2/groups/%3/action")
                                               .arg(settings.value("address").toString())
                                               .arg(settings.value("key").toString())
                                               .arg(group)));
    /*QNetworkReply *rep = */nam.put(req,doc.toJson());
}

void HueWrapper::ipRequest()
{
    QNetworkRequest req = QNetworkRequest(QUrl("https://www.meethue.com/api/nupnp"));
    QNetworkReply *rep = nam.get(req);
    connect(rep, SIGNAL(finished()), this, SLOT(ipRequestFinished()));
}

void HueWrapper::doUpdateLights()
{
    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/%2/lights")
                                               .arg(settings.value("address").toString())
                                               .arg(settings.value("key").toString())));
    QNetworkReply *rep = nam.get(req);
    connect(rep, SIGNAL(finished()), this, SLOT(sendUpdateLightsSignal()));
}

void HueWrapper::doUpdateGroups()
{
    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/%2/groups")
                                               .arg(settings.value("address").toString())
                                               .arg(settings.value("key").toString())));
    QNetworkReply *rep = nam.get(req);
    connect(rep, SIGNAL(finished()), this, SLOT(sendUpdateGroupsSignal()));
}

void HueWrapper::sendUpdateLightsSignal()
{
    QNetworkReply *rep = (QNetworkReply*) sender();
    if (rep != 0)
    {
        QByteArray data = rep->readAll();
        emit dataLightsUpdated(data);
    }
}

void HueWrapper::sendUpdateGroupsSignal()
{
    QNetworkReply *rep = (QNetworkReply*) sender();
    if (rep != 0)
    {
        QByteArray data = rep->readAll();
        emit dataGroupsUpdated(data);
    }
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
