#include "nfilterdlg.h"
#include "ui_nfilterdlg.h"
#include "c5dateedit.h"
#include "c5lineedit.h"
#include "nloadingdlg.h"
#include "nsearchdlg.h"
#include "c5message.h"
#include "c5filelogwriter.h"
#include "c5config.h"
#include "ndataprovider.h"
#include <QCheckBox>
#include <QJsonObject>
#include <QPushButton>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QLabel>
#include <QComboBox>

NFilterDlg::NFilterDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NFilterDlg)
{
    ui->setupUi(this);
}

NFilterDlg::~NFilterDlg()
{
    delete ui;
}

void NFilterDlg::setup(const QJsonArray &fields)
{
    QWidget *first = nullptr;
    for (int i = 0; i < fields.size(); i++) {
        const QJsonObject &jo = fields.at(i).toObject();
        auto *l = new QLabel(jo["title"].toString());
        ui->gl->addWidget(l, i, 0);
        if (jo["type"].toString() == "date") {
            auto *d = new C5DateEdit(this);
            d->setProperty("field", jo["field"].toString());
            d->setProperty("type", jo["type"].toString());
            ui->gl->addWidget(d, i, 1, 1, 4);
            if (!first) {
                first = d;
            }
        } else if (jo["type"].toString() == "date1") {
            auto *d = new C5DateEdit(this);
            d->setProperty("field", jo["field"].toString());
            d->setProperty("type", jo["type"].toString());
            ui->gl->addWidget(d, i, 1);
            if (!first) {
                first = d;
            }
        } else if (jo["type"].toString() == "date2") {
            auto *d = new C5DateEdit(this);
            d->setProperty("field", jo["field"].toString());
            d->setProperty("type", jo["type"].toString());
            ui->gl->addWidget(d, i, 2);
            if (!first) {
                first = d;
            }
        } else if (jo["type"].toString() == "combo") {
            auto *c = new QComboBox(this);
            c->setProperty("field", jo["field"].toString());
            c->setProperty("type", jo["type"].toString());
            QJsonArray values = jo["values"].toArray();
            int cindex = 0;
            for (int i = 0; i < values.size(); i++) {
                QJsonObject co = values.at(i).toObject();
                c->addItem(co["key"].toString(), co["value"].toInt());
                if (co["value"].toInt() == jo["default"].toInt()) {
                    cindex = i;
                }
            }
            c->setCurrentIndex(cindex);
            ui->gl->addWidget(c, i, 1, 1, 4);
        } else if (jo["type"].toString() == "keyvalue") {
            auto *c = new C5LineEdit(this);
            c->setMaximumWidth(60);
            c->setMinimumWidth(60);
            c->setProperty("field", jo["field"].toString());
            c->setProperty("type", jo["type"].toString());
            ui->gl->addWidget(c, i, 1);
            auto *l = new QLabel(this);
            ui->gl->addWidget(l, i, 3, Qt::AlignLeft);
            auto *btn = new QPushButton(QIcon(":/down-arrow-thin.png"), "");
            btn->setProperty("lineedit", QVariant::fromValue(c));
            btn->setProperty("label", QVariant::fromValue(l));
            btn->setProperty("filter", jo["filter"].toString());
            btn->setMaximumWidth(50);
            connect(btn, &QPushButton::clicked, this, &NFilterDlg::openSuggestions);
            ui->gl->addWidget(btn, i, 2, Qt::AlignLeft);
            if (!first) {
                first = c;
            }
        } else if (jo["type"].toString() == "checkbox") {
            auto *ch = new QCheckBox(this);
            ch->setProperty("field", jo["field"].toString());
            ch->setProperty("type", jo["type"].toString());
            ui->gl->addWidget(ch, i, 1);
        } else if (jo["type"].toString() == "text") {
            auto *c = new C5LineEdit(this);
            c->setProperty("field", jo["field"].toString());
            c->setProperty("type", jo["type"].toString());
            ui->gl->addWidget(c, i, 1, 1, 2);
        }
    }
    ui->gl->setColumnStretch(3, 1);
    adjustSize();
    if (first) {
        first->setFocus();
    }
}

QJsonObject NFilterDlg::filter() const
{
    QJsonObject jo;
    const QObjectList &wl = children();
    for (QObject *o : wl) {
        if (!o->property("field").toString().isEmpty()) {
            if (o->property("type").toString() == "date"
                    || o->property("type").toString() == "date1"
                    || o->property("type").toString() == "date2") {
                jo[o->property("field").toString()] = static_cast<C5DateEdit *>(o)->date().toString("yyyy-MM-dd");
            } else if (o->property("type").toString() == "combo") {
                jo[o->property("field").toString()] = static_cast<QComboBox *>(o)->currentData().toInt();
            } else if (o->property("type").toString() == "keyvalue") {
                jo[o->property("field").toString()] = static_cast<QLineEdit *>(o)->text();
            } else if (o->property("type").toString() == "checkbox") {
                jo[o->property("field").toString()] = static_cast<QCheckBox *>(o)->isChecked() ? 1 : 0;
            } else if (o->property("type").toString() == "text") {
                auto *l = static_cast<C5LineEdit *>(o);
                if (l->text().isEmpty() == false) {
                    jo[o->property("field").toString()] = l->text();
                }
            }
        }
    }
    return jo;
}

QVariant NFilterDlg::filterValue(const QString &name)
{
    const QObjectList &wl = children();
    for (QObject *o : wl) {
        if (o->property("field").toString() == name) {
            if (o->property("type").toString() == "date"
                    || o->property("type").toString() == "date1"
                    || o->property("type").toString() == "date2") {
                return static_cast<C5DateEdit *>(o)->date().toString("yyyy-MM-dd");
            } else if (o->property("type").toString() == "combo") {
                return static_cast<QComboBox *>(o)->currentData().toInt();
            } else if (o->property("type").toString() == "keyvalue") {
                return static_cast<QLineEdit *>(o)->text();
            } else if (o->property("type").toString() == "checkbox") {
                return static_cast<QCheckBox *>(o)->isChecked();
            }
        }
    }
    return QVariant();
}

void NFilterDlg::clear()
{
    const QObjectList &wl = children();
    for (QObject *o : wl) {
        auto *btn = dynamic_cast<QPushButton *>(o);
        if (btn) {
            auto *c = btn->property("lineedit").value<C5LineEdit *>();
            auto *l = btn->property("label").value<QLabel *>();
            if (c) {
                c->clear();
            }
            if (l) {
                l->clear();
            }
        }
    }
}

void NFilterDlg::openSuggestions()
{
    auto *btn = static_cast<QPushButton *>(sender());
    auto *c = btn->property("lineedit").value<C5LineEdit *>();
    auto *l = btn->property("label").value<QLabel *>();
    if (mData.contains(btn->property("filter").toString())) {
        QPoint btnPos = c->pos();
        btnPos.setY(btnPos.y() + c->height());
        NSearchDlg f(this);
        f.move(mapToGlobal(btnPos));
        f.setData(mCols[btn->property("filter").toString()], mData[btn->property("filter").toString()]);
        if (f.exec()) {
            c->setText(f.mId);
            l->setText(f.mName);
        }
    } else {
        auto *nd = new NDataProvider(this);
        nd->setProperty("btn", QVariant::fromValue(btn));
        connect(nd, &NDataProvider::started, this, &NFilterDlg::queryStarted);
        connect(nd, &NDataProvider::error, this, &NFilterDlg::queryError);
        connect(nd, &NDataProvider::done, this, &NFilterDlg::queryFinished);
        nd->getData(QString("/engine/reports/filters/%1.php").arg(btn->property("filter").toString()), QJsonObject());
    }
}

void NFilterDlg::queryStarted()
{
    mLoadingDlg = new NLoadingDlg(this);
    mLoadingDlg->open();
}

void NFilterDlg::queryError(const QString &error)
{
    sender()->deleteLater();
    c5loge(error);
    C5Message::error(error);
    mLoadingDlg->deleteLater();
}

void NFilterDlg::queryFinished(const QJsonObject &ba)
{
    mLoadingDlg->reject();
    mLoadingDlg->deleteLater();
    auto *btn = sender()->property("btn").value<QPushButton *>();
    sender()->deleteLater();
    QJsonArray jcols = ba["cols"].toArray();
    QJsonArray jdata = ba["rows"].toArray();
    mData[btn->property("filter").toString()] = jdata;
    mCols[btn->property("filter").toString()] = jcols;
    btn->click();
}

void NFilterDlg::on_btnCancel_clicked()
{
    reject();
}

void NFilterDlg::on_btnApply_clicked()
{
    accept();
}
