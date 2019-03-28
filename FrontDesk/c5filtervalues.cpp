#include "c5filtervalues.h"
#include "ui_c5filtervalues.h"
#include <QCheckBox>

C5FilterValues::C5FilterValues() :
    C5Dialog(QStringList()),
    ui(new Ui::C5FilterValues)
{
    ui->setupUi(this);
}

C5FilterValues::~C5FilterValues()
{
    delete ui;
}

bool C5FilterValues::filterValues(QStringList &values)
{
    C5FilterValues *fv = new C5FilterValues();
    QListWidget *lv = fv->ui->lst;
    for (QStringList::const_iterator it = values.begin(); it != values.end(); it++) {
        QListWidgetItem *item = new QListWidgetItem(lv);
        QCheckBox *c = new QCheckBox(*it);
        lv->addItem(item);
        lv->setItemWidget(item, c);
    }
    bool result = false;
    if (fv->exec() == QDialog::Accepted) {
        values.clear();
        for (int i = 0; i < lv->count(); i++) {
            QCheckBox *c = static_cast<QCheckBox*>(lv->itemWidget(lv->item(i)));
            if (c->checkState() == Qt::Checked) {
                values << c->text();
            }
        }
        result = true;
    }
    delete fv;
    return result;
}

void C5FilterValues::on_leFilter_textChanged(const QString &arg1)
{
    QListWidget *l = ui->lst;
    for (int i = 0, count = l->count(); i < count; i++) {
        QCheckBox *c = static_cast<QCheckBox*>(ui->lst->itemWidget(ui->lst->item(i)));
        bool hidden = !c->text().contains(arg1, Qt::CaseInsensitive);
        l->setItemHidden(l->item(i), hidden);
    }
}

void C5FilterValues::on_buttonBox_rejected()
{
    reject();
}

void C5FilterValues::on_buttonBox_accepted()
{
    accept();
}
