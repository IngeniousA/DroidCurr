#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void log(QString msg = "");
    QString getShortName(QString name = "");
    int getFileType(QString name = "");
    void switchUI(int mode = 0);
    void core(const char sr[], const char des[], const char k[], char mode);
    void unpack(char sr[], char des[], char size[], char k[], long skip);
    ~MainWindow();

private slots:
    void on_openFileBtn_clicked();

    void on_clrBtn_clicked();

    void on_infoBtn_clicked();

    void on_cancelBtn_clicked();

    void on_actionBtn_clicked();

private:
    Ui::MainWindow *ui;

public slots:
    void openFile(QString name);
};

#endif // MAINWINDOW_H
