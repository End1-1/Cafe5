#include "rmenureviewreport.h"
#include "c5config.h"
#include "c5htmlprint.h"
#include "c5mainwindow.h"
#include "c5message.h"
#include "c5tablewidget.h"
#include "c5user.h"
#include "c5utils.h"
#include "dlginventoryblankchooser.h"
#include "ntreemodel.h"
#include "ntreenode.h"
#include <QAction>
#include <QJsonArray>
#include <QJsonObject>
#include <QPageSize>
#include <QPainter>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QTextDocument>
#include <QToolBar>
#include <QTreeView>

namespace {

constexpr int kFixedCols = 6;
constexpr int kTailCols = 6;
constexpr int kColStatus = 1;
constexpr int kColType = 3;
constexpr int kColDishName = 4;
constexpr int kColWeight = 5;

QString menuReviewTreeStyleSheet()
{
    return QStringLiteral(
        "QTreeView#mTreeView {"
        "    border: none;"
        "    background-color: #fafcfe;"
        "    alternate-background-color: #f4f7fb;"
        "}"
        "QTreeView#mTreeView::item {"
        "    border-right: 1px solid #e2eaf4;"
        "    border-bottom: 1px solid #e2eaf4;"
        "    color: #1a2533;"
        "    padding: 4px 8px;"
        "}"
        "QTreeView#mTreeView::item:selected {"
        "    background-color: rgba(99, 132, 232, 0.18);"
        "    color: #1a2533;"
        "}"
        "QTreeView#mTreeView::item:hover:!selected {"
        "    background-color: rgba(99, 132, 232, 0.08);"
        "}"
        "QHeaderView::section {"
        "    background-color: #eef2f8;"
        "    color: #1a2533;"
        "    border: none;"
        "    border-right: 1px solid #e2eaf4;"
        "    border-bottom: 1px solid #d0dae6;"
        "    padding: 5px 10px;"
        "}");
}

QString menuReviewTotalStyleSheet()
{
    return QStringLiteral(
        "C5TableWidget#tblTotal {"
        "    border-top: 1px solid #d0dae6;"
        "    background-color: #f4f7fb;"
        "}");
}

QJsonArray insertSpacerRows(const QJsonArray &rows)
{
    if(rows.isEmpty()) {
        return rows;
    }

    const int columnCount = rows.at(0).toObject().value(QStringLiteral("data")).toArray().size();
    if(columnCount <= 0) {
        return rows;
    }

    QJsonArray result;

    for(int i = 0; i < rows.size(); ++i) {
        result.append(rows.at(i));

        if(i + 1 >= rows.size()) {
            continue;
        }

        QJsonArray emptyData;

        for(int c = 0; c < columnCount; ++c) {
            emptyData.append(QString());
        }

        QJsonObject spacer;
        spacer.insert(QStringLiteral("data"), emptyData);
        spacer.insert(QStringLiteral("children"), QJsonArray());
        spacer.insert(QStringLiteral("spacer"), true);
        result.append(spacer);
    }

    return result;
}

void applyMenuReviewTableStyle(QWidget *widget)
{
    if(auto *tree = widget->findChild<QTreeView*>(QStringLiteral("mTreeView"))) {
        tree->setAlternatingRowColors(true);
        tree->setUniformRowHeights(false);
        tree->setSortingEnabled(false);
        tree->setStyleSheet(menuReviewTreeStyleSheet());
    }

    if(auto *total = widget->findChild<C5TableWidget*>(QStringLiteral("tblTotal"))) {
        total->setStyleSheet(menuReviewTotalStyleSheet());
    }
}

QString cellText(NTreeModel *model, const QModelIndex &rowIndex, int column)
{
    return model->data(model->index(rowIndex.row(), column, rowIndex.parent()), Qt::DisplayRole).toString().trimmed();
}

QString formatNumericDisplay(const QString &raw)
{
    const QString t = raw.trimmed();
    if(t.isEmpty()) {
        return t;
    }
    return float_str(str_float(t), 2);
}

qreal documentHeight(const QString &html, qreal textWidth)
{
    QTextDocument doc;
    doc.setHtml(html);
    doc.setTextWidth(textWidth);
    return doc.size().height();
}

void drawHtml(QPainter *painter, const QString &html, const QRectF &rect)
{
    QTextDocument doc;
    doc.setHtml(html);
    doc.setTextWidth(rect.width());
    painter->save();
    painter->translate(rect.topLeft());
    painter->setClipRect(QRectF(0, 0, rect.width(), rect.height()));
    doc.drawContents(painter, QRectF(0, 0, rect.width(), rect.height()));
    painter->restore();
}

// QTextDocument sizes HTML in ~96dpi pixels; HighResolution printers use ~600–1200dpi.
constexpr qreal kHtmlSourceDpi = 96.0;
/** Small vertical gap between recipe cards when printing continuously. */
constexpr qreal kRecipeCardGapPx = 12.0;
/** If less than this remains on the page, start the next recipe on a new sheet
 *  (avoids leaving only a header stub at the bottom). */
constexpr qreal kMinRecipeBlockPx = 130.0;

} // namespace

RMenuReviewReport::RMenuReviewReport(C5User *user, const QString &title, QIcon icon, QWidget *parent)
    : NTreeWidget(user, QStringLiteral("/engine/v2/reports/menu-review/get"), parent)
{
    fLabel = title;
    fIcon = icon;
    mMainWindow = __mainWindow;
    mHost = __c5config.dbParams().at(1);
    applyMenuReviewTableStyle(this);
    initParams({});
    query();
}

QToolBar *RMenuReviewReport::toolBar()
{
    NTreeWidget::toolBar();
    if(!mInventoryBlankActionInserted && fToolBar) {
        auto *a = new QAction(QIcon(QStringLiteral(":/print.png")), tr("Inventory blanks"), this);
        connect(a, &QAction::triggered, this, &RMenuReviewReport::inventoryBlanks);
        fToolBar->addAction(a);
        mInventoryBlankActionInserted = true;
    }
    return fToolBar;
}

void RMenuReviewReport::inventoryBlanks()
{
    C5User *user = reportUser();
    if(!user) {
        return;
    }
    DlgInventoryBlankChooser::open(user->mSessionKey, reportFilter(), this);
}

void RMenuReviewReport::queryFinished(const QJsonObject &ba)
{
    QJsonObject jo = ba;

    if(!jo.value(QStringLiteral("childcols")).toArray().isEmpty()) {
        jo.insert(QStringLiteral("rows"), insertSpacerRows(jo.value(QStringLiteral("rows")).toArray()));
    }

    NTreeWidget::queryFinished(jo);
}

void RMenuReviewReport::print()
{
    auto *tree = findChild<QTreeView*>(QStringLiteral("mTreeView"));
    if(!tree || !tree->model()) {
        C5Message::info(tr("Empty report!"));
        return;
    }

    auto *model = static_cast<NTreeModel*>(tree->model());
    const int colCount = model->columnCount();
    if(colCount < kFixedCols + kTailCols) {
        C5Message::info(tr("Empty report!"));
        return;
    }

    const int menuCount = (colCount - kFixedCols - kTailCols) / 2;
    const int selfCostCol = kFixedCols + menuCount * 2;

    bool hasRecipes = false;
    const int topCount = model->rowCount(QModelIndex());
    for(int r = 0; r < topCount; ++r) {
        QModelIndex idx0 = model->index(r, 0);
        auto *node = static_cast<NTreeNode*>(idx0.internalPointer());
        if(!node || node->spacer) {
            continue;
        }
        if(model->rowCount(idx0) > 0) {
            hasRecipes = true;
            break;
        }
    }

    if(!hasRecipes) {
        C5Message::info(tr("Switch to dishes and recipes mode"));
        return;
    }

    const int printChoice = C5Message::question(tr("Print menu"),
                                                tr("With self cost"),
                                                tr("Cancel"),
                                                tr("Without self cost"));
    if(printChoice == QDialog::Rejected) {
        return;
    }
    const bool showSelfCost = (printChoice == QDialog::Accepted);

    QString cardTemplate = loadTemplate(QStringLiteral("menu_recipe_card.html"));
    if(cardTemplate.isEmpty()) {
        C5Message::error(tr("Template not found"));
        return;
    }

    QStringList cardHtmls;
    for(int r = 0; r < topCount; ++r) {
        QModelIndex dishIdx = model->index(r, 0);
        auto *node = static_cast<NTreeNode*>(dishIdx.internalPointer());
        if(!node || node->spacer) {
            continue;
        }

        const QString group = cellText(model, dishIdx, kColType);
        const QString dishName = cellText(model, dishIdx, kColDishName);
        const QString weight = cellText(model, dishIdx, kColWeight);
        const QString selfCost = cellText(model, dishIdx, selfCostCol);

        QString pricesBlock;
        {
            QString headers;
            QString values;
            for(int m = 0; m < menuCount; ++m) {
                const int priceCol = kFixedCols + m * 2;
                const QString header = model->headerData(priceCol, Qt::Horizontal, Qt::DisplayRole).toString();
                const QString price = cellText(model, dishIdx, priceCol);
                headers += QStringLiteral("<th>%1</th>").arg(htmlEscape(header));
                values += QStringLiteral("<td class='right'>%1</td>").arg(htmlEscape(formatNumericDisplay(price)));
            }
            if(showSelfCost) {
                headers += QStringLiteral("<th>%1</th>").arg(htmlEscape(tr("Self cost")));
                values += QStringLiteral("<td class='right'>%1</td>")
                              .arg(htmlEscape(formatNumericDisplay(selfCost)));
            }
            pricesBlock = QStringLiteral("<table class='prices'><thead><tr>%1</tr></thead>"
                                         "<tbody><tr>%2</tr></tbody></table>")
                              .arg(headers, values);
        }

        QString recipeTable;
        double recipeTotal = 0;
        {
            QString rowsHtml;
            const int childCount = model->rowCount(dishIdx);
            int nn = 0;
            for(int c = 0; c < childCount; ++c) {
                QModelIndex childIdx = model->index(c, 0, dishIdx);
                const QString unit = cellText(model, childIdx, kColStatus);
                const QString name = cellText(model, childIdx, kColDishName);
                const QString qty = cellText(model, childIdx, kColWeight);
                const QString price = cellText(model, childIdx, selfCostCol);
                const QString total = cellText(model, childIdx, selfCostCol + 1);
                recipeTotal += str_float(total);
                ++nn;
                if(showSelfCost) {
                    rowsHtml += QStringLiteral(
                                    "<tr>"
                                    "<td class='center'>%1</td>"
                                    "<td>%2</td>"
                                    "<td class='right'>%3</td>"
                                    "<td class='center'>%4</td>"
                                    "<td class='right'>%5</td>"
                                    "<td class='right'>%6</td>"
                                    "</tr>")
                                    .arg(nn)
                                    .arg(htmlEscape(name),
                                         htmlEscape(formatNumericDisplay(qty)),
                                         htmlEscape(unit),
                                         htmlEscape(formatNumericDisplay(price)),
                                         htmlEscape(formatNumericDisplay(total)));
                } else {
                    rowsHtml += QStringLiteral(
                                    "<tr>"
                                    "<td class='center'>%1</td>"
                                    "<td>%2</td>"
                                    "<td class='right'>%3</td>"
                                    "<td class='center'>%4</td>"
                                    "</tr>")
                                    .arg(nn)
                                    .arg(htmlEscape(name),
                                         htmlEscape(formatNumericDisplay(qty)),
                                         htmlEscape(unit));
                }
            }

            if(showSelfCost) {
                recipeTable = QStringLiteral(
                                  "<table class='recipe'><thead><tr>"
                                  "<th>%1</th><th>%2</th><th class='right'>%3</th>"
                                  "<th>%4</th><th class='right'>%5</th><th class='right'>%6</th>"
                                  "</tr></thead><tbody>%7</tbody></table>")
                                  .arg(htmlEscape(QStringLiteral("NN")),
                                       htmlEscape(tr("Goods name")),
                                       htmlEscape(tr("Qty")),
                                       htmlEscape(tr("Unit")),
                                       htmlEscape(tr("Price")),
                                       htmlEscape(tr("Total")),
                                       rowsHtml);
            } else {
                recipeTable = QStringLiteral(
                                  "<table class='recipe'><thead><tr>"
                                  "<th>%1</th><th>%2</th><th class='right'>%3</th><th>%4</th>"
                                  "</tr></thead><tbody>%5</tbody></table>")
                                  .arg(htmlEscape(QStringLiteral("NN")),
                                       htmlEscape(tr("Goods name")),
                                       htmlEscape(tr("Qty")),
                                       htmlEscape(tr("Unit")),
                                       rowsHtml);
            }
        }

        QString meta;
        if(!weight.isEmpty()) {
            meta = tr("Weight") + QStringLiteral(": ") + weight;
        }

        QMap<QString, QString> vars;
        vars.insert(QStringLiteral("group"), htmlEscape(group));
        vars.insert(QStringLiteral("dish_name"), htmlEscape(dishName));
        vars.insert(QStringLiteral("meta"), htmlEscape(meta));
        vars.insert(QStringLiteral("prices_block"), pricesBlock);
        vars.insert(QStringLiteral("recipe_title"), htmlEscape(tr("Recipe")));
        vars.insert(QStringLiteral("recipe_table"), recipeTable);
        vars.insert(QStringLiteral("total_line"),
                    showSelfCost
                        ? htmlEscape(tr("Complectation cost") + QStringLiteral(": ") + float_str(recipeTotal, 2))
                        : QString());

        cardHtmls.append(applyTemplate(cardTemplate, vars));
    }

    if(cardHtmls.isEmpty()) {
        C5Message::info(tr("Empty report!"));
        return;
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize::A4);
    printer.setFullPage(false);
    QPrintPreviewDialog pp(&printer, this);
    connect(&pp, &QPrintPreviewDialog::paintRequested, this, [&](QPrinter *p) {
        const qreal scaleX = p->logicalDpiX() / kHtmlSourceDpi;
        const qreal scaleY = p->logicalDpiY() / kHtmlSourceDpi;
        const QRectF pageDevice = p->pageRect(QPrinter::DevicePixel);
        const qreal pageW = pageDevice.width() / scaleX;
        const qreal pageH = pageDevice.height() / scaleY;
        const qreal gap = kRecipeCardGapPx;

        QPainter painter(p);
        painter.translate(pageDevice.topLeft());
        painter.scale(scaleX, scaleY);

        qreal y = 0;
        bool firstOnPage = true;
        for(const QString &html : cardHtmls) {
            qreal h = documentHeight(html, pageW);
            if(h > pageH) {
                h = pageH;
            }

            if(!firstOnPage) {
                const qreal remaining = pageH - y;
                // Not enough room for a meaningful block (header-only leftover) → new page.
                const bool onlyHeaderWouldFit = remaining < kMinRecipeBlockPx;
                const bool cardDoesNotFit = y + h > pageH;
                if(onlyHeaderWouldFit || cardDoesNotFit) {
                    p->newPage();
                    painter.resetTransform();
                    painter.translate(pageDevice.topLeft());
                    painter.scale(scaleX, scaleY);
                    y = 0;
                    firstOnPage = true;
                }
            }

            drawHtml(&painter, html, QRectF(0, y, pageW, h));
            y += h + gap;
            firstOnPage = false;
        }
    });
    pp.setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    pp.raise();
    pp.activateWindow();
    pp.exec();
}
