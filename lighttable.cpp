#include "lighttable.h"
#include "mainwindow.h"

LightTable::LightTable(QWidget *parent) : QTableWidget(parent)
{
    QStringList labels;
    labels << "Name";
    labels << "Type";
    labels << "Status";
    labels << "Farbe";
    labels << "Helligkeit";
    labels << "ColorLoop";
    labels << "UUID";
    labels << "ID";

    this->setSelectionMode(QTableWidget::NoSelection);
    this->setColumnCount(8);
    this->setHorizontalHeaderLabels(labels);
    this->verticalHeader()->hide();

    this->setSortingEnabled(true);
    this->hideColumn(6);
    this->hideColumn(7);

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClick(QModelIndex)));
}

LightTable::~LightTable()
{

}

void LightTable::init(HueWrapper *hue)
{
    this->hue = hue;
    connect(hue, SIGNAL(dataLightsUpdated(QByteArray)), this, SLOT(update(QByteArray)));
}

void LightTable::update(QByteArray json)
{
    this->sortItems(-1);
    this->clearContents();
    this->setRowCount(0);
    this->hideColumn(6);
    this->hideColumn(7);

    QJsonDocument doc = QJsonDocument::fromJson(json);
    QHash<QString, QVariant> hash = doc.object().toVariantHash();

    QStringList key_list = hash.uniqueKeys();
    for (int i = 0; i < key_list.length(); i++)
    {
        //qDebug() << doc.object().value(key_list.at(i));
        this->setRowCount(this->rowCount()+1);
        this->setItem(this->rowCount()-1,0, new QTableWidgetItem(
                                           QString("%1: %2").arg(key_list.at(i),2, '0').arg(doc.object().value(key_list.at(i)).toObject().value("name").toString())));

        this->setItem(this->rowCount()-1,1, new QTableWidgetItem(
                                           doc.object().value(key_list.at(i)).toObject().value("type").toString()));

        this->setItem(this->rowCount()-1,2, new QTableWidgetItem(
                                           (doc.object().value(key_list.at(i)).toObject().value("state").toObject().value("on").toBool()?"ein":"aus")));

        int bri = doc.object().value(key_list.at(i)).toObject().value("state").toObject().value("bri").toInt();
        int hue = doc.object().value(key_list.at(i)).toObject().value("state").toObject().value("hue").toInt();
        int sat = doc.object().value(key_list.at(i)).toObject().value("state").toObject().value("sat").toInt();
        QColor col = QColor::fromHsv((int)(hue/65535.0 * 360),sat, bri);
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setBackgroundColor(col);
        this->setItem(this->rowCount()-1,3, item);

        this->setItem(this->rowCount()-1,4, new QTableWidgetItem(
                                           QString::number(qCeil((doc.object().value(key_list.at(i)).toObject().value("state").toObject().value("bri").toInt())/2.54)) + "\%"));

        this->setItem(this->rowCount()-1,5, new QTableWidgetItem(
                                           (doc.object().value(key_list.at(i)).toObject().value("state").toObject().value("effect").toString()=="colorloop"?"ein":"aus")));


        this->setItem(this->rowCount()-1,6, new QTableWidgetItem(
                                           doc.object().value(key_list.at(i)).toObject().value("uniqueid").toString()));

        this->setItem(this->rowCount()-1,7, new QTableWidgetItem(QString("%1").arg(key_list.at(i),2, '0')));



    }
    this->resizeColumnsToContents();
    this->sortItems(0, Qt::AscendingOrder);

    for (int row = 0; row < this->rowCount(); row++)
    {
        for (int col = 0; col < this->columnCount(); col++)
        {
            this->item(row,col)->setFlags(this->item(row,col)->flags() & ~Qt::ItemIsEditable);
        }
    }
}

void LightTable::doubleClick(const QModelIndex index)
{
    if (index.column() == 2)
    {
        hue->setLightOn(this->item(this->currentRow(),7)->text().toInt(),
                    (this->item(this->currentRow(),2)->text() == "ein"?false:true) );
    }
    else if (index.column() == 3)
    {
        QColorDialog dia(this->currentItem()->backgroundColor(),this);
        dia.exec();
        if (dia.selectedColor().isValid())
            hue->setLightColor(this->item(this->currentRow(),7)->text().toInt(),dia.selectedColor());
    }
    else if (index.column() == 4)
    {
        bool ok;
        int bri = QInputDialog::getInt(this, "Helligkeit", "Geben Sie die Helligkeit an", this->item(this->currentRow(),4)->text().replace("\%","").toInt(),0, 100, 10, &ok);
        if (ok)
            hue->setLightBri(this->item(this->currentRow(),7)->text().toInt(), bri*2.54 );
    }
    else if (index.column() == 5)
    {
        hue->setLightColorLoop(this->item(this->currentRow(),7)->text().toInt(),
                           (this->item(this->currentRow(),5)->text() == "ein"?false:true) );
    }

    QTimer::singleShot(100, this->hue, SLOT(doUpdateLights()));
}
