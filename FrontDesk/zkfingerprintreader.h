#pragma once

#include <QByteArray>
#include <QImage>
#include <QJsonArray>
#include <QObject>
#include <QThread>
#include <atomic>

class QThread;

class ZkfingerprintReader : public QObject
{
    Q_OBJECT

public:
    static constexpr int EnrollScans = 3;

    explicit ZkfingerprintReader(QObject *parent = nullptr);
    ~ZkfingerprintReader() override;

    bool isAvailable() const;
    bool openDevice();
    void closeDevice();
    void startEnrollment();
    void startIdentification();
    void cancelCapture();

    int identifyProbe(const QJsonArray &registeredUsers, const QByteArray &probe, int *scoreOut = nullptr);

    static bool matchTemplates(const QByteArray &registered, const QByteArray &probe, int registeredSize = 0, int probeSize = 0, int *scoreOut = nullptr);

    std::atomic<bool> mStopCapture {false};

signals:
    void imageReady(const QImage &image);
    void progress(int step, int total, const QString &message);
    void templateReady(const QByteArray &mergedTemplate, int verifyScore);
    void probeReady(const QByteArray &probeTemplate);
    void error(const QString &message);

private:
    QThread *mCaptureThread = nullptr;
    void *mDevice = nullptr;
    void *mDbCache = nullptr;
    unsigned char *mImageBuffer = nullptr;
    int mImageWidth = 0;
    int mImageHeight = 0;
    bool mDeviceOpen = false;
};
