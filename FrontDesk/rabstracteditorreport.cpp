#include "rabstracteditorreport.h"
#include "ui_rabstracteditorreport.h"
#include "c5user.h"
#include "dict_workstation.h"
#include "rabstracteditordialog.h"
#include <QAbstractTableModel>
#include <QDialog>
#include <QJsonArray>

class RAbstractEditorTableModel: public QAbstractTableModel
{
public:
    QJsonArray mRows;
    QStringList mHeaders;

    int rowCount(const QModelIndex&) const override
    {
        return mRows.size();
    }

    int columnCount(const QModelIndex&) const override
    {
        return mHeaders.size();
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        if(!index.isValid() || role != Qt::DisplayRole)
            return {};

        const QJsonArray row = mRows[index.row()].toArray();

        return row.at(index.column()).toVariant();
    }

    QVariant headerData(int section, Qt::Orientation o, int role) const override
    {
        if(role != Qt::DisplayRole)
            return {};

        return (o == Qt::Horizontal) ? QVariant(mHeaders[section]) : QVariant(section + 1);
    }

    void setJson(const QJsonObject &jdoc)
    {
        beginResetModel();
        mRows = jdoc["rows"].toArray();
        mHeaders.clear();

        for(auto h : jdoc["headers"].toArray()) {
            mHeaders.append(h.toString());
        }

        endResetModel();
    }
};

RAbstractEditorReport::RAbstractEditorReport(const QString &title, QIcon icon, const QString &editorName)
    : C5Widget(),
      ui(new Ui::RAbstractEditorReport)
{
    ui->setupUi(this);
    fLabel = title;
    fIcon = icon;
    mEditorName = editorName;
    mModel = new RAbstractEditorTableModel();
    ui->tbl->setModel(mModel);
}

RAbstractEditorReport::~RAbstractEditorReport()
{
    delete ui;
}

QToolBar* RAbstractEditorReport::toolBar()
{
    if(!fToolBar) {
        C5Widget::toolBar();
        auto *action = fToolBar->addAction(QIcon(":/new-file.png"), tr("New"), this, []() {
        });
        action->setProperty("name", "new");
        action = fToolBar->addAction(QIcon(":/reload.png"), tr("Reload"), this, [this] {
            getData();
        });
        action->setProperty("name", "reload");
        action = fToolBar->addAction(QIcon(":/cancel.png"), tr("Delete"), this, [] {

        });
        action->setProperty("name", "delete");
    }

    return fToolBar;
}

void RAbstractEditorReport::showEvent(QShowEvent *e)
{
    C5Widget::showEvent(e);

    if(mFirstLoad) {
        mFirstLoad = false;
        getData();
    }
}

void RAbstractEditorReport::on_tbl_doubleClicked(const QModelIndex &index)
{
    if(!index.isValid()) {
        return;
    }

    QDialog *dialog = nullptr;

    if(mEditorName == "Workstations") {
        int type = mModel->data(mModel->index(index.row(), 1), Qt::DisplayRole).toInt();

        switch(type) {
        case WORKSTATION_WAITER:
            dialog = createEditorDialog("Workstations");
            break;

        default:
            Q_ASSERT_X(false, "check editor type", QString("Invalid type of editor %1").arg(type).toLatin1());
            break;
        }
    }

    if(dialog) {
        if(dialog->exec() == QDialog::Accepted) {
        }

        dialog->deleteLater();
    }
}

void RAbstractEditorReport::getData()
{
    NInterface::query1("/engine/v2/officen/editors/get-all", mUser->mSessionKey, this, {{"editor", mEditorName}},
    [this](const QJsonObject & jdoc) {
        mModel->setJson(jdoc);

        for(auto column : jdoc["hidden_columns"].toArray()) {
            ui->tbl->setColumnHidden(column.toInt(), true);
        }

        QJsonObject jtoolBar = jdoc["toolbar"].toObject();

        for(auto *a : fToolBar->actions()) {
            if(a->property("name").isValid()) {
                a->setVisible(jtoolBar[a->property("name").toString()].toBool());
            }
        }
    });
}

RAbstractEditorDialog* RAbstractEditorReport::createEditorDialog(const QString &editorName)
{
    auto *d = new RAbstractEditorDialog(mUser, editorName);
    return d;
}
