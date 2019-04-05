#ifndef C5TOOLBARWIDGET_H
#define C5TOOLBARWIDGET_H

#include <QWidget>

namespace Ui {
class C5ToolBarWidget;
}

class C5ToolBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit C5ToolBarWidget(QWidget *parent = nullptr);

    ~C5ToolBarWidget();

    void setUpdateButtonVisible(bool v);

private slots:
    void on_btnUpdate_clicked();

private:
    Ui::C5ToolBarWidget *ui;
};

#endif // C5TOOLBARWIDGET_H
