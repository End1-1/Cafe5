#include "c5dlgselectreporttemplate.h"
#include "ui_c5dlgselectreporttemplate.h"

C5DlgSelectReportTemplate::C5DlgSelectReportTemplate(int reportGroup) :
    C5Dialog(),
    ui(new Ui::C5DlgSelectReportTemplate)
{
    ui->setupUi(this);
    QList<TemplateDriver::Template> templates;
    C5ReportTemplateDriver::getTemplatesList(reportGroup, templates);
    for (TemplateDriver::Template &t : templates) {
        QListWidgetItem *item = new QListWidgetItem(ui->lst);
        item->setData(Qt::UserRole, QVariant::fromValue(t));
        item->setText(t.name);
        ui->lst->addItem(item);
    }
    TemplateDriver::Template t;
    t.id = 0;
    QListWidgetItem *item = new QListWidgetItem(ui->lst);
    item->setData(Qt::UserRole, QVariant::fromValue(t));
    item->setText(tr("Cancel"));
    ui->lst->addItem(item);
}

C5DlgSelectReportTemplate::~C5DlgSelectReportTemplate()
{
    delete ui;
}

void C5DlgSelectReportTemplate::on_lst_itemClicked(QListWidgetItem *item)
{
    TemplateDriver::Template t = item->data(Qt::UserRole).value<TemplateDriver::Template>();
    if (t.id == 0) {
        reject();
        return;
    }
    fSelectedTemplate = t;
    accept();
}
