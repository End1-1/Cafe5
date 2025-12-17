#pragma once

#include "c5structmodel.h"
#include "c5jsonparser.h"
#include <QDialog>
#include <QTableView>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonArray>

static const QString search_storage = "search_storage";
static const QString search_goods = "search_goods_item";

namespace Ui
{
class C5StructTableView;
}

class C5StructTableView : public QDialog
{
    Q_OBJECT
public:
    explicit C5StructTableView(QWidget *parent = nullptr);
    ~C5StructTableView();

    template<typename T>
    static QVector<T> get(QWidget *parent, const QString &searchEngine, bool getAllListFirst, bool multiSelect)
    {
        C5StructTableView tv(parent);
        tv.mSearchEngine = searchEngine;
        auto *model = new C5StructModel<T>();
        tv.tableView()->setModel(model);

        if(getAllListFirst) {
            tv.on_leSearchText_textChanged("");
        }

        model->setMultiSelect(multiSelect);
        tv.exec();
        return model->selectedData();
    };

    template<typename T>
    void handleSearchResult(const QJsonArray &jarr, C5StructModel<T>* model)
    {
        QVector<T> data = parseJsonArray<T>(jarr);
        model->setData(std::move(data));
    }
private slots:
    void on_leSearchText_textChanged(const QString &arg1);

    void sendSearchRequest();

    void on_btnSelect_clicked();

    void on_tbl_doubleClicked(const QModelIndex &index);

private:
    Ui::C5StructTableView* ui;

    QTableView* tableView();

    QString mSearchEngine;

    QTimer* mSearchTimer = nullptr;

    int mSearchDelay = 400;

};
