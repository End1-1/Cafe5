#ifndef DLGCREDITCARDLIST_H
#define DLGCREDITCARDLIST_H

#include <QDialog>

namespace Ui {
class DlgCreditCardList;
}

class DlgCreditCardList : public QDialog
{
    Q_OBJECT

public:
    explicit DlgCreditCardList(QWidget *parent = 0);

    ~DlgCreditCardList();

    static bool getCardInfo(int &id, QString &name);

private slots:
    void on_lst_clicked(const QModelIndex &index);

private:
    Ui::DlgCreditCardList *ui;

    int fId;

    QString fName;
};

#endif // DLGCREDITCARDLIST_H
