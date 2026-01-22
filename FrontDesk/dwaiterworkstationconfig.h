#pragma once

#include <QDialog>

namespace Ui {
class DWaiterWorkstationConfig;
}

class DWaiterWorkstationConfig : public QDialog {
    Q_OBJECT
  public:
    explicit DWaiterWorkstationConfig(QWidget *parent = nullptr);
    ~DWaiterWorkstationConfig();
  private:
    Ui::DWaiterWorkstationConfig *ui;
};
