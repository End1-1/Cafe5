#ifndef RTOTALREVIEWREPORT_H
#define RTOTALREVIEWREPORT_H

#include "rabstracteditorreport.h"

class RTotalReviewReport : public RAbstractEditorReport
{
    Q_OBJECT
public:
    RTotalReviewReport(const QString &title, QIcon icon, const QString &editorName);

protected slots:
    virtual void on_tbl_doubleClicked(const QModelIndex &index) override;
};

#endif // RTOTALREVIEWREPORT_H
