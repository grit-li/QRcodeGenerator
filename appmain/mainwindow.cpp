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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QRcode* qrcode;
    qrcode= QRcode_encodeString("sissi",2, QR_ECLEVEL_L, QR_MODE_8,0);
    printf("====version=%d========\r\n",qrcode->version);
    QImage image(qrcode->width, qrcode->width, QImage::Format::Format_RGB888);

    for(int y = 0; y < qrcode->width; y++) {
        for(int x = 0; x < qrcode->width; x++) {
            if(qrcode->data[y*qrcode->width+x]&0x01) {
                printf("#");
                image.setPixel(x, y, qRgb(0,0,0));
            } else {
                printf(" ");
                image.setPixel(x, y, qRgb(255,255,255));
            }
        }
        printf("\r\n");
    }
    printf("\r\n");
    image.save("qrcode.png", "PNG");
    QRcode_free(qrcode);
}

MainWindow::~MainWindow()
{
    delete ui;
}

