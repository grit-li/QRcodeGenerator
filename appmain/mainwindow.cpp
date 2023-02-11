#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#include "qrencode.h"
#include "qrspec.h"
#include "mqrspec.h"
#include "bitstream.h"
#include "qrinput.h"
#include "rsecc.h"
#include "split.h"
#include "mask.h"
#include "mmask.h"

static inline QPixmap encodeQRCode(const QString& text)
{
    QRcode* qrcode;
    qrcode = QRcode_encodeString(text.toStdString().c_str(), 1, QR_ECLEVEL_H, QR_MODE_8, 0);
    QImage image(qrcode->width, qrcode->width, QImage::Format::Format_RGB888);
    for(int y = 0; y < qrcode->width; y++) {
        for(int x = 0; x < qrcode->width; x++) {
            if(qrcode->data[y * qrcode->width + x] & 0x01) {
                image.setPixel(x, y, qRgb(0,0,0));
            } else {
                image.setPixel(x, y, qRgb(255,255,255));
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
}

MainWindow::~MainWindow(void)
{
    delete ui;
}

void MainWindow::on_pushButton_generate_clicked(void)
{
    QString text = ui->lineEdit_text->text();

    if(!text.isEmpty()) {
        ui->label_qrcode->setPixmap(encodeQRCode(text));
    }
}
