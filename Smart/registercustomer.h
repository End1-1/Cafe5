#ifndef REGISTERCUSTOMER_H
#define REGISTERCUSTOMER_H

#include <QDialog>
#include <QMediaCaptureSession>

namespace Ui
{
class RegisterCustomer;
}

class QCamera;
class QMediaCaptureSession;
class QImageCapture;
class QVideoWidget;
class QVideoSink;

class RegisterCustomer : public QDialog
{
    Q_OBJECT
public:
    explicit RegisterCustomer(QWidget *parent = nullptr);
    ~RegisterCustomer();
private slots:
    void on_btnCapture_clicked();

private:
    Ui::RegisterCustomer* ui;
    QCamera* camera;
    QMediaCaptureSession captureSession;
    QVideoWidget* videoWidget;
    QVideoSink* videoSink;

    QImage currentFrame;
};

#endif // REGISTERCUSTOMER_H
