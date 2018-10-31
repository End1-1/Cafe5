#include "c5selector.h"
#include "ui_c5selector.h"
#include "c5cache.h"
#include "c5grid.h"

QMap<QString, QMap<int, C5Selector*> > C5Selector::fSelectorList;

C5Selector::C5Selector(const QStringList &dbParams, QWidget *parent) :
    C5Dialog(dbParams, parent),
    ui(new Ui::C5Selector)
{
    ui->setupUi(this);
    fGrid = new C5Grid(dbParams, 0);
    ui->hl->addWidget(fGrid);
    connect(fGrid, SIGNAL(tblDoubleClicked(int,int,QList<QVariant>)), this, SLOT(tblDoubleClicked(int,int,QList<QVariant>)));
}

C5Selector::~C5Selector()
{
    delete ui;
}

bool C5Selector::getValue(const QStringList &dbParams, int cache, QList<QVariant> &values)
{
    QString cacheName = C5Cache::cacheName(dbParams, cache);
    C5Selector *c = 0;
    if (!fSelectorList[cacheName].contains(cache)) {
        c = new C5Selector(dbParams, C5Config::fParentWidget);
        c->fQuery = C5Cache(dbParams).query(cache);
        c->fCache = cache;
        c->refresh();
        fSelectorList[cacheName][cache] = c;
    } else {
        c = fSelectorList[cacheName][cache];
    }
    values.clear();
    c->ui->leFilter->setFocus();
    bool result = c->exec() == QDialog::Accepted;
    values = c->fValues;
    return result;
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
