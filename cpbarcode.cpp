#include "cpbarcode.h"


#include "TextUtfEncoding.h"
#include "GTIN.h"
#include <QDebug>

#include <QGraphicsEffect>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPainter>

using namespace TextUtfEncoding;

CPBarcode::CPBarcode() :m_stop(false), decoding(false), decodeResult(DecodeStatus::NotFound), resizeFactor(1) {

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
            decode(image, resizeFactor, 2, true);
            decoding = false;
        }
        //msleep(500);
    }
    qDebug()<<"From worker thread ended: "<<currentThreadId();
}


void CPBarcode::setImage(const QImage &image, float resizeFactor) {
    this->image = image;
    this->resizeFactor = resizeFactor;
    this->decoding = true;
}

QImage CPBarcode::getImage() {
    return this->image;
}

void CPBarcode::stop() {
    qDebug()<<"Thread::stop called from main thread: "<<currentThreadId();
    QMutexLocker locker(&m_mutex);
    m_stop = true;
    this->decoding = false;
    qDebug()<<"stop end";
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


QImage applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent) {
    if(src.isNull()) return QImage();   //No need to do anything else!
    if(!effect) return src;             //No need to do anything else!
    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(QPixmap::fromImage(src));
    item.setGraphicsEffect(effect);
    scene.addItem(&item);
    QImage res(src.size()+QSize(extent*2, extent*2), src.format());
    res.fill(Qt::transparent);
    QPainter ptr(&res);
    scene.render(&ptr, QRectF(), QRectF( -extent, -extent, src.width()+extent*2, src.height()+extent*2 ) );
    return res;
}

void CPBarcode::decode(const QImage& image, float resizeFactor, int filterSize, bool useSignal) {
    qDebug()<<"decode: "<<currentThreadId() << " image: " << image.width() << "-" << image.height() << " " << image.format();

    QImage small = image.scaled(image.width()/resizeFactor, image.height()/resizeFactor);

    QImage smallGray;
    if (filterSize > 0) {
        QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;
        blur->setBlurRadius(filterSize);
        //QImage source("://img1.png");
        QImage result = applyEffectToImage(small, blur, 0);
        //result.save("final.png");
        smallGray = result.convertToFormat(QImage::Format_Grayscale8);
    } else {
        smallGray = small.convertToFormat(QImage::Format_Grayscale8);
    }

    DecodeHints hints;
    hints.setFormats(BarcodeFormat::QRCode);
    hints.setTryRotate(true);
    hints.setEanAddOnSymbol(EanAddOnSymbol::Read);
    hints.setTryHarder(true);

    ImageView imageView{smallGray.bits(), smallGray.width(), smallGray.height(), getImageFormat(smallGray), smallGray.bytesPerLine()};
    decodeResult = ReadBarcode(imageView, hints);

    qDebug()<<"decode result: " << decodeResult.isValid();
    // if we did not find anything, insert a dummy to produce some output for each file
    int ret = 0;
    if (!decodeResult.isValid())
        ret = -1;

    if (useSignal) {
        emit updateBarcodeDecodeResult(ret);
    }
}

int CPBarcode::decodeImage(const QImage &image, float resizeFactor, int filterSize) {
    decode(image, resizeFactor, filterSize, false);
    qDebug() << "return";
    if (!decodeResult.isValid())
        return -1;
    else
        return 0;
}

