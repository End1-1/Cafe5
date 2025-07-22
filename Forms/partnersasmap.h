#ifndef PARTNERSASMAP_H
#define PARTNERSASMAP_H

#include "c5widget.h"

namespace Ui {
class PartnersAsMap;
}

class PartnersAsMap : public C5Widget
{
    Q_OBJECT

public:
    explicit PartnersAsMap(QWidget *parent = nullptr);
    ~PartnersAsMap();
    virtual QToolBar *toolBar() override;

public slots:
    void saveDataChanges();

private:
    Ui::PartnersAsMap *ui;
    QMap<int, int> asList;
    QMap<int, int> asColId;
};

#endif // PARTNERSASMAP_H
