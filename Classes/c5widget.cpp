#include "c5widget.h"
#include <QEvent>
#include <QKeyEvent>
#include <QUuid>

C5Widget::C5Widget(const QStringList &dbParams, QWidget *parent) :
    QWidget(parent),
    fDBParams(dbParams)
{
    fToolBar = nullptr;
    fFocusNextChild = true;
    fWindowUuid = QUuid::createUuid().toString();
    fHttp = new NInterface(this);
}

C5Widget::~C5Widget()
{
    fHttp->deleteLater();
}

QIcon C5Widget::icon()
{
    return QIcon(fIcon);
}

QString C5Widget::label()
{
    return fLabel;
}

void C5Widget::postProcess()
{
}

QToolBar *C5Widget::toolBar()
{
    createToolBar();
    return fToolBar;
}

bool C5Widget::createToolBar()
{
    if (!fToolBar) {
        fToolBar = new QToolBar();
        fToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        return true;
    }
    return false;
}

QToolBar *C5Widget::createStandartToolbar(const QList<ToolBarButtons> &btn)
{
    if (createToolBar()) {
        if (btn.contains(ToolBarButtons::tbNew)) {
            fToolBar->addAction(QIcon(":/new.png"), tr("New"), this, SLOT(newRow()));
        }
        if (btn.contains(ToolBarButtons::tbEdit)) {
            fToolBar->addAction(QIcon(":/edit.png"), tr("Edit"), this, SLOT(editRow()));
        }
        if (btn.contains(ToolBarButtons::tbDelete)) {
            fToolBar->addAction(QIcon(":/delete.png"), tr("Remove"), this, SLOT(removeRow()));
        }
        if (btn.contains(ToolBarButtons::tbSave)) {
            fToolBar->addAction(QIcon(":/save.png"), tr("Save\nchanges"), this, SLOT(saveDataChanges()));
        }
        if (btn.contains(ToolBarButtons::tbFilter)) {
            auto *a = fToolBar->addAction(QIcon(":/filter_set.png"), tr("Search by\nparameters"), this,
                                          SLOT(setSearchParameters()));
            a->setProperty("filter", true);
        }
        if (btn.contains(ToolBarButtons::tbClearFilter)) {
            fToolBar->addAction(QIcon(":/filter_clear.png"), tr("Clear\nfilter"), this, SLOT(clearFilter()));
        }
        if (btn.contains(ToolBarButtons::tbRefresh)) {
            fToolBar->addAction(QIcon(":/refresh.png"), tr("Refresh"), this, SLOT(refreshData()));
        }
        if (btn.contains(ToolBarButtons::tbPrint)) {
            fToolBar->addAction(QIcon(":/print.png"), tr("Print"), this, SLOT(print()));
        }
        if (btn.contains(ToolBarButtons::tbExcel)) {
            fToolBar->addAction(QIcon(":/excel.png"), tr("Export to\nMS Excel"), this, SLOT(exportToExcel()));
        }
    }
    return fToolBar;
}

void C5Widget::nextChild()
{
}

void C5Widget::print()
{
}

QWidget *C5Widget::widget()
{
    return nullptr;
}

void C5Widget::hotKey(const QString &key)
{
    Q_UNUSED(key);
}

void C5Widget::selectorCallback(int row, const QVector<QJsonValue> &values)
{
    Q_UNUSED(values);
    Q_UNUSED(row);
}

bool C5Widget::event(QEvent *event)
{
    if (event->type() == QEvent::KeyRelease) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        switch (ke->key()) {
            case Qt::Key_Enter:
            case Qt::Key_Return:
                if (ke->modifiers() &Qt::ControlModifier) {
                    break;
                } else {
                    if (fFocusNextChild) {
                        focusNextChild();
                    } else {
                        nextChild();
                    }
                    return true;
                }
        }
    }
    return QWidget::event(event);
}

void C5Widget::changeDatabase(const QStringList &dbParams)
{
    fDBParams = dbParams;
}

bool C5Widget::allowChangeDatabase()
{
    return true;
}

bool C5Widget::parseBroadcastMessage(int what, const QString &msg, QString &replystr)
{
    Q_UNUSED(what);
    Q_UNUSED(msg);
    Q_UNUSED(replystr);
    return true;
}

bool C5Widget::reportHandler(const QString &handleId, const QVariant &data)
{
    Q_UNUSED(handleId);
    Q_UNUSED(data);
    return true;
}
