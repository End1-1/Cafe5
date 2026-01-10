#pragma once

#include <QWidget>

namespace Ui {
class C5Menu;
}

class C5Menu : public QWidget {
    Q_OBJECT
  public:
    explicit C5Menu(QWidget *parent = nullptr);
    ~C5Menu();
  private:
    Ui::C5Menu *ui;
};
