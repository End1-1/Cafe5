#include "wdashboard.h"
#include "c5mainwindow.h"
#include "ui_wdashboard.h"
#include "rabstracteditorreport.h"
#include <QDialog>

WDashboard::WDashboard(QWidget *parent)
    : C5Widget(parent),
      ui(new Ui::WDashboard)
{
    ui->setupUi(this);
    fLabel = tr("Dashboard");
    auto buttons = findChildren<QAbstractButton*>();

    for(auto *btn : std::as_const(buttons)) {
        if(btn->property("form").isValid()) {
            connect(btn, &QAbstractButton::clicked, this, &WDashboard::onCommandButtonClicked);
        }
    }
}

WDashboard::~WDashboard() { delete ui; }

void WDashboard::onCommandButtonClicked()
{
    auto *btn = qobject_cast<QAbstractButton*>(sender());

    if(!btn)
        return;

    QString form = btn->property("form").toString();
    auto *w = createForm(form, btn->icon());
    auto *dialog = qobject_cast<QDialog*>(w);

    if(dialog) {
        dialog->exec();
        return;
    }

    auto *c5widget = qobject_cast<C5Widget*>(w);

    if(c5widget) {
        __mainWindow->addWidget(c5widget);
        return;
    }
}

QWidget* WDashboard::createForm(const QString &name, QIcon icon)
{
    if(name == "Workstations") {
        return new RAbstractEditorReport(tr("Workstations"), icon, name);
    }

    Q_ASSERT_X(false, "check name", QString("NO WIDGET NAMED %1 ").arg(name).toLatin1());
    return nullptr;
}
