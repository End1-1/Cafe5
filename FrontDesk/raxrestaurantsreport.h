#pragma once

#include "rabstracteditorreport.h"

class RAxRestaurantsReport : public RAbstractEditorReport
{
    Q_OBJECT

public:
    explicit RAxRestaurantsReport(const QString &title, QIcon icon, const QString &editorName,
                                  QWidget *parent = nullptr);

protected:
    void newData() override;
    void removeAction() override;
    void on_tbl_doubleClicked(const QModelIndex &index) override;

private:
    void openEditor(int id);
    int selectedRowId() const;
};
