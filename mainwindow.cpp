#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->ui->stackedWidget->setCurrentIndex(0);
    this->ui->line_hue_key->setText(settings.value("key").toString());
    this->ui->line_hue_address->setText(settings.value("address").toString());

    connect(&hue, SIGNAL(ipFound(QString)), this->ui->line_hue_address, SLOT(setText(QString)));
    if (this->ui->line_hue_address->text().isEmpty())
    {
        hue.ipRequest();
    }

    this->ui->table_lights->init(&this->hue);
    this->ui->table_groups->init(&this->hue);
    this->timer.setInterval(1000);
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(timer_tick()));

}

MainWindow::~MainWindow()
{
    delete ui;
}






void MainWindow::keyRequest()
{
    QJsonObject obj;
    obj.insert("devicetype", "jowinapp");
    QJsonDocument doc(obj);

    QNetworkRequest req = QNetworkRequest(QUrl(QString("http://%1/api/").arg(this->ui->line_hue_address->text())));
    QNetworkReply *rep = nam.post(req,doc.toJson());
    connect(rep, SIGNAL(finished()), this, SLOT(recvKey()));
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
            qDebug() << "host not found";
        }
        if (error_type == 101)
        {
            qDebug() << "waiting";
            //doing nothing
        }
        QString key = doc.array().first().toObject().value("success").toObject().value("username").toString();
        if (!key.isEmpty())
        {
            this->ui->line_hue_key->setText(key);
            this->timer.stop();
            this->dia->close(); //TODO: check if dia exists
        }
    }
}

void MainWindow::timer_tick()
{
    if (this->dia->value() % 10 == 0)
        this->keyRequest();
    this->dia->setValue(this->dia->value() + 1);
    qDebug() << "timer_tick " << dia->value();
    if (this->dia->value() == this->dia->maximum() - 1)
    {
        this->dia->close();
        timer.stop();
        qDebug() << "dialog stopped";
        this->keyRequest();
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
    hue.doUpdateLights();
    hue.doUpdateGroups();
}



void MainWindow::on_pushButton_3_clicked()
{
    //Key holen
    dia = new QProgressDialog;
    dia->setLabel(new QLabel("Drücken Sie den Button an der Hue Bridge"));
    dia->setMaximum(30);
    dia->setValue(0);
    dia->setModal(true);
    dia->show();
    connect(dia, SIGNAL(canceled()), &this->timer, SLOT(stop()));
    this->timer.start();
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
    box.setText("<font color=red><b>Dieses Programm wird nicht von Philips bereitgestellt</b></font><br><br>"
                "<b>Was bisher möglich ist:</b> <ul>"
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
                "<br>"
                "Quelltext ist auf Github");
    box.exec();
}

void MainWindow::on_line_hue_address_textChanged(const QString &arg1)
{
    settings.setValue("address", arg1);
}

void MainWindow::on_button_get_ip_clicked()
{
    hue.ipRequest();
}

void MainWindow::on_radio_lights_toggled(bool checked)
{
    if (checked)
        this->ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_radio_groups_toggled(bool checked)
{
    if (checked)
        this->ui->stackedWidget->setCurrentIndex(1);
}
