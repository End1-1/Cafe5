#include "dlgdatetimepicker.h"

DlgDateTimePicker::DlgDateTimePicker(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Set date"));
    setMinimumSize(600, 450); // Крупный размер для пальцев

    auto *mainLayout = new QVBoxLayout(this);
    auto *selectorsLayout = new QHBoxLayout();

    // Создаем колонки и сразу добавляем их лайауты в горизонтальный ряд
    selectorsLayout->addLayout(createColumn("День", 0).layout);
    selectorsLayout->addLayout(createColumn("Мес", 1).layout);
    selectorsLayout->addLayout(createColumn("Год", 2).layout);
    selectorsLayout->addSpacing(30); // Разделитель между датой и временем
    selectorsLayout->addLayout(createColumn("Час", 3).layout);
    selectorsLayout->addLayout(createColumn("Мин", 4).layout);

    mainLayout->addLayout(selectorsLayout);

    // Кнопки ОК / Отмена
    auto *btnLayout = new QHBoxLayout();
    auto *btnOk = new QPushButton(tr("ОК"));
    auto *btnCancel = new QPushButton(tr("Cancel"));

    // Стили под тачскрин (POS)
    QString btnStyle
        = "QPushButton { font-size: 22px; min-height: 70px; background: #e1e1e1; border-radius: 0px; border: 1px solid #999; } "
          "QPushButton:pressed { background: #bbb; }";
    btnOk->setStyleSheet(btnStyle + "QPushButton { background: #4CAF50; color: white; font-weight: bold; }");
    btnCancel->setStyleSheet(btnStyle);

    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnOk);
    mainLayout->addLayout(btnLayout);

    connect(btnOk, &QPushButton::clicked, this, &DlgDateTimePicker::accept);
    connect(btnCancel, &QPushButton::clicked, this, &DlgDateTimePicker::reject);

    m_dt = QDateTime::currentDateTime();
    updateLabels();
}

DlgDateTimePicker::PickerColumn DlgDateTimePicker::createColumn(const QString &title, int type)
{
    auto *colLayout = new QVBoxLayout();

    auto *btnUp = new QPushButton("▲");
    auto *lblVal = new QLabel("--");
    auto *btnDown = new QPushButton("▼");
    auto *lblTitle = new QLabel(title);

    // Стили колонок
    QString style = "QPushButton { font-size: 28px; min-width: 90px; min-height: 90px; background: #f8f8f8; } "
                    "QLabel { font-size: 24px; font-weight: bold; qproperty-alignment: AlignCenter; }";

    btnUp->setStyleSheet(style);
    btnDown->setStyleSheet(style);
    lblVal->setStyleSheet(style + "QLabel { background: green; border: 1px solid #ccc; }");
    lblTitle->setStyleSheet("font-size: 16px; color: #555; qproperty-alignment: AlignCenter; text-transform: uppercase;");

    colLayout->addWidget(lblTitle);
    colLayout->addWidget(btnUp);
    colLayout->addWidget(lblVal);
    colLayout->addWidget(btnDown);

    PickerColumn col = {btnUp, btnDown, lblVal, colLayout, type};
    m_columns.append(col);

    connect(btnUp, &QPushButton::clicked, this, &DlgDateTimePicker::adjustValue);
    connect(btnDown, &QPushButton::clicked, this, &DlgDateTimePicker::adjustValue);

    return col;
}

void DlgDateTimePicker::adjustValue()
{
    auto *btn = qobject_cast<QPushButton *>(sender());
    if (!btn)
        return;

    int delta = 0;
    int type = -1;

    for (const auto &col : m_columns) {
        if (col.btnUp == btn) {
            delta = 1;
            type = col.type;
            break;
        }
        if (col.btnDown == btn) {
            delta = -1;
            type = col.type;
            break;
        }
    }

    if (type == -1)
        return;

    switch (type) {
    case 0:
        m_dt = m_dt.addDays(delta);
        break;
    case 1:
        m_dt = m_dt.addMonths(delta);
        break;
    case 2:
        m_dt = m_dt.addYears(delta);
        break;
    case 3:
        m_dt = m_dt.addSecs(delta * 3600);
        break;
    case 4:
        m_dt = m_dt.addSecs(delta * 60);
        break;
    }
    updateLabels();
}

void DlgDateTimePicker::updateLabels()
{
    for (const auto &col : m_columns) {
        switch (col.type) {
        case 0:
            col.lblValue->setText(m_dt.toString("dd"));
            break;
        case 1:
            col.lblValue->setText(m_dt.toString("MM"));
            break;
        case 2:
            col.lblValue->setText(m_dt.toString("yyyy"));
            break;
        case 3:
            col.lblValue->setText(m_dt.toString("HH"));
            break;
        case 4:
            col.lblValue->setText(m_dt.toString("mm"));
            break;
        }
    }
}

QDateTime DlgDateTimePicker::dateTime() const
{
    return m_dt;
}

void DlgDateTimePicker::setDateTime(const QDateTime &dt)
{
    m_dt = dt;
    updateLabels();
}

void DlgDateTimePicker::accept()
{
    QDialog::accept();
}
