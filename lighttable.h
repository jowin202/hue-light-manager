#ifndef LIGHTTABLE_H
#define LIGHTTABLE_H

#include <QTimer>
#include <QWidget>
#include <QTableWidget>
#include <QColorDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtMath>
#include <QInputDialog>
#include <QDebug>
#include <QHeaderView>

#include "huewrapper.h"

class LightTable : public QTableWidget
{
    Q_OBJECT

public:
    explicit LightTable(QWidget *parent = 0);
    ~LightTable();
    void init(HueWrapper *hue);

public slots:
    void update(QByteArray json);
    void doubleClick(const QModelIndex index);

private:
    HueWrapper *hue;
};

#endif // LIGHTTABLE_H
