#pragma once

#include "rabstracteditorreport.h"

class RPrintersReport : public RAbstractEditorReport
{
    Q_OBJECT

public:
    explicit RPrintersReport(const QString &title, QIcon icon, const QString &editorName,
                             QWidget *parent = nullptr);

protected:
    void on_tbl_doubleClicked(const QModelIndex &index) override;
    void newData() override;
    void removeAction() override;

private:
    void openEditor(int id);
    int selectedRowId() const;
};
