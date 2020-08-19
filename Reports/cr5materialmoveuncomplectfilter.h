#ifndef CR5MATERIALMOVEUNCOMPLECTFILTER_H
#define CR5MATERIALMOVEUNCOMPLECTFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5MaterialmoveUncomplectFilter;
}

class CR5MaterialmoveUncomplectFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5MaterialmoveUncomplectFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5MaterialmoveUncomplectFilter();

    virtual QString condition() override;

    virtual QString cond();

    const QString unit() const;

    const QString store() const;

    const QString goods() const;

    const QString group() const;

    int viewMode();

private:
    Ui::CR5MaterialmoveUncomplectFilter *ui;
};

#endif // CR5MATERIALMOVEUNCOMPLECTFILTER_H
