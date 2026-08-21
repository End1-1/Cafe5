#pragma once

#include "storeinputxmlimport.h"
#include <QDialog>

namespace Ui {
class DlgStoreInputXmlImportOptions;
}

class DlgStoreInputXmlImportOptions : public QDialog
{
    Q_OBJECT

public:
    explicit DlgStoreInputXmlImportOptions(QWidget *parent = nullptr);
    ~DlgStoreInputXmlImportOptions() override;

    StoreInputXmlImportOptions options() const;
    void setOptions(const StoreInputXmlImportOptions &options);

    static bool edit(StoreInputXmlImportOptions &options, QWidget *parent = nullptr);

protected:
    void accept() override;

private:
    Ui::DlgStoreInputXmlImportOptions *ui;
};
