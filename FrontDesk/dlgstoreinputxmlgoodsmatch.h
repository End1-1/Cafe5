#pragma once

#include "storeinputxmlimport.h"
#include <QDialog>
#include <QVector>

class C5CodeNameSelector;
class QCheckBox;

namespace Ui
{
class DlgStoreInputXmlGoodsMatch;
}

class DlgStoreInputXmlGoodsMatch : public QDialog
{
    Q_OBJECT

public:
    explicit DlgStoreInputXmlGoodsMatch(const QVector<StoreInputXmlGoodLine> &lines, QWidget *parent = nullptr);

    ~DlgStoreInputXmlGoodsMatch() override;

    QVector<StoreInputXmlGoodsMappingRow> rows() const;

private slots:
    void tryAccept();

private:
    Ui::DlgStoreInputXmlGoodsMatch *ui;

    QVector<StoreInputXmlGoodLine> mSourceLines;

    QVector<C5CodeNameSelector *> mSelectors;

    QVector<QCheckBox *> mUpdateChecks;
};
