#pragma once

#include "ntreewidget.h"

class RMenuReviewReport : public NTreeWidget
{
    Q_OBJECT

public:
    explicit RMenuReviewReport(C5User *user, const QString &title, QIcon icon, QWidget *parent = nullptr);

protected slots:
    void queryFinished(const QJsonObject &ba) override;
};
