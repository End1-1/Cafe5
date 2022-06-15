#ifndef C5REPORTTEMPLATEDRIVER_H
#define C5REPORTTEMPLATEDRIVER_H

#include <QObject>
#include <QHash>

namespace TemplateDriver {

typedef struct {
    int paramType;
    QString name;
    QString replaceText;
} TemplateParam;

typedef struct {
    int id;
    int reportGroup;
    QString name;
    QString sql;
    QList<TemplateParam> params;
} Template;

}


Q_DECLARE_METATYPE(TemplateDriver::Template)

class C5ReportTemplateDriver : public QObject
{
    Q_OBJECT
public:
    explicit C5ReportTemplateDriver();

    static void init(int reportGroup);

    static void getTemplatesList(int reportGroup, QList<TemplateDriver::Template> &list);

private:
    static C5ReportTemplateDriver *fTemplateDriver;

    void getReportsTemplates(int group);

    QHash<int, TemplateDriver::Template> fTemplates;

};

#endif // C5REPORTTEMPLATEDRIVER_H
