QT -= gui
QT += multimedia

TEMPLATE = lib
DEFINES += CPQTBARCODE_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cpbarcode.cpp

HEADERS += \
    cp-qt-barcode_global.h \
    cpbarcode.h



INCLUDEPATH += $$PWD/zxing
HEADERS += \
    zxing/BarcodeFormat.h \
    zxing/BitHacks.h \
    zxing/ByteArray.h \
    zxing/DecodeHints.h \
    zxing/DecodeStatus.h \
    zxing/Flags.h \
    zxing/GTIN.h \
    zxing/ImageView.h \
    zxing/Point.h \
    zxing/Quadrilateral.h \
    zxing/ReadBarcode.h \
    zxing/Result.h \
    zxing/StructuredAppend.h \
    zxing/TextUtfEncoding.h \
    zxing/ZXContainerAlgorithms.h

TRANSLATIONS += \
    cp-qt-barcode_zh_CN.ts

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/x64/release/ -lZXing
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/x64/debug/ -lZXing
else:unix: LIBS += -L$$PWD/libs/x64/ -lZXing.1

DEPENDPATH += $$PWD/libs/x64
