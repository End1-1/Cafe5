#ifndef C5DOCUMENT_H
#define C5DOCUMENT_H

#include "c5officewidget.h"

class C5Document : public C5OfficeWidget
{
    Q_OBJECT
public:
    explicit C5Document(QWidget *parent = nullptr);

    int genNumber(int docType) const;

    QString genNumberText(int docType) const;

    void updateGenNumber(int i, int docType);
};

#endif // C5DOCUMENT_H
