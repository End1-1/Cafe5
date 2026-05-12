#pragma once

#include "c5dialog.h"
#include <QString>
#include <QVector>

namespace Ui
{
class DlgKitchenSettings;
}

class C5User;
class QSpinBox;
class QCheckBox;

/**
 * Settings dialog for the kitchen-in-progress table:
 *  - per-column visibility (checkbox)
 *  - per-column width (spin box, pixels)
 *  - reset to defaults
 *
 * Caller passes a list of column descriptors and gets back the
 * (possibly) modified list when the user accepts the dialog.
 */
struct KitchenColSetting {
    QString id;
    QString label;
    bool visible = true;
    int width = 100;
    int defaultWidth = 100;
    bool defaultVisible = true;
    int minWidth = 30;
    int maxWidth = 1000;
};

class DlgKitchenSettings : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgKitchenSettings(C5User *user,
                                const QVector<KitchenColSetting> &cols,
                                QWidget *parent = nullptr);
    ~DlgKitchenSettings() override;

    /** Result after accept() - widths/visibility taken from controls. */
    QVector<KitchenColSetting> result() const { return mCols; }

private slots:
    void on_btnSave_clicked();

    void on_btnCancel_clicked();

    void on_btnReset_clicked();

private:
    Ui::DlgKitchenSettings *ui;

    QVector<KitchenColSetting> mCols;

    void populateRows();

    void readControlsIntoModel();

    QCheckBox *checkBoxAt(int row) const;

    QSpinBox *spinBoxAt(int row) const;
};
