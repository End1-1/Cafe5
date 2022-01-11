#include "tablewidget.h"
#include "datadriver.h"
#include "c5utils.h"
#include <QLabel>
#include <QFrame>
#include <QStyle>

TableWidget::TableWidget(QWidget *parent) : QWidget(parent)
{

}

void TableWidget::configOrder(const QString &orderid)
{
    bool commentVisible = !dboheader->comment(orderid).isEmpty();
    labelComment()->setVisible(commentVisible);
    QString state = "1";
    if (orderid.isEmpty()) {
        labelStaff()->setText("-");
        labelTime()->setText("00:00");
        labelAmount()->clear();
    } else {
        state = "2";
        if (dboheader->precheck(orderid) > 0) {
            state = "3";
        }
        if (dboheader->print(orderid) > 0) {
            state = "4";
        }
        labelComment()->setText(dboheader->comment(orderid));
        labelStaff()->setText(dbuser->fullShort(dboheader->staff(orderid)));
        labelAmount()->setText(float_str(dboheader->amount(orderid), 2));
        QString dd = QString("<html><body><p align=\"center\" "
               "style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> "
               "<span style=\" font-size:6pt;\">%1</span></p>\n<p align=\"center\" "
               "style=\" margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> "
               "<span style=\" font-size:8pt;\">%2</span></p></body></html>")
                                                 .arg(dboheader->dateOpen(orderid).toString(FORMAT_DATE_TO_STR))
                                                 .arg(dboheader->timeOpen(orderid).toString(FORMAT_TIME_TO_SHORT_STR));
        labelTime()->setText(QDate::currentDate() == dboheader->dateOpen(orderid) ?
                                   dboheader->timeOpen(orderid).toString(FORMAT_TIME_TO_SHORT_STR) : dd );
    }
    frame()->setProperty("t1_state", state);
    frame()->style()->polish(frame());
}

void TableWidget::config(int id)
{
    fTable = id;
    labelTable()->setText(dbtable->name(id));
}

void TableWidget::mouseReleaseEvent(QMouseEvent *me)
{
    QWidget::mouseReleaseEvent(me);
    emit clicked(fTable);
}
