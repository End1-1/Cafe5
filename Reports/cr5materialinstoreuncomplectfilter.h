#ifndef CR5MATERIALINSTOREUNCOMPLECTFILTER_H
#define CR5MATERIALINSTOREUNCOMPLECTFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5MaterialInStoreUncomplectFilter;
}

class CR5MaterialInStoreUncomplectFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5MaterialInStoreUncomplectFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5MaterialInStoreUncomplectFilter();

    virtual QString condition() override;

    const QDate date() const;

    const QString unit() const;

    const QString store() const;

    const QString goods() const;

    const QString group() const;

    int viewMode();

private:
    Ui::CR5MaterialInStoreUncomplectFilter *ui;
};

#endif // CR5MATERIALINSTOREUNCOMPLECTFILTER_H
