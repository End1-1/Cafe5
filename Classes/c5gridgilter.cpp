#include "c5gridgilter.h"
#include "ui_c5gridgilter.h"
#include "c5filterwidget.h"
#include <QListWidgetItem>
#include <QCheckBox>

C5GridGilter::C5GridGilter(QWidget *parent) :
    C5Dialog(parent),
    ui(new Ui::C5GridGilter)
{
    ui->setupUi(this);
}

C5GridGilter::~C5GridGilter()
{
    delete ui;
}

bool C5GridGilter::filter(C5FilterWidget *filterWidget, QString &condition, QMap<QString, bool> &showColumns)
{
    C5GridGilter *gf = new C5GridGilter(C5Config::fParentWidget);
    gf->ui->vl->addWidget(filterWidget);
    QListWidget *lv = gf->ui->lvColumns;
    for (QMap<QString, bool>::const_iterator it = showColumns.begin(); it != showColumns.end(); it++) {
        QListWidgetItem *item = new QListWidgetItem(lv);
        item->setData(Qt::UserRole, it.key());
        QCheckBox *c = new QCheckBox(it.key());
        c->setChecked(it.value());
        lv->setItemWidget(item, c);
    }
    bool result = gf->exec() == QDialog::Accepted;
    filterWidget->setParent(0);
    if (result) {
        condition = filterWidget->condition();
        for (int i = 0; i < lv->count(); i++) {
            QCheckBox *c = static_cast<QCheckBox*>(lv->itemWidget(lv->item(i)));
            showColumns[lv->item(i)->data(Qt::UserRole).toString()] = c->isChecked();
        }
    }
    delete gf;
    return result;
}

void C5GridGilter::on_btnCancel_clicked()
{
    reject();
}

void C5GridGilter::on_btnOK_clicked()
{
    accept();
}
