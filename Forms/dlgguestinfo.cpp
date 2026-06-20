#include "dlgguestinfo.h"
#include "dlgtext.h"
#include "ui_dlgguestinfo.h"
#include <QDialog>
#include <QJsonArray>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPointer>
#include <QRegularExpression>
#include <QVBoxLayout>
#include "c5user.h"
#include "ninterface.h"
#include "rkeyboard.h"

DlgGuestInfo::DlgGuestInfo(C5User *user) :
    C5Dialog(user),
    ui(new Ui::DlgGuestInfo)
{
    ui->setupUi(this);
}

DlgGuestInfo::~DlgGuestInfo()
{
    delete ui;
}

void DlgGuestInfo::setInfo(const QJsonObject &g)
{
    ui->leContactName->setText(g.value("f_guest_name").toString());
    ui->lePhoneNumber->setText(displayPhone(g.value("f_guest_phone").toString()));
    ui->leAddress->setText(g.value("f_guest_address").toString());
}

QJsonObject DlgGuestInfo::getInfo() const
{
    return {{"f_guest_name", ui->leContactName->text()},
            {"f_guest_phone", normalizePhone(ui->lePhoneNumber->text())},
            {"f_guest_address", ui->leAddress->text()}};
}

void DlgGuestInfo::on_btnCancel_clicked()
{
    reject();
}

void DlgGuestInfo::on_btnSave_clicked()
{
    accept();
}

void DlgGuestInfo::on_btnEditAddress_clicked()
{
    QString txt;
    if (!DlgText::getText(mUser, tr("Address"), txt)) {
        return;
    }
    ui->leAddress->setText(txt);
}

void DlgGuestInfo::on_btnEditPhone_clicked()
{
    QString txt;
    if (!DlgText::getText(mUser, tr("Phone"), txt)) {
        return;
    }
    ui->lePhoneNumber->setText(displayPhone(txt));
}

void DlgGuestInfo::on_btnEditContact_clicked()
{
    QString txt;
    if (!DlgText::getText(mUser, tr("Contact"), txt)) {
        return;
    }
    ui->leContactName->setText(txt);
}

QString DlgGuestInfo::normalizePhone(const QString &phone)
{
    QString digits = phone;
    digits.remove(QRegularExpression("[^0-9]"));

    if(digits.startsWith("374")) {
        digits.remove(0, 3);
    } else if(digits.startsWith("0")) {
        digits.remove(0, 1);
    }

    if(digits.length() > 8) {
        digits = digits.left(8);
    }

    return QString("+374%1").arg(digits);
}

QString DlgGuestInfo::displayPhone(const QString &phone)
{
    QString normalized = normalizePhone(phone);

    if(normalized.length() != 12) {
        return normalized;
    }

    const QString local = normalized.mid(4); // xxyyyyyy
    return QString("+374 %1 %2-%3")
            .arg(local.mid(0, 2))
            .arg(local.mid(2, 3))
            .arg(local.mid(5, 3));
}

void DlgGuestInfo::on_btnSearch_clicked()
{
    QDialog dlg(this);
    dlg.setObjectName(QStringLiteral("guestSearchDialog"));
    dlg.setWindowTitle(tr("Search customer"));

    auto *v = new QVBoxLayout(&dlg);
    v->setContentsMargins(2, 2, 2, 2);
    v->setSpacing(2);

    auto *hint = new QLabel(tr("Enter name or phone (min 2 characters)"));
    v->addWidget(hint);

    auto *list = new QListWidget();
    list->setObjectName(QStringLiteral("guestSearchList"));
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    v->addWidget(list, 1);

    auto *kb = new RKeyboard();
    v->addWidget(kb);

    dlg.resize(700, 720);

    QPointer<QListWidget> listPtr(list);

    connect(kb, &RKeyboard::textChanged, this, [this, listPtr](const QString &text) {
        if(!listPtr) {
            return;
        }

        const QString q = text.trimmed();
        if(q.length() <= 1) {
            listPtr->clear();
            return;
        }

        NInterface::query1(QStringLiteral("/engine/v2/waiter/customer/search"),
                           mUser->mSessionKey,
                           this,
                           {{"query", q}},
                           [listPtr](const QJsonObject &jo) {
                               if(!listPtr) {
                                   return;
                               }

                               listPtr->clear();
                               const QJsonArray arr = jo.value(QStringLiteral("customers")).toArray();
                               for(const QJsonValue &cv : arr) {
                                   const QJsonObject c = cv.toObject();
                                   const QString name = c.value(QStringLiteral("f_name")).toString();
                                   const QString phone = displayPhone(c.value(QStringLiteral("f_phone")).toString());
                                   const QString address = c.value(QStringLiteral("f_address")).toString();
                                   QStringList lines;
                                   lines << QString("%1   %2").arg(name, phone);
                                   if(!address.isEmpty()) {
                                       lines << address;
                                   }
                                   auto *it = new QListWidgetItem(lines.join('\n'));
                                   it->setData(Qt::UserRole, c);
                                   listPtr->addItem(it);
                               }
                           });
    });

    connect(list, &QListWidget::itemClicked, &dlg, [this, &dlg](QListWidgetItem *item) {
        if(!item) {
            return;
        }

        const QJsonObject c = item->data(Qt::UserRole).toJsonObject();
        ui->leContactName->setText(c.value(QStringLiteral("f_name")).toString());
        ui->lePhoneNumber->setText(displayPhone(c.value(QStringLiteral("f_phone")).toString()));
        ui->leAddress->setText(c.value(QStringLiteral("f_address")).toString());
        dlg.accept();
    });

    connect(kb, &RKeyboard::reject, &dlg, &QDialog::reject);

    dlg.exec();
}
