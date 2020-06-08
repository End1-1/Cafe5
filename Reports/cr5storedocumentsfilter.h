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
    explicit CR5StoreDocumentsFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5StoreDocumentsFilter();

    virtual QString condition();

    QString storages() const;

    QString reason() const;

    void setPartnerFilter(int partner);

    void setDateFilter(const QDate &d1, const QDate &d2);

    void setPaidFilter(int paid);

private:
    Ui::CR5StoreDocumentsFilter *ui;
};

#endif // CR5STOREDOCUMENTSFILTER_H
