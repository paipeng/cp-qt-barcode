#ifndef CPQTBARCODE_GLOBAL_H
#define CPQTBARCODE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CPQTBARCODE_LIBRARY)
#  define CPQTBARCODE_EXPORT Q_DECL_EXPORT
#else
#  define CPQTBARCODE_EXPORT Q_DECL_IMPORT
#endif

#endif // CPQTBARCODE_GLOBAL_H
