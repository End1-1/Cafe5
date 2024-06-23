#include "ntablewidget.h"
#include "ui_ntablewidget.h"
#include "ntablemodel.h"
#include "ndataprovider.h"
#include "nloadingdlg.h"
#include "c5mainwindow.h"
#include "c5filelogwriter.h"
#include "c5filtervalues.h"
#include "xlsxall.h"
#include "c5utils.h"
#include "nfilterdlg.h"
#include "nhandler.h"
#include "c5message.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QScrollBar>
#include <QHeaderView>
#include <QMenu>

#define xls_page_size_a4 9
#define xls_page_orientation_landscape "landscape"
#define xls_page_orientation_portrait "portrait"

QString NTableWidget::mHost;

NTableWidget::NTableWidget(const QString &route, QWidget *parent) :
    C5Widget(QStringList(), parent),
    ui(new Ui::NTableWidget),
    mRoute(route),
    fFilter(nullptr),
    mToolWidget(nullptr)
{
    ui->setupUi(this);
    ui->mTableView->setModel(new NTableModel());
    ui->mTableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->mTableView->horizontalHeader(), &QHeaderView::customContextMenuRequested, this,
            &NTableWidget::tableViewHeaderContextMenuRequested);
    connect(ui->mTableView->horizontalHeader(), &QHeaderView::sectionClicked, this, &NTableWidget::tableViewHeaderClicked);
    connect(ui->mTableView->horizontalHeader(), &QHeaderView::sectionResized, this,
            &NTableWidget::tableViewHeaderResized);
    connect(ui->mTableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this,
            SLOT(selectionChanged(QItemSelection, QItemSelection)));
    connect(ui->tblTotal->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(tblValueChanged(int)));
    ui->widget->setVisible(false);
    mHandler = new NHandler(this);
}

NTableWidget::~NTableWidget()
{
    delete ui;
}

void NTableWidget::query()
{
    auto *nd = new NDataProvider(this);
    connect(nd, &NDataProvider::started, this, &NTableWidget::queryStarted);
    connect(nd, &NDataProvider::error, this, &NTableWidget::queryError);
    connect(nd, &NDataProvider::done, this, &NTableWidget::queryFinished);
    nd->getData(mRoute, fFilter ? fFilter->filter() : QJsonObject());
}

QToolBar *NTableWidget::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn
                << ToolBarButtons::tbFilter
                << ToolBarButtons::tbRefresh
                << ToolBarButtons::tbClearFilter
                << ToolBarButtons::tbExcel
                << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
    }
    return fToolBar;
}

void NTableWidget::hotKey(const QString &key)
{
    if (key.toLower() == "ctrl+f") {
        ui->widget->setVisible(true);
        ui->leFilterLineedit->setFocus();
        ui->leFilterLineedit->setSelection(0, ui->leFilterLineedit->text().length());
    } else if (key.toLower() == "esc") {
        ui->widget->setVisible(false);
        ui->leFilterLineedit->clear();
        static_cast<NTableModel *>(ui->mTableView->model())->setFilter("");
    } else {
        C5Widget::hotKey(key);
    }
}

void NTableWidget::sum()
{
    auto *model = static_cast<NTableModel *>(ui->mTableView->model());
    ui->tblTotal->setColumnCount(model->columnCount());
    ui->tblTotal->setVisible(model->fColSum.count() > 0);
    QStringList l;
    l.append(QString::number(model->rowCount()));
    ui->tblTotal->setVerticalHeaderLabels(l);
    for (int i = 0; i < ui->tblTotal->columnCount(); i++) {
        if (model->fColSum.contains(i)) {
            ui->tblTotal->setItem(0, i, new QTableWidgetItem(float_str(model->fColSum[i], 2)));
        } else {
            ui->tblTotal->setItem(0, i, new QTableWidgetItem(""));
        }
    }
}

void NTableWidget::queryStarted()
{
    mLoadingDlg = new NLoadingDlg(this);
    mLoadingDlg->open();
}

void NTableWidget::filterByColumn()
{
    const QSet<QString> &filterValues = static_cast<NTableModel *>(ui->mTableView->model())->uniqueValuesForColumn(
                                            fFilterColumn);
    QStringList sortedValues = filterValues.values();
    std::sort(sortedValues.begin(), sortedValues.end());
    if (C5FilterValues::filterValues(sortedValues)) {
        static_cast<NTableModel *>(ui->mTableView->model())->setColumnFilter(sortedValues.join("|"), fFilterColumn);
    }
    sum();
}

void NTableWidget::queryError(const QString &error)
{
    sender()->deleteLater();
    c5loge(error);
    C5Message::error(error);
    mLoadingDlg->deleteLater();
}

void NTableWidget::queryFinished(const QJsonObject &ba)
{
    QJsonObject jo = ba;
    QJsonObject jwidget = jo["widget"].toObject();
    mMainWindow->nTabDesign(QIcon(jwidget["icon"].toString()), jwidget["title"].toString(), this);
    QJsonArray jcols = jo["cols"].toArray();
    QJsonArray colSum = jo["sum"].toArray();
    QJsonArray jdata = jo["rows"].toArray();
    QJsonArray jhiddencols = jo["hiddencols"].toArray();
    if (!fFilter) {
        QJsonArray jfilter = jo["filter"].toArray();
        fFilter = new NFilterDlg(this);
        fFilter->setProperty("host", mHost);
        fFilter->setup(jfilter);
    }
    static_cast<NTableModel * >(ui->mTableView->model())->setDatasource(jcols, jdata);
    auto *model = static_cast<NTableModel * >(ui->mTableView->model());
    model->fSumColumnsSpecial = jo["sumspecial"].toArray();
    model->setSumColumns(colSum);
    sum();
    ui->mTableView->resizeColumnsToContents();
    for (int i = 0; i < ui->tblTotal->columnCount(); i++) {
        ui->mTableView->setColumnHidden(i, false);
        ui->tblTotal->setColumnHidden(i, false);
        ui->tblTotal->setColumnWidth(i, ui->mTableView->columnWidth(i));
    }
    for (int i = 0; i < jhiddencols.size(); i++) {
        ui->mTableView->setColumnHidden(jhiddencols[i].toInt(), true);
        ui->tblTotal->setColumnHidden(jhiddencols[i].toInt(), true);
    }
    mHandler->configure(fFilter, jo["handler"].toArray().toVariantList(), ui->mTableView);
    if (!mToolWidget) {
        mToolWidget = new QWidget();
        fToolBar->addWidget(mToolWidget);
        mToolWidget->setLayout(new QGridLayout());
        mHandler->toolWidget(mToolWidget);
    }
    sender()->deleteLater();
    mLoadingDlg->deleteLater();
}

void NTableWidget::exportToExcel()
{
    int fXlsxFitToPage = 0;
    QString fXlsxPageOrientation = xls_page_orientation_portrait;
    int fXlsxPageSize = xls_page_size_a4;
    auto *fModel = static_cast<NTableModel *>(ui->mTableView->model());
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
    d.style()->addBackgrounFill("body", QColor(Qt::white).toRgb());
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
    if (ui->tblTotal->isVisible()) {
        QFont totalFont(qApp->font());
        totalFont.setBold(true);
        d.style()->addFont("footer", headerFont);
        d.style()->addBorder("footer", XlsxBorder());
        color = QColor::fromRgb(193, 206, 221);
        d.style()->addBackgrounFill("footer", color);
        //d.style()->addHAlignment("footer", xls_alignment_right);
        for (int i = 0; i < colCount; i++) {
            s->addCell(1 + fModel->rowCount() + 1, i + 1,
                       fModel->fColSum.contains(i) ?
                       fModel->fColSum[i] : QVariant(),
                       d.style()->styleNum("footer"));
        }
    }
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
    if (fFilter) {
        fFilter->clear();
    }
    query();
}

void NTableWidget::setSearchParameters()
{
    if (!fFilter) {
        return;
    }
    if (fFilter->exec() == QDialog::Accepted) {
        query();
    }
}

void NTableWidget::tableViewHeaderResized(int column, int oldSize, int newSize)
{
    Q_UNUSED(oldSize);
    ui->tblTotal->setColumnWidth(column, newSize);
}

void NTableWidget::tblValueChanged(int v)
{
    ui->mTableView->horizontalScrollBar()->setValue(v);
}

void NTableWidget::on_leFilterLineedit_textEdited(const QString &arg1)
{
    static_cast<NTableModel *>(ui->mTableView->model())->setFilter(arg1);
    sum();
}

void NTableWidget::on_mTableView_doubleClicked(const QModelIndex &index)
{
    auto *m = static_cast<NTableModel *>(ui->mTableView->model());
    mHandler->handle(m->rowData(index.row()));
}

void NTableWidget::tableViewHeaderContextMenuRequested(const QPoint &point)
{
    fFilterColumn = ui->mTableView->columnAt(point.x());
    QString colName = ui->mTableView->model()->headerData(fFilterColumn, Qt::Horizontal, Qt::DisplayRole).toString();
    QMenu m;
    m.addAction(QIcon(":/filter_set.png"), QString("%1 '%2'").arg(tr("Set filter")).arg(colName), this,
                SLOT(filterByColumn()));
    m.addAction(QIcon(":/filter_clear.png"), QString("%1 '%2'").arg(tr("Remove filter")).arg(colName), this,
                SLOT(removeFilterForColumn()));
    m.addAction(QIcon(":/expand.png"), tr("Autofit columns widths"), this, SLOT(autofitColumns()));
    m.exec(ui->mTableView->mapToGlobal(point));
}

void NTableWidget::tableViewHeaderClicked(int index)
{
    static_cast<NTableModel *>(ui->mTableView->model())->sort(index);
}
