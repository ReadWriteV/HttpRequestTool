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
    delete manager;
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
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    timer.start(30 * 1000);
    eventLoop.exec();
    if (timer.isActive())
    {
        timer.stop();
    }
    else
    {
        disconnect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        reply->abort();
        reply->deleteLater();
        return -1;
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
    httpReply reply;
    QString urlString;
    QUrl url;

    urlString = ui->lineEdit->text();
    if (urlString.isEmpty())
    {
        QMessageBox::information(this, tr("Error"), tr("Please input valid URL!"));
        return;
    }
    if (ui->comboBox_2->currentIndex() == 0)
    {
        url.setUrl("http://" + urlString);
    }
    request.url = url;
    userAgent.key.append("User-Agent");
    userAgent.value.append("HttpRequestTool");
    request.header.append(userAgent);

    if (!ui->textEdit->toPlainText().isEmpty())
    {
        request.postData.append(ui->textEdit->toPlainText());
    }

    switch (ui->comboBox->currentIndex())
    {
    case 0:
        request.method = GET;
        break;
    case 1:
        request.method = POST;
        break;
    case 2:
        request.method = PUT;
        break;
    case 3:
        request.method = HEAD;
        break;
    case 4:
        request.method = DELETE;
        break;
    default:
        break;
    }
    sendRequest(&request, &reply);
    QString res = reply.response;
    ui->textBrowser->setText(res);
}