#include "tablewidget.h"
#include "c5utils.h"
#include <QLabel>
#include <QFrame>
#include <QStyle>

TableWidget::TableWidget(QWidget *parent) : QWidget(parent)
{
}

void TableWidget::configOrder(const QJsonObject &jo)
{
    bool commentVisible = !jo["f_comment"].toString().isEmpty();
    labelComment()->setVisible(commentVisible);
    QString state = "1";

    if(jo.isEmpty()) {
        labelStaff()->setText("-");
        labelTime()->setText("00:00");
        labelAmount()->clear();
    } else {
        state = "2";

        if(jo["f_precheck"].toInt() > 0) {
            state = "3";
        }

        if(jo["f_print"].toInt() > 0) {
            state = "4";
        }

        labelComment()->setText(jo["f_comment"].toString());
        labelStaff()->setText(jo["f_staffname"].toString());
        labelAmount()->setText(float_str(jo["f_amounttotal"].toDouble(), 2));
        QString dd = QString("<html><body><p align=\"center\" "
                             "style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> "
                             "<span style=\" font-size:6pt;\">%1</span></p>\n<p align=\"center\" "
                             "style=\" margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> "
                             "<span style=\" font-size:8pt;\">%2</span></p></body></html>")
                     .arg(jo["f_dateopen"].toString(), jo["f_timeopen"].toString());
        labelTime()->setText(QDate::currentDate() == QDate::fromString(jo["f_dateopen"].toString(), FORMAT_DATE_TO_STR) ?
                             jo["f_timeopen"].toString() : dd);

        if(jo["f_state"].toInt() == ORDER_STATE_PREORDER_EMPTY
                || jo["f_state"].toInt() == ORDER_STATE_PREORDER_WITH_ORDER) {
            state = "5";
        }

        if(jo["f_state"].toInt() == ORDER_STATE_OPEN) {
            if(!jo["f_fortablename"].toString().isEmpty()) {
                //TODO
                // if(QDate::fromString(__c5config.dateCash(), FORMAT_DATE_TO_STR_MYSQL) == QDate::fromString(jo["f_checkout"].toString(),
                //         FORMAT_DATE_TO_STR)) {
                //     state = "6";
                // }
            }
        }
    }

    frame()->setProperty("t1_state", state);
    frame()->style()->polish(frame());
}

void TableWidget::config(int id)
{
    fTable = id;
    //TODO
    //labelTable()->setText(dbtable->name(id));
}

void TableWidget::mouseReleaseEvent(QMouseEvent *me)
{
    QWidget::mouseReleaseEvent(me);
    emit clicked(fTable);
}
