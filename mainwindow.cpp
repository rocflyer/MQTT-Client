#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("MQTT Client-V0.1");
    client = new QMQTT::Client;
    m_pTimer=new QTimer(this);
    countTime=0;

    connect(client, SIGNAL(connected()), this, SLOT(onMQTT_Connected()));
    connect(client, SIGNAL(disconnected()), this, SLOT(onMQTT_Disconnected()));
    connect(client, SIGNAL(subscribed(const QString &)), this, SLOT(onMQTT_subscribed(const QString &)));
    connect(client, SIGNAL(published(const QMQTT::Message&)), this, SLOT(onMQTT_published(const QMQTT::Message&)));
    connect(client, SIGNAL(received(const QMQTT::Message&)), this, SLOT(onMQTT_received(const QMQTT::Message&)));
    connect(client, SIGNAL(error(QMQTT::ClientError)), this, SLOT(onMQTT_error(QMQTT::ClientError)));

    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
//    m_pTimer->start(10*1000);

    readSettings();
    this->uiChange(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//连接成功
void MainWindow::onMQTT_Connected()
{
    if(client->isConnectedToHost())
    {
        qDebug() << "Connect sussess!!" ;
    }
    else
    {
        qDebug() << "Connect fail!!" ;
    }
    this->uiChange(false);
}

//断开成功
void MainWindow::onMQTT_Disconnected()
{
    if(client->isConnectedToHost())
    {
        qDebug() << "Disconnect fail!!" ;
    }
    else
    {
        qDebug() << "Disconnect sussess!!" ;
    }
    m_pTimer->stop();
    ui->autoPubPushButton->setText("自动发布");
    this->uiChange(true);
}

void MainWindow::onMQTT_subscribed(const QString &topic)
{
     qDebug() << "subscribed success!!" <<topic;
     ui->displayTextEdit->setTextColor(QColor("seagreen"));
     ui->displayTextEdit->append(QString("时间[%1] << %2").arg(TIMEMS).arg("订阅成功"));
}

void MainWindow::onMQTT_published(const QMQTT::Message&msg)
{
     qDebug() << "published success:" <<msg.payload();
     ui->displayTextEdit->setTextColor(QColor("dodgerblue"));
     ui->displayTextEdit->append(QString("时间[%1] >> %2").arg(TIMEMS).arg("发布成功"));
}

void MainWindow::onMQTT_received(const QMQTT::Message&msg)
{
    QString strPayload(msg.payload());
    QString strQos = QString::number(msg.qos());
    QString strId = QString::number(msg.id());

//    str=msg.payload();
     qDebug() << "received: " <<msg.payload();
     ui->displayTextEdit->setTextColor(QColor("red"));
     ui->displayTextEdit->append(QString("时间[%1] Qos:%2 ID:%3 接收消息 >> %4").arg(TIMEMS).arg(strQos).arg(strId).arg(strPayload));
}

void MainWindow::onMQTT_error(const QMQTT::ClientError error)
{
    qDebug()<<"error:"<<error;
    ui->conPushButton->setText("连接");
//    ui->conPushButton->setEnabled(true);
    QMessageBox::warning(this,tr("Warning MQTT"),
                         tr("<h2>Warning</h2>"
                            "<p>Connect failed"));
}

//订阅
void MainWindow::on_subPushButton_clicked()
{
    if(client->isConnectedToHost())
    {
        client->subscribe(ui->subLineEdit->text(),ui->SubComboBox->currentText().toInt());
    }
    else
    {
        this->uiChange(true);
    }

}

//连接
void MainWindow::on_conPushButton_clicked()
{
    if(ui->conPushButton->text()=="连接")
    {
        if(client->isConnectedToHost())
        {
        }
        else
        {
            QHostAddress host(ui->domainLineEdit->text());
            if(!host.isNull())
            {
                qDebug() << host.isNull();
                client->setVersion(QMQTT::MQTTVersion::V3_1_1);
                client->setCleanSession(ui->CleanSessionCheckBox->isChecked());
                client->setHost(host);
                client->setPort(ui->PortLineEdit->text().toInt());
                client->setClientId(ui->clientIDLineEdit->text());
                client->setUsername(ui->userLineEdit->text());
                client->setPassword(ui->passwordLineEdit->text().toLatin1());
                client->setKeepAlive(ui->keepLiveLineEdit->text().toInt());
                ui->conPushButton->setText("连接中请等待...");
                client->connectToHost();
            }
            else
            {
                QHostInfo info = QHostInfo::fromName(ui->domainLineEdit->text());
                qDebug() << info.error();
                if(info.error()==QHostInfo::NoError)
                {
                    host.setAddress(QString(info.addresses().first().toString()));

                    client->setVersion(QMQTT::MQTTVersion::V3_1_1);
                    client->setCleanSession(ui->CleanSessionCheckBox->isChecked());
                    client->setHost(host);
                    client->setPort(ui->PortLineEdit->text().toInt());
                    client->setClientId(ui->clientIDLineEdit->text());
                    client->setUsername(ui->userLineEdit->text());
                    client->setPassword(ui->passwordLineEdit->text().toLatin1());
                    client->setKeepAlive(ui->keepLiveLineEdit->text().toInt());
                    client->connectToHost();
                }
           }
        }
    }
    else if(ui->conPushButton->text()=="断开")
    {
        if(client->isConnectedToHost())
        {
            client->disconnectFromHost();
        }
    }
}

//0-断开 1-连接
void MainWindow::uiChange(bool isEnabled)
{
    if(isEnabled)
    {
        ui->conPushButton->setText("连接");
        ui->domainLineEdit->setEnabled(true);
        ui->PortLineEdit->setEnabled(true);
        ui->userLineEdit->setEnabled(true);
        ui->passwordLineEdit->setEnabled(true);
        ui->clientIDLineEdit->setEnabled(true);
        ui->keepLiveLineEdit->setEnabled(true);
        ui->CleanSessionCheckBox->setEnabled(true);
        ui->SubComboBox->setEnabled(true);
        ui->subLineEdit->setEnabled(false);
        ui->pubLineEdit->setEnabled(false);
        ui->subPushButton->setEnabled(false);
        ui->sendPushButton->setEnabled(false);
        ui->autoPubPushButton->setEnabled(false);
        ui->intervalLineEdit->setEnabled(false);
    }
    else
    {
        ui->conPushButton->setText("断开");
        ui->domainLineEdit->setEnabled(false);
        ui->PortLineEdit->setEnabled(false);
        ui->userLineEdit->setEnabled(false);
        ui->passwordLineEdit->setEnabled(false);
        ui->clientIDLineEdit->setEnabled(false);
        ui->keepLiveLineEdit->setEnabled(false);
        ui->CleanSessionCheckBox->setEnabled(false);
        ui->SubComboBox->setEnabled(false);
        ui->subLineEdit->setEnabled(true);
        ui->pubLineEdit->setEnabled(true);
        ui->subPushButton->setEnabled(true);
        ui->sendPushButton->setEnabled(true);
        ui->autoPubPushButton->setEnabled(true);
        ui->intervalLineEdit->setEnabled(true);
    }
}

//发布消息
void MainWindow::on_sendPushButton_clicked()
{
    if(client->isConnectedToHost())
    {
        QMQTT::Message msg(2,ui->pubLineEdit->text(),ui->sendTextEdit->toPlainText().toLatin1(),ui->pubComboBox->currentText().toInt());
        msg.setId(20);
        client->publish(msg);
    }
}

//定时器
void MainWindow::handleTimeout(void)
{
    if(client->isConnectedToHost())
    {
        countTime++;
        if(countTime>=ui->intervalLineEdit->text().toInt())
        {
            countTime=0;

            QMQTT::Message msg(0,ui->pubLineEdit->text(),ui->sendTextEdit->toPlainText().toLatin1(),ui->pubComboBox->currentText().toInt());
            client->publish(msg);
        }
    }
}

void MainWindow::on_autoPubPushButton_clicked()
{
    if(ui->autoPubPushButton->text()=="自动发布")
    {
//        countTime=ui->intervalLineEdit->text().toInt();
//        m_pTimer->start(ui->intervalLineEdit->text().toInt()*1000);
        m_pTimer->start(1000);
        ui->autoPubPushButton->setText("取消自动");
    }
    else
    {
        m_pTimer->stop();
        ui->autoPubPushButton->setText("自动发布");
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::writeSettings()
{
    QSettings settings("Rocfly Inc.", "MQTT");

    settings.setValue("geometry", saveGeometry());
    settings.setValue("domain", ui->domainLineEdit->text());
    settings.setValue("port", ui->PortLineEdit->text());

    settings.setValue("cleanSession", ui->CleanSessionCheckBox->isChecked());
    settings.setValue("clientID", ui->clientIDLineEdit->text());
    settings.setValue("user", ui->userLineEdit->text());
    settings.setValue("password", ui->passwordLineEdit->text());
    settings.setValue("keepAlive",ui->keepLiveLineEdit->text());
    settings.setValue("Sub",ui->subLineEdit->text());
    settings.setValue("Pub",ui->pubLineEdit->text());

}

void MainWindow::readSettings()
{
    QSettings settings("Rocfly Inc.", "MQTT");

    restoreGeometry(settings.value("geometry").toByteArray());

    ui->domainLineEdit->setText(settings.value("domain").toString());
    ui->PortLineEdit->setText(settings.value("port").toString());
    ui->CleanSessionCheckBox->setChecked(settings.value("cleanSession",true).toBool());
    ui->clientIDLineEdit->setText(settings.value("clientID").toString());
    ui->userLineEdit->setText(settings.value("user").toString());
    ui->passwordLineEdit->setText(settings.value("password").toString());
    ui->keepLiveLineEdit->setText(settings.value("keepAlive").toString());
    ui->subLineEdit->setText(settings.value("Sub").toString());
    ui->pubLineEdit->setText(settings.value("Pub").toString());
}

