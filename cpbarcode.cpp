#include "cpbarcode.h"


#include "TextUtfEncoding.h"
#include "GTIN.h"
#include <QDebug>

using namespace TextUtfEncoding;

CPBarcode::CPBarcode() :m_stop(false), decoding(false), decodeResult(DecodeStatus::NotFound) {

}


CPBarcode::~CPBarcode() {
    qDebug()<<"~CPBarcode()";
    requestInterruption();
    while(decoding) {
        qDebug()<<"wait decoding finish...";
        msleep(50);
    }
    m_stop = true;
    //exit(0);
    quit();
    wait();
    qDebug()<<"~CPBarcode() done";
}


void CPBarcode::run() {
    qDebug()<<"From worker thread: "<<currentThreadId();
    while (1) {
        {
        QMutexLocker locker(&m_mutex);
        if (m_stop)
            break;
        }
        if (decoding) {
            decode(image);
            decoding = false;
        }
        //msleep(500);
    }
    qDebug()<<"From worker thread ended: "<<currentThreadId();
}


void CPBarcode::setImage(const QImage &image) {
    this->image = image;
    this->decoding = true;
}

void CPBarcode::stop() {
    qDebug()<<"Thread::stop called from main thread: "<<currentThreadId();
    QMutexLocker locker(&m_mutex);
    m_stop=true;
}

ImageFormat CPBarcode::getImageFormat(const QImage& img) {
    switch (img.format()) {
        case QImage::Format_ARGB32:
        case QImage::Format_RGB32:
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
            return ImageFormat::BGRX;
#else
            return ImageFormat::XRGB;
#endif
        case QImage::Format_RGB888: return ImageFormat::RGB;
        case QImage::Format_RGBX8888:
        case QImage::Format_RGBA8888: return ImageFormat::RGBX;
        case QImage::Format_Grayscale8: return ImageFormat::Lum;
        default: return ImageFormat::None;
    }
}


void CPBarcode::decode(const QImage& image) {
    qDebug()<<"decode: "<<currentThreadId() << " image: " << image.width() << "-" << image.height() << " " << image.format();
    DecodeHints hints;
    hints.setFormats(BarcodeFormat::QRCode);
    hints.setTryRotate(true);
    hints.setEanAddOnSymbol(EanAddOnSymbol::Read);
    hints.setTryHarder(true);

    ImageView imageView{image.bits(), image.width(), image.height(), getImageFormat(image), image.bytesPerLine()};
    decodeResult = ReadBarcode(imageView, hints);

    qDebug()<<"decode result: " << decodeResult.isValid();
    // if we did not find anything, insert a dummy to produce some output for each file
    int ret = 0;
    if (!decodeResult.isValid())
        ret = -1;

    emit updateBarcodeDecodeResult(ret);
}

