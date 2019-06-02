#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->ui->tableWidget->setSortingEnabled(true);
    this->ui->tableWidget->hideColumn(6);
    this->ui->tableWidget->hideColumn(7);
    this->ui->line_hue_key->setText(settings.value("key").toString());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    //Hue Suchen
    QNetworkRequest req = QNetworkRequest(QUrl("https://www.meethue.com/api/nupnp"));
    QNetworkReply *rep = nam.get(req);
    connect(rep, SIGNAL(finished()), this, SLOT(getHueIP()));
}

void MainWindow::getHueIP()
{
    QNetworkReply *rep = (QNetworkReply*)sender();
    if (rep != 0)
    {
        if (rep->error() == QNetworkReply::ProtocolUnknownError)
        {
            QMessageBox box(this);
            box.setWindowTitle("Hinweis");
            box.setTextFormat(Qt::RichText);
            box.setText("Die IP von der Hue Bridge kann auf folgender Seite ausgelesen werden:<br><a href='https://www.meethue.com/api/nupnp'>https://www.meethue.com/api/nupnp</a><br><br>Dort steht eine IP, die man in das Feld nebenan eintragen muss.<br><br>Jetzt könnte man fragen, warum ich das nicht vollautomatisch beim Programmstart mache: Hab ich gemacht! Es geht unter Linux aber meine Windows-Qt-Version kann kein HTTPS, weil ich an meiner Version rumgebastelt habe. Bis ich das in Ordnung gebracht habe, muss man das eben händisch eintragen.");
            box.exec();
            return;
        }
        else if (rep->error() == QNetworkReply::HostNotFoundError)
        {
            QMessageBox::critical(this, "Fehler", "Keine Internetverbindung vorhanden");
            return;
        }
        QByteArray json = rep->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(json);
        if (doc.array().count() > 0)
            this->ui->line_hue_address->setText(doc.array().first().toObject().value("internalipaddress").toString());
    }
}

void MainWindow::updateTable()
{
    QNetworkReply *rep = (QNetworkReply*)sender();
    if (rep != 0)
    {
        this->ui->tableWidget->sortItems(-1);
        this->ui->tableWidget->clearContents();
        this->ui->tableWidget->setRowCount(0);
        this->ui->tableWidget->hideColumn(6);
        this->ui->tableWidget->hideColumn(7);

        QByteArray json = rep->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(json);
        QHash<QString, QVariant> hash = doc.object().toVariantHash();

        QStringList key_list = hash.uniqueKeys();
        for (int i = 0; i < key_list.length(); i++)
        {
            //qDebug() << doc.object().value(key_list.at(i));
            this->ui->tableWidget->setRowCount(this->ui->tableWidget->rowCount()+1);
            this->ui->tableWidget->setItem(this->ui->tableWidget->rowCount()-1,0, new QTableWidgetItem(
                                               QString("%1: %2").arg(key_list.at(i),2, '0').arg(doc.object().value(key_list.at(i)).toObject().value("name").toString())));

            this->ui->tableWidget->setItem(this->ui->tableWidget->rowCount()-1,1, new QTableWidgetItem(
                                               doc.object().value(key_list.at(i)).toObject().value("type").toString()));

            this->ui->tableWidget->setItem(this->ui->tableWidget->rowCount()-1,2, new QTableWidgetItem(
                                               (doc.object().value(key_list.at(i)).toObject().value("state").toObject().value("on").toBool()?"ein":"aus")));

            int bri = doc.object().value(key_list.at(i)).toObject().value("state").toObject().value("bri").toInt();
            int hue = doc.object().value(key_list.at(i)).toObject().value("state").toObject().value("hue").toInt();
            int sat = doc.object().value(key_list.at(i)).toObject().value("state").toObject().value("sat").toInt();
            QColor col = QColor::fromHsv((int)(hue/65535.0 * 360),sat, bri);
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setBackgroundColor(col);
            this->ui->tableWidget->setItem(this->ui->tableWidget->rowCount()-1,3, item);

            this->ui->tableWidget->setItem(this->ui->tableWidget->rowCount()-1,4, new QTableWidgetItem(
                                               QString::number(qCeil((doc.object().value(key_list.at(i)).toObject().value("state").toObject().value("bri").toInt())/2.54)) + "\%"));

            this->ui->tableWidget->setItem(this->ui->tableWidget->rowCount()-1,5, new QTableWidgetItem(
                                               (doc.object().value(key_list.at(i)).toObject().value("state").toObject().value("effect").toString()=="colorloop"?"ein":"aus")));


            this->ui->tableWidget->setItem(this->ui->tableWidget->rowCount()-1,6, new QTableWidgetItem(
                                               doc.object().value(key_list.at(i)).toObject().value("uniqueid").toString()));

            this->ui->tableWidget->setItem(this->ui->tableWidget->rowCount()-1,7, new QTableWidgetItem(QString("%1").arg(key_list.at(i),2, '0')));



        }
        this->ui->tableWidget->resizeColumnsToContents();
        this->ui->tableWidget->sortItems(0, Qt::AscendingOrder);

        for (int row = 0; row < this->ui->tableWidget->rowCount(); row++)
        {
            for (int col = 0; col < this->ui->tableWidget->columnCount(); col++)
            {
                this->ui->tableWidget->item(row,col)->setFlags(this->ui->tableWidget->item(row,col)->flags() & ~Qt::ItemIsEditable);
            }
        }
    }

}

void MainWindow::setColorLoop(int light, bool loop)
{
    QJsonObject obj;
    if (loop == true)
    {
        obj.insert("effect", "colorloop");
    }
    else
    {
        obj.insert("effect", "none");
    }
    QJsonDocument doc(obj);

    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/%2/lights/%3/state").arg(this->ui->line_hue_address->text()).arg(this->ui->line_hue_key->text()).arg(light)));
    QNetworkReply *rep = nam.put(req,doc.toJson());
    connect(rep, SIGNAL(finished()), this, SLOT(on_pushButton_2_clicked())); //update list
}




void MainWindow::setLightColor(int light, QColor col)
{
    //Licht Farbe Ändern
    QJsonObject obj;
    obj.insert("hue", (int)(col.hsvHue()/360.0*65535));
    obj.insert("sat", col.saturation());
    obj.insert("bri", col.value());
    QJsonDocument doc(obj);

    //qDebug() << doc.toJson();
    //qDebug() << QUrl(QString("http://%1/api/%2/lights/%3/state").arg(this->ui->line_hue_address->text()).arg(this->ui->line_hue_key->text()).arg(light));
    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/%2/lights/%3/state").arg(this->ui->line_hue_address->text()).arg(this->ui->line_hue_key->text()).arg(light)));
    QNetworkReply *rep = nam.put(req,doc.toJson());
    connect(rep, SIGNAL(finished()), this, SLOT(on_pushButton_2_clicked())); //update list
}

void MainWindow::setOn(int light, bool on)
{
    //Ein und Aus
    QJsonObject obj;
    obj.insert("on", on);
    QJsonDocument doc(obj);

    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/%2/lights/%3/state").arg(this->ui->line_hue_address->text()).arg(this->ui->line_hue_key->text()).arg(light)));
    QNetworkReply *rep = nam.put(req,doc.toJson());
    connect(rep, SIGNAL(finished()), this, SLOT(on_pushButton_2_clicked())); //update list
}

void MainWindow::setBri(int light, int bri)
{
    //Dimmen
    QJsonObject obj;
    obj.insert("bri", bri);
    QJsonDocument doc(obj);

    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/%2/lights/%3/state").arg(this->ui->line_hue_address->text()).arg(this->ui->line_hue_key->text()).arg(light)));
    QNetworkReply *rep = nam.put(req,doc.toJson());
    connect(rep, SIGNAL(finished()), this, SLOT(on_pushButton_2_clicked())); //update list
}

void MainWindow::recvKey()
{
    QNetworkReply *rep = (QNetworkReply*)sender();
    if (rep != 0)
    {
        QByteArray json = rep->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(json);
        int error_type = doc.array().first().toObject().value("error").toObject().value("type").toInt();
        if (rep->error() == QNetworkReply::HostNotFoundError)
        {
            QMessageBox::critical(this, "Hinweis", "Hue Bridge nicht gefunden");
        }
        if (error_type == 101)
        {
            QMessageBox::critical(this, "Hinweis", "Bitte zuerst den Button auf der Hue Bridge drücken");
        }
        QString key = doc.array().first().toObject().value("success").toObject().value("username").toString();
        if (!key.isEmpty())
            this->ui->line_hue_key->setText(key);
    }
}

void MainWindow::debugRequest()
{
    QNetworkReply *rep = (QNetworkReply*)sender();
    if (rep != 0)
    {
        qDebug() << rep->readAll();
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    //Liste Aktualisieren
    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/%2/lights").arg(this->ui->line_hue_address->text()).arg(this->ui->line_hue_key->text())));
    QNetworkReply *rep = nam.get(req);
    connect(rep, SIGNAL(finished()), this, SLOT(updateTable()));

}

void MainWindow::on_tableWidget_doubleClicked(const QModelIndex &index)
{

    if (index.column() == 2)
    {
        this->setOn(this->ui->tableWidget->item(this->ui->tableWidget->currentRow(),7)->text().toInt(),
                    (this->ui->tableWidget->item(this->ui->tableWidget->currentRow(),2)->text() == "ein"?false:true) );
    }
    else if (index.column() == 3)
    {
        QColorDialog dia(this->ui->tableWidget->currentItem()->backgroundColor(),this);
        dia.exec();
        if (dia.selectedColor().isValid())
            this->setLightColor(this->ui->tableWidget->item(this->ui->tableWidget->currentRow(),7)->text().toInt(),dia.selectedColor());
    }
    else if (index.column() == 4)
    {
        bool ok;
        int bri = QInputDialog::getInt(this, "Helligkeit", "Geben Sie die Helligkeit an", this->ui->tableWidget->item(this->ui->tableWidget->currentRow(),4)->text().replace("\%","").toInt(),0, 100, 10, &ok);
        if (ok)
            this->setBri(this->ui->tableWidget->item(this->ui->tableWidget->currentRow(),7)->text().toInt(), bri*2.54 );
    }
    else if (index.column() == 5)
    {
        this->setColorLoop(this->ui->tableWidget->item(this->ui->tableWidget->currentRow(),7)->text().toInt(),
                           (this->ui->tableWidget->item(this->ui->tableWidget->currentRow(),5)->text() == "ein"?false:true) );
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    //Key
    QJsonObject obj;
    obj.insert("devicetype", "jowinapp");
    QJsonDocument doc(obj);

    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/").arg(this->ui->line_hue_address->text())));
    QNetworkReply *rep = nam.post(req,doc.toJson());
    connect(rep, SIGNAL(finished()), this, SLOT(recvKey()));
}

void MainWindow::on_line_hue_key_textChanged(const QString &arg1)
{
    settings.setValue("key", arg1);
}

void MainWindow::on_actionSchlie_en_triggered()
{
    qApp->exit();
}

void MainWindow::on_actionInfo_triggered()
{
    QMessageBox box(this);
    box.setWindowTitle("Hinweis");
    box.setTextFormat(Qt::RichText);
    box.setText("<b>Was bisher möglich ist:</b> <ul>"
                "<li>&nbsp;Von der Hue Bridge einen Key anfordern</li>"
                "<li>&nbsp;Alle Hue Lampen im Haus auflisten</li>"
                "<li>&nbsp;IP von Hue Bridge automatisch herausfinden</li>"
                "<li>&nbsp;Hue Lichter ein und ausschalten (Doppelklick auf ein/aus in Status-Spalte)</li>"
                "<li>&nbsp;Dimmen (Doppelklick auf Helligkeit-Spalte)</li>"
                "<li>&nbsp;Farbe ändern durch Doppelklick auf das Farbsymbol</li></ul><br>"
                ""
                ""
                "<b>Was noch nicht möglich ist:</b><ul>"
                "<li>&nbsp; Szenen und Gruppen von Lampen</li>"
                "<li>&nbsp;Zeitschaltuhr und Event-basiertes Ein- und Ausschalten</li></ul>"
                ""
                "<br>Dauer bis zu diesem Stand des Programmes: 3 Stunden (mit Stoppuhr gemessen)");
    box.exec();
}
