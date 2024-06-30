#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "model.h"
#include "encrypt_func.h"


#include <qclipboard.h>
#include <qfiledialog.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <QFileDialog>
#include <qmessagebox>
#include <QClipboard>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("EasyDRM By LiMingyang");
    SetSelectFile(ui->pushButton);
    SetSaveFile(ui->pushButton_5, ui->lineEdit);
    SetGenerateKey(ui->pushButton_4, ui->lineEdit_2);
    SetClipBroad(ui->pushButton_3, ui->lineEdit_2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetClipBroad(QPushButton*& button, QLineEdit*& lineEdit)
{
    connect(button, &QPushButton::clicked, [this, lineEdit]()
    {
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(lineEdit->text());
        QMessageBox::information(this, "复制成功", "密钥已经复制到剪贴板，请妥善保存！");
    });
}

void MainWindow::SetGenerateKey(QPushButton*& button, QLineEdit*& lineEdit)
{
    connect(button, &QPushButton::clicked, [this, lineEdit]()
    {
        Encrypt::GenerateAESKey(42);
        auto& key = Model::getInstance().GetKey();
        auto& keySize = Model::getInstance().GetKeySize();
        QByteArray byteArray(reinterpret_cast<char*>(key.get()), keySize);
        QString hexString = byteArray.toHex();
        lineEdit->setText(hexString);
    });
}

void MainWindow::SetSelectFile(QPushButton*& button)
{
    connect(button, &QPushButton::clicked, [this, button]()
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "/path/to/your/default/folder", tr("All Files (*.*)"));
        if (!fileName.isEmpty()) 
        {
            Model::getInstance().SetChoosePath(fileName);
            button->setText(fileName);
        }
    });
}

void MainWindow::SetSaveFile(QPushButton*& button, QLineEdit*& lineEdit)
{
    
    connect(button, &QPushButton::clicked, [this, lineEdit]()
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("*.easyDRM"));
        if (!fileName.isEmpty())
        {
            Model::getInstance().SetSavePath(fileName);
            lineEdit->setText(fileName);
        }
    });
}
