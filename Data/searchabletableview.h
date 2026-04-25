#ifndef SEARCHABLETABLEVIEW_H
#define SEARCHABLETABLEVIEW_H

#include <QHeaderView>
#include <QLineEdit>
#include <QModelIndex>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>
#include "MultiFilterProxyModel.h"
#include "UniversalTableModel.h"

class QToolButton;
class DataWidget;
class DataDialog;

class SearchableTableView : public QWidget
{
    Q_OBJECT
public:
    explicit SearchableTableView(QWidget *parent = nullptr);

    // Методы для доступа к моделям снаружи
    UniversalTableModel *sourceModel() const { return m_sourceModel; }
    MultiFilterProxyModel *proxyModel() const { return m_proxyModel; }
    QTableView *tableView() const { return m_view; }

protected:
    QString m_route;

    // Перехватываем Ctrl+F и Esc на уровне виджета
    bool eventFilter(QObject *obj, QEvent *event) override;
    virtual void showEvent(QShowEvent *e) override;
    virtual void setupWidget() = 0;
    void getData();
    virtual DataWidget *getDataWidget();
    virtual DataDialog *getDataDialog();

signals:
    /// Двойной клик по строке; значение первого столбца (как правило id).
    void rowDoubleClicked(const QVariant &firstColumnValue);

private slots:
    void toggleSearch();
    void onSearchTextChanged(const QString &text);
    void exportToExcel();
    void onTableDoubleClicked(const QModelIndex &index);

private:
    QVBoxLayout *m_layout;
    QWidget *m_toolbar;
    QToolButton *m_refreshButton;
    QToolButton *m_exportExcelButton;
    QLineEdit *m_searchEdit;
    QTableView *m_view;

    UniversalTableModel *m_sourceModel;
    MultiFilterProxyModel *m_proxyModel;
};

#endif // SEARCHABLETABLEVIEW_H
