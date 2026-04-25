#include "searchabletableview.h"
#include <QApplication>
#include <QColor>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFont>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPointer>
#include <QStyle>
#include <QToolButton>
#include <QUrl>
#include "datadialog.h"
#include "datawidget.h"
#include "message.h"
#include "ninterface.h"
#include <xlsxdocument.h>

SearchableTableView::SearchableTableView(QWidget *parent)
    : QWidget(parent)
{
    setProperty("icon", ":/images/app.ico");
    setProperty("text", "Unimplemented");

    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(2);

    m_toolbar = new QWidget(this);
    auto *toolbarLayout = new QHBoxLayout(m_toolbar);
    toolbarLayout->setContentsMargins(0, 0, 0, 0);
    toolbarLayout->setSpacing(4);
    m_refreshButton = new QToolButton(m_toolbar);
    m_refreshButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_refreshButton->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    m_refreshButton->setText(tr("Refresh"));
    m_refreshButton->setAutoRaise(true);
    toolbarLayout->addWidget(m_refreshButton);

    m_exportExcelButton = new QToolButton(m_toolbar);
    m_exportExcelButton->setFocusPolicy(Qt::NoFocus);
    m_exportExcelButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_exportExcelButton->setIcon(QIcon(QLatin1String(":/images/excel.png")));
    m_exportExcelButton->setText(tr("Excel"));
    m_exportExcelButton->setAutoRaise(true);
    toolbarLayout->addWidget(m_exportExcelButton);

    toolbarLayout->addStretch();
    m_layout->addWidget(m_toolbar);

    // 1. Поле поиска (скрыто по умолчанию)
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Поиск (Esc для закрытия)...");
    m_searchEdit->setVisible(false);
    m_layout->addWidget(m_searchEdit);

    // 2. Таблица
    m_view = new QTableView(this);
    m_layout->addWidget(m_view);

    // 3. Модели
    m_sourceModel = new UniversalTableModel(this);
    m_proxyModel = new MultiFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_sourceModel);

    m_view->setModel(m_proxyModel);
    m_view->setSortingEnabled(true);

    // Настройки производительности для 100к строк
    m_view->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_view->verticalHeader()->setDefaultSectionSize(28);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->setAlternatingRowColors(true);

    // Коннекты
    connect(m_refreshButton, &QToolButton::clicked, this, [this]() { getData(); });
    connect(m_exportExcelButton, &QToolButton::clicked, this, &SearchableTableView::exportToExcel);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &SearchableTableView::onSearchTextChanged);
    connect(m_view, &QTableView::doubleClicked, this, &SearchableTableView::onTableDoubleClicked);

    // Устанавливаем фильтр событий, чтобы ловить кнопки
    this->installEventFilter(this);
    m_searchEdit->installEventFilter(this);
    m_view->installEventFilter(this);

    QPointer<SearchableTableView> self(this);
    connect(this, &SearchableTableView::rowDoubleClicked, this, [this, self](const QVariant &firstColumnValue) {
        NInterface::query1(QString("%1/get-object").arg(m_route),
                           "",
                           self,
                           {{"id", firstColumnValue.toJsonValue()}},
                           [this, self](const QJsonObject jdoc) {
                               if (!self) {
                                   return;
                               }
                               if (auto *w = getDataWidget()) {
                                   w->setData(jdoc);
                               }
                               if (auto *d = getDataDialog()) {
                                   d->setData(jdoc);
                                   d->exec();
                                   d->deleteLater();
                               }
                           });
    });
}

void SearchableTableView::onTableDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid() || m_proxyModel->columnCount() <= 0) {
        return;
    }
    const QModelIndex first = index.siblingAtColumn(0);
    if (!first.isValid()) {
        return;
    }
    QVariant v = m_proxyModel->data(first, Qt::EditRole);
    if (!v.isValid()) {
        v = m_proxyModel->data(first, Qt::DisplayRole);
    }
    emit rowDoubleClicked(v);
}

bool SearchableTableView::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        // Ctrl + F -> Показать поиск
        if (keyEvent->key() == Qt::Key_F && (keyEvent->modifiers() & Qt::ControlModifier)) {
            toggleSearch();
            return true;
        }

        // Esc -> Скрыть поиск и очистить фильтр
        if (keyEvent->key() == Qt::Key_Escape && m_searchEdit->isVisible()) {
            m_searchEdit->clear();
            m_searchEdit->hide();
            m_view->setFocus();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void SearchableTableView::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
    setupWidget();
    getData();
}

void SearchableTableView::getData()
{
    QPointer<SearchableTableView> self(this);

    // Предполагаем, что m_route уже содержит нужный путь к API
    NInterface::query1(m_route, "", self, {}, [self](const QJsonObject &jdoc) {
        if (!self)
            return;

        if (jdoc["status"].toInt() != 1)
            return;

        // 1. Парсим структуру колонок
        QStringList headers;
        QStringList dataKeys;
        QJsonArray columnsArray = jdoc["columns"].toArray();

        for (const QJsonValue &colVal : columnsArray) {
            QJsonObject colObj = colVal.toObject();
            const QString key = colObj["name"].toString();
            if (key.isEmpty()) {
                continue;
            }
            headers << colObj["title"].toString();
            dataKeys << key;
        }

        // 2. Парсим массив данных
        QJsonArray dataArray = jdoc["data"].toArray();
        QVector<QVector<QVariant>> matrix;
        matrix.reserve(dataArray.size());

        for (const QJsonValue &rowVal : dataArray) {
            QJsonObject rowObj = rowVal.toObject();
            QVector<QVariant> rowData;
            rowData.reserve(dataKeys.size());

            // Динамически вытягиваем данные только для тех полей, что описаны в columns
            for (const QString &key : dataKeys) {
                // toVariant() сам определит: число это или строка
                rowData << rowObj[key].toVariant();
            }
            matrix.append(rowData);
        }

        // 3. Обновляем модель (она сама вызовет reset и перерисует View)
        self->sourceModel()->updateData(matrix, headers);

        // 4. Тонкая настройка внешнего вида
        auto *hHeader = self->tableView()->horizontalHeader();

        // По умолчанию всё по контенту
        hHeader->setSectionResizeMode(QHeaderView::ResizeToContents);

        // Если колонок много, последнюю растягиваем, чтобы не было дырки
        if (!headers.isEmpty()) {
            hHeader->setSectionResizeMode(headers.size() - 1, QHeaderView::Stretch);
        }

        // Включаем интерактив, чтобы юзер мог сам подвигать границы после загрузки
        hHeader->setCascadingSectionResizes(true);
    });
}

DataWidget *SearchableTableView::getDataWidget()
{
    return nullptr;
}

DataDialog *SearchableTableView::getDataDialog()
{
    return nullptr;
}

void SearchableTableView::exportToExcel()
{
    QAbstractItemModel *model = m_proxyModel;
    const int colCount = model->columnCount();
    const int rowCount = model->rowCount();
    if (colCount == 0 || rowCount == 0) {
        message_error(tr("Empty report!"));
        return;
    }

    QXlsx::Document d;
    d.addSheet(QStringLiteral("Sheet1"));

    QColor color = QColor::fromRgb(0, 0, 0);
    QFont headerFont(qApp->font());
    headerFont.setBold(true);
    QXlsx::Format formatHeader;
    formatHeader.setFont(headerFont);
    formatHeader.setBorderStyle(QXlsx::Format::BorderThin);

    for (int i = 0; i < colCount; i++) {
        const QString title = model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
        d.write(1, i + 1, title, formatHeader);
        const int w = m_view->columnWidth(i);
        d.setColumnWidth(i + 1, w > 0 ? w / 7 : 15);
    }

    QFont bodyFont(qApp->font());
    QXlsx::Format formatBody;
    formatBody.setFontColor(color);
    formatBody.setFontSize(bodyFont.pointSize());
    formatBody.setBorderStyle(QXlsx::Format::BorderThin);

    for (int j = 0; j < rowCount; j++) {
        for (int i = 0; i < colCount; i++) {
            const QModelIndex idx = model->index(j, i);
            QVariant v = model->data(idx, Qt::EditRole);
            if (!v.isValid()) {
                v = model->data(idx, Qt::DisplayRole);
            }
            d.write(j + 2, i + 1, v, formatBody);
        }
    }

    const QString filename = QFileDialog::getSaveFileName(this, QString(), QString(), QStringLiteral("*.xlsx"));
    if (filename.isEmpty()) {
        return;
    }
    if (!d.saveAs(filename)) {
        QMessageBox::critical(this, tr("Error"), tr("Excel file not saved"));
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
}

void SearchableTableView::toggleSearch()
{
    if (m_searchEdit->isVisible()) {
        m_searchEdit->hide();
        m_view->setFocus();
    } else {
        m_searchEdit->show();
        m_searchEdit->setFocus();
    }
}

void SearchableTableView::onSearchTextChanged(const QString &text)
{
    // Фильтруем по всем колонкам (пустой список вторым аргументом)
    m_proxyModel->filter(text, {});
}
