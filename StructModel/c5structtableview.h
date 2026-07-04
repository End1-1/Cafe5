#pragma once

#include "c5dialog.h"
#include "c5structmodel.h"
#include "c5jsonparser.h"
#include <QTableView>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonArray>
#include <QHash>
#include <QMap>

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
    static QVector<T> get(const QString &searchEngine, bool getAllListFirst, bool multiSelect, QPoint point);;

    static void updateGoodsLastInputPrices(const QHash<int, double> &prices);

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

    void on_btnRefreash_clicked();

private:
    Ui::C5StructTableView* ui;

    QTableView* tableView();

    QString mSearchEngine;

    QTimer* mSearchTimer = nullptr;

    int mSearchDelay = 400;

    bool mEmptySearch = true;

    QString mLastRequestId;

    QString mReloadRequestId;

    static QMap<QString, QJsonArray> sLastResultsByEngine;

    QMap<QString, QString> selectorTitles();

};

template<typename T>
inline QVector<T> C5StructTableView::get(const QString &searchEngine, bool getAllListFirst, bool multiSelect, QPoint point)
{
    C5StructTableView tv;
    tv.mInitialPos = point;
    tv.setWindowTitle(tv.selectorTitles().value(searchEngine));
    tv.mSearchEngine = searchEngine;
    tv.mEmptySearch = getAllListFirst;
    auto *model = new C5StructModel<T>(&tv);
    tv.tableView()->setModel(model);

    if(sLastResultsByEngine.contains(searchEngine)) {
        model->setData(parseJsonArray<T>(sLastResultsByEngine.value(searchEngine)));
        tv.tableView()->resizeColumnsToContents();
    }

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
