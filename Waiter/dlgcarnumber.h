#ifndef DLGCARNUMBER_H
#define DLGCARNUMBER_H

#include "c5dialog.h"

namespace Ui {
class DlgCarNumber;
}

class QListWidgetItem;


struct Car {
    int fCar;
    int fCostumer;
    QString fNumber;
};

class DlgCarNumber : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgCarNumber();

    ~DlgCarNumber();

    static bool getNumber(int &id);

private slots:
    void kbdText(const QString &txt);

    void kbdAccept();

    void kbdReject();

    void on_btnCar_clicked();

    void on_lst_itemClicked(QListWidgetItem *item);

    void on_btnClose_clicked();

    void on_btnAccept_clicked();

    void on_btnSetNumber_clicked();

    void on_btnCostumer_clicked();

    void on_btnAdditional_clicked();

private:
    Ui::DlgCarNumber *ui;

    static QMap<int, QString> fCarList;

    static QMap<int, Car> fCostumerCars;

    static QMap<int, QString> fCostumerNames;

    static QMap<int, QString> fCostumerPhones;

    int fRecordId;

    bool fModel;

    bool fCarNumber;

    bool fCostumer;

    bool fAdditional;

    void makeMap(const QMap<int, QString> &map);

    void hideKbd();

    void showKbd();

    void resetMode();
};

#endif // DLGCARNUMBER_H
