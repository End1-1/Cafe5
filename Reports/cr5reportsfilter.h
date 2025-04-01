#ifndef CR5REPORTSFILTER_H
#define CR5REPORTSFILTER_H

#include "c5filterwidget.h"

namespace Ui
{
class CR5ReportsFilter;
}

class CR5ReportsFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5ReportsFilter(const QStringList &dbParams, QWidget *parent = nullptr);
    ~CR5ReportsFilter();

    void setFields(const QStringList &cache);

    virtual QString condition() override;

    QString replacement();

    QString d1();

    QString d2();

private:
    Ui::CR5ReportsFilter *ui;

    QMap<int, C5LineEditWithSelector *> fCache;

    void addFilterField(const QString &title, const QString &data, int row);
};

#endif // CR5REPORTSFILTER_H
