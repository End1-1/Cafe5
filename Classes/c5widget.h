#ifndef C5WIDGET_H
#define C5WIDGET_H

#include "c5database.h"
#include "c5permissions.h"
#include "c5config.h"
#include <QWidget>
#include <QIcon>
#include <QToolBar>

class C5Widget : public QWidget
{
    Q_OBJECT

public:
    friend class C5Editor;

    enum ToolBarButtons {tbNone = 0, tbNew, tbEdit, tbSave, tbRefresh, tbFilter, tbClearFilter, tbPrint, tbExcel, tbDelete };

    explicit C5Widget(const QStringList &dbParams, QWidget *parent = nullptr);

    QIcon icon();

    QString label();

    virtual void postProcess();

    virtual QToolBar *toolBar();

    virtual QWidget *widget();

    virtual void hotKey(const QString &key);

    virtual void selectorCallback(int row, const QList<QVariant> &values);

    virtual bool event(QEvent *event);

    virtual void changeDatabase(const QStringList &dbParams);

    virtual bool allowChangeDatabase();

protected:
    QToolBar *fToolBar;

    QStringList fDBParams;

    QString fIcon;

    QString fLabel;

    bool fFocusNextChild;

    bool createToolBar();

    QToolBar *createStandartToolbar(const QList<ToolBarButtons> &btn);

    virtual void nextChild();

protected slots:
    virtual void print();

};

#endif // C5WIDGET_H
