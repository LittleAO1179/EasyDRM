#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "model.h"


#include <qfiledialog.h>
#include <qlineedit.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("EasyDRM By LiMingyang");
    SetSelectFile(ui->pushButton);
    SetSaveFile(ui->pushButton_5, ui->lineEdit);
}

MainWindow::~MainWindow()
{
    delete ui;
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
