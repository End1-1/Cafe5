#ifndef CR5USERSFILTER_H
#define CR5USERSFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5UsersFilter;
}

class CR5UsersFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5UsersFilter();
    ~CR5UsersFilter();
    virtual QString condition() override;
    virtual QString filterText() override;

private:
    Ui::CR5UsersFilter *ui;
};

#endif // CR5USERSFILTER_H
