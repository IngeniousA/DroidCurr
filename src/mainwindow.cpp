#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "androidfiledialog.h"
#include "blowfish.h"
#include "imagepickerandroid.h"
#include <QFileDialog>
#include <fstream>
#include <string>
#include <QtAndroidExtras/QtAndroid>
#include <QFile>
#include <vector>

using namespace std;
typedef vector<char> strV;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->actionLog->addItem("Welcome to ImCurr Android Edition 1.0!");
    switchUI(-1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

enum FileType
{
    Raw = 0,
    Encrypted = 1,
    Container = 2,
    Old = 3
};

string src, dst;
FileType ft;


void MainWindow::core(const char sr[], const char des[], const char k[], char mode)
{
    log("Preparing...");
    ifstream src;
    ofstream dst;
    strV key;
    key.resize(0);
    for (uint i = 0; i < strlen(k); i++)
        key.push_back(k[i]);
    strV srcV;
    Blowfish bf(key);
    src.open(sr, ios::binary);
    src.unsetf(std::ios::skipws);
    std::streampos fileSize;
    src.seekg(0, std::ios::end);
    fileSize = src.tellg();
    src.seekg(0, std::ios::beg);
        srcV.reserve(fileSize);
        srcV.insert(srcV.begin(),
            std::istream_iterator<char>(src),
            std::istream_iterator<char>());
        src.close();
        strV dstV;
        if (mode == '1' || mode == '2')
            dstV = bf.Encrypt(srcV);
        else if (mode == '0')
            dstV = bf.Decrypt(srcV);
        if (mode != '2')
            dst.open(des, ios::out | ios::binary);
        else
            dst.open(des, ios::app | ios::binary);
        log("Copying...");
        for (uint i = 0; i < dstV.size(); i++)
        {
            dst.put(dstV[i]);
        }
    dstV.resize(0);
    srcV.resize(0);
    key.resize(0);
    dst.close();
}

void MainWindow::unpack(char sr[], char des[], char size[], char k[], long skip)
{
    strV key;
    key.resize(0);
    for (uint i = 0; i < strlen(k); i++)
        key.push_back(k[i]);
    long sizel = atol(size);
    ifstream  src(sr, ios::binary);
    ofstream  dst(des, ios::binary | ios::trunc);
    src.seekg(-(sizel + skip), ios::end);
    vector<char> text;
    text.resize(sizel);
    for (int i = 0; i < sizel; i++)
        text[i] = src.get();
    Blowfish bf(key);
    text = bf.Decrypt(text);
    for (uint i = 0; i < text.size(); i++)
        dst.put(text[i]);
    dst.close();
    src.close();
}

void MainWindow::log(QString msg)
{
    ui->actionLog->addItem(msg);
}

QString MainWindow::getShortName(QString name)
{
    QString md = "", res = "";
    int i = name.size() - 1;
    while (name[i] != QChar('/') && i > 0)
    {
        md += name[i];
        i--;
    }
    for (int it = md.size() - 1; it >= 0; it--)
    {
        res.push_back(md[it]);
    }
    return res;
}

int MainWindow::getFileType(QString name)
{
    QString md = "", res = "";
    int i = name.size() - 1;
    while (i > 0 && name[i] != QChar('/') && name[i] != QChar('.'))
    {
        md += name[i];
        i--;
    }
    for (int it = md.size() - 1; it >= 0; it--)
    {
        res.push_back(md[it]);
    }
    if (res == "ic6")
    {
        return 1;
    }
    else if (res == "i6c")
    {
        return 2;
    }
    else if (res == "ict" || res == "icf")
    {
        return 3;
    }
    else
    {
        return 0;
    }
}

void MainWindow::switchUI(int mode)
{
    switch (mode)
    {
    case -1: //None
        ui->actionBtn->setEnabled(false);
        ui->actionBtn->setText("No file selected");
        ui->openFileBtn->setEnabled(true);
        ui->pwdEdit->setText("");
        ui->pwdEdit->setReadOnly(true);
        ui->cancelBtn->hide();
        ui->cancelBtn->setEnabled(false);
        ui->pwdEdit->setGeometry(10, 250, 671, 91);
        ui->extEdit->setEnabled(false);
        ui->extEdit->hide();
        ui->extEdit->setReadOnly(true);
        ui->label_2->hide();
        src = "";
        dst = "";
        log("Select file or folder to proceed.");
        break;
    case 0: //Single
        ui->actionBtn->setEnabled(true);
        ui->openFileBtn->setEnabled(false);
        ui->pwdEdit->setReadOnly(false);
        ui->actionBtn->setText("Encrypt");
        ui->cancelBtn->show();
        ui->cancelBtn->setEnabled(true);
        break;
    case 1: //Encrypted
        ui->actionBtn->setEnabled(true);
        ui->openFileBtn->setEnabled(false);
        ui->pwdEdit->setReadOnly(false);
        ui->pwdEdit->setGeometry(10, 250, 371, 91);
        ui->extEdit->setEnabled(true);
        ui->extEdit->show();
        ui->extEdit->setReadOnly(false);
        ui->label_2->show();
        ui->actionBtn->setText("Decrypt");
        ui->cancelBtn->show();
        ui->cancelBtn->setEnabled(true);
        break;
    case 2: //Container
        ui->actionBtn->setEnabled(true);
        ui->openFileBtn->setEnabled(false);
        ui->pwdEdit->setReadOnly(false);
        ui->actionBtn->setText("Unpack");
        ui->cancelBtn->show();
        ui->cancelBtn->setEnabled(true);
        break;
    case 3: //Old
        ui->actionBtn->setEnabled(false);
        ui->actionBtn->setText("No file selected");
        ui->openFileBtn->setEnabled(true);
        ui->pwdEdit->setText("");
        ui->pwdEdit->setReadOnly(true);
        ui->cancelBtn->hide();
        ui->cancelBtn->setEnabled(false);
        log("Select file or folder to proceed.");
        src = "";
        dst = "";
        break;
    default:
        break;
    }
}

void MainWindow::on_openFileBtn_clicked()
{
    AndroidFileDialog *fileDialog = new AndroidFileDialog();
    connect(fileDialog, SIGNAL(existingFileNameReady(QString)), this, SLOT(openFile(QString)));
    fileDialog->provideExistingFileName();
}

void MainWindow::openFile(QString name)
{
    qDebug() << "NICE";
    if (!name.isNull() && name != "-1")
    {
        QFile *f = new QFile(name);
        src = name.toStdString();
        dst = src;
        log("File selected: " + getShortName(name));
        log("File size: " + QString::number(f->size()) + " bytes");
        switch(getFileType(name))
        {
        case 1:
            ft = Encrypted;
            break;
        case 2:
            ft = Container;
            break;
        case 3:
            ft = Old;
            break;
        case 0:
            ft = Raw;
            break;
        }
        switch (ft)
        {
        case Encrypted:
            log("Enter password to decrypt file");
            switchUI(1);
            break;
        case Old:
            log("Cannot work with deprecated files,");
            log("please select another one");
            switchUI(3);
            break;
        case Container:
            log("Cannot work with containers");
            switchUI(-1);
            //log("Enter password to unpack container");
            //switchUI(2);
            break;
        case Raw:
            log("Enter password to encrypt file");
            dst += ".ic6";
            switchUI(0);
            break;
        default:
            break;
        }
    }
    else if (name != "-1")
    {
        log("Action cancelled.");
    }
    else
    {
        log("Could not select file.");
        log("Please use external app from left menu while choosing file.");
    }
}

void MainWindow::on_clrBtn_clicked()
{
    ui->actionLog->clear();
}

void MainWindow::on_infoBtn_clicked()
{
    log("--------INFO--------");
    log("ImCurr Android Edition");
    log("ver. 1.0 BETA build 2");
    log("Made by Ingenious");
    log("github.com/IngeniousA");
    log("--------------------");
}

void MainWindow::on_cancelBtn_clicked()
{
    switchUI(-1);
}

void MainWindow::on_actionBtn_clicked()
{
    switch (ft) {
    case Raw:
        core(src.c_str(), dst.c_str(), ui->pwdEdit->text().toStdString().c_str(), '1');
        break;
    case Encrypted:
        dst += ui->extEdit->text().toStdString();
        core(src.c_str(), dst.c_str(), ui->pwdEdit->text().toStdString().c_str(), '0');
        break;
    default:
        break;
    }
    log("Done!");
    log("Output file placed into source file\'s folder.");
    switchUI(-1);
}
