#ifndef CR5TSTOREEXTRAFILTER_H
#define CR5TSTOREEXTRAFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5TStoreExtraFilter;
}

class CR5TStoreExtraFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5TStoreExtraFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5TStoreExtraFilter();

    virtual QString condition();

    QDate dateStart();

    QDate dateEnd();

    int store();

private:
    Ui::CR5TStoreExtraFilter *ui;
};

#endif // CR5TSTOREEXTRAFILTER_H
