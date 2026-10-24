#include "c5tableview.h"
#include "c5tablemodel.h"
#include <QAbstractItemView>
#include <QApplication>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMenu>
#include <algorithm>

C5TableView::C5TableView(QWidget *parent)  :
    QTableView(parent)
{
}

void C5TableView::setItemDelegateForColumn(int column, QAbstractItemDelegate *delegate)
{
    auto *m = static_cast<C5TableModel*>(model());
    m->setEditableFlag(column, Qt::ItemIsEditable);
    QTableView::setItemDelegateForColumn(column, delegate);
}

void C5TableView::copySelectionFrom(QAbstractItemView *view)
{
    if (!view || !view->selectionModel()) {
        return;
    }

    QModelIndexList cells;
    const QModelIndexList selected = view->selectionModel()->selectedIndexes();
    cells.reserve(selected.size());
    for (const QModelIndex &index : selected) {
        if (!index.isValid() || !view->visualRect(index).isValid()) {
            continue;
        }
        cells.append(index);
    }
    if (cells.isEmpty()) {
        return;
    }

    std::sort(cells.begin(), cells.end(), [](const QModelIndex &a, const QModelIndex &b) {
        if (a.row() != b.row()) {
            return a.row() < b.row();
        }
        return a.column() < b.column();
    });

    QString data;
    int row = cells.first().row();
    bool firstInRow = true;
    for (const QModelIndex &index : cells) {
        if (index.row() != row) {
            data += QStringLiteral("\r\n");
            row = index.row();
            firstInRow = true;
        }
        if (!firstInRow) {
            data += QLatin1Char('\t');
        }
        firstInRow = false;
        data += index.data(Qt::DisplayRole).toString();
    }

    QApplication::clipboard()->setText(data);
}

void C5TableView::copySelection()
{
    copySelectionFrom(this);
}

void C5TableView::contextMenuEvent(QContextMenuEvent *event)
{
    if (!selectionModel() || selectionModel()->selectedIndexes().isEmpty()) {
        QTableView::contextMenuEvent(event);
        return;
    }

    QMenu menu(this);
    menu.addAction(tr("Copy"), this, [this] { copySelection(); });
    menu.exec(event->globalPos());
}

void C5TableView::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Copy)) {
        copySelection();
        event->accept();
        return;
    }
    QTableView::keyPressEvent(event);
}
