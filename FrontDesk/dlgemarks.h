#ifndef DLGEMARKS_H
#define DLGEMARKS_H

#include "c5dialog.h"

namespace Ui
{
class DlgEmarks;
}

class DlgEmarks : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgEmarks(C5User *user, const QStringList &emarks);

    ~DlgEmarks();

private slots:
    void checkQrResponse(const QJsonObject &jdoc);

    void on_leCode_returnPressed();

private:
    Ui::DlgEmarks *ui;
};

#endif // DLGEMARKS_H
