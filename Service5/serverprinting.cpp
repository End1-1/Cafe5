#include "serverprinting.h"
#include <QtPrintSupport/QPrinterInfo>
#include "c5printing.h"
#include "c5printjson.h"

ServerPrinting::ServerPrinting(const QJsonObject &data)
{
    mData = data;
}

bool ServerPrinting::print()
{
    C5PrintJson pj;
    QString printerName = mData.value("printer_name").toString();
    C5Printing p;
    QPrinterInfo pi = QPrinterInfo::printerInfo(printerName);
    QPrinter printer(pi);
    printer.setPageSize(QPageSize::Custom);
    printer.setFullPage(false);
    QRectF pr = printer.pageRect(QPrinter::DevicePixel);
    constexpr qreal SAFE_RIGHT_MM = 4.0;
    qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
    p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
    pj.parse(p, mData.value("print_data").toArray());
    p.print(printer);
    return true;
}
