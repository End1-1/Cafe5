#pragma once

#include "waitertablestyle.h"
#include <QStyledItemDelegate>

enum WaiterTableRoles {
    WaiterRoleId = Qt::UserRole,
    WaiterRoleState,
    WaiterRoleAmount,
    WaiterRoleStaff,
    WaiterRoleName
};

class TableCellDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit TableCellDelegate(const WaiterTblTablesStyle &style, QObject *parent = nullptr);

    void setStyle(const WaiterTblTablesStyle &style);

    void paint(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &idx) const override;
    QSize sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &idx) const override;

private:
    WaiterTblTablesStyle mStyle;
};
