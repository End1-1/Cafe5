#pragma once

#include "c5dialog.h"
#include "c5structmodel.h"
#include "c5jsonparser.h"
#include <QTableView>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonArray>

static const QString search_storage = "search_storage";
static const QString search_goods = "search_goods_item";
static const QString search_partner = "search_partner_item";

namespace Ui
{
class C5StructTableView;
}

class C5StructTableView : public C5Dialog
{
    Q_OBJECT
public:
    explicit C5StructTableView(C5User *user = nullptr);
    ~C5StructTableView();

    template<typename T>
    static QVector<T> get(const QString &searchEngine, bool getAllListFirst, bool multiSelect);;

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

    void on_btnCancel_clicked();

private:
    Ui::C5StructTableView* ui;

    QTableView* tableView();

    QString mSearchEngine;

    QTimer* mSearchTimer = nullptr;

    int mSearchDelay = 400;

    bool mEmptySearch = true;

};

template<typename T>
inline QVector<T> C5StructTableView::get(const QString &searchEngine, bool getAllListFirst, bool multiSelect)
{
    C5StructTableView tv;
    tv.mSearchEngine = searchEngine;
    tv.mEmptySearch = getAllListFirst;
    auto *model = new C5StructModel<T>(&tv);
    tv.tableView()->setModel(model);

    if(getAllListFirst) {
        tv.on_leSearchText_textChanged("");
    }

    model->setMultiSelect(multiSelect);

    if(tv.exec() == QDialog::Accepted) {
        return model->selectedData();
    } else {
        return {};
    }
}
