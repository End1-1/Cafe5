#include "excel.h"

const QString alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

Excel::Excel()
{
    fLib = new QLibrary(QDir::currentPath() + "/excel");
    if (!fLib->load()) {
        delete fLib;
        fLib = 0;
        return;
    }

    fExcelCreate = (Excel_Create)(fLib->resolve("Excel_Create"));
    fExcelShow = (Excel_Show)(fLib->resolve("Excel_Show"));
    fWorkbookAdd = (Workbook_Add)(fLib->resolve("Workbook_Add"));
    fSheetSelect = (Sheet_Select)(fLib->resolve("Sheet_Select"));
    fSheetCellValue = (Sheet_Set_Cell_Value)(fLib->resolve("Sheet_Set_Cell_Value"));
    fSheetSetColWidth = (Sheet_Set_Col_Width)(fLib->resolve("Sheet_Set_Col_Width"));
    fMergeCells = (Merge_Cells)(fLib->resolve("Merge_Cells"));
    fTextHAlign = (Text_HorizontalAlign)(fLib->resolve("Text_HorizontalAlign"));
    fTextVAlign = (Text_VerticalAlign)(fLib->resolve("Text_VerticalAlign"));
    fCellsBorder = (Cells_Border)(fLib->resolve("Cells_Border"));
    fCellsBackground = (Cells_Background)(fLib->resolve("Cells_Background"));
    fFontName = (Font_Name)(fLib->resolve("Font_Name"));
    fFontSize = (Font_Size)(fLib->resolve("Font_Size"));
    fFontColor = (Font_Color)(fLib->resolve("Font_Color"));
    fFontBold = (Font_Bold)(fLib->resolve("Font_Bold"));
    fFontItalic = (Font_Italic)(fLib->resolve("Font_Italic"));
    fClear = (Clear)(fLib->resolve("Clear"));
    fSaveToFile = (SaveToFile)(fLib->resolve("SaveToFile"));


    fExcel = fExcelCreate();
    fWorkbook = fWorkbookAdd(fExcel);
    fSheet = fSheetSelect(fWorkbook, 1);

//    mergeCells(sheet, QString("A1").toStdWString().c_str(), QString("B2").toStdWString().c_str(), 1);
}

Excel::~Excel()
{
    if (fLib) {
        fClear(fSheet);
        fClear(fWorkbook);
        fClear(fExcel);
        delete fLib;
    }
}

QString Excel::address(int row, int column)
{
    QString result;
    if (column > 25) {
        result += "A";
        column -= 26;
    }
    result += alphabet.at(column);
    result += QString::number(row + 1);
    return result;
}

void Excel::setValue(const QString &value, int row, int col)
{
    fSheetCellValue(fSheet, col, row, value.toStdWString().c_str());
}

void Excel::setColumnWidth(int col, int width)
{
    fSheetSetColWidth(fSheet, col, width);
}

void Excel::mergeCells(QString r1, QString r2, bool across)
{
    fMergeCells(fSheet, r1.toStdWString().c_str(), r2.toStdWString().c_str(), (across ? 1 : 0));
}

void Excel::setHorizontalAlignment(QString r1, QString r2, int align)
{
    fTextHAlign(fSheet, r1.toStdWString().c_str(), r2.toStdWString().c_str(), align);
}

void Excel::setVerticalAlignment(QString r1, QString r2, int align)
{
    fTextVAlign(fSheet, r1.toStdWString().c_str(), r2.toStdWString().c_str(), align);
}

void Excel::setBorder(QString r1, QString r2, int border)
{
    fCellsBorder(fSheet, r1.toStdWString().c_str(), r2.toStdWString().c_str(), border);
}

void Excel::setBackground(QString r1, QString r2, int red, int green, int blue)
{
    fCellsBackground(fSheet, r1.toStdWString().c_str(), r2.toStdWString().c_str(), red, green, blue);
}

void Excel::setFontSize(QString r1, QString r2, int size)
{
    fFontSize(fSheet, r1.toStdWString().c_str(), r2.toStdWString().c_str(), size);
}

void Excel::setFontBold(QString r1, QString r2, bool bold)
{
    fFontBold(fSheet, r1.toStdWString().c_str(), r2.toStdWString().c_str(), (bold ? 1 : 0));
}

void Excel::setFontItalic(QString r1, QString r2, bool italic)
{
    fFontItalic(fSheet, r1.toStdWString().c_str(), r2.toStdWString().c_str(), (italic ? 1 : 0));
}

void Excel::setFontColor(QString r1, QString r2, int red, int green, int blue)
{
    fFontColor(fSheet, r1.toStdWString().c_str(), r2.toStdWString().c_str(), red, green, blue);
}

void Excel::show()
{
    fExcelShow(fExcel);
}

void Excel::save(const QString &fileName)
{
    fSaveToFile(fExcel, fileName.toStdWString().c_str());
}
