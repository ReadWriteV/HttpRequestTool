#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtWidgets/QCompleter>
#include <QtCore/QStringListModel>

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
    QCompleter *completer;
    QStringList historyList;
    QStringListModel *listModel;

private slots:
    void sendBtnClicked();
    void AddComplete();
};

#endif // MAINWINDOW_H