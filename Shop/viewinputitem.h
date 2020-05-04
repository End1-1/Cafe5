#ifndef VIEWINPUTITEM_H
#define VIEWINPUTITEM_H

#include <QDialog>

namespace Ui {
class ViewInputItem;
}

class ViewInputItem : public QDialog
{
    Q_OBJECT

public:
    explicit ViewInputItem(const QString &code, QWidget *parent = nullptr);

    ~ViewInputItem();

private:
    Ui::ViewInputItem *ui;
};

#endif // VIEWINPUTITEM_H
