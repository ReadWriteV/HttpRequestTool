#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

struct httpHeader
{
    QByteArray key;
    QByteArray value;
};

enum httpMethod
{
    GET = 0,
    POST = 1,
    PUT = 2,
    HEAD = 3,
    DELETE = 4,
};

struct httpRequest
{
    httpMethod method;
    QUrl url;
    QList<httpHeader> header;
    QByteArray postData;
};

struct httpReply
{
    int code;
    QList<httpHeader> header;
    QByteArray response;
};

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    int sendRequest(httpRequest *httpRequest, httpReply *httpReply);

private slots:
    void sendBtnClicked();
};

#endif // MAINWINDOW_H