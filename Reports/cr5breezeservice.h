#ifndef CR5BREEZESERVICE_H
#define CR5BREEZESERVICE_H

#include "c5reportwidget.h"

namespace Ui {
class CR5BreezeService;
}

class CR5BreezeService : public C5ReportWidget
{
    Q_OBJECT

public:
    explicit CR5BreezeService(const QStringList &dbParams, QWidget *parent = nullptr);
    ~CR5BreezeService();

private:
    Ui::CR5BreezeService *ui;
};

#endif // CR5BREEZESERVICE_H
