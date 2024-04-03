#include "ntablewidget.h"
#include "ui_ntablewidget.h"
#include "ntablemodel.h"
#include "ndataprovider.h"
#include "nloadingdlg.h"
#include "c5mainwindow.h"
#include "c5filelogwriter.h"
#include "xlsxall.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "c5message.h"

#define xls_page_size_a4 9
#define xls_page_orientation_landscape "landscape"
#define xls_page_orientation_portrait "portrait"

QString NTableWidget::mHost;

NTableWidget::NTableWidget(const QString &route, QWidget *parent) :
    C5Widget(QStringList(), parent),
    ui(new Ui::NTableWidget),
    mRoute(route)
{
    ui->setupUi(this);
    ui->mTableView->setModel(new NTableModel());
}

NTableWidget::~NTableWidget()
{
    delete ui;
}

void NTableWidget::query()
{
    auto *nd = new NDataProvider(mHost, this);
    connect(nd, &NDataProvider::started, this, &NTableWidget::queryStarted);
    connect(nd, &NDataProvider::error, this, &NTableWidget::queryError);
    connect(nd, &NDataProvider::done, this, &NTableWidget::queryFinished);
    nd->getData(mRoute);
}

QToolBar *NTableWidget::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
    }
    return fToolBar;
}

void NTableWidget::queryStarted()
{
    mLoadingDlg = new NLoadingDlg(this);
    mLoadingDlg->open();
}

void NTableWidget::queryError(const QString &error)
{
    C5FileLogWriter::write(error);
    mLoadingDlg->deleteLater();
}

void NTableWidget::queryFinished(int elapsed, const QByteArray &ba)
{
    QJsonParseError err;
    QJsonObject jo = QJsonDocument::fromJson(ba, &err).object();
    if (err.error == QJsonParseError::NoError) {
        QJsonObject jwidget = jo["widget"].toObject();
        mMainWindow->nTabDesign(QIcon(jwidget["icon"].toString()), jwidget["title"].toString(), this);
        QJsonArray jcols = jo["cols"].toArray();

        QJsonArray jdata = jo["rows"].toArray();
        static_cast<NTableModel*>(ui->mTableView->model())->setDatasource(jcols, jdata);
        ui->mTableView->resizeColumnsToContents();
    } else {

    }
#ifdef QT_DEBUG
    C5FileLogWriter::write(QString(ba));
#else
    if (__c5config.getValue(param_debuge_mode).toInt() > 0) {
        C5FileLogWriter::write(QString(ba));
    }
#endif
    mLoadingDlg->deleteLater();
}

void NTableWidget::exportToExcel()
{
    int fXlsxFitToPage = 0;
    QString fXlsxPageOrientation = xls_page_orientation_portrait;
    int fXlsxPageSize = xls_page_size_a4;

    auto *fModel = static_cast<NTableModel*>(ui->mTableView->model());
    int colCount = fModel->columnCount();
    int rowCount = fModel->rowCount();
    if (colCount == 0 || rowCount == 0) {
        C5Message::info(tr("Empty report!"));
        return;
    }
    XlsxDocument d;
    XlsxSheet *s = d.workbook()->addSheet("Sheet1");
    s->setupPage(fXlsxPageSize, fXlsxFitToPage, fXlsxPageOrientation);
    /* HEADER */
    QColor color = QColor::fromRgb(200, 200, 250);
    QFont headerFont(qApp->font());
    headerFont.setBold(true);
    d.style()->addFont("header", headerFont);
    d.style()->addBackgrounFill("header", color);
    d.style()->addHAlignment("header", xls_alignment_center);
    d.style()->addBorder("header", XlsxBorder());
    for (int i = 0; i < colCount; i++) {
        s->addCell(1, i + 1, fModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString(), d.style()->styleNum("header"));
        s->setColumnWidth(i + 1, ui->mTableView->columnWidth(i) / 7);
    }

    /* BODY */
    QMap<int, QString> bgFill;
    QMap<int, QString> bgFillb;
    QFont bodyFont(qApp->font());
    d.style()->addFont("body", bodyFont);
    d.style()->addBackgrounFill("body", QColor(Qt::white));
    d.style()->addVAlignment("body", xls_alignment_center);
    d.style()->addBorder("body", XlsxBorder());
    bgFill[QColor(Qt::white).rgb()] = "body";

    bodyFont.setBold(true);
    d.style()->addFont("body_b", bodyFont);
    d.style()->addBackgrounFill("body_b", QColor(Qt::white));
    d.style()->addVAlignment("body_b", xls_alignment_center);
    d.style()->addBorder("body_b", XlsxBorder());
    bgFillb[QColor(Qt::white).rgb()] = "body_b";

    for (int j = 0; j < rowCount; j++) {
        for (int i = 0; i < colCount; i++) {
            int bgColor = fModel->data(j, i, Qt::BackgroundColorRole).value<QColor>().rgb();
            if (!bgFill.contains(bgColor)) {
                bodyFont.setBold(false);
                d.style()->addFont(QString::number(bgColor), bodyFont);
                d.style()->addBackgrounFill(QString::number(bgColor), QColor::fromRgb(bgColor));
                bgFill[bgColor] = QString::number(bgColor);
            }
            if (!bgFill.contains(bgColor)) {
                bodyFont.setBold(true);
                d.style()->addFont(QString::number(bgColor), bodyFont);
                d.style()->addBackgrounFill(QString::number(bgColor), QColor::fromRgb(bgColor));
                bgFillb[bgColor] = QString::number(bgColor);
            }
            QString bgStyle = bgFill[bgColor];
            if (fModel->data(j, i, Qt::FontRole).value<QFont>().bold()) {
                bgStyle = bgFillb[bgColor];
            }
            s->addCell(j + 2, i + 1, fModel->data(j, i, Qt::EditRole), d.style()->styleNum(bgStyle));
        }
    }

    /* MERGE cells */
    QMap<int, QList<int> > skiprow, skipcol;
    for (int r = 0; r < rowCount; r++) {
        for (int c = 0; c < colCount; c++) {
            if (ui->mTableView->columnSpan(r, c) > 1 || ui->mTableView->rowSpan(r, c) > 1) {
                int rs = -1, cs = -1;
                if (ui->mTableView->columnSpan(r, c) > 1 && skipcol[r].contains(c) == false) {
                    cs = ui->mTableView->columnSpan(r, c) - 1;
                    for (int i = c + 1; i < c + cs + 1; i++) {
                        skipcol[r].append(i);
                    }
                }
                if (ui->mTableView->rowSpan(r, c) > 1 && skiprow[c].contains(r) == false) {
                    rs = ui->mTableView->rowSpan(r, c) - 1;
                    for (int i = r + 1; i < r + rs + 1; i++) {
                        skiprow[c].append(i);
                    }
                }
                if (rs == -1 && cs == -1) {
                    continue;
                }
                rs = rs < 0 ? 0 : rs;
                cs = cs < 0 ? 0 : cs;
                s->setSpan(r + 2, c + 1, r + 2 + rs, c + 1 + cs);
            }
        }
    }

    /* TOTALS ROWS */
//    if (ui->tblTotal->isVisible()) {
//        QFont totalFont(qApp->font());
//        totalFont.setBold(true);
//        d.style()->addFont("footer", headerFont);
//        d.style()->addBorder("footer", XlsxBorder());
//        color = QColor::fromRgb(193, 206, 221);
//        d.style()->addBackgrounFill("footer", color);
//        //d.style()->addHAlignment("footer", xls_alignment_right);
//        for (int i = 0; i < colCount; i++) {
//            s->addCell(1 + fModel->rowCount() + 1, i + 1, ui->tblTotal->getData(0, i), d.style()->styleNum("footer"));
//        }
//    }
    QString err;
    if (!d.save(err, true)) {
        if (!err.isEmpty()) {
            C5Message::error(err);
        }
    }
}

void NTableWidget::refreshData()
{
    query();
}

void NTableWidget::clearFilter()
{
    query();
}
