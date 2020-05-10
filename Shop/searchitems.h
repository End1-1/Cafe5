#ifndef SEARCHITEMS_H
#define SEARCHITEMS_H

#include <QDialog>

namespace Ui {
class SearchItems;
}

class SearchItems : public QDialog
{
    Q_OBJECT

public:
    explicit SearchItems(QWidget *parent = nullptr);
    ~SearchItems();

private slots:
    void on_btnSearch_clicked();

    void on_leCode_returnPressed();

private:
    Ui::SearchItems *ui;
};

#endif // SEARCHITEMS_H
