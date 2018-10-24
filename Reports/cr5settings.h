#ifndef CR5SETTINGS_H
#define CR5SETTINGS_H

#include "c5reportwidget.h"

class C5SettingsWidget;

class CR5Settings : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Settings(const QStringList &dbParams, QWidget *parent = 0);

    virtual QToolBar *toolBar();

private:
    C5SettingsWidget *fSettingsWidget;

protected slots:
    virtual void saveDataChanges();

    virtual void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private slots:
    void removeSettings();
};

#endif // CR5SETTINGS_H
