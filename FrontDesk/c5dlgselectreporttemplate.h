#ifndef C5DLGSELECTREPORTTEMPLATE_H
#define C5DLGSELECTREPORTTEMPLATE_H

#include "c5dialog.h"
#include "c5reporttemplatedriver.h"

namespace Ui {
class C5DlgSelectReportTemplate;
}

class QListWidgetItem;

class C5DlgSelectReportTemplate : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5DlgSelectReportTemplate(int reportGroup, const QStringList &dbParams);

    ~C5DlgSelectReportTemplate();

    TemplateDriver::Template fSelectedTemplate;

private slots:
    void on_lst_itemClicked(QListWidgetItem *item);

private:
    Ui::C5DlgSelectReportTemplate *ui;
};

#endif // C5DLGSELECTREPORTTEMPLATE_H
