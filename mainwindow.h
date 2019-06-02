#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QColorDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void getHueIP();
    void updateTable();
    void setColorLoop(int light, bool loop);
    void setLightColor(int light, QColor col);
    void setOn(int light, bool on);
    void setBri(int light, int bri);
    void recvKey();

    void debugRequest();

    void on_pushButton_2_clicked();


    void on_tableWidget_doubleClicked(const QModelIndex &index);

    void on_pushButton_3_clicked();

    void on_line_hue_key_textChanged(const QString &arg1);

    void on_actionSchlie_en_triggered();

    void on_actionInfo_triggered();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager nam;
    QSettings settings;
};

#endif // MAINWINDOW_H
