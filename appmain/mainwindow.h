#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow(void);

protected:
    QPixmap encodeQRCode(void);

private slots:
    void on_pushButton_generate_clicked(void);

    void on_pushButton_export_clicked(void);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
