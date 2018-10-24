#ifndef C5SETTINGSWIDGET_H
#define C5SETTINGSWIDGET_H

#include "c5widget.h"

namespace Ui {
class C5SettingsWidget;
}

class C5SettingsWidget : public C5Widget
{
    Q_OBJECT

public:
    explicit C5SettingsWidget(const QStringList &dbParams, QWidget *parent = 0);

    ~C5SettingsWidget();

    void setSettingsId(int id);

    void save();

    void clear(QWidget *parent);

private:
    Ui::C5SettingsWidget *ui;

    int fSettingsId;

    void clearWidgetValue(QWidget *w);

    void setWidgetValue(QWidget *w, const QString &value);

    QWidget *widget(QWidget *parent, int tag);
};

#endif // C5SETTINGSWIDGET_H
