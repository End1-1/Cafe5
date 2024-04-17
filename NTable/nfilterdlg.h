#ifndef NFILTERDLG_H
#define NFILTERDLG_H

#include <QDialog>
#include <QJsonArray>

namespace Ui {
class NFilterDlg;
}

class NLoadingDlg;

class NFilterDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NFilterDlg(QWidget *parent = nullptr);
    ~NFilterDlg();
    void setup(const QJsonArray &fields);
    QJsonObject filter() const;
    QVariant filterValue(const QString &name);
    void clear();

private slots:
    void openSuggestions();
    void queryStarted();
    void queryError(const QString &error);
    void queryFinished(int elapsed, const QByteArray &ba);
    void on_btnCancel_clicked();
    void on_btnApply_clicked();

private:
    Ui::NFilterDlg *ui;
    QMap<QString, QJsonArray> mData;
    QMap<QString, QJsonArray> mCols;
    NLoadingDlg *mLoadingDlg;
};

#endif // NFILTERDLG_H
