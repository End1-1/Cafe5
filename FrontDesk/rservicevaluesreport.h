#pragma once

#include "rabstracteditorreport.h"

class RServiceValuesReport : public RAbstractEditorReport
{
    Q_OBJECT

public:
    explicit RServiceValuesReport(const QString &title, QIcon icon, const QString &editorName, QWidget *parent = nullptr);

protected:
    void newData() override;
    void removeAction() override;

private:
    void openEditor(int id);
    int selectedRowId() const;
};
