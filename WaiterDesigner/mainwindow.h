#pragma once

#include "waitertablestyle.h"
#include <QMainWindow>

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
    void onReloadClicked();
    void onSaveClicked();

private:
    void setupPreviewFrames();
    void applyStyleToPreview();
    void setColorButton(QPushButton *button, const QColor &color);
    bool loadCss(QString *error = nullptr);
    bool saveCss(QString *error = nullptr);
    QString pickCssFile(const QString &startDir = {}) const;
    bool ensureCssFile(QString *error = nullptr);
    void polishPreviewFrame(QFrame *frame, int state);

    Ui::MainWindow *ui;
    WaiterTblTablesStyle mStyle;
    QString mCssPath;
    QSpinBox *mBorderWidth = nullptr;
    QPushButton *mBtnGrid = nullptr;
    QPushButton *mBtnEmpty = nullptr;
    QPushButton *mBtnEmptyText = nullptr;
    QPushButton *mBtnOrder = nullptr;
    QPushButton *mBtnOrderText = nullptr;
    QPushButton *mBtnPrecheck = nullptr;
    QPushButton *mBtnPrecheckText = nullptr;
};
