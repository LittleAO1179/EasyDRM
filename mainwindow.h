#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qstyle.h>
#include <qtabwidget.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void OnComboBoxValueChanged(QComboBox*& comboBox, QLineEdit*& lineEdit, QLabel*& label);
    void OnTabValueChanged(QTabWidget*& tabWidget);

    void SetSelectFile(QPushButton*& button, QString extension);
    void SetSaveFile(QPushButton*& button, QLineEdit*& lineEdit, bool isDecrypt);
    void SetSaveFile(QPushButton*& button, QLineEdit*& lineEdit);
    void SetGenerateKey(QPushButton*& button, QLineEdit*& lineEdit, QComboBox*& comboBox);
    void SetClipBroad(QPushButton*& button, QLineEdit*& lineEdit);
    void SetEncrypt(QPushButton*& button);
    void SetDecrypt(QPushButton*& button);
    void SetKeyWriteToFile(QPushButton*& button);

    void CreateLicence();

private:
    Ui::MainWindow *ui;

    QString mChooseFilePath;
    QString mSaveFilePath;
};
#endif // MAINWINDOW_H
