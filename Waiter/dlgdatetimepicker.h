#ifndef DLGDATETIMEPICKER_H
#define DLGDATETIMEPICKER_H

#include <QDateTime>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QPushButton>
#include <QVBoxLayout>

class DlgDateTimePicker : public QDialog
{
    Q_OBJECT
public:
    explicit DlgDateTimePicker(QWidget *parent = nullptr);
    void setDateTime(const QDateTime &dt);
    QDateTime dateTime() const;

private slots:
    void adjustValue();
    void accept() override;

private:
    QDateTime m_dt;

    struct PickerColumn
    {
        QPushButton *btnUp;
        QPushButton *btnDown;
        QLabel *lblValue;
        QVBoxLayout *layout; // Ссылка на лайаут колонки
        int type;            // 0-Day, 1-Month, 2-Year, 3-Hour, 4-Min
    };

    PickerColumn createColumn(const QString &title, int type);
    QList<PickerColumn> m_columns;
    void updateLabels();
};

#endif
