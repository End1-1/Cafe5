#ifndef CR5STOREDOCUMENTSFILTER_H
#define CR5STOREDOCUMENTSFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5StoreDocumentsFilter;
}

class CR5StoreDocumentsFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5StoreDocumentsFilter(const QStringList &dbParams, QWidget *parent = 0);

    ~CR5StoreDocumentsFilter();

    virtual QString condition();

    QString storages();

private:
    Ui::CR5StoreDocumentsFilter *ui;
};

#endif // CR5STOREDOCUMENTSFILTER_H
