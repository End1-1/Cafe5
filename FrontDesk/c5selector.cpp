#include "c5selector.h"
#include "ui_c5selector.h"
#include "c5cache.h"
#include "c5grid.h"
#include "c5tablemodel.h"
#include <QKeyEvent>

QMap<QString, QMap<int, C5Selector*> > C5Selector::fSelectorList;

C5Selector::C5Selector(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::C5Selector)
{
    ui->setupUi(this);
    fGrid = new C5Grid(dbParams, nullptr);
    fGrid->fTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->hl->addWidget(fGrid);
    connect(fGrid, SIGNAL(tblDoubleClicked(int,int,QList<QVariant>)), this, SLOT(tblDoubleClicked(int,int,QList<QVariant>)));
    fReset = true;
}

C5Selector::~C5Selector()
{
    delete ui;
}

bool C5Selector::getValue(const QStringList &dbParams, int cache, QList<QVariant> &values)
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
    if (c->fReset) {
        c->fReset = false;
        c->refresh();
    }
    c->ui->leFilter->setFocus();
    bool result = c->exec() == QDialog::Accepted;
    values = c->fValues;
    return result;
}

bool C5Selector::getValues(const QStringList &dbParams, const QString &sql, QList<QVariant> &values, const QMap<QString, QString> &translator)
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
        if (key->modifiers() & Qt::ControlModifier) {
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

void C5Selector::tblDoubleClicked(int row, int column, const QList<QVariant> &values)
{
    Q_UNUSED(column);
    Q_UNUSED(row);
    fValues = values;
    if (fValues.count() > 0) {
        accept();
    }
}

void C5Selector::on_btnRefreshCache_clicked()
{
    refresh();
}

void C5Selector::refresh()
{
    C5Cache::cache(fDBParams, fCache)->refresh();
    fGrid->buildQuery(fQuery);
}

void C5Selector::on_leFilter_textChanged(const QString &arg1)
{
    fGrid->setFilter(-1, arg1);
}
