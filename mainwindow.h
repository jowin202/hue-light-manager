#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QColorDialog>
#include <QInputDialog>
#include <QProgressDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QDebug>

#include "huewrapper.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    HueWrapper hue;

private slots:
    void keyRequest();
    void recvKey();
    void timer_tick();

    void debugRequest();

    void on_pushButton_2_clicked();



    void on_pushButton_3_clicked();
    void on_line_hue_key_textChanged(const QString &arg1);
    void on_actionSchlie_en_triggered();
    void on_actionInfo_triggered();
    void on_line_hue_address_textChanged(const QString &arg1);
    void on_button_get_ip_clicked();
    void on_radio_lights_toggled(bool checked);
    void on_radio_groups_toggled(bool checked);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager nam;
    QSettings settings;
    QTimer timer;
    QProgressDialog *dia;
};

#endif // MAINWINDOW_H
