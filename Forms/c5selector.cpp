#include "c5selector.h"
#include "ui_c5selector.h"
#include "c5cache.h"
#include "c5grid.h"
#include "c5tablemodel.h"
#include <QKeyEvent>

QMap<QString, QMap<int, C5Selector *> > C5Selector::fSelectorList;

C5Selector::C5Selector(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::C5Selector)
{
    ui->setupUi(this);
    fGrid = new C5Grid(dbParams, nullptr);
    fGrid->fTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->hl->addWidget(fGrid);
    connect(fGrid, SIGNAL(tblDoubleClick(int, int, QVector<QJsonValue>)), this, SLOT(tblDoubleClicked(int, int,
            QVector<QJsonValue>)));
    //connect(fGrid, SIGNAL(tblSingleClick(QModelIndex)), this, SLOT(tblSingleClick(QModelIndex)));
    connect(fGrid->fTableView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this,
            SLOT(selectionChanged(QItemSelection, QItemSelection)));
    fReset = true;
    fSearchColumn = -1;
    fCache = 0;
}

C5Selector::~C5Selector()
{
    delete ui;
}

bool C5Selector::getValue(const QStringList &dbParams, int cache, QVector<QJsonValue> &values)
{
    QString cacheName = C5Cache::cacheName(dbParams, cache);
    C5Selector *c = nullptr;
    if (!fSelectorList[cacheName].contains(cache)) {
        c = new C5Selector(dbParams);
        c->fQuery = C5Cache(dbParams).query(cache);
        c->fCache = cache;
        fSelectorList[cacheName][cache] = c;
    } else {
        c = fSelectorList[cacheName][cache];
    }
    c->fSearchColumn = -1;
    c->fMultipleSelection = false;
    c->fGrid->fModel->setSingleCheckBoxSelection(true);
    if (c->fReset) {
        c->fReset = false;
        c->refresh();
        c->on_leFilter_textChanged(c->ui->leFilter->text());
    }
    c->fGrid->fTableView->resizeColumnsToContents();
    c->ui->leFilter->setFocus();
    bool result = c->exec() == QDialog::Accepted;
    values = c->fValues;
    return result;
}

bool C5Selector::getValue(const QStringList &dbParams, const QString &query, QVector<QJsonValue> &values)
{
    C5Selector *c = new C5Selector(dbParams);
    c->fQuery = query;
    c->fSearchColumn = -1;
    c->fMultipleSelection = false;
    c->fGrid->fModel->setSingleCheckBoxSelection(true);
    c->refresh();
    c->fGrid->fTableView->resizeColumnsToContents();
    c->ui->leFilter->setFocus();
    bool result = c->exec() == QDialog::Accepted;
    values = c->fValues;
    c->deleteLater();
    return result;
}

bool C5Selector::getValueOfColumn(const QStringList &dbParams, int cache, QVector<QJsonValue> &values, int column)
{
    QString cacheName = C5Cache::cacheName(dbParams, cache);
    C5Selector *c = nullptr;
    if (!fSelectorList[cacheName].contains(cache)) {
        c = new C5Selector(dbParams);
        c->fQuery = C5Cache(dbParams).query(cache).replace("%idcond%", "");
        c->fCache = cache;
        fSelectorList[cacheName][cache] = c;
    } else {
        c = fSelectorList[cacheName][cache];
    }
    c->fSearchColumn = column;
    c->fMultipleSelection = false;
    c->fGrid->fModel->setSingleCheckBoxSelection(true);
    if (c->fReset) {
        c->fReset = false;
        c->refresh();
    }
    c->ui->leFilter->setFocus();
    bool result = c->exec() == QDialog::Accepted;
    values = c->fValues;
    return result;
}

bool C5Selector::getMultipleValues(const QStringList &dbParams, int cache, QList<QVector<QJsonValue> > &values)
{
    QString cacheName = C5Cache::cacheName(dbParams, cache);
    C5Selector *c = nullptr;
    if (!fSelectorList[cacheName].contains(cache)) {
        c = new C5Selector(dbParams);
        c->fQuery = C5Cache(dbParams).query(cache).replace("%idcond%", "");
        c->fCache = cache;
        fSelectorList[cacheName][cache] = c;
    } else {
        c = fSelectorList[cacheName][cache];
    }
    c->fMultipleSelection = true;
    c->fGrid->fModel->setSingleCheckBoxSelection(false);
    if (c->fReset) {
        c->fReset = false;
        c->refresh();
    }
    c->ui->leFilter->setFocus();
    bool result = c->exec() == QDialog::Accepted;
    if (result) {
        for (int i = 0; i < c->fGrid->fModel->rowCount(); i++) {
            if (c->fGrid->fModel->data(i, 0, Qt::CheckStateRole) == Qt::Checked) {
                values.append(c->fGrid->fModel->getRowValues(i));
            }
        }
    }
    return result;
}

bool C5Selector::getValues(const QStringList &dbParams, const QString &sql, QVector<QJsonValue> &values,
                           const QHash<QString, QString> &translator)
{
    C5Selector *c = new C5Selector(dbParams);
    c->fQuery = sql;
    c->fGrid->fTranslation = translator;
    c->refresh();
    bool result = c->exec() == QDialog::Accepted;
    values = c->fValues;
    delete c;
    return result;
}

void C5Selector::keyPressEvent(QKeyEvent *key)
{
    int row = fGrid->fTableView->currentIndex().row();
    QModelIndex index = fGrid->fTableView->model()->index(row, 0);
    switch (key->key()) {
        case Qt::Key_Up:
            if (row > 0) {
                row--;
            }
            break;
        case Qt::Key_Down:
            if (row < fGrid->fModel->rowCount() - 1) {
                row++;
            }
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            if (key->modifiers() &Qt::ControlModifier) {
                fGrid->on_tblView_doubleClicked(index);
            }
            break;
        default:
            break;
    }
    index = fGrid->fTableView->model()->index(row, 0);
    fGrid->fTableView->setCurrentIndex(index);
    C5Dialog::keyPressEvent(key);
}

void C5Selector::resetCache(const QStringList &dbParams, int cacheId)
{
    QString cacheName = C5Cache::cacheName(dbParams, cacheId);
    if (fSelectorList.contains(cacheName)) {
        if (fSelectorList[cacheName].contains(cacheId)) {
            fSelectorList[cacheName][cacheId]->fReset = true;
        }
    }
}

void C5Selector::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    for (QModelIndex i : deselected.indexes()) {
        fGrid->fModel->setData(i.row(), 0, 0, Qt::CheckStateRole);
    }
    for (QModelIndex i : selected.indexes()) {
        fGrid->fModel->setData(i.row(), 0, 1, Qt::CheckStateRole);
    }
}

bool C5Selector::tblDoubleClicked(int row, int column, const QVector<QJsonValue> &values)
{
    Q_UNUSED(column);
    Q_UNUSED(row);
    fValues = values;
    if (fValues.count() > 0) {
        accept();
    }
    return true;
}

void C5Selector::on_btnRefreshCache_clicked()
{
    refresh();
}

void C5Selector::refresh()
{
    if (fCache > 0) {
        C5Cache::cache(fDBParams, fCache)->refresh();
    }
    fGrid->setCheckboxes(true);
    fGrid->fModel->setSingleCheckBoxSelection(!fMultipleSelection);
    fGrid->buildQuery(fQuery.replace("%idcond%", ""));
    fGrid->fTableView->setColumnWidth(0, 25);
}

void C5Selector::on_leFilter_textChanged(const QString &arg1)
{
    fGrid->setFilter(fSearchColumn, arg1);
}

void C5Selector::on_btnCheck_clicked()
{
    for (int i = 0; i < fGrid->fModel->rowCount(); i++) {
        if (fGrid->fModel->data(i, 0, Qt::CheckStateRole) == Qt::Checked) {
            fValues = fGrid->fModel->getRowValues(i);
            accept();
            return;
        }
    }
}
