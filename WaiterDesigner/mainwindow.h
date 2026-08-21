#pragma once

#include "waitergoodsgroupstyle.h"
#include "waitergoodsdishstyle.h"
#include "waitertablestyle.h"
#include <QMainWindow>

class QCheckBox;
class QFrame;
class QSpinBox;
class QPushButton;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onBorderWidthChanged(int value);
    void onPickGridColor();
    void onPickEmptyColor();
    void onPickEmptyTextColor();
    void onPickOrderColor();
    void onPickOrderTextColor();
    void onPickPrecheckColor();
    void onPickPrecheckTextColor();
    void onGroupSpacingChanged(int value);
    void onGroupBorderWidthChanged(int value);
    void onGroupVisibleRowsChanged(int value);
    void onGroupFontSizeChanged(int value);
    void onGroupFontBoldChanged(bool checked);
    void onPickGroupBgColor();
    void onPickGroupFontColor();
    void onDishSpacingChanged(int value);
    void onDishBorderWidthChanged(int value);
    void onDishHeightChanged(int value);
    void onDishFontSizeChanged(int value);
    void onDishFontBoldChanged(bool checked);
    void onPickDishBgColor();
    void onPickDishFontColor();
    void onReloadClicked();
    void onSaveClicked();

private:
    void setupPreviewFrames();
    void applyStyleToPreview();
    void applyGroupStyleToPreview();
    void applyDishStyleToPreview();
    void setColorButton(QPushButton *button, const QColor &color);
    bool loadCss(QString *error = nullptr);
    bool saveCss(QString *error = nullptr);
    QString pickCssFile(const QString &startDir = {}) const;
    bool ensureCssFile(QString *error = nullptr);
    void polishPreviewFrame(QFrame *frame, int state);
    void polishGroupPreviewFrame(QFrame *frame);
    void polishDishPreviewFrame(QFrame *frame);

    Ui::MainWindow *ui;
    WaiterTblTablesStyle mStyle;
    WaiterGoodsGroupStyle mGroupStyle;
    WaiterGoodsDishStyle mDishStyle;
    QString mCssPath;
    QSpinBox *mBorderWidth = nullptr;
    QPushButton *mBtnGrid = nullptr;
    QPushButton *mBtnEmpty = nullptr;
    QPushButton *mBtnEmptyText = nullptr;
    QPushButton *mBtnOrder = nullptr;
    QPushButton *mBtnOrderText = nullptr;
    QPushButton *mBtnPrecheck = nullptr;
    QPushButton *mBtnPrecheckText = nullptr;
    QSpinBox *mGroupSpacing = nullptr;
    QSpinBox *mGroupBorderWidth = nullptr;
    QSpinBox *mGroupVisibleRows = nullptr;
    QSpinBox *mGroupFontSize = nullptr;
    QCheckBox *mGroupFontBold = nullptr;
    QPushButton *mBtnGroupBg = nullptr;
    QPushButton *mBtnGroupFontColor = nullptr;
    QSpinBox *mDishSpacing = nullptr;
    QSpinBox *mDishBorderWidth = nullptr;
    QSpinBox *mDishHeight = nullptr;
    QSpinBox *mDishFontSize = nullptr;
    QCheckBox *mDishFontBold = nullptr;
    QPushButton *mBtnDishBg = nullptr;
    QPushButton *mBtnDishFontColor = nullptr;
};
