#include "grouptable.h"
#include "mainwindow.h"

GroupTable::GroupTable(QWidget *parent) : QTableWidget(parent)
{
    QStringList labels;
    labels << "Name";
    labels << "Status";
    labels << "Farbe";
    labels << "Helligkeit";
    labels << "ColorLoop";
    labels << "ID";

    this->setSelectionMode(QTableWidget::NoSelection);
    this->setColumnCount(6);
    this->setHorizontalHeaderLabels(labels);
    this->verticalHeader()->hide();

    this->setSortingEnabled(true);
    this->hideColumn(5);



    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClick(QModelIndex)));
}

GroupTable::~GroupTable()
{

}

void GroupTable::init(HueWrapper *hue)
{
    this->hue = hue;
    connect(hue, SIGNAL(dataGroupsUpdated(QByteArray)), this, SLOT(update(QByteArray)));
}

void GroupTable::update(QByteArray json)
{
    this->sortItems(-1);
    this->clearContents();
    this->setRowCount(0);
    this->hideColumn(5);


    QJsonDocument doc = QJsonDocument::fromJson(json);
    QHash<QString, QVariant> hash = doc.object().toVariantHash();

    QStringList key_list = hash.uniqueKeys();
    for (int i = 0; i < key_list.length(); i++)
    {
        this->setRowCount(this->rowCount()+1);
        this->setItem(this->rowCount()-1,0, new QTableWidgetItem(
                                           QString("%1: %2").arg(key_list.at(i),2, '0').arg(doc.object().value(key_list.at(i)).toObject().value("name").toString())));

        this->setItem(this->rowCount()-1,1, new QTableWidgetItem(
                                           (doc.object().value(key_list.at(i)).toObject().value("state").toObject().value("all_on").toBool()?"ein":"aus")));


        int bri = doc.object().value(key_list.at(i)).toObject().value("action").toObject().value("bri").toInt();
        int hue = doc.object().value(key_list.at(i)).toObject().value("action").toObject().value("hue").toInt();
        int sat = doc.object().value(key_list.at(i)).toObject().value("action").toObject().value("sat").toInt();
        QColor col = QColor::fromHsv((int)(hue/65535.0 * 360),sat, bri);
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setBackgroundColor(col);
        this->setItem(this->rowCount()-1,2, item);

        this->setItem(this->rowCount()-1,3, new QTableWidgetItem(
                                           QString::number(qCeil((doc.object().value(key_list.at(i)).toObject().value("action").toObject().value("bri").toInt())/2.54)) + "\%"));

        this->setItem(this->rowCount()-1,4, new QTableWidgetItem(
                                           (doc.object().value(key_list.at(i)).toObject().value("action").toObject().value("effect").toString()=="colorloop"?"ein":"aus")));

        this->setItem(this->rowCount()-1,5, new QTableWidgetItem(QString("%1").arg(key_list.at(i),2, '0')));

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

void GroupTable::doubleClick(const QModelIndex index)
{
    if (index.column() == 1)
    {
        hue->setGroupOn(this->item(this->currentRow(),5)->text().toInt(),
                    (this->item(this->currentRow(),1)->text() == "ein"?false:true) );
    }
    else if (index.column() == 2)
    {
        QColorDialog dia(this->currentItem()->backgroundColor(),this);
        dia.exec();
        if (dia.selectedColor().isValid())
            hue->setGroupColor(this->item(this->currentRow(),5)->text().toInt(),dia.selectedColor());
    }
    else if (index.column() == 3)
    {
        bool ok;
        int bri = QInputDialog::getInt(this, "Helligkeit", "Geben Sie die Helligkeit an", this->item(this->currentRow(),3)->text().replace("\%","").toInt(),0, 100, 10, &ok);
        if (ok)
            hue->setGroupBri(this->item(this->currentRow(),5)->text().toInt(), bri*2.54 );
    }
    else if (index.column() == 4)
    {
        hue->setGroupColorLoop(this->item(this->currentRow(),5)->text().toInt(),
                           (this->item(this->currentRow(),4)->text() == "ein"?false:true) );
    }

    QTimer::singleShot(100, this->hue, SLOT(doUpdateGroups()));
}
