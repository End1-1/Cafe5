#include "wdashboardsettings.h"
#include "ui_wdashboardsettings.h"

#include <QAbstractButton>
#include <QMap>
#include <QListWidgetItem>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

static QString dashboardGroupTitle(const QString &groupName)
{
    if (groupName == "wSaleReport") {
        return QObject::tr("Sale report");
    }
    if (groupName == "wStore") {
        return QObject::tr("Store");
    }
    if (groupName == "wCashbox") {
        return QObject::tr("Cashbox");
    }
    if (groupName == "wGoods") {
        return QObject::tr("Goods");
    }
    if (groupName == "wSystem") {
        return QObject::tr("System");
    }
    return QObject::tr("Other");
}

static bool isDashboardGroupName(const QString &name)
{
    return name == "wCashbox"
            || name == "wGoods"
            || name == "wStore"
            || name == "wSystem"
            || name == "wSaleReport";
}

WDashboardSettings::WDashboardSettings(const QList<QAbstractButton*> &buttons, const QVariantMap &visibility, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::WDashboardSettings)
{
    ui->setupUi(this);

    QMap<QString, QListWidget*> groups;

    for (QAbstractButton *btn : buttons) {
        if (!btn || !btn->property("form").isValid()) {
            continue;
        }
        QWidget *groupWidget = btn->parentWidget();
        while (groupWidget && !isDashboardGroupName(groupWidget->objectName())) {
            groupWidget = groupWidget->parentWidget();
        }
        const QString groupName = groupWidget ? groupWidget->objectName() : QStringLiteral("other");
        QListWidget *list = groups.value(groupName, nullptr);
        if (!list) {
            QWidget *page = new QWidget(ui->tabWidget);
            QVBoxLayout *v = new QVBoxLayout(page);
            v->setContentsMargins(0, 0, 0, 0);
            list = new QListWidget(page);
            v->addWidget(list);
            page->setLayout(v);
            ui->tabWidget->addTab(page, dashboardGroupTitle(groupName));
            groups.insert(groupName, list);
        }

        QListWidgetItem *it = new QListWidgetItem(btn->text(), list);
        it->setData(Qt::UserRole, btn->objectName());
        it->setFlags(it->flags() | Qt::ItemIsUserCheckable);
        const QString key = btn->objectName();
        const bool checked = visibility.contains(key) ? visibility.value(key).toBool() : btn->isVisible();
        it->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
    }
}

WDashboardSettings::~WDashboardSettings()
{
    delete ui;
}

QVariantMap WDashboardSettings::visibilityMap() const
{
    QVariantMap vm;
    for (int t = 0; t < ui->tabWidget->count(); ++t) {
        QWidget *page = ui->tabWidget->widget(t);
        QListWidget *list = page ? page->findChild<QListWidget*>() : nullptr;
        if (!list) {
            continue;
        }
        for (int i = 0; i < list->count(); ++i) {
            const QListWidgetItem *it = list->item(i);
            vm[it->data(Qt::UserRole).toString()] = (it->checkState() == Qt::Checked);
        }
    }
    return vm;
}

void WDashboardSettings::on_btnAll_clicked()
{
    QListWidget *list = currentList();
    if (!list) {
        return;
    }
    for (int i = 0; i < list->count(); ++i) {
        list->item(i)->setCheckState(Qt::Checked);
    }
}

void WDashboardSettings::on_btnNone_clicked()
{
    QListWidget *list = currentList();
    if (!list) {
        return;
    }
    for (int i = 0; i < list->count(); ++i) {
        list->item(i)->setCheckState(Qt::Unchecked);
    }
}

QListWidget *WDashboardSettings::currentList() const
{
    QWidget *page = ui->tabWidget->currentWidget();
    return page ? page->findChild<QListWidget*>() : nullptr;
}
