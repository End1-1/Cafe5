#pragma once

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QPrinter>
#include <QPushButton>
#include <QTextDocument>
#include <QVBoxLayout>

class PrintPreviewDialog : public QDialog
{
    Q_OBJECT
public:
    PrintPreviewDialog(QTextDocument *doc, QPrinter *printer, QWidget *parent = nullptr);

private:
    QTextDocument *m_doc;
    QGraphicsView *m_view;
    QGraphicsScene *m_scene;
    QPrinter *m_printer;

    void renderPages();

private slots:
    void slotPrint();
};
