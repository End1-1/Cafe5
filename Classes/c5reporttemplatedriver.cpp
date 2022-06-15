#include "c5reporttemplatedriver.h"
#include "c5database.h"
#include "c5config.h"

C5ReportTemplateDriver *C5ReportTemplateDriver::fTemplateDriver = nullptr;

C5ReportTemplateDriver::C5ReportTemplateDriver() :
    QObject()
{

}

void C5ReportTemplateDriver::init(int reportGroup)
{
    if (fTemplateDriver == nullptr) {
        fTemplateDriver = new C5ReportTemplateDriver();
    }
    fTemplateDriver->getReportsTemplates(reportGroup);
}

void C5ReportTemplateDriver::getTemplatesList(int reportGroup, QList<TemplateDriver::Template> &list)
{
    for (QHash<int, TemplateDriver::Template>::const_iterator it = fTemplateDriver->fTemplates.constBegin(); it != fTemplateDriver->fTemplates.constEnd(); it++) {
        if (it.value().reportGroup == reportGroup) {
            list.append(it.value());
        }
    }
}

void C5ReportTemplateDriver::getReportsTemplates(int group)
{
    fTemplates.clear();
    C5Database db(__c5config.dbParams());
    db[":f_group"] = group;
    db.exec("select * from s_report_template where f_id in (select f_report from s_report_template_access where f_group=:f_group)");
    while (db.nextRow()) {
        TemplateDriver::Template t;
        t.id = db.getInt("f_id");
        t.reportGroup = db.getInt("f_group");
        t.name = db.getString("f_name");
        t.sql = db.getString("f_sql");
        fTemplates[t.id] = t;
    }
    db[":f_group"] = group;
    db.exec("select f_report, f_param_type, f_param_name, f_replace_text "
            "from s_report_template_params where f_report in ("
            "select f_id from s_report_template where f_id in (select f_report from s_report_template_access where f_group=:f_group) "
            ")");
    while (db.nextRow()) {
        if (!fTemplates.contains(db.getInt("f_report"))) {
            continue;
        }
        TemplateDriver::Template &t = fTemplates[db.getInt("f_report")];
        TemplateDriver::TemplateParam p;
        p.paramType = db.getInt("f_param_type");
        p.name = db.getString("f_param_name");
        p.replaceText = db.getString("f_replace_text");
        t.params.append(p);
    }
}
