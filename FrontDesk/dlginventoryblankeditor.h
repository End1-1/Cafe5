#pragma once

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class DlgInventoryBlankEditor;
}

class DlgInventoryBlankEditor : public QDialog
{
    Q_OBJECT

public:
    explicit DlgInventoryBlankEditor(const QString &sessionKey,
                                     const QJsonObject &reportFilter,
                                     QWidget *parent = nullptr);
    ~DlgInventoryBlankEditor() override;

    void setBlank(const QJsonObject &blank);
    QJsonObject blank() const;

    static bool edit(const QString &sessionKey,
                     const QJsonObject &reportFilter,
                     QJsonObject &blank,
                     QWidget *parent = nullptr);
    static void printBlank(const QJsonObject &blank, QWidget *parent);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onAddRow();
    void onRemoveRow();
    void onRefresh();
    void onSave();
    void cleanupEmptyRows();

private:
    void appendItemRow(const QJsonObject &item, const QString &storeName, int storeId);
    void fillBuildParams(QJsonObject &params) const;
    static QString itemKey(const QJsonObject &item);
    static bool isItemEmpty(const QJsonObject &item);

    Ui::DlgInventoryBlankEditor *ui;
    QString mSessionKey;
    QJsonObject mReportFilter;
    int mBlankId = 0;
    QString mCreated;
};
