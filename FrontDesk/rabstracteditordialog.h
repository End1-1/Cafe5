#pragma once

#include "c5dialog.h"

namespace Ui
{
class RAbstractEditorDialog;
}

class RAbstractEditorDialog : public C5Dialog
{
    Q_OBJECT
public:
    explicit RAbstractEditorDialog(C5User *user, const QString &editorName);
    ~RAbstractEditorDialog();

protected:
    virtual void showEvent(QShowEvent *e) override;

private:
    Ui::RAbstractEditorDialog* ui;

    QString mEditorName;

    void createWidget();
};
