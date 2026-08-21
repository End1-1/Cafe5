#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QFrame>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStyle>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Waiter style"));

    mCssPath = WaiterTblTablesStyle::cssFilePath();
    ui->leCssPath->setText(mCssPath);

    mBorderWidth = ui->sbBorderWidth;
    mBtnGrid = ui->btnGridColor;
    mBtnEmpty = ui->btnEmptyColor;
    mBtnEmptyText = ui->btnEmptyTextColor;
    mBtnOrder = ui->btnOrderColor;
    mBtnOrderText = ui->btnOrderTextColor;
    mBtnPrecheck = ui->btnPrecheckColor;
    mBtnPrecheckText = ui->btnPrecheckTextColor;
    mGroupSpacing = ui->sbGroupSpacing;
    mGroupBorderWidth = ui->sbGroupBorderWidth;
    mGroupVisibleRows = ui->sbGroupVisibleRows;
    mGroupFontSize = ui->sbGroupFontSize;
    mGroupFontBold = ui->chGroupFontBold;
    mBtnGroupBg = ui->btnGroupBgColor;
    mBtnGroupFontColor = ui->btnGroupFontColor;
    mDishSpacing = ui->sbDishSpacing;
    mDishBorderWidth = ui->sbDishBorderWidth;
    mDishHeight = ui->sbDishHeight;
    mDishFontSize = ui->sbDishFontSize;
    mDishFontBold = ui->chDishFontBold;
    mBtnDishBg = ui->btnDishBgColor;
    mBtnDishFontColor = ui->btnDishFontColor;

    connect(mBorderWidth, qOverload<int>(&QSpinBox::valueChanged), this, &MainWindow::onBorderWidthChanged);
    connect(mBtnGrid, &QPushButton::clicked, this, &MainWindow::onPickGridColor);
    connect(mBtnEmpty, &QPushButton::clicked, this, &MainWindow::onPickEmptyColor);
    connect(mBtnEmptyText, &QPushButton::clicked, this, &MainWindow::onPickEmptyTextColor);
    connect(mBtnOrder, &QPushButton::clicked, this, &MainWindow::onPickOrderColor);
    connect(mBtnOrderText, &QPushButton::clicked, this, &MainWindow::onPickOrderTextColor);
    connect(mBtnPrecheck, &QPushButton::clicked, this, &MainWindow::onPickPrecheckColor);
    connect(mBtnPrecheckText, &QPushButton::clicked, this, &MainWindow::onPickPrecheckTextColor);
    connect(mGroupSpacing, qOverload<int>(&QSpinBox::valueChanged), this, &MainWindow::onGroupSpacingChanged);
    connect(mGroupBorderWidth, qOverload<int>(&QSpinBox::valueChanged), this, &MainWindow::onGroupBorderWidthChanged);
    connect(mGroupVisibleRows, qOverload<int>(&QSpinBox::valueChanged), this, &MainWindow::onGroupVisibleRowsChanged);
    connect(mGroupFontSize, qOverload<int>(&QSpinBox::valueChanged), this, &MainWindow::onGroupFontSizeChanged);
    connect(mGroupFontBold, &QCheckBox::toggled, this, &MainWindow::onGroupFontBoldChanged);
    connect(mBtnGroupBg, &QPushButton::clicked, this, &MainWindow::onPickGroupBgColor);
    connect(mBtnGroupFontColor, &QPushButton::clicked, this, &MainWindow::onPickGroupFontColor);
    connect(mDishSpacing, qOverload<int>(&QSpinBox::valueChanged), this, &MainWindow::onDishSpacingChanged);
    connect(mDishBorderWidth, qOverload<int>(&QSpinBox::valueChanged), this, &MainWindow::onDishBorderWidthChanged);
    connect(mDishHeight, qOverload<int>(&QSpinBox::valueChanged), this, &MainWindow::onDishHeightChanged);
    connect(mDishFontSize, qOverload<int>(&QSpinBox::valueChanged), this, &MainWindow::onDishFontSizeChanged);
    connect(mDishFontBold, &QCheckBox::toggled, this, &MainWindow::onDishFontBoldChanged);
    connect(mBtnDishBg, &QPushButton::clicked, this, &MainWindow::onPickDishBgColor);
    connect(mBtnDishFontColor, &QPushButton::clicked, this, &MainWindow::onPickDishFontColor);
    connect(ui->btnReload, &QPushButton::clicked, this, &MainWindow::onReloadClicked);
    connect(ui->btnSave, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
    connect(ui->btnBrowse, &QPushButton::clicked, this, [this]() {
        const QString path = pickCssFile();
        if (!path.isEmpty()) {
            mCssPath = path;
            ui->leCssPath->setText(path);
            onReloadClicked();
        }
    });

    setupPreviewFrames();

    QString error;
    if (!loadCss(&error) && !error.isEmpty()) {
        QMessageBox::warning(this, tr("Load CSS"), error);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupPreviewFrames()
{
    const auto addLabel = [](QFrame *frame, const QString &text, const QString &labelObjectName) {
        if (frame->layout()) {
            return;
        }
        auto *layout = new QVBoxLayout(frame);
        layout->setContentsMargins(6, 6, 6, 6);
        auto *label = new QLabel(text, frame);
        if (!labelObjectName.isEmpty()) {
            label->setObjectName(labelObjectName);
        }
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
    };

    addLabel(ui->framePreviewEmpty, tr("A1"), {});
    addLabel(ui->framePreviewOrder, tr("B2 - 101"), {});
    addLabel(ui->framePreviewPrecheck, tr("C3 - 102"), {});

    ui->frameGroupPreview1->setObjectName(QStringLiteral("goodsGroupFrame"));
    ui->frameGroupPreview2->setObjectName(QStringLiteral("goodsGroupFrame"));
    ui->frameGroupPreview3->setObjectName(QStringLiteral("goodsGroupFrame"));
    ui->frameGroupPreview4->setObjectName(QStringLiteral("goodsGroupFrame"));
    addLabel(ui->frameGroupPreview1, tr("Group A"), QStringLiteral("goodsGroupName"));
    addLabel(ui->frameGroupPreview2, tr("Group B"), QStringLiteral("goodsGroupName"));
    addLabel(ui->frameGroupPreview3, tr("Group C"), QStringLiteral("goodsGroupName"));
    addLabel(ui->frameGroupPreview4, tr("Group D"), QStringLiteral("goodsGroupName"));

    ui->frameDishPreview1->setObjectName(QStringLiteral("goodsDishFrame"));
    ui->frameDishPreview2->setObjectName(QStringLiteral("goodsDishFrame"));
    ui->frameDishPreview3->setObjectName(QStringLiteral("goodsDishFrame"));
    ui->frameDishPreview4->setObjectName(QStringLiteral("goodsDishFrame"));
    addLabel(ui->frameDishPreview1, tr("Dish A"), QStringLiteral("goodsDishName"));
    addLabel(ui->frameDishPreview2, tr("Dish B"), QStringLiteral("goodsDishName"));
    addLabel(ui->frameDishPreview3, tr("Dish C"), QStringLiteral("goodsDishName"));
    addLabel(ui->frameDishPreview4, tr("Dish D"), QStringLiteral("goodsDishName"));
}

void MainWindow::polishPreviewFrame(QFrame *frame, int state)
{
    frame->setProperty("t1_state", state);
    frame->style()->unpolish(frame);
    frame->style()->polish(frame);
    frame->update();
}

void MainWindow::polishGroupPreviewFrame(QFrame *frame)
{
    frame->style()->unpolish(frame);
    frame->style()->polish(frame);
    frame->update();
}

void MainWindow::polishDishPreviewFrame(QFrame *frame)
{
    frame->style()->unpolish(frame);
    frame->style()->polish(frame);
    frame->update();
}

void MainWindow::setColorButton(QPushButton *button, const QColor &color)
{
    button->setStyleSheet(QStringLiteral("background-color: %1;").arg(color.name(QColor::HexRgb)));
}

void MainWindow::applyStyleToPreview()
{
    mBorderWidth->blockSignals(true);
    mBorderWidth->setValue(mStyle.borderWidth);
    mBorderWidth->blockSignals(false);

    setColorButton(mBtnGrid, mStyle.gridColor);
    setColorButton(mBtnEmpty, mStyle.colorEmpty);
    setColorButton(mBtnEmptyText, mStyle.colorEmptyText);
    setColorButton(mBtnOrder, mStyle.colorOrder);
    setColorButton(mBtnOrderText, mStyle.colorOrderText);
    setColorButton(mBtnPrecheck, mStyle.colorPrecheck);
    setColorButton(mBtnPrecheckText, mStyle.colorPrecheckText);

    ui->wPreviewHall->setStyleSheet(mStyle.hallTableStylesheet());
    polishPreviewFrame(ui->framePreviewEmpty, 1);
    polishPreviewFrame(ui->framePreviewOrder, 2);
    polishPreviewFrame(ui->framePreviewPrecheck, 3);

    applyGroupStyleToPreview();
    applyDishStyleToPreview();
}

void MainWindow::applyGroupStyleToPreview()
{
    mGroupSpacing->blockSignals(true);
    mGroupBorderWidth->blockSignals(true);
    mGroupVisibleRows->blockSignals(true);
    mGroupFontSize->blockSignals(true);
    mGroupFontBold->blockSignals(true);
    mGroupSpacing->setValue(mGroupStyle.spacing);
    mGroupBorderWidth->setValue(mGroupStyle.borderWidth);
    mGroupVisibleRows->setValue(mGroupStyle.visibleRows);
    mGroupFontSize->setValue(mGroupStyle.fontSize);
    mGroupFontBold->setChecked(mGroupStyle.fontBold);
    mGroupSpacing->blockSignals(false);
    mGroupBorderWidth->blockSignals(false);
    mGroupVisibleRows->blockSignals(false);
    mGroupFontSize->blockSignals(false);
    mGroupFontBold->blockSignals(false);

    setColorButton(mBtnGroupBg, mGroupStyle.colorDefault);
    setColorButton(mBtnGroupFontColor, mGroupStyle.fontColor);

    ui->glPreviewGroups->setSpacing(mGroupStyle.spacing);
    ui->wPreviewGroups->setStyleSheet(mGroupStyle.groupStylesheet());
    polishGroupPreviewFrame(ui->frameGroupPreview1);
    polishGroupPreviewFrame(ui->frameGroupPreview2);
    polishGroupPreviewFrame(ui->frameGroupPreview3);
    polishGroupPreviewFrame(ui->frameGroupPreview4);

    ui->lblGroupStripHint->setText(
        tr("Strip height: %1 px (%2 rows × %3 + spacing)")
            .arg(mGroupStyle.stripMaxHeight())
            .arg(mGroupStyle.visibleRows)
            .arg(WaiterGoodsGroupStyle::buttonHeight));
}

void MainWindow::applyDishStyleToPreview()
{
    mDishSpacing->blockSignals(true);
    mDishBorderWidth->blockSignals(true);
    mDishHeight->blockSignals(true);
    mDishFontSize->blockSignals(true);
    mDishFontBold->blockSignals(true);
    mDishSpacing->setValue(mDishStyle.spacing);
    mDishBorderWidth->setValue(mDishStyle.borderWidth);
    mDishHeight->setValue(mDishStyle.height);
    mDishFontSize->setValue(mDishStyle.fontSize);
    mDishFontBold->setChecked(mDishStyle.fontBold);
    mDishSpacing->blockSignals(false);
    mDishBorderWidth->blockSignals(false);
    mDishHeight->blockSignals(false);
    mDishFontSize->blockSignals(false);
    mDishFontBold->blockSignals(false);

    setColorButton(mBtnDishBg, mDishStyle.colorDefault);
    setColorButton(mBtnDishFontColor, mDishStyle.fontColor);

    ui->glPreviewDishes->setSpacing(mDishStyle.spacing);
    ui->wPreviewDishes->setStyleSheet(mDishStyle.dishStylesheet());

    const int h = qBound(40, mDishStyle.height, 200);
    for (QFrame *frame : {ui->frameDishPreview1, ui->frameDishPreview2, ui->frameDishPreview3, ui->frameDishPreview4}) {
        frame->setMinimumHeight(h);
        frame->setMaximumHeight(h);
        polishDishPreviewFrame(frame);
    }

    ui->lblDishHint->setText(tr("Button height: %1 px").arg(h));
}

QString MainWindow::pickCssFile(const QString &startDir) const
{
    QString dir = startDir;
    if (dir.isEmpty()) {
        dir = QCoreApplication::applicationDirPath();
        if (!QFileInfo(dir).isDir()) {
            dir = QDir::homePath();
        }
    }

    return QFileDialog::getOpenFileName(
        const_cast<MainWindow *>(this),
        tr("Open waiter.css"),
        dir,
        tr("CSS (*.css)"));
}

bool MainWindow::ensureCssFile(QString *error)
{
    if (QFile::exists(mCssPath)) {
        return true;
    }

    const auto answer = QMessageBox::question(
        this,
        tr("File not found"),
        tr("File not found:\n%1\n\nSelect another file?").arg(mCssPath),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes);

    if (answer != QMessageBox::Yes) {
        if (error) {
            *error = tr("CSS file not selected.");
        }
        return false;
    }

    const QString path = pickCssFile(QFileInfo(mCssPath).absolutePath());
    if (path.isEmpty()) {
        if (error) {
            *error = tr("CSS file not selected.");
        }
        return false;
    }

    mCssPath = path;
    ui->leCssPath->setText(path);
    return true;
}

bool MainWindow::loadCss(QString *error)
{
    if (!ensureCssFile(error)) {
        return false;
    }

    if (!mStyle.loadFromCssFile(mCssPath, error)) {
        return false;
    }
    if (!mGroupStyle.loadFromCssFile(mCssPath, error)) {
        return false;
    }
    if (!mDishStyle.loadFromCssFile(mCssPath, error)) {
        return false;
    }
    applyStyleToPreview();
    return true;
}

bool MainWindow::saveCss(QString *error)
{
    mStyle.borderWidth = mBorderWidth->value();
    mGroupStyle.spacing = mGroupSpacing->value();
    mGroupStyle.borderWidth = mGroupBorderWidth->value();
    mGroupStyle.visibleRows = mGroupVisibleRows->value();
    mGroupStyle.fontSize = mGroupFontSize->value();
    mGroupStyle.fontBold = mGroupFontBold->isChecked();
    mDishStyle.spacing = mDishSpacing->value();
    mDishStyle.borderWidth = mDishBorderWidth->value();
    mDishStyle.height = mDishHeight->value();
    mDishStyle.fontSize = mDishFontSize->value();
    mDishStyle.fontBold = mDishFontBold->isChecked();

    if (!mStyle.saveToCssFile(mCssPath, error)) {
        return false;
    }
    if (!mGroupStyle.saveToCssFile(mCssPath, error)) {
        return false;
    }
    return mDishStyle.saveToCssFile(mCssPath, error);
}

void MainWindow::onBorderWidthChanged(int value)
{
    mStyle.borderWidth = value;
    applyStyleToPreview();
}

void MainWindow::onPickGridColor()
{
    const QColor color = QColorDialog::getColor(mStyle.gridColor, this, tr("Border color"));
    if (!color.isValid()) {
        return;
    }
    mStyle.gridColor = color;
    applyStyleToPreview();
}

void MainWindow::onPickEmptyColor()
{
    const QColor color = QColorDialog::getColor(mStyle.colorEmpty, this, tr("Empty table — background"));
    if (!color.isValid()) {
        return;
    }
    mStyle.colorEmpty = color;
    applyStyleToPreview();
}

void MainWindow::onPickEmptyTextColor()
{
    const QColor color = QColorDialog::getColor(mStyle.colorEmptyText, this, tr("Empty table — text"));
    if (!color.isValid()) {
        return;
    }
    mStyle.colorEmptyText = color;
    applyStyleToPreview();
}

void MainWindow::onPickOrderColor()
{
    const QColor color = QColorDialog::getColor(mStyle.colorOrder, this, tr("Table with order — background"));
    if (!color.isValid()) {
        return;
    }
    mStyle.colorOrder = color;
    applyStyleToPreview();
}

void MainWindow::onPickOrderTextColor()
{
    const QColor color = QColorDialog::getColor(mStyle.colorOrderText, this, tr("Table with order — text"));
    if (!color.isValid()) {
        return;
    }
    mStyle.colorOrderText = color;
    applyStyleToPreview();
}

void MainWindow::onPickPrecheckColor()
{
    const QColor color = QColorDialog::getColor(mStyle.colorPrecheck, this, tr("Table with precheck — background"));
    if (!color.isValid()) {
        return;
    }
    mStyle.colorPrecheck = color;
    applyStyleToPreview();
}

void MainWindow::onPickPrecheckTextColor()
{
    const QColor color = QColorDialog::getColor(mStyle.colorPrecheckText, this, tr("Table with precheck — text"));
    if (!color.isValid()) {
        return;
    }
    mStyle.colorPrecheckText = color;
    applyStyleToPreview();
}

void MainWindow::onGroupSpacingChanged(int value)
{
    mGroupStyle.spacing = value;
    applyGroupStyleToPreview();
}

void MainWindow::onGroupBorderWidthChanged(int value)
{
    mGroupStyle.borderWidth = value;
    applyGroupStyleToPreview();
}

void MainWindow::onGroupVisibleRowsChanged(int value)
{
    mGroupStyle.visibleRows = value;
    applyGroupStyleToPreview();
}

void MainWindow::onGroupFontSizeChanged(int value)
{
    mGroupStyle.fontSize = value;
    applyGroupStyleToPreview();
}

void MainWindow::onGroupFontBoldChanged(bool checked)
{
    mGroupStyle.fontBold = checked;
    applyGroupStyleToPreview();
}

void MainWindow::onPickGroupBgColor()
{
    const QColor color = QColorDialog::getColor(mGroupStyle.colorDefault, this, tr("Default group background"));
    if (!color.isValid()) {
        return;
    }
    mGroupStyle.colorDefault = color;
    applyGroupStyleToPreview();
}

void MainWindow::onPickGroupFontColor()
{
    const QColor color = QColorDialog::getColor(mGroupStyle.fontColor, this, tr("Group font color"));
    if (!color.isValid()) {
        return;
    }
    mGroupStyle.fontColor = color;
    applyGroupStyleToPreview();
}

void MainWindow::onDishSpacingChanged(int value)
{
    mDishStyle.spacing = value;
    applyDishStyleToPreview();
}

void MainWindow::onDishBorderWidthChanged(int value)
{
    mDishStyle.borderWidth = value;
    applyDishStyleToPreview();
}

void MainWindow::onDishHeightChanged(int value)
{
    mDishStyle.height = value;
    applyDishStyleToPreview();
}

void MainWindow::onDishFontSizeChanged(int value)
{
    mDishStyle.fontSize = value;
    applyDishStyleToPreview();
}

void MainWindow::onDishFontBoldChanged(bool checked)
{
    mDishStyle.fontBold = checked;
    applyDishStyleToPreview();
}

void MainWindow::onPickDishBgColor()
{
    const QColor color = QColorDialog::getColor(mDishStyle.colorDefault, this, tr("Default dish background"));
    if (!color.isValid()) {
        return;
    }
    mDishStyle.colorDefault = color;
    applyDishStyleToPreview();
}

void MainWindow::onPickDishFontColor()
{
    const QColor color = QColorDialog::getColor(mDishStyle.fontColor, this, tr("Dish font color"));
    if (!color.isValid()) {
        return;
    }
    mDishStyle.fontColor = color;
    applyDishStyleToPreview();
}

void MainWindow::onReloadClicked()
{
    mCssPath = ui->leCssPath->text().trimmed();
    QString error;
    if (!loadCss(&error) && !error.isEmpty()) {
        QMessageBox::warning(this, tr("Load CSS"), error);
    }
}

void MainWindow::onSaveClicked()
{
    mCssPath = ui->leCssPath->text().trimmed();
    if (!ensureCssFile()) {
        return;
    }
    QString error;
    if (!saveCss(&error)) {
        QMessageBox::warning(this, tr("Save CSS"), error);
        return;
    }
    QMessageBox::information(this, tr("Save CSS"), tr("Saved to %1").arg(mCssPath));
}
