#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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

    void SetSelectFile(QPushButton*& button);
    void SetSaveFile(QPushButton*& button, QLineEdit*& lineEdit);

private:
    Ui::MainWindow *ui;

    QString mChooseFilePath;
    QString mSaveFilePath;
};
#endif // MAINWINDOW_H
