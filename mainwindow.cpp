#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtWidgets/QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    manager = new QNetworkAccessManager(this);
    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(sendBtnClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::sendRequest(httpRequest *httpRequest, httpReply *httpReply)
{
    QNetworkRequest request;
    QNetworkReply *reply;
    QVariant statusCode;
    QEventLoop eventLoop;
    QTimer timer;

    request.setUrl(httpRequest->url);
    for (int i = 0; i < httpRequest->header.size(); i++)
    {
        request.setRawHeader(httpRequest->header.at(i).key, httpRequest->header.at(i).value);
    }
    switch (httpRequest->method)
    {
    case GET:
        reply = manager->get(request);
        break;
    case POST:
        reply = manager->post(request, httpRequest->postData);
        break;
    case PUT:
        reply = manager->put(request, httpRequest->postData);
        break;
    case HEAD:
        reply = manager->head(request);
        break;
    case DELETE:
        reply = manager->deleteResource(request);
        break;
    default:
        return -1;
        break;
    }
    httpReply->response = reply->readAll();
    statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (statusCode.isValid())
    {
        httpReply->code = statusCode.toInt();
    }
    QList<QByteArray> list = reply->rawHeaderList();
    for (int i = 0; i < list.size(); i++)
    {
        httpHeader header;
        header.key = list.at(i);
        header.value = reply->rawHeader(list.at(i));
        httpReply->header.append(header);
    }
    httpReply->code = statusCode.toInt();
    if (reply != nullptr)
    {
        disconnect(reply, SIGNAL(readyRead()), &eventLoop, SLOT(quit()));
        reply->abort();
        reply->deleteLater();
    }
    return httpReply->code;
}

void MainWindow::sendBtnClicked()
{
    httpRequest request;
    httpHeader userAgent;
    httpHeader auth;
    httpReply reply;
    QString urlString;
    QString authString;
    QByteArray basic;
    QUrl url;
    int retCode = 0;

    urlString = ui->lineEdit->text();
    if (urlString.isEmpty())
    {
        QMessageBox::information(this, tr("Error"), tr("Please input valid URL!"));
        return;
    }
    url.setUrl(urlString);
    request.url = url;
    userAgent.key.append("User-Agent");
    userAgent.value.append("qhttpc 1.0");
    request.header.append(userAgent);

    switch (ui->comboBox->currentIndex())
    {
    case 0:
        request.method = GET;
        break;
    case 1:
        request.method = POST;
        break;
    default:
        break;
    }
    retCode = sendRequest(&request, &reply);
    QString res = reply.response;
    ui->textEdit->setText("dfsafas");
}