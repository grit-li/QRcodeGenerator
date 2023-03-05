#include <QDebug>
#include <QFileDialog>
#include <QMap>
#include <QBuffer>
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
    QPixmap pixmap = QPixmap();
    if(text.isEmpty()) {
        return QPixmap();
    }
    qrcode = QRcode_encodeString(text.toStdString().c_str(), version, level, QR_MODE_8, 0);
    if(qrcode) {
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
        pixmap = QPixmap::fromImage(image);
    }
    return pixmap;
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

static inline QString vCardFormat(const QMap<QString, QString>& map)
{
    QString text = QString();
    text += "BEGIN:VCARD\n";
    text += "VERSION:3.0\n";
    text += QString("N:%1\n").arg(map["Name"]);
    text += QString("TEL;CELL;VOICE:%1\n").arg(map["Phone"]);
    if(!map["Email"].isEmpty()) {
        text += QString("EMAIL;PREF;INTERNET:%1\n").arg(map["Email"]);
    }
    if(!map["ORG"].isEmpty()) {
        text += QString("ORG:%1\n").arg(map["ORG"]);
    }
    if(!map["ADR"].isEmpty()) {
        text += QString("ADR:%1\n").arg(map["ADR"]);
    }
    if(!map["BDAY"].isEmpty()) {
        text += QString("BDAY:%1\n").arg(map["BDAY"]);
    }
    if(!map["NOTE"].isEmpty()) {
        text += QString("NOTE:%1\n").arg(map["NOTE"]);
    }
    if(!map["TITLE"].isEmpty()) {
        text += QString("TITLE:%1\n").arg(map["TITLE"]);
    }
    if(!map["ROLE"].isEmpty()) {
        text += QString("ROLE:%1\n").arg(map["ROLE"]);
    }
    if(!map["PHOTO"].isEmpty()) {
        QImage image(map["PHOTO"]);
        QByteArray ba;
        QBuffer buf(&ba);
        image.save(&buf, "png");
        text += QString("PHOTO;ENCODING=b;TYPE=PNG:%1\n").arg(ba.toBase64(QByteArray::Base64Option::Base64UrlEncoding | QByteArray::Base64Option::OmitTrailingEquals).toStdString().c_str());
        buf.close();
    }
    text += "END:VCARD";

#ifndef QT_NO_DEBUG
    qDebug() << text;
#endif
    return text;
}

QPixmap MainWindow::encodeQRCode(void)
{
    int version = ui->spinBox_qrcode_version->value();
    QRecLevel level = static_cast<QRecLevel>(ui->comboBox_qrcode_error_level->currentIndex());
    QSize size = ui->comboBox_qrcode_size->currentData().toSize();
    QString text = QString();
    switch(ui->tabWidget_input->currentIndex()) {
        case 0:
            text = ui->lineEdit_text->text();
            break;
        case 1:
            if(!ui->lineEdit_vcard_name->text().isEmpty() && !ui->lineEdit_vcard_phone->text().isEmpty()) {
                QMap<QString, QString> map;
                map.insert("Name", ui->lineEdit_vcard_name->text());
                map.insert("Phone", ui->lineEdit_vcard_phone->text());
                map.insert("Email", ui->lineEdit_vcard_email->text());
                map.insert("ORG", ui->lineEdit_vcard_organization->text());
                map.insert("ADR", ui->lineEdit_vcard_home_address->text());
                map.insert("BDAY", ui->lineEdit_vcard_birthday->text());
                map.insert("NOTE", ui->lineEdit_vcard_note->text());
                map.insert("TITLE", ui->lineEdit_vcard_job_title->text());
                map.insert("ROLE", ui->lineEdit_vcard_occupation->text());
                map.insert("PHOTO", ui->pushButton_vcard_photo->property("photo name").toString());
                text = vCardFormat(map);
            }
            break;
    }
    return encodeQRCodeToPixmap(text, version, level, size);
}

void MainWindow::on_pushButton_generate_clicked(void)
{
    ui->label_qrcode->setPixmap(encodeQRCode());
}

void MainWindow::on_pushButton_export_clicked(void)
{
    switch(ui->tabWidget_input->currentIndex()) {
        case 0:
            if(ui->lineEdit_text->text().isEmpty()) {
                return;
            }
            break;
        case 1:
            if(ui->lineEdit_vcard_name->text().isEmpty() || ui->lineEdit_vcard_phone->text().isEmpty()) {
                return;
            }
            break;
    }

    QString fileName = QFileDialog::getSaveFileName(this, QObject::tr("save file"), QApplication::applicationDirPath(), QObject::tr("PNG File(*.PNG)"));
    if(!fileName.isEmpty()) {
        QPixmap pixmap = encodeQRCode();
        pixmap.toImage().save(fileName, "PNG");
    }
}

void MainWindow::on_pushButton_vcard_photo_clicked(void)
{
    QString fileName = QFileDialog::getOpenFileName(this, QObject::tr("open file"), QApplication::applicationDirPath(), QObject::tr("PNG File(*.PNG)"));
    if(!fileName.isEmpty()) {
        ui->pushButton_vcard_photo->setStyleSheet(QString("border-image: url(%1)").arg(fileName));
        ui->pushButton_vcard_photo->setProperty("photo name", fileName);
    }
}
