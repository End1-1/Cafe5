#include "c5menu.h"
#include "ui_c5menu.h"

C5Menu::C5Menu(QWidget *parent) : QWidget(parent), ui(new Ui::C5Menu) {
    ui->setupUi(this);
}

C5Menu::~C5Menu() { delete ui; }
