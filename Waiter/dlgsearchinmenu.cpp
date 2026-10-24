#include "dlgsearchinmenu.h"
#include <QApplication>
#include <QKeyEvent>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QTextLayout>
#include <QTimer>
#include "c5utils.h"
#include "struct_workstationitem.h"
#include "ui_dlgsearchinmenu.h"

class DishCellDelegate : public QStyledItemDelegate
{
public:
    enum Roles {
        RoleData      = Qt::UserRole,
        RoleGroupName = Qt::UserRole + 1,
        RoleDishName  = Qt::UserRole + 2,
        RolePrice     = Qt::UserRole + 3
    };

    explicit DishCellDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent) {}

    void paint(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &idx) const override
    {
        QStyleOptionViewItem o(opt);
        initStyleOption(&o, idx);
        // Рисуем фон/выделение стандартно
        const QWidget *w = o.widget;
        QStyle *style = w ? w->style() : QApplication::style();
        style->drawPrimitive(QStyle::PE_PanelItemViewItem, &o, p, w);
        // Достаём данные
        const QString groupName = idx.data(RoleGroupName).toString();
        const QString dishName  = idx.data(RoleDishName).toString();
        const QString priceStr  = idx.data(RolePrice).toString(); // например "2500" или "2 500 AMD"
        // Паддинги
        const int padL = 8, padT = 6, padR = 8, padB = 6;
        QRect r = o.rect.adjusted(padL, padT, -padR, -padB);
        // Цвет текста с учетом selection
        QColor textColor = (o.state & QStyle::State_Selected)
                           ? o.palette.color(QPalette::HighlightedText)
                           : o.palette.color(QPalette::Text);
        p->save();
        p->setPen(textColor);
        // --- Price (правый нижний) ---
        QFont priceFont = o.font;
        priceFont.setBold(true);
        p->setFont(priceFont);
        QFontMetrics fmPrice(priceFont);
        const int priceW = fmPrice.horizontalAdvance(priceStr);
        const int priceH = fmPrice.height();
        QRect priceRect(r.right() - priceW, r.bottom() - priceH + 1, priceW, priceH);
        p->drawText(priceRect, Qt::AlignRight | Qt::AlignVCenter, priceStr);
        // Чтобы текст блюда не залезал на цену:
        QRect textRect = r;
        textRect.setRight(priceRect.left() - 10); // зазор
        // --- Group name (первая строка) ---
        QFont groupFont = o.font;
        groupFont.setBold(true);
        groupFont.setPointSizeF(groupFont.pointSizeF() * 0.95);
        p->setFont(groupFont);
        QFontMetrics fmGroup(groupFont);
        const int lineH1 = fmGroup.height();
        QRect groupRect(r.left(), r.top(), r.width(), lineH1);
        QString groupText = groupName;

        if(fmGroup.horizontalAdvance(groupName) > groupRect.width()) {
            groupText = fmGroup.elidedText(groupName, Qt::ElideRight, groupRect.width());
        }

        p->drawText(groupRect, Qt::AlignLeft | Qt::AlignVCenter, groupText);
        // --- Dish name (2 строки максимум) ---
        QFont dishFont = o.font;
        dishFont.setBold(false);
        p->setFont(dishFont);
        QFontMetrics fmDish(dishFont);
        const int dishLineH = fmDish.height();
        QRect dishRect(textRect.left(),
                       groupRect.bottom() + 2,
                       textRect.width(),
                       dishLineH * 2 + 2);
        drawTwoLineWrappedElidedText(p, dishRect, dishName, dishFont, textColor);
        p->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &idx) const override
    {
        Q_UNUSED(idx);
        // 1 строка group + 2 строки dish + паддинги
        QFont groupFont = opt.font; groupFont.setBold(true);
        QFont dishFont  = opt.font;
        const int padT = 6, padB = 6;
        QFontMetrics fmG(groupFont);
        QFontMetrics fmD(dishFont);
        int h = padT + fmG.height() + 2 + fmD.height() * 2 + 2 + padB;
        return { opt.rect.width(), h };
    }

private:
    static void drawTwoLineWrappedElidedText(QPainter *p,
            const QRect &rect,
            const QString &text,
            const QFont &font,
            const QColor &color)
    {
        p->save();
        p->setFont(font);
        p->setPen(color);
        // QTextLayout даёт нормальный wrap по словам
        QTextLayout layout(text, font);
        QTextOption to;
        to.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        layout.setTextOption(to);
        layout.beginLayout();
        QVector<QTextLine> lines;

        for(int i = 0; i < 2; ++i) {
            QTextLine line = layout.createLine();

            if(!line.isValid())
                break;

            line.setLineWidth(rect.width());
            lines.push_back(line);
        }

        // Проверим, остался ли ещё текст (значит нужно эллипсировать последнюю строку)
        QTextLine third = layout.createLine();
        const bool hasMore = third.isValid();
        layout.endLayout();
        QFontMetrics fm(font);
        // Рисуем 1-ю строку
        int y = rect.top();

        if(!lines.isEmpty()) {
            QString s = text.mid(lines[0].textStart(), lines[0].textLength());
            s = fm.elidedText(s, Qt::ElideRight, rect.width()); // на всякий
            p->drawText(QRect(rect.left(), y, rect.width(), fm.height()),
                        Qt::AlignLeft | Qt::AlignVCenter, s);
            y += fm.height();
        }

        // 2-я строка: если hasMore — эллипсируем так, чтобы было "...".
        if(lines.size() >= 2) {
            QString s2 = text.mid(lines[1].textStart(), lines[1].textLength());

            if(hasMore) {
                s2 = fm.elidedText(s2, Qt::ElideRight, rect.width());
            } else {
                s2 = fm.elidedText(s2, Qt::ElideRight, rect.width());
            }

            p->drawText(QRect(rect.left(), y, rect.width(), fm.height()),
                        Qt::AlignLeft | Qt::AlignVCenter, s2);
        }

        p->restore();
    }
};

DlgSearchInMenu::DlgSearchInMenu(const QVector<DishAItem*>* dishes, int menu, C5User *user) :
    C5Dialog(user),
    ui(new Ui::DlgSearchInMenu),
    mDishes(dishes),
    mMenu(menu)
{
    ui->setupUi(this);
    ui->tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tbl->horizontalHeader()->setMinimumSectionSize(120);
    connect(ui->kb, SIGNAL(textChanged(QString)), this, SLOT(searchDish(QString)));
    connect(ui->kb, SIGNAL(accept()), this, SLOT(kbdAccept()));
    connect(ui->kb, SIGNAL(reject()), this, SLOT(reject()));
    ui->tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tbl->horizontalHeader()->setStretchLastSection(true);
    ui->tbl->setItemDelegate(new DishCellDelegate(ui->tbl));
    qApp->installEventFilter(this);
}

DlgSearchInMenu::~DlgSearchInMenu()
{
    qApp->removeEventFilter(this);
    delete ui;
}

void DlgSearchInMenu::showEvent(QShowEvent *e)
{
    C5Dialog::showEvent(e);
    int hSectionSize = mWorkStation.data.value("dlgsearchmenu_hsection_size").toInt();
    if (hSectionSize < 80) {
        hSectionSize = 160;
    }
    int vSectionSize = mWorkStation.data.value("dlgsearchmenu_vsection_size").toInt();
    if (vSectionSize < 40) {
        vSectionSize = 64;
    }
    ui->tbl->horizontalHeader()->setDefaultSectionSize(hSectionSize);
    ui->tbl->verticalHeader()->setDefaultSectionSize(vSectionSize);
    const int cols = qMax(1, width() / hSectionSize);
    ui->tbl->setColumnCount(cols);
    ui->tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    searchDish(ui->kb->text());
    activateWindow();
    raise();
    if (QLineEdit *ed = ui->kb->editor()) {
        ed->setFocus();
    }
    QTimer::singleShot(0, this, [this]() {
        activateWindow();
        raise();
        if (QLineEdit *ed = ui->kb->editor()) {
            ed->setFocus();
        }
    });
}

bool DlgSearchInMenu::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() != QEvent::KeyPress && e->type() != QEvent::ShortcutOverride) {
        return false;
    }

    auto *w = qobject_cast<QWidget *>(o);
    if (!w) {
        return false;
    }
    if (w == this || isAncestorOf(w)) {
        return false;
    }

    QWidget *modal = qobject_cast<QWidget *>(QApplication::activeModalWidget());
    if (modal && modal != this && !isAncestorOf(modal)) {
        return false;
    }

    if (e->type() == QEvent::ShortcutOverride) {
        e->accept();
        return true;
    }

    auto *k = static_cast<QKeyEvent *>(e);
    QLineEdit *ed = ui->kb->editor();
    if (!ed) {
        return false;
    }
    auto *copy = new QKeyEvent(QEvent::KeyPress,
                               k->key(),
                               k->modifiers(),
                               k->nativeScanCode(),
                               k->nativeVirtualKey(),
                               k->nativeModifiers(),
                               k->text(),
                               k->isAutoRepeat(),
                               k->count());
    QCoreApplication::postEvent(ed, copy);
    return true;
}

void DlgSearchInMenu::searchDish(const QString &name)
{
    QList<DishAItem*> result;

    for(auto *d : *mDishes) {
        if(d->menuId != mMenu) {
            continue;
        }

        if(!d->name.contains(name, Qt::CaseInsensitive)) {
            continue;
        }

        result.append(d);
    }

    if (ui->tbl->columnCount() < 1) {
        ui->tbl->setColumnCount(qMax(1, width() / 160));
        ui->tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    }

    ui->tbl->clearContents();
    const int cols = ui->tbl->columnCount();
    const int rowCount = result.isEmpty() ? 0 : (result.size() + cols - 1) / cols;
    ui->tbl->setRowCount(rowCount);
    int r = 0, c = 0;

    for(int i = 0; i < result.size(); i++) {
        auto *d = result.at(i);
        auto *it = new QTableWidgetItem();
        it->setData(DishCellDelegate::RoleData, QVariant::fromValue<quintptr>(reinterpret_cast<quintptr>(d)));
        it->setData(DishCellDelegate::RoleGroupName, d->groupName);
        it->setData(DishCellDelegate::RoleDishName,  d->name);
        it->setData(DishCellDelegate::RolePrice, float_str(d->price, 2)); // или готовая строка
        ui->tbl->setItem(r, c, it);
        c++;

        if(c >= ui->tbl->columnCount()) {
            c = 0;
            r++;
        }
    }
}

void DlgSearchInMenu::kbdAccept()
{
    QModelIndexList ml = ui->tbl->selectionModel()->selectedRows();

    if(ml.count() == 0) {
        return;
    }

    int menuid = ui->tbl->getInteger(ml.at(0).row(), 0);
    emit dish(menuid, "");
    accept();
}

void DlgSearchInMenu::on_tbl_itemClicked(QTableWidgetItem *item)
{
    auto *d = reinterpret_cast<DishAItem*>(item->data(DishCellDelegate::RoleData).value<quintptr>());

    if(!d) {
        return;
    }

    mDish = d;
    accept();
}
