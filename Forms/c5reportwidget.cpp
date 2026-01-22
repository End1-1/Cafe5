#include "c5reportwidget.h"
#include "c5cache.h"
#include "c5tablemodel.h"
#include "ce5editor.h"
#include <QIcon>
#include <QLayout>
#include <QLabel>
#include <QTimer>

C5ReportWidget::C5ReportWidget(QWidget *parent) :
    C5Grid(parent)
{
    fIconName = ":/configure.png";
    fLabel = "C5ReportWidget";
    fToolBar = 0;
    fEditor = nullptr;
    QLabel *l = new QLabel(tr("Global search"), widget());
    fFilterLineEdit = new C5LineEdit(widget());
    //fFilterLineEdit->setPlaceholderText(tr("Use \"|\" for separator multiple values"));
    widget()->layout()->addWidget(l);
    widget()->layout()->addWidget(fFilterLineEdit);
    widget()->setVisible(false);
    connect(fFilterLineEdit, &C5LineEdit::textChanged, [this](const QString & arg1) {
        fSearchTimer->start();
    });
    fSearchTimer = new QTimer(this);
    fSearchTimer->setSingleShot(true);
    fSearchTimer->setInterval(600);
    connect(fSearchTimer, &QTimer::timeout, this, &C5ReportWidget::searchText);
}

C5ReportWidget::~C5ReportWidget()
{
    if(fToolBar) {
        delete fToolBar;
    }

    if(fEditor) {
        delete fEditor;
    }
}

bool C5ReportWidget::hasDataUpdates()
{
    return static_cast<C5TableModel*>(fTableView->model())->hasUpdates();
}

void C5ReportWidget::hotKey(const QString &key)
{
    if(key.toLower() == "ctrl+f") {
        widget()->setVisible(true);
        fFilterLineEdit->setFocus();
        fFilterLineEdit->setSelection(0, fFilterLineEdit->text().length());
    } else if(key.toLower() == "esc") {
        widget()->setVisible(false);
    } else {
        C5Grid::hotKey(key);
    }
}

bool C5ReportWidget::on_tblView_doubleClicked(const QModelIndex &index)
{
    if(index.row() < 0 || index.column() < 0) {
        return false;
    }

    QJsonArray values = fModel->getRowValues(index.row());

    if(tblDoubleClicked(index.row(), index.column(), values)) {
        return false;
    }

    if(fEditor) {
        if(values.count() > 0) {
            C5Editor *e = C5Editor::createEditor(mUser, fEditor, values.at(0).toInt());
            QList<QMap<QString, QVariant> > data;
            bool yes = e->getResult(data);
            fEditor->setParent(nullptr);
            delete e;

            if(!yes) {
                return false;
            }

            int row = index.row();

            for(int i = 0; i < data.count(); i++) {
                if(i > 0) {
                    fModel->insertRow(row);
                    row++;
                }

                for(QMap<QString, QVariant>::const_iterator it = data.at(i).begin(); it != data.at(i).end(); it++) {
                    int col = fModel->indexForColumnName(it.key());

                    if(col > -1) {
                        fModel->setData(row, col, it.value());
                    }
                }
            }
        }
    }

    return fEditor != nullptr;
}

C5Cache* C5ReportWidget::createCache(int cacheId)
{
    return C5Cache::cache(cacheId);
}

void C5ReportWidget::clearFilter()
{
    fFilterLineEdit->clear();
    widget()->setVisible(false);
    C5Grid::clearFilter();
}

void C5ReportWidget::completeRefresh()
{
    C5Grid::completeRefresh();
    fModel->setFilter(-1, fFilterLineEdit->text());
    sumColumnsData();
}

int C5ReportWidget::newRow()
{
    if(fEditor == nullptr) {
        return -1;
    }

    C5Editor *e = C5Editor::createEditor(mUser, fEditor, 0);
    QList<QMap<QString, QVariant> > data;
    bool yes = e->getResult(data);
    fEditor->setParent(nullptr);
    delete e;

    if(!yes) {
        return -1;
    }

    int row = 0;
    QModelIndexList ml = fTableView->selectionModel()->selectedIndexes();

    if(ml.count() > 0) {
        row = ml.at(0).row();
    } else {
        row = fModel->rowCount();
    }

    for(int i = 0; i < data.count(); i++) {
        fModel->insertRow(row);

        if(ml.count() > 0) {
            row++;
        }

        for(QMap<QString, QVariant>::const_iterator it = data.at(i).begin(); it != data.at(i).end(); it++) {
            int col = fModel->indexForColumnName(it.key());

            if(col > -1) {
                fModel->setData(row, col, it.value());
            }
        }
    }

    fTableView->setCurrentIndex(fModel->index(row + 1, 0));
    return row;
}

void C5ReportWidget::searchText()
{
    fModel->setFilter(-1, fFilterLineEdit->text());
    sumColumnsData();
}
