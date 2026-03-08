#include "rfilterdialog.h"
#include <QIntValidator>
#include <QJsonObject>
#include <QLabel>
#include <QShowEvent>
#include "c5codenameselector.h"
#include "c5codenameselectorfunctions.h"
#include "c5config.h"
#include "c5dateedit.h"
#include "c5lineedit.h"
#include "format_date.h"
#include "ui_rfilterdialog.h"

RFilterDialog::RFilterDialog(C5User *user)
    : C5Dialog(user),
      ui(new Ui::RFilterDialog)
{
    ui->setupUi(this);
}

RFilterDialog::~RFilterDialog()
{
    delete ui;
}

void RFilterDialog::buildWidget(const QString &settingsPrefix, const QJsonArray &ja)
{
    mSettingsPrefix = settingsPrefix;
    int row = 0;

    for(int i = 0; i < ja.size(); i++) {
        auto const &jo = ja.at(i).toObject();

        if(jo.value("type").toString() == "date") {
            ui->gl->addWidget(new QLabel(jo.value("label").toString()), row, 0);
            auto  *ed = new C5DateEdit();
            QDate defaultDate = __c5config.getRegValue(QString("rfilter_%1_%2").arg(mSettingsPrefix, jo.value("name").toString()), QDate::currentDate()).toDate();
            ed->setDate(defaultDate);
            ui->gl->addWidget(ed, row, 1);
            ed->setProperty("name", jo.value("name").toString());
        }

        if(jo.value("type").toString() == "number") {
            ui->gl->addWidget(new QLabel(jo.value("label").toString()), row, 0);
            auto *en = new C5LineEdit();
            en->setValidator(new QIntValidator());
            en->setProperty("name", jo.value("name").toString());
            int defaultNumber = __c5config.getRegValue(QString("rfilter_%1_%2").arg(mSettingsPrefix, jo.value("name").toString()), 0).toInt();

            if(defaultNumber > 0) {
                en->setInteger(defaultNumber);
            }

            ui->gl->addWidget(en, row, 1);
        }

        if (jo.value("type").toString() == "keyvalue") {
            auto *cn = new C5CodeNameSelector();
            cn->setProperty("name", jo.value("name").toString());
            cn->setSelectorName(jo.value("label").toString());
            if (jo.value("function").toString() == "store") {
                cn->selectorCallback = storeItemSelector;
            } else if (jo.value("function").toString() == "store_doc_status") {
                cn->selectorCallback = storeDocStatusItemSelector;
            } else if (jo.value("function").toString() == "store_doc_type") {
                cn->selectorCallback = storeDocTypeItemSelector;
            } else if (jo.value("function").toString() == "partner") {
                cn->selectorCallback = partnerItemSelector;
            }
            ui->gl->addWidget(cn, row, 0, 1, 3);
        }

        row++;
    }

    ui->gl->setRowStretch(row, 1);
    ui->gl->setSpacing(6);
    ui->gl->setColumnMinimumWidth(0, 200);
    ui->gl->layout()->setContentsMargins(0, 0, 0, 0);
}

QJsonArray RFilterDialog::filterValues()
{
    QList<QWidget*> result;
    QJsonArray jfilter;
    auto addparam = [&jfilter](const QString & name, const QJsonValue & value) {
        QJsonObject jo;
        jo[name] = value;
        jfilter.append(jo);
    };

    for(auto *w : findChildren<QWidget*>()) {
        if(w->property("name").isValid()) {
            auto *ed = qobject_cast<C5DateEdit*>(w);

            if(ed) {
                addparam(w->property("name").toString(), ed->date().toString(FORMAT_DATE_TO_STR_MYSQL));
                __c5config.setRegValue(QString("rfilter_%1_%2").arg(mSettingsPrefix, w->property("name").toString()), ed->date());
                continue;
            }

            auto *en = qobject_cast<C5LineEdit*>(w);

            if(en) {
                addparam(w->property("name").toString(), en->getInteger());
                __c5config.setRegValue(QString("rfilter_%1_%2").arg(mSettingsPrefix, w->property("name").toString()), en->getInteger());
                continue;
            }

            auto *kv = qobject_cast<C5CodeNameSelector *>(w);
            if (kv) {
                addparam(w->property("name").toString(), kv->value());
                __c5config.setRegValue(QString("rfilter_%1_%2").arg(mSettingsPrefix, w->property("name").toString()), kv->value());
                continue;
            }
        }
    }

    return jfilter;
}

void RFilterDialog::showEvent(QShowEvent *e)
{
    adjustSize();
    C5Dialog::showEvent(e);
}

void RFilterDialog::on_btnCancel_clicked()
{
    reject();
}

void RFilterDialog::on_btnApply_clicked()
{
    accept();
}
