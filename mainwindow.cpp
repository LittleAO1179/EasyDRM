#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "model.h"
#include "encrypt_func.h"


#include <qclipboard.h>
#include <qcombobox.h>
#include <qcontainerfwd.h>
#include <qfiledialog.h>
#include <qlabel.h>
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
    SetGenerateKey(ui->pushButton_4, ui->lineEdit_2, ui->comboBox);
    SetClipBroad(ui->pushButton_3, ui->lineEdit_2);

    OnComboBoxValueChanged(ui->comboBox, ui->lineEdit_2, ui->label_7);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnComboBoxValueChanged(QComboBox*& comboBox, QLineEdit*& lineEdit, QLabel*& label)
{
    connect(comboBox, &QComboBox::currentIndexChanged, [this, lineEdit, comboBox, label](){
        // 首先清空
        lineEdit->show();
        lineEdit->setText("");
        label->setText("生成的密钥");
        // RSA
        if (comboBox->currentIndex() == 2)
        {
            lineEdit->hide();
            label->setText("点击生成密钥自动保存公钥和私钥");
        }
    });
}

void MainWindow::SetClipBroad(QPushButton*& button, QLineEdit*& lineEdit)
{
    connect(button, &QPushButton::clicked, [this, lineEdit]()
    {
        if (lineEdit->text().isEmpty()) return;
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(lineEdit->text());
        QMessageBox::information(this, "复制成功", "密钥已经复制到剪贴板，请妥善保存！");
    });
}

void MainWindow::SetGenerateKey(QPushButton*& button, QLineEdit*& lineEdit, QComboBox*& comboBox)
{
    connect(button, &QPushButton::clicked, [this, lineEdit, comboBox]()
    {
        // Encrypt::GenerateAESKey(32);
        switch (comboBox->currentIndex())
        {
        case 0:
            // AES
            Encrypt::GenerateAESKey(32);
            break;
        case 1:
            // DES
            Encrypt::GenerateDESKey();
            break;
        case 2:
            {
                // RSA
                QString privateKeyPath, publicKeyPath;
                privateKeyPath = QFileDialog::getSaveFileName(this, tr("将私钥保存到"), "private_key.pem",tr(".pem"));
                publicKeyPath = QFileDialog::getSaveFileName(this, tr("将公钥保存到"), "public_key.pem",tr(".pem"));
                if (!Encrypt::GenerateRSAKey(privateKeyPath, publicKeyPath))
                {
                    QMessageBox::warning(this, "生成错误", "请确保路径不含中文且本机装有OpenSSL");
                    return;
                }
                QMessageBox::information(this, "生成成功", "公钥和私钥已经成功保存！");
                break;
            }
        default:
            break;
        }
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
        QString fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), "", tr("All Files (*.*)"));
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
