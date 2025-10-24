#include "registercustomer.h"
#include "ui_registercustomer.h"
#include <QImageCapture>
#include <QVideoWidget>
#include <QCamera>
#include <QVideoSink>

RegisterCustomer::RegisterCustomer(QWidget *parent)
    : QDialog(parent), ui(new Ui::RegisterCustomer)
{
    ui->setupUi(this);
    videoWidget = new QVideoWidget(this);
    videoWidget->setMinimumSize(340, 180);
    camera = new QCamera(this);
    captureSession.setCamera(camera);
    captureSession.setVideoOutput(videoWidget);
    videoSink = new QVideoSink(this);
    connect(videoSink, &QVideoSink::videoFrameChanged,
    this, [this](const QVideoFrame & frame) {
        if(frame.isValid()) {
            QImage img = frame.toImage();

            if(!img.isNull()) {
                currentFrame = img;
                ui->lbPhoto->setPixmap(
                    QPixmap::fromImage(img).scaled(ui->lbPhoto->size(),
                                                   Qt::KeepAspectRatio,
                                                   Qt::SmoothTransformation));
            }
        }
    });
}

RegisterCustomer::~RegisterCustomer() { delete ui; }

void RegisterCustomer::on_btnCapture_clicked()
{
    //auto *b = static_cast<QPushButton*>(sender());
    // if(b->property("c").toBool()) {
    //     b->setProperty("c", false);
    //     camera->stop();
    //     if(!currentFrame.isNull()) {
    //         QString fileName = "snapshot.jpg";
    //         QVideoFrame frame = captureSession.videoFrame();
    //         if(frame.isValid()) {
    //             currentFrame = frame.toImage();
    //             currentFrame.save("snapshot.jpg");
    //         }
    //         currentFrame.save(fileName);
    //         ui->lbPhoto->setPixmap(
    //             QPixmap::fromImage(currentFrame).scaled(ui->lbPhoto->size(),
    //                     Qt::KeepAspectRatio,
    //                     Qt::SmoothTransformation));
    //     }
    // } else {
    //     b->setProperty("c", true);
    //     camera->start();
    // }
}
