#ifndef CR5GOODSMOVEMENTFILTER_H
#define CR5GOODSMOVEMENTFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5GoodsMovementFilter;
}

class CR5GoodsMovementFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5GoodsMovementFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5GoodsMovementFilter();

    virtual QString condition();

    virtual QString filterText();

    void setDate(const QDate &d1, const QDate &d2);

    void setDocType(const QString &docType);

    void setStore(const QString &store);

    void setGoods(const QString &goods);

    void setReason(const QString &reason);

    void setInOut(int inout);

    QString date1s() const;

    QString date2s() const;

    QString partners() const;

private:
    Ui::CR5GoodsMovementFilter *ui;
};

#endif // CR5GOODSMOVEMENTFILTER_H
