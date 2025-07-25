#include "c5editor.h"
#include "ui_c5editor.h"
#include "ce5editor.h"
#include "c5message.h"
#include <QShortcut>
#include <QKeyEvent>

C5Editor::C5Editor() :
    C5Dialog(),
    ui(new Ui::C5Editor)
{
    ui->setupUi(this);
    fVerticalLayout = ui->vl;
    QShortcut *s = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this);
    connect(s, SIGNAL(activated()), this, SLOT(on_btnSave_clicked()));
    installEventFilter(this);
}

C5Editor::~C5Editor()
{
    delete ui;
}

C5Editor* C5Editor::createEditor(CE5Editor *e, int id)
{
    C5Editor *de = new C5Editor();
    e->fFocusNextChild = false;
    de->fEditor = e;
    de->fVerticalLayout->addWidget(e);
    de->adjustSize();
    e->getLineEdit(e);
    e->clear();
    e->setId(id);
    de->ui->btnCopy->setVisible(e->canCopy());
    de->setWindowTitle(e->title());

    if(e->b1()) {
        de->insertButton(e->b1());
    }

    e->fEditor = de;
    connect(e, &CE5Editor::Accept, de, &C5Editor::EditorAccepted);
    return de;
}

bool C5Editor::getResult(QList<QMap<QString, QVariant> >& data)
{
    fEditor->focusFirst();

    if(exec() == QDialog::Accepted) {
        data = fData;
        return true;
    }

    return false;
}

bool C5Editor::getJsonObject(QJsonObject &j)
{
    fEditor->focusFirst();

    if(exec() == QDialog::Accepted) {
        j = fEditor->fJsonData;
        return true;
    }

    return false;
}

void C5Editor::reject()
{
    fEditor->clear();
    C5Dialog::reject();
}

void C5Editor::insertButton(QPushButton *b)
{
    ui->horizontalLayout->insertWidget(3, b);
}

bool C5Editor::event(QEvent *e)
{
    if(e->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);

        if(ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) {
            if(ke->modifiers() &Qt::ControlModifier) {
                if(ke->modifiers() &Qt::ShiftModifier) {
                    on_btnSaveAndNew_clicked();
                } else {
                    on_btnSave_clicked();
                }
            }
        }
    }

    return C5Dialog::event(e);
}

void C5Editor::closeEvent(QCloseEvent *e)
{
    fEditor->clear();
    C5Dialog::closeEvent(e);
}

void C5Editor::EditorAccepted()
{
    fEditor->clear();
    accept();
}

void C5Editor::on_btnSave_clicked()
{
    QString err;

    if(!fEditor->checkData(err)) {
        C5Message::error(err);
        return;
    }

    err = "";

    if(!fEditor->save(err, fData)) {
        C5Message::error(err);
        return;
    }

    setProperty("saveandnew", false);

    if(fEditor->isOnline()) {
        return;
    }

    if(!fEditor->acceptOnSave()) {
        fEditor->clear();
        accept();
    }
}

bool C5Editor::on_btnSaveAndNew_clicked()
{
    QString err;

    if(!fEditor->checkData(err)) {
        C5Message::error(err);
        return false;
    }

    err = "";

    if(!fEditor->save(err, fData)) {
        C5Message::error(err);
        return false;
    }

    setProperty("saveandnew", true);

    if(fEditor->isOnline()) {
        return true;
    }

    if(!fEditor->acceptOnSave()) {
        fEditor->clear();
    }

    return true;
}

void C5Editor::on_btnCancel_clicked()
{
    fEditor->clear();
    reject();
}

void C5Editor::on_btnCopy_clicked()
{
    fEditor->copyObject();
}
