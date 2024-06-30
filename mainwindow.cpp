#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "model.h"
#include "encrypt_func.h"
#include "decrypt_func.h"


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
#include <qtabwidget.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("EasyDRM By LiMingyang");
    // 加密
    SetSelectFile(ui->pushButton, "ALL *.*");
    SetSaveFile(ui->pushButton_5, ui->lineEdit, "*.easyDRM");
    SetGenerateKey(ui->pushButton_4, ui->lineEdit_2, ui->comboBox);
    SetClipBroad(ui->pushButton_3, ui->lineEdit_2);
    SetEncrypt(ui->pushButton_2);
    SetDecrypt(ui->pushButton_9);
    SetKeyWriteToFile(ui->pushButton_6);

    // 解密
    SetSelectFile(ui->pushButton_7, "*.easyDRM*");
    SetSaveFile(ui->pushButton_8, ui->lineEdit_3, Model::getInstance().GetFileExtension());

    OnComboBoxValueChanged(ui->comboBox, ui->lineEdit_2, ui->label_7);
    OnTabValueChanged(ui->tabWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnTabValueChanged(QTabWidget*& tabWidget)
{
    connect(tabWidget, &QTabWidget::currentChanged, [this](){
        Model::getInstance().clear();
    });
}


void MainWindow::OnComboBoxValueChanged(QComboBox*& comboBox, QLineEdit*& lineEdit, QLabel*& label)
{
    connect(comboBox, &QComboBox::currentIndexChanged, [this, lineEdit, comboBox, label](){
        // 首先清空
        lineEdit->show();
        lineEdit->setReadOnly(true);
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

void MainWindow::SetKeyWriteToFile(QPushButton*& button)
{
    connect(button, &QPushButton::clicked, [this]()
    {
        QString filePath = QFileDialog::getSaveFileName(this, "保存到", "key.txt", "*.txt");
        QByteArray byte = reinterpret_cast<char*>(Model::getInstance().GetKey().get());
        if (!filePath.isEmpty() && Encrypt::WriteKeyToTxtFile(byte.toHex(), filePath))
        {
            QMessageBox::information(this, "保存成功", "密钥成功保存至" + filePath);
        }
        else 
        {
            QMessageBox::warning(this, "保存失败", "请确保正确生成密钥后保存。");
        }
    });
}

void MainWindow::SetDecrypt(QPushButton*& button)
{
    connect(button, &QPushButton::clicked, [this]()
    {
        if (ui->lineEdit_4->text().isEmpty())
        {
            QMessageBox::information(this, "注意", "密钥不能为空，请填写密钥！");
            return;
        }

        if (Model::getInstance().GetSavePath().isEmpty() || Model::getInstance().GetChoosePath().isEmpty())
        {
            QMessageBox::information(this, "注意", "输入和输出路径不能为空！");
            return;
        }

        QString hexText = ui->lineEdit_4->text();
        QByteArray qKey = QByteArray::fromHex(hexText.toUtf8());
        Model::getInstance().SetKey(reinterpret_cast<unsigned char*>(qKey.data()), qKey.size());

        bool flag = true;
        switch (ui->comboBox->currentIndex()) 
        {
            case 0:
            // AES
            {
                flag = Decrypt::DecryptByAESKey(Model::getInstance().GetChoosePath(), Model::getInstance().GetSavePath(), Model::getInstance().GetKey().get());
                break;
            }
            // case 1:
            // {
            //     flag = Encrypt::EncryptByDESKey(Model::getInstance().GetChoosePath(), Model::getInstance().GetSavePath(), Model::getInstance().GetKey().get());
            //     break;
            // }
        }
        if (flag)
        {
            QMessageBox::information(this, "解密成功","文件解密成功，解密文件存储在" + Model::getInstance().GetSavePath());
        }
        else
        {
            QMessageBox::warning(this, "解密失败", "解密失败，请重新尝试。");
        }
    });
}

void MainWindow::SetEncrypt(QPushButton*& button)
{
    connect(button, &QPushButton::clicked, [this]()
    {
        if (ui->lineEdit_2->text().isEmpty())
        {
            QMessageBox::information(this, "注意", "密钥不能为空，请点击生成密钥！");
            return;
        }

        if (Model::getInstance().GetSavePath().isEmpty() || Model::getInstance().GetChoosePath().isEmpty())
        {
            QMessageBox::information(this, "注意", "输入和输出路径不能为空！");
            return;
        }

        bool flag = true;
        switch (ui->comboBox->currentIndex()) 
        {
            case 0:
            // AES
            {
                flag = Encrypt::EncryptByAESKey(Model::getInstance().GetChoosePath(), Model::getInstance().GetSavePath(), Model::getInstance().GetKey().get());
                break;
            }
            case 1:
            {
                flag = Encrypt::EncryptByDESKey(Model::getInstance().GetChoosePath(), Model::getInstance().GetSavePath(), Model::getInstance().GetKey().get());
                break;
            }
        }
        if (flag)
        {
            QMessageBox::information(this, "加密成功","文件加密成功，加密文件存储在" + Model::getInstance().GetSavePath());
        }
        else
        {
            QMessageBox::warning(this, "加密失败", "加密失败，请重新尝试。");
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

void MainWindow::SetSelectFile(QPushButton*& button, QString extension)
{
    connect(button, &QPushButton::clicked, [this, button, extension]()
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), "", extension);
        if (!fileName.isEmpty()) 
        {
            Model::getInstance().SetChoosePath(fileName);
            button->setText(fileName);
        }
    });
}

void MainWindow::SetSaveFile(QPushButton*& button, QLineEdit*& lineEdit, QString extension)
{

    connect(button, &QPushButton::clicked, [this, lineEdit, extension]()
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", extension);
        if (!fileName.isEmpty())
        {
            Model::getInstance().SetSavePath(fileName);
            lineEdit->setText(fileName);
        }
    });
}
