#include "c5customfilter.h"
#include "ui_c5customfilter.h"
#include "c5dateedit.h"
#include "c5lineeditwithselector.h"
#include <QListWidgetItem>
#include <QRegularExpression>
#include <QLabel>
#include <QLayoutItem>

C5CustomFilter::C5CustomFilter(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::C5CustomFilter)
{
    ui->setupUi(this);
}

C5CustomFilter::~C5CustomFilter()
{
    delete ui;
}

void C5CustomFilter::setQueries(const QStringList &names, const QStringList &sqls)
{
    for (int  i = 0; i < names.count(); i++) {
        QListWidgetItem *item = new QListWidgetItem(ui->lw);
        item->setText(names.at(i));
        item->setData(Qt::UserRole, sqls.at(i));
        ui->lw->addItem(item);
    }
}

QString C5CustomFilter::sql() const
{
    return fSql;
}

void C5CustomFilter::on_btnReject_clicked()
{
    reject();
}

void C5CustomFilter::on_btnOK_clicked()
{
    if (ui->lw->currentRow() < 0) {
        C5Message::error(tr("Nothing was selected"));
        return;
    }
    for (QWidget *w: fWidgets) {
        QString r = w->property("replace").toString();
        if (w->property("t").toString() == "date") {
            C5DateEdit *d = static_cast<C5DateEdit*>(w);
            fSql.replace("%" + r + "%", d->toMySQLDate());
        } else if (w->property("t").toString() == "cache") {
            C5LineEditWithSelector *l = static_cast<C5LineEditWithSelector*>(w);
            if (l->isNotEmpty()) {
                QString cond = r.mid(0, r.indexOf(";"));
                cond.replace("~list", l->text());
                fSql.replace("%" + r + "%", cond);
            } else {
                fSql.replace("%" + r + "%", "");
            }
        }
    }
    accept();
}

void C5CustomFilter::on_lw_itemClicked(QListWidgetItem *item)
{
    fSql = item->data(Qt::UserRole).toString();
    fParams.clear();
    fWidgets.clear();
    QLayoutItem *li;
    while ((li = ui->gl->takeAt(0))) {
        if (li->widget()) {
            li->widget()->deleteLater();
        }
    }
    QStringList duplicate;
    QRegularExpression re("%.*?%");
    int pos = 0;
    ;
    do {
        QRegularExpressionMatch rm = re.match(fSql, pos);
        if (rm.hasMatch()) {
            fParams.append(rm.captured(0).replace("%", ""));
            pos = rm.capturedEnd();
        } else {
            break;
        }
    } while (true);
    int row = 0;
    QStringList availableWidgets;
    availableWidgets.append("date");
    availableWidgets.append("cache");
    for (QString &s: fParams) {
        if (duplicate.contains(s)) {
            continue;
        } else {
            duplicate.append(s);
        }
        QString type = s.mid(s.indexOf(":") + 1, s.length() - s.indexOf(":") + 1);
        if (!availableWidgets.contains(type) && !type.contains("cache")) {
            continue;
        }
        ui->gl->addWidget(new QLabel(s.mid(s.indexOf(";") + 1, s.indexOf(":") - s.indexOf(";") - 1)), row, 0, 1, 1);
        if (type == "date") {
            C5DateEdit *d = new C5DateEdit();
            d->setProperty("replace", s);
            d->setProperty("FilterName", s);
            d->setProperty("FixDate", true);
            d->setProperty("t", "date");
            ui->gl->addWidget(d, row, 1, 1, 2);
            fWidgets.append(d);
        } else if (type.contains("cache")) {
            C5LineEditWithSelector *l1 = new C5LineEditWithSelector();
            C5LineEditWithSelector *l2 = new C5LineEditWithSelector();
            l1->setSelector(fDBParams, l2, type.midRef(5, type.length() - 5).toInt());
            l1->setProperty("FilterName", s);
            ui->gl->addWidget(l1, row, 1, 1, 1);
            ui->gl->addWidget(l2, row, 2, 1, 1);
            l1->setProperty("replace", s);
            l1->setProperty("t", "cache");
            l1->setMaximumWidth(100);
            fWidgets.append(l1);
        }
        row ++;
    }
    ui->gl->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row, 0, 1, 1);
}
