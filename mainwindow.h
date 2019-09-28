#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "include/qmqtt/qmqtt.h"
#include <QHostInfo>
#include <QTime>
#include <QTimer>
#include <QObject>
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>

#define TIMEMS qPrintable (QTime::currentTime().toString("HH:mm:ss zzz"))

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

protected:
    void closeEvent(QCloseEvent *event);
private:
    void readSettings();
    void writeSettings();

    QMQTT::Client *client;
    QTimer *m_pTimer;
    void uiChange(bool isEnabled);
    uint16_t countTime;//定时时间
private slots:
    void onMQTT_Connected();
    void onMQTT_Disconnected();
    void onMQTT_subscribed(const QString &topic);
    void onMQTT_published(const QMQTT::Message&msg);
    void onMQTT_received(const QMQTT::Message&msg);
    void onMQTT_error(const QMQTT::ClientError error);
    void on_subPushButton_clicked();
    void on_conPushButton_clicked();
    void on_sendPushButton_clicked();

    void handleTimeout(void);
    void on_autoPubPushButton_clicked();
};

#endif // MAINWINDOW_H
