#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    setWindowTitle(tr("Waiter TableWidget style"));

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

    connect(mBorderWidth, qOverload<int>(&QSpinBox::valueChanged), this, &MainWindow::onBorderWidthChanged);
    connect(mBtnGrid, &QPushButton::clicked, this, &MainWindow::onPickGridColor);
    connect(mBtnEmpty, &QPushButton::clicked, this, &MainWindow::onPickEmptyColor);
    connect(mBtnEmptyText, &QPushButton::clicked, this, &MainWindow::onPickEmptyTextColor);
    connect(mBtnOrder, &QPushButton::clicked, this, &MainWindow::onPickOrderColor);
    connect(mBtnOrderText, &QPushButton::clicked, this, &MainWindow::onPickOrderTextColor);
    connect(mBtnPrecheck, &QPushButton::clicked, this, &MainWindow::onPickPrecheckColor);
    connect(mBtnPrecheckText, &QPushButton::clicked, this, &MainWindow::onPickPrecheckTextColor);
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
    const auto addLabel = [](QFrame *frame, const QString &text) {
        auto *layout = new QVBoxLayout(frame);
        layout->setContentsMargins(6, 6, 6, 6);
        auto *label = new QLabel(text, frame);
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
    };

    addLabel(ui->framePreviewEmpty, tr("A1"));
    addLabel(ui->framePreviewOrder, tr("B2 - 101"));
    addLabel(ui->framePreviewPrecheck, tr("C3 - 102"));
}

void MainWindow::polishPreviewFrame(QFrame *frame, int state)
{
    frame->setProperty("t1_state", state);
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
    applyStyleToPreview();
    return true;
}

bool MainWindow::saveCss(QString *error)
{
    mStyle.borderWidth = mBorderWidth->value();
    return mStyle.saveToCssFile(mCssPath, error);
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
