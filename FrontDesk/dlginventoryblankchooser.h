#pragma once

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class DlgInventoryBlankChooser;
}

class DlgInventoryBlankChooser : public QDialog
{
    Q_OBJECT

public:
    explicit DlgInventoryBlankChooser(const QString &sessionKey,
                                      const QJsonObject &reportFilter,
                                      QWidget *parent = nullptr);
    ~DlgInventoryBlankChooser() override;

    static void open(const QString &sessionKey,
                     const QJsonObject &reportFilter,
                     QWidget *parent = nullptr);

private slots:
    void reloadList();
    void onNew();
    void onEdit();
    void onPrint();
    void onDelete();

private:
    int selectedId() const;
    void loadAndEdit(int id);
    void loadAndPrint(int id);

    Ui::DlgInventoryBlankChooser *ui;
    QString mSessionKey;
    QJsonObject mReportFilter;
};
