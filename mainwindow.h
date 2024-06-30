#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qobject.h>
#include <qpushbutton.h>

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

    void SetSelectFile(QPushButton*& button);
    void SetSaveFile(QPushButton*& button, QLineEdit*& lineEdit);
    void SetGenerateKey(QPushButton*& button, QLineEdit*& lineEdit, QComboBox*& comboBox);
    void SetClipBroad(QPushButton*& button, QLineEdit*& lineEdit);
    void SetEncrypt(QPushButton*& button);
    void SetKeyWriteToFile(QPushButton*& button);

private:
    Ui::MainWindow *ui;

    QString mChooseFilePath;
    QString mSaveFilePath;
};
#endif // MAINWINDOW_H
