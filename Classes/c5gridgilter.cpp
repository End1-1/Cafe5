#include "c5gridgilter.h"
#include "ui_c5gridgilter.h"
#include "c5filterwidget.h"
#include <QListWidgetItem>
#include <QCheckBox>

C5GridGilter::C5GridGilter() :
    C5Dialog(),
    ui(new Ui::C5GridGilter)
{
    ui->setupUi(this);
}

C5GridGilter::~C5GridGilter()
{
    delete ui;
}

bool C5GridGilter::filter(C5FilterWidget *filterWidget, QString &condition, QMap<QString, bool> &showColumns,
                          QHash<QString, QString> &colTranslation)
{
    C5GridGilter *gf = new C5GridGilter();
    filterWidget->restoreFilter(filterWidget);
    gf->ui->vl->addWidget(filterWidget);
    if (filterWidget->children().count() > 0) {
        QObjectList ol = filterWidget->children();
        foreach (QObject *o, ol) {
            QWidget *w = dynamic_cast<QWidget *>(o);
            if (w) {
                if (w->focusPolicy() == Qt::StrongFocus) {
                    w->setFocus();
                    break;
                }
            }
        }
    }
    QListWidget *lv = gf->ui->lvColumns;
    QStringList keys;
    QMap<QString, QString> keyvalue;
    for (QMap<QString, bool>::const_iterator it = showColumns.constBegin(); it != showColumns.constEnd(); it++) {
        QString s = it.key();
        int pos = s.indexOf(" as");
        if (pos > -1) {
            s = s.mid(pos + 4, s.length() - pos + 4);
        } else {
            pos = s.indexOf(".");
            s = s.mid(pos + 1, s.length() - pos);
        }
        keys.append(s);
        keyvalue[s] = it.key();
    }
    std::sort(keys.begin(), keys.end());
    QStringList trans = colTranslation.values();
    std::sort(trans.begin(), trans.end());
    for (const QString &tt : trans) {
        auto s = colTranslation.key(tt);
        QListWidgetItem *item = new QListWidgetItem(lv);
        item->setData(Qt::UserRole, keyvalue[s]);
        QCheckBox *c = new QCheckBox(colTranslation[s].replace("\n", " ").replace("\r", " "));
        c->setChecked(showColumns[keyvalue[s]]);
        lv->setItemWidget(item, c);
    }
    bool result = gf->exec() == QDialog::Accepted;
    filterWidget->setParent(nullptr);
    if (result) {
        filterWidget->saveFilter(filterWidget);
        condition = filterWidget->condition();
        for (int i = 0; i < lv->count(); i++) {
            QCheckBox *c = static_cast<QCheckBox *>(lv->itemWidget(lv->item(i)));
            showColumns[lv->item(i)->data(Qt::UserRole).toString()] = c->isChecked();
        }
    }
    delete gf;
    return result;
}

void C5GridGilter::keyControlPlusEnter()
{
    ui->btnOK->click();
}

void C5GridGilter::keyAlterPlusEnter()
{
}

void C5GridGilter::on_btnCancel_clicked()
{
    reject();
}

void C5GridGilter::on_btnOK_clicked()
{
    accept();
}
