#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include "socketwidget.h"

namespace Ui {
class TableWidget;
}

class TableModel;

class TableWidget : public SocketWidget
{
    Q_OBJECT

public:
    explicit TableWidget(QWidget *parent = nullptr);
    ~TableWidget();

public slots:
    virtual void externalDataReceived(quint16 cmd, quint32 messageId, const QByteArray &data) override;

private:
    Ui::TableWidget *ui;
    TableModel *fTableModel;

private slots:
    void resizeTotalHeader(int logicalIndex, int oldSize, int newSize);
    void changeViewHorizontalScroll(int value);
    void on_leQuickFilter_textChanged(const QString &arg1);
};

#endif // TABLEWIDGET_H
