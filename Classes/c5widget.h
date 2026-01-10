#ifndef C5WIDGET_H
#define C5WIDGET_H

#include "ninterface.h"
#include <QWidget>
#include <QIcon>
#include <QToolBar>

class C5User;

class C5Widget : public QWidget
{
    Q_OBJECT

public:
    friend class C5Editor;

    enum ToolBarButtons {tbNone = 0, tbNew, tbEdit, tbSave, tbRefresh, tbFilter, tbClearFilter, tbPrint, tbExcel, tbDelete };

    explicit C5Widget(QWidget *parent = nullptr);

    ~C5Widget();

    QIcon icon();

    QString label();

    virtual void postProcess();

    virtual QToolBar* toolBar();

    virtual QWidget* widget();

    virtual void hotKey(const QString &key);

    virtual void selectorCallback(int row, const QJsonArray &values);

    virtual bool event(QEvent *event);

    virtual bool allowChangeDatabase();

    virtual bool parseBroadcastMessage(int what, const QString &msg, QString &replystr);

    virtual bool reportHandler(const QString &handleId, const QVariant &data);

    QString fWindowUuid;

    C5User* mUser;

protected:
    QToolBar* fToolBar;

    QString fIcon;

    QString fLabel;

    NInterface* fHttp;

    bool fFocusNextChild;

    bool createToolBar();

    QToolBar* createStandartToolbar(const QList<ToolBarButtons>& btn);

    virtual void nextChild();

protected slots:
    virtual void print();

};

#endif // C5WIDGET_H
