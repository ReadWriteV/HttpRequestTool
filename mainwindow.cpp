#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtWidgets/QMessageBox>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager(this);
    completer = new QCompleter();
    listModel = new QStringListModel();

    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(sendBtnClicked()));
    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(AddComplete()));

    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    completer->setMaxVisibleItems(5);
    completer->setModel(listModel);
    ui->lineEdit->setCompleter(completer);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete manager;
    delete completer;
}

void MainWindow::sendBtnClicked()
{
    QNetworkRequest request;
    QNetworkReply *reply;
    QString urlString;
    QUrl url;
    QByteArray postData;
    QEventLoop eventLoop;
    QTimer timer;
    QVariant statusCode;

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
    request.setUrl(url);

    if (!ui->textEdit->toPlainText().isEmpty())
    {
        postData.append(ui->textEdit->toPlainText());
    }

    switch (ui->comboBox->currentIndex())
    {
    case 0: // GET
        reply = manager->get(request);
        break;
    case 1: // POST
        request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
        reply = manager->post(request, postData);
        break;
    case 2: // PUT
        reply = manager->put(request, postData);
        break;
    case 3: // HEAD
        reply = manager->head(request);
        break;
    case 4: // DELETE
        reply = manager->deleteResource(request);
        break;
    default:
        reply = nullptr;
        break;
    }
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    timer.start(10 * 1000);
    eventLoop.exec();
    if (timer.isActive())
    {
        timer.stop();
    }
    else
    {
        disconnect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        if (reply != nullptr)
        {
            reply->abort();
            reply->deleteLater();
        }
        return;
    }
    statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (statusCode.toInt() / 100 == 2)
    {
        ui->label_2->setStyleSheet("color: green");
    }
    else if (statusCode.toInt() / 100 == 3)
    {
        ui->label_2->setStyleSheet("color: blue");
    }
    else
    {
        ui->label_2->setStyleSheet("color: red");
    }
    ui->label_2->setText(statusCode.toString());
    QVariant variant = reply->header(QNetworkRequest::ContentTypeHeader);
    if (variant.isValid())
    {
        if (variant.toString().contains("text/html", Qt::CaseInsensitive))
        {
            QString res = reply->readAll();
            ui->textBrowser->setText(res);
        }
        else if (variant.toString().contains("application/json", Qt::CaseInsensitive))
        {
            QByteArray bytes = reply->readAll();
            QJsonParseError jsonError;
            QJsonDocument document = QJsonDocument::fromJson(bytes, &jsonError);
            if (jsonError.error != QJsonParseError::NoError)
            {
                ui->textBrowser->setText("Error formatting json");
                return;
            }
            ui->textBrowser->setText(document.toJson(QJsonDocument::Indented));
        }
        else
        {
            ui->textBrowser->setText("Error");
        }
    }
}

void MainWindow::AddComplete()
{
    QString text = ui->lineEdit->text();
    if (QString::compare(text, QString("")) != 0)
    {
        bool is_contains = historyList.contains(text, Qt::CaseInsensitive);
        if (!is_contains)
        {
            historyList << text;
            listModel->setStringList(historyList);
        }
    }
}