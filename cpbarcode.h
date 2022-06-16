#ifndef CPBARCODE_H
#define CPBARCODE_H

#include "cp-qt-barcode_global.h"


#include <QThread>
#include <QImage>
#include "ReadBarcode.h"

using namespace ZXing;

class CPQTBARCODE_EXPORT CPBarcode: public QThread
{
    Q_OBJECT
public:
    CPBarcode();
    ~CPBarcode();

public slots:
    void stop();
    void decode(const QImage& image);
    void setImage(const QImage &image, float resizeFactor);
    QImage getImage();

signals:
    Q_SIGNAL void updateBarcodeDecodeResult(int decodeState);

private:
    ImageFormat getImageFormat(const QImage& img);

private:
    QMutex m_mutex;
    bool m_stop;
    bool decoding;

    void run();
    QImage image;

public:
    Result decodeResult;
    float resizeFactor;
};


#endif // CPBARCODE_H
