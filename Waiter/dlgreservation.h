#ifndef DLGRESERVATION_H
#define DLGRESERVATION_H

#include "c5dialog.h"

namespace Ui
{
class DlgReservation;
}

class C5User;
class QTableWidgetItem;

class DlgReservation : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgReservation(C5User *u);
    ~DlgReservation();

private slots:
    void openReservationResponse(const QJsonObject &jdoc);

    void on_btnCreateReservation_clicked();

    void on_btnExit_clicked();

    void horizontalScroll(int v);

    void verticalScroll(int v);

    void verticalRoomScroll(int v);

    void on_tbl_itemClicked(QTableWidgetItem *item);

    void on_btnClearFilter_clicked();

    void on_btnList_clicked();

private:
    Ui::DlgReservation *ui;

    QMap<int, bool> fHallFilter;

    QList<int> fRoomId;

    C5User *fUser;

    void loadTable();

    void filterHall();
};

#endif // DLGRESERVATION_H
