#include <QDebug>
#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qrencode.h"
#include "qrspec.h"
#include "mqrspec.h"
#include "bitstream.h"
#include "qrinput.h"
#include "rsecc.h"
#include "split.h"
#include "mask.h"
#include "mmask.h"

static void scaleQRCodeImage(QImage* image, int x, int y, unsigned int scale, uint color)
{
    for(unsigned int h = 0; h < scale; h++) {
        for(unsigned int w = 0; w < scale; w++) {
            image->setPixel(w + x * scale, h + y * scale, color);
        }
    }
}
static inline QPixmap encodeQRCodeToPixmap(const QString& text, int version, QRecLevel level, const QSize& size)
{
    QRcode* qrcode;
    qrcode = QRcode_encodeString(text.toStdString().c_str(), version, level, QR_MODE_8, 0);
    unsigned int scale = size.width() / qrcode->width;
    unsigned int imageWidth = scale * qrcode->width;
    QImage image(imageWidth, imageWidth, QImage::Format::Format_RGB888);
    for(int y = 0; y < qrcode->width; y++) {
        for(int x = 0; x < qrcode->width; x++) {
            if(qrcode->data[y * qrcode->width + x] & 0x01) {
                scaleQRCodeImage(&image, x, y, scale, qRgb(0, 0, 0));
            } else {
                scaleQRCodeImage(&image, x, y, scale, qRgb(255, 255, 255));
            }
        }
    }

    QRcode_free(qrcode);
    return QPixmap::fromImage(image);
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->comboBox_qrcode_size->setItemData(0, QSize(300, 300));
    ui->comboBox_qrcode_size->setItemData(1, QSize(400, 400));
    ui->comboBox_qrcode_size->setItemData(2, QSize(500, 500));
    ui->comboBox_qrcode_size->setItemData(3, QSize(1000, 1000));
}

MainWindow::~MainWindow(void)
{
    delete ui;
}

QPixmap MainWindow::encodeQRCode(void)
{
    QString text = ui->lineEdit_text->text();
    int version = ui->spinBox_qrcode_version->value();
    QRecLevel level = static_cast<QRecLevel>(ui->comboBox_qrcode_error_level->currentIndex());
    QSize size = ui->comboBox_qrcode_size->currentData().toSize();
    if(text.isEmpty()) {
        return QPixmap();
    }
    return encodeQRCodeToPixmap(text, version, level, size);
}

void MainWindow::on_pushButton_generate_clicked(void)
{
    ui->label_qrcode->setPixmap(encodeQRCode());
}

void MainWindow::on_pushButton_export_clicked(void)
{
    if(ui->lineEdit_text->text().isEmpty()) {
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, QObject::tr("save file"), QApplication::applicationDirPath(), QObject::tr("PNG File(*.PNG)"));
    if(!fileName.isEmpty()) {
        QPixmap pixmap = encodeQRCode();
        pixmap.toImage().save(fileName, "PNG");
    }
}
