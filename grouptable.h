#ifndef GROUPTABLE_H
#define GROUPTABLE_H

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

class GroupTable : public QTableWidget
{
    Q_OBJECT

public:
    explicit GroupTable(QWidget *parent = 0);
    ~GroupTable();
    void init(HueWrapper *hue);

public slots:
    void update(QByteArray json);
    void doubleClick(const QModelIndex index);

private:
    HueWrapper *hue;
};

#endif // GROUPTABLE_H
