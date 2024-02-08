#include "calculator.h"
#include "ui_calculator.h"
#include <QKeyEvent>

/* fOperation
 * 0 - none
 * 43 - plus
 * 45 - minus
 * 42 - mult
 * 47 - div
 */

Calculator::Calculator(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::Calculator)
{
    ui->setupUi(this);
    fResult = 0.0;
    fOperation = 0;
    fOperationStart = false;
    fNumber = 0;
    fNumberStart = false;
    installEventFilter(this);
}

Calculator::~Calculator()
{
    delete ui;
}

bool Calculator::get(const QStringList &dbParams, double &v)
{
    bool result = false;
    Calculator *c = new Calculator(dbParams);
    if (c->exec() == QDialog::Accepted) {
        result = true;
        v = c->fResult;
    }
    delete c;
    return result;
}

bool Calculator::event(QEvent *e)
{
    if (e->type() == QEvent::KeyRelease) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);
        newKey(ke->key());
    }
    return C5Dialog::event(e);
}

void Calculator::newKey(int key)
{
    switch(key) {
    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
        if (fOperationStart) {
            fOperationStart = false;
            ui->leText->clear();
        }
        ui->leText->setText(ui->leText->text() + QChar(key));
        break;
    case 46:
    case 44:
    case 8228:
        if (fOperationStart) {
            fOperationStart = false;
            ui->leText->clear();
        }
        if (!ui->leText->text().contains(QLocale().decimalPoint())) {
            ui->leText->setText(ui->leText->text() + QLocale().decimalPoint());
        }
        break;
    case 47:
    case 42:
    case 45:
    case 43:
        if (fNumberStart) {
        }
        if (ui->plHistory->toPlainText().length() > 0) {
            QChar l = ui->plHistory->toPlainText().right(1).at(0);
            if (l.toLatin1() == 47 || l.toLatin1() == 45 || l.toLatin1() == 42 || l.toLatin1() == 43) {
                ui->plHistory->setPlainText(ui->plHistory->toPlainText().left(ui->plHistory->toPlainText().length() - 1) + QChar(key));
            } else {
                ui->plHistory->appendPlainText(ui->leText->text());
                ui->plHistory->appendPlainText("\n");
                ui->plHistory->appendPlainText(QChar(key));
            }
        } else {
            ui->plHistory->appendPlainText(ui->leText->text());
            ui->plHistory->appendPlainText("\n");
            ui->plHistory->appendPlainText(QChar(key));
        }

        fOperationStart = true;
        fOperation = key;
        fNumber = ui->leText->getDouble();
        fNumberStart = true;
        break;
    case 67:
        on_btnClear_clicked();
        break;
    case 16777219:
        ui->leText->setText(ui->leText->text().left(ui->leText->text().length() - 1));
        break;
    case 16777221:
    case 16777220:
        if (fNumberStart) {
            switch (fOperation) {
            case 42:
                fResult = fNumber * ui->leText->getDouble();
                break;
            case 43:
                fResult = fNumber + ui->leText->getDouble();
                break;
            case 45:
                fResult = fNumber - ui->leText->getDouble();
                break;
            case 47:
                if (ui->leText->getDouble() >= 0.0000001 && ui->leText->getDouble() <= 0.0000001) {
                    fResult = fNumber / ui->leText->getDouble();
                }
                break;
            }
            on_btnClear_clicked();
            ui->leText->setDouble(fResult);
        } else {
            accept();
        }
        break;
    }
}

void Calculator::buttonPressed()
{

}

void Calculator::on_btnBackspace_clicked()
{
    newKey(16777219);
}

void Calculator::on_btnSign_clicked()
{
    ui->leText->setDouble(ui->leText->getDouble() * -1);
}

void Calculator::on_btnClear_clicked()
{
    ui->leText->clear();
    ui->plHistory->clear();
    fOperation = 0;
    fOperationStart = false;
    fNumber = 0;
    fNumberStart = 0;
}

void Calculator::actionKey()
{
    QPushButton *b = static_cast<QPushButton*>(sender());
    newKey(b->text().at(0).toLatin1());
}

