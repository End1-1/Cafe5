#include "c5gridgilter.h"
#include "ui_c5gridgilter.h"
#include "c5filterwidget.h"
#include <QListWidgetItem>
#include <QCheckBox>

C5GridGilter::C5GridGilter(QWidget *parent) :
    C5Dialog(QStringList(), parent),
    ui(new Ui::C5GridGilter)
{
    ui->setupUi(this);
}

C5GridGilter::~C5GridGilter()
{
    delete ui;
}

bool C5GridGilter::filter(C5FilterWidget *filterWidget, QString &condition, QMap<QString, bool> &showColumns, QMap<QString, QString> &colTranslation)
{
    C5GridGilter *gf = new C5GridGilter(C5Config::fParentWidget);
    filterWidget->restoreFilter(filterWidget);
    gf->ui->vl->addWidget(filterWidget);
    if (filterWidget->children().count() > 0) {
        QObjectList ol = filterWidget->children();
        foreach (QObject *o, ol) {
            QWidget *w = dynamic_cast<QWidget*>(o);
            if (w) {
                if (w->focusPolicy() == Qt::StrongFocus) {
                    w->setFocus();
                    break;
                }
            }
        }
    }
    QListWidget *lv = gf->ui->lvColumns;
    for (QMap<QString, bool>::const_iterator it = showColumns.begin(); it != showColumns.end(); it++) {
        QListWidgetItem *item = new QListWidgetItem(lv);
        item->setData(Qt::UserRole, it.key());

        QString s = it.key();
        int pos = s.indexOf(" as");
        if (pos > -1) {
            s = s.mid(pos + 4, s.length() - pos + 4);
        } else {
            pos = s.indexOf(".");
            s = s.mid(pos + 1, s.length() - pos);
        }

        QCheckBox *c = new QCheckBox(colTranslation[s]);
        c->setChecked(it.value());
        lv->setItemWidget(item, c);
    }
    bool result = gf->exec() == QDialog::Accepted;
    filterWidget->setParent(0);
    if (result) {
        filterWidget->saveFilter(filterWidget);
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
