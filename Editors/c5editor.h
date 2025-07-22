#ifndef C5EDITOR_H
#define C5EDITOR_H

#include "c5dialog.h"
#include <QVBoxLayout>

namespace Ui
{
class C5Editor;
}

class CE5Editor;

class C5Editor : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5Editor();

    ~C5Editor();

    static C5Editor *createEditor(CE5Editor *e, int id);

    bool getResult(QList<QMap<QString, QVariant> > &data);

    bool getJsonObject(QJsonObject &j);

    virtual void reject() override;

    void insertButton(QPushButton *b);

    bool event(QEvent *e) override;

protected:
    virtual void closeEvent(QCloseEvent *e);

private slots:
    void EditorAccepted();

    void on_btnSave_clicked();

    bool on_btnSaveAndNew_clicked();

    void on_btnCancel_clicked();

    void on_btnCopy_clicked();

private:
    Ui::C5Editor *ui;

    QVBoxLayout *fVerticalLayout;

    CE5Editor *fEditor;

    QList<QMap<QString, QVariant> > fData;
};

#endif // C5EDITOR_H
