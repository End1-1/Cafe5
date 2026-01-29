#pragma once

#include "c5dialog.h"

namespace Ui
{
class RAbstractEditorDialog;
}

class RAbstractSpecialWidget;

class RAbstractEditorDialog : public C5Dialog
{
    Q_OBJECT
public:
    explicit RAbstractEditorDialog(C5User *user, const QString &editorName);

    ~RAbstractEditorDialog();

    void setId(const QJsonValue &id);

protected:
    virtual void showEvent(QShowEvent *e) override;

private slots:
    void on_btnCancel_clicked();

    void on_btnSave_clicked();

private:
    Ui::RAbstractEditorDialog* ui;

    QString mEditorName;

    RAbstractSpecialWidget* mWidget;

    void createWidget();

    QJsonValue mId;
};
