#ifndef EXCEL_H
#define EXCEL_H

#include <QLibrary>
#include <QDir>

typedef int* (*Excel_Create)();
typedef int (*Excel_Show)(void*);
typedef int* (*Workbook_Add)(void *);
typedef int* (*Sheet_Select)(void *, int);
typedef int (*Sheet_Set_Cell_Value)(void*, int, int, const wchar_t *);
typedef int (*Sheet_Set_Col_Width)(void *, int, int);
typedef int (*Merge_Cells)(void*, const wchar_t*, const wchar_t*, int);
typedef int (*Text_HorizontalAlign) (void*, const wchar_t*, const wchar_t*, int);
typedef int (*Text_VerticalAlign) (void*, const wchar_t*, const wchar_t*, int);
typedef int (*Font_Name) (void*, const wchar_t*, const wchar_t*, const wchar_t*);
typedef int (*Cells_Border) (void*, const wchar_t*, const wchar_t*, int);
typedef int (*Cells_Background) (void*, const wchar_t*, const wchar_t*, int, int, int);
typedef int (*Font_Size) (void*, const wchar_t*, const wchar_t*, int);
typedef int (*Font_Color) (void*, const wchar_t*, const wchar_t*, int, int, int);
typedef int (*Font_Bold) (void*, const wchar_t*, const wchar_t*, int);
typedef int (*Font_Italic) (void*, const wchar_t*, const wchar_t*, int);
typedef int (*Clear)(void*);
typedef int (*SaveToFile)(void*, const wchar_t *);

class Excel
{
public:
    enum HAlign {hLeft = 2, hCenter, hRight};
    enum VAlign {vCenter = 2, vBottom, vTop};
    Excel();
    ~Excel();
    QString address(int row, int column);
    void setValue(const QString &value, int row, int col);
    void setColumnWidth(int col, int width);
    void mergeCells(QString r1, QString r2, bool across);
    void setHorizontalAlignment(QString r1, QString r2, int align);
    void setVerticalAlignment(QString r1, QString r2, int align);
    void setBorder(QString r1, QString r2, int border);
    void setBackground(QString r1, QString r2, int red, int green, int blue);
    void setFontSize(QString r1, QString r2, int size);
    void setFontBold(QString r1, QString r2, bool bold = true);
    void setFontItalic(QString r1, QString r2, bool italic = true);
    void setFontColor(QString r1, QString r2, int red, int green, int blue);
    void show();
    void save(const QString &fileName);
private:
    QLibrary *fLib;
    Excel_Create fExcelCreate;
    Excel_Show fExcelShow;
    Workbook_Add fWorkbookAdd;
    Sheet_Select fSheetSelect;
    Sheet_Set_Cell_Value fSheetCellValue;
    Sheet_Set_Col_Width fSheetSetColWidth;
    Merge_Cells fMergeCells;
    Text_HorizontalAlign fTextHAlign;
    Text_VerticalAlign fTextVAlign;
    Cells_Border fCellsBorder;
    Cells_Background fCellsBackground;
    Font_Name fFontName;
    Font_Size fFontSize;
    Font_Color fFontColor;
    Font_Bold fFontBold;
    Font_Italic fFontItalic;
    SaveToFile fSaveToFile;
    Clear fClear;
    int *fExcel;
    int *fWorkbook;
    int *fSheet;
};

#endif // EXCEL_H
