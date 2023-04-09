#ifndef COMBOSEARCHVIEW_H
#define COMBOSEARCHVIEW_H

#include <QWidget>

namespace Ui {
class ComboSearchView;
}

class ComboSearchView : public QWidget
{
    Q_OBJECT

public:
    explicit ComboSearchView(QWidget *parent = nullptr);
    ~ComboSearchView();

private:
    Ui::ComboSearchView *ui;
};

#endif // COMBOSEARCHVIEW_H
