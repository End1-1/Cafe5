#ifndef SHOP_PRINTRECEIPT_GLOBAL_H
#define SHOP_PRINTRECEIPT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SHOP_PRINTRECEIPT_LIBRARY)
#  define SHOP_PRINTRECEIPTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SHOP_PRINTRECEIPTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SHOP_PRINTRECEIPT_GLOBAL_H
