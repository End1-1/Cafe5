#include "dlgupdaterequired.h"
#include "ui_dlgupdaterequired.h"

#include <QLabel>
#include <QMouseEvent>
#include <QRegularExpression>
#include <QToolButton>

DlgUpdateRequired::DlgUpdateRequired(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgUpdateRequired)
{
    ui->setupUi(this);
    setModal(true);
    setWindowModality(Qt::ApplicationModal);
    // Frameless + translucent: looks like other modules' update prompt.
    // Drag by mouse on empty area; buttons still receive clicks.
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowOpacity(0.92);
    setMinimumSize(420, 280);
}

DlgUpdateRequired::~DlgUpdateRequired()
{
    delete ui;
}

void DlgUpdateRequired::setUpdateInfo(const QString &message, const QString &oldVersion, const QString &newVersion)
{
    QString body = message;
    body.replace(QStringLiteral("<br>"), QStringLiteral("\n"), Qt::CaseInsensitive);
    body.replace(QRegularExpression(QStringLiteral("<[^>]+>")), QString());

    QString oldVer = oldVersion.trimmed();
    QString newVer = newVersion.trimmed();
    if (oldVer.isEmpty() || newVer.isEmpty()) {
        const QRegularExpression re(QStringLiteral("(\\d+(?:\\.\\d+){1,3})\\s*(?:→|->|=>)\\s*(\\d+(?:\\.\\d+){1,3})"));
        const auto m = re.match(body);
        if (m.hasMatch()) {
            if (oldVer.isEmpty()) {
                oldVer = m.captured(1);
            }
            if (newVer.isEmpty()) {
                newVer = m.captured(2);
            }
            body.remove(m.capturedStart(), m.capturedLength());
        }
    }

    body = body.trimmed();
    while (body.contains(QStringLiteral("\n\n\n"))) {
        body.replace(QStringLiteral("\n\n\n"), QStringLiteral("\n\n"));
    }
    if (body.isEmpty()) {
        body = tr("A new version of the application is required.");
    }
    ui->lblMessage->setText(body);

    if (!oldVer.isEmpty() && !newVer.isEmpty()) {
        ui->lblVersions->setText(QStringLiteral("%1 → %2").arg(oldVer, newVer));
        ui->lblVersions->show();
    } else if (!newVer.isEmpty()) {
        ui->lblVersions->setText(newVer);
        ui->lblVersions->show();
    } else {
        ui->lblVersions->hide();
    }
}

void DlgUpdateRequired::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // Drag window; ignore presses on buttons (they handle themselves).
        if (!childAt(event->pos()) || qobject_cast<QLabel *>(childAt(event->pos()))) {
            m_dragging = true;
            m_dragOffset = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept();
            return;
        }
    }
    QDialog::mousePressEvent(event);
}

void DlgUpdateRequired::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragOffset);
        event->accept();
        return;
    }
    QDialog::mouseMoveEvent(event);
}

void DlgUpdateRequired::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
    }
    QDialog::mouseReleaseEvent(event);
}

void DlgUpdateRequired::on_btnYes_clicked()
{
    accept();
}

void DlgUpdateRequired::on_btnNo_clicked()
{
    reject();
}
