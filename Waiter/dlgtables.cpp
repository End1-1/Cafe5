#include "dlgtables.h"
#include "ui_dlgtables.h"
#include "ninterface.h"
#include "c5utils.h"
#include "c5user.h"
#include "struct_workstationitem.h"
#include <QScrollBar>
#include <QStyledItemDelegate>
#include <QPainter>

enum TableRoles {
    RoleId = Qt::UserRole,
    RoleState,
    RoleAmount,
    RoleStaff,
    RoleName
};

class TableCellDelegate : public QStyledItemDelegate
{
public:
    explicit TableCellDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent) {}

    void paint(QPainter *p,
               const QStyleOptionViewItem &opt,
               const QModelIndex &idx) const override
    {
        p->save();
        // inset = толщина "разделителя"
        const int inset = 1;
        QRect r = opt.rect.adjusted(inset, inset, -inset, -inset);
        // id
        int id = idx.data(RoleId).toInt();
        // --- фон по состоянию ---
        QColor bg(Qt::white);
        int state = idx.data(RoleState).toInt();

        if(state == 2)
            bg = QColor(200, 247, 197);   // зелёный
        else if(state == 3)
            bg = QColor(247, 197, 197);   // красный

        // если id == 0 — просто фон и выходим
        if(id == 0) {
            p->fillRect(r, bg);
            p->restore();
            return;
        }

        // фон ячейки
        p->fillRect(r, bg);
        // --- данные ---
        QString name  = idx.data(RoleName).toString();
        QString staff = idx.data(RoleStaff).toString();
        double amount = idx.data(RoleAmount).toDouble();
        const int pad = 6;
        // --- имя стола (лево-верх) ---
        QFont f = opt.font;
        f.setBold(true);
        QRect nameRect(r.left() + pad, r.top() + pad, r.width() / 2, 18);
        // начальный размер
        int fontSize = 14;
        const int minFontSize = 8;
        QFontMetrics fm(f);

        while(fontSize >= minFontSize) {
            f.setPointSize(fontSize);
            fm = QFontMetrics(f);

            if(fm.horizontalAdvance(name) <= nameRect.width())
                break;

            fontSize--;
        }

        p->setFont(f);
        p->setPen(Qt::black);
        p->drawText(nameRect, Qt::AlignCenter | Qt::AlignVCenter, name);
        f.setPointSize(10);
        f.setBold(true);
        p->setFont(f);

        // --- сумма (право-верх) ---
        if(amount > 0.01) {
            p->drawText(
                QRect(r.center().x(), r.top() + pad,
                      r.width() / 2 - pad, 18),
                Qt::AlignRight | Qt::AlignVCenter,
                float_str(amount,  2)
            );
        }

        // --- staff (вторая строка) ---
        f.setBold(false);
        p->setFont(f);
        p->drawText(
            QRect(r.left() + pad, r.top() + pad + 20,
                  r.width() - pad * 2, 18),
            Qt::AlignLeft | Qt::AlignVCenter,
            staff
        );
        p->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override
    {
        return QSize(200, 56);
    }
};

DlgTables::DlgTables(C5User *user) :
    C5WaiterDialog(user),
    ui(new Ui::DlgTables)
{
    ui->setupUi(this);
    ui->tblTables->setItemDelegate(new TableCellDelegate(ui->tblTables));
}

DlgTables::~DlgTables()
{
    delete ui;
}

int DlgTables::exec()
{
    C5WaiterDialog::exec();
    return mResult;
}

void DlgTables::showEvent(QShowEvent *e)
{
    C5WaiterDialog::showEvent(e);
    NInterface::query1("/engine/v2/waiter/hall/get", mUser->mSessionKey, this, {},
    [this](const QJsonObject & jdoc) {
        mHall = parseJsonArray<HallItem>(jdoc["halls"].toArray());
        mTables = parseJsonArray<TableItem>(jdoc["tables"].toArray());
        int i = 0;

        for(auto const &h : std::as_const(mHall)) {
            QToolButton *btn = new QToolButton();
            btn->setText(h.name);
            btn->setToolButtonStyle(Qt::ToolButtonTextOnly);
            btn->setMinimumSize(QSize(140, 50));
            btn->setProperty("id", h.id);
            connect(btn, &QToolButton::clicked, this, &DlgTables::hallClicked);
            ui->gh->insertWidget(ui->gh->count() - 2, btn);
        }

        auto *tbl = ui->tblTables;
        auto *header = tbl->horizontalHeader();
        int baseWidth = header->defaultSectionSize();
        int tableWidth = tbl->viewport()->width();

        if(tbl->verticalScrollBar()->isVisible()) {
            tableWidth -= tbl->verticalScrollBar()->width();
        }

        int cols = qMax(1, tableWidth / baseWidth);
        tbl->setColumnCount(cols);
        int extra = tableWidth - cols * baseWidth;
        int add = extra / cols;
        int finalWidth = baseWidth + add;
        header->setSectionResizeMode(QHeaderView::Fixed);

        for(int c = 0; c < cols; ++c) {
            tbl->setColumnWidth(c, finalWidth);
        }

        hallClicked();
    });
}

void DlgTables::hallClicked()
{
    auto* btn = qobject_cast<QToolButton*>(sender());
    int hall = btn ? btn->property("id").toInt() : mWorkStation.defaultHallId();
    QVector<TableItem> copy;

    for(auto t : mTables) {
        if(hall == 0  || t.hall == hall) {
            copy.append(t);
        }
    }

    int r = 0;
    int c = 0;
    int rows = (copy.size() + ui->tblTables->columnCount() - 1) / ui->tblTables->columnCount();
    ui->tblTables->setRowCount(rows);

    for(auto t : copy) {
        auto *item = new QTableWidgetItem();
        item->setData(RoleId, t.id);
        item->setData(RoleState, t.tableState);
        item->setData(RoleAmount, t.amount);
        item->setData(RoleStaff, t.staffName);
        item->setData(RoleName, t.name);
        ui->tblTables->setItem(r, c, item);
        ++c;

        if(c >= ui->tblTables->columnCount()) {
            c = 0;
            ++r;
        }
    }
}

void DlgTables::on_btnCancel_clicked()
{
    reject();
}

void DlgTables::on_tblTables_itemClicked(QTableWidgetItem *item)
{
    if(!item) {
        return;
    }

    if(item->data(RoleId).toInt() == 0) {
        return;
    }

    mResult = item->data(RoleId).toInt();
    accept();
}
