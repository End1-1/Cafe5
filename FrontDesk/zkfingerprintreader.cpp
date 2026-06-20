#include "zkfingerprintreader.h"

#ifdef Q_OS_WIN
#include <Windows.h>
extern "C" {
#include "libzkfp.h"
}
#include <QJsonObject>
#include <QThread>
#include "libzkfperrdef.h"

namespace {

constexpr int kVerifyScanStep = 4;
constexpr int kTotalSteps = 4;

class CaptureThread : public QThread
{
public:
    ZkfingerprintReader *owner = nullptr;
    void *device = nullptr;
    void *dbCache = nullptr;
    unsigned char *imageBuffer = nullptr;
    int imageWidth = 0;
    int imageHeight = 0;
    bool identifyOnly = false;

protected:
    void run() override
    {
        if(identifyOnly) {
            runIdentify();
            return;
        }

        runEnroll();
    }

private:
    void runIdentify()
    {
        unsigned char templateBuffer[MAX_TEMPLATE_SIZE] = {0};

        while(true) {
            if(owner && owner->mStopCapture.load()) {
                break;
            }

            unsigned int templateSize = MAX_TEMPLATE_SIZE;
            const int ret = ZKFPM_AcquireFingerprint(
                device,
                imageBuffer,
                static_cast<unsigned int>(imageWidth * imageHeight),
                templateBuffer,
                &templateSize);

            if(ret != ZKFP_ERR_OK) {
                msleep(100);
                continue;
            }

            QImage image(imageWidth, imageHeight, QImage::Format_Grayscale8);
            for(int y = 0; y < imageHeight; ++y) {
                memcpy(image.scanLine(y), imageBuffer + imageWidth * y, static_cast<size_t>(imageWidth));
            }
            emit owner->imageReady(image);
            emit owner->probeReady(
                QByteArray(reinterpret_cast<const char *>(templateBuffer), static_cast<int>(templateSize)));
            break;
        }
    }

    void runEnroll()
    {
        unsigned char templateBuffer[MAX_TEMPLATE_SIZE] = {0};
        unsigned char preTemplates[ZkfingerprintReader::EnrollScans][MAX_TEMPLATE_SIZE] = {{0}};
        unsigned int preTemplateSizes[ZkfingerprintReader::EnrollScans] = {0};
        unsigned char mergedTemplate[MAX_TEMPLATE_SIZE] = {0};
        unsigned int mergedSize = MAX_TEMPLATE_SIZE;

        int enrollIdx = 0;
        bool verifyPhase = false;

        while(true) {
            if(owner && owner->mStopCapture.load()) {
                break;
            }

            unsigned int templateSize = MAX_TEMPLATE_SIZE;
            const int ret = ZKFPM_AcquireFingerprint(
                device,
                imageBuffer,
                static_cast<unsigned int>(imageWidth * imageHeight),
                templateBuffer,
                &templateSize);

            if(ret != ZKFP_ERR_OK) {
                msleep(100);
                continue;
            }

            QImage image(imageWidth, imageHeight, QImage::Format_Grayscale8);
            for(int y = 0; y < imageHeight; ++y) {
                memcpy(image.scanLine(y), imageBuffer + imageWidth * y, static_cast<size_t>(imageWidth));
            }
            emit owner->imageReady(image);

            if(!verifyPhase) {
                if(enrollIdx > 0) {
                    const int match = ZKFPM_DBMatch(
                        dbCache,
                        preTemplates[enrollIdx - 1],
                        preTemplateSizes[enrollIdx - 1],
                        templateBuffer,
                        templateSize);
                    if(match <= 0) {
                        enrollIdx = 0;
                        emit owner->error(QObject::tr("Use the same finger for all scans"));
                        emit owner->progress(0, kTotalSteps, QObject::tr("Registration restarted"));
                        continue;
                    }
                }

                preTemplateSizes[enrollIdx] = templateSize;
                memcpy(preTemplates[enrollIdx], templateBuffer, templateSize);
                ++enrollIdx;

                emit owner->progress(
                    enrollIdx,
                    kTotalSteps,
                    QObject::tr("Press finger %1 of %2").arg(enrollIdx).arg(ZkfingerprintReader::EnrollScans));

                if(enrollIdx >= ZkfingerprintReader::EnrollScans) {
                    mergedSize = MAX_TEMPLATE_SIZE;
                    const int mergeRet = ZKFPM_DBMerge(
                        dbCache,
                        preTemplates[0],
                        preTemplates[1],
                        preTemplates[2],
                        mergedTemplate,
                        &mergedSize);
                    if(mergeRet != ZKFP_ERR_OK) {
                        enrollIdx = 0;
                        emit owner->error(QObject::tr("Failed to merge fingerprint templates"));
                        continue;
                    }

                    verifyPhase = true;
                    emit owner->progress(
                        kVerifyScanStep,
                        kTotalSteps,
                        QObject::tr("Verification: press the same finger again"));
                }
            } else {
                const int score = ZKFPM_DBMatch(
                    dbCache,
                    mergedTemplate,
                    mergedSize,
                    templateBuffer,
                    templateSize);
                if(score <= 0) {
                    enrollIdx = 0;
                    verifyPhase = false;
                    emit owner->error(QObject::tr("Verification failed, try again"));
                    emit owner->progress(0, kTotalSteps, QObject::tr("Registration restarted"));
                    continue;
                }

                emit owner->templateReady(
                    QByteArray(reinterpret_cast<const char *>(mergedTemplate), static_cast<int>(mergedSize)),
                    score);
                break;
            }

            msleep(100);
        }
    }
};

} // namespace

ZkfingerprintReader::ZkfingerprintReader(QObject *parent)
    : QObject(parent)
{
}

ZkfingerprintReader::~ZkfingerprintReader()
{
    cancelCapture();
    closeDevice();
}

bool ZkfingerprintReader::isAvailable() const
{
    return true;
}

bool ZkfingerprintReader::openDevice()
{
    if(mDeviceOpen) {
        return true;
    }

    if(ZKFPM_Init() != ZKFP_ERR_OK) {
        emit error(tr("Failed to initialize fingerprint reader"));
        return false;
    }

    mDevice = ZKFPM_OpenDevice(0);
    if(!mDevice) {
        ZKFPM_Terminate();
        emit error(tr("Failed to open fingerprint reader"));
        return false;
    }

    mDbCache = ZKFPM_DBInit();
    if(!mDbCache) {
        ZKFPM_CloseDevice(mDevice);
        mDevice = nullptr;
        ZKFPM_Terminate();
        emit error(tr("Failed to initialize fingerprint database"));
        return false;
    }

    unsigned int size = 4;
    mImageWidth = 0;
    mImageHeight = 0;
    ZKFPM_GetParameters(mDevice, 1, reinterpret_cast<unsigned char *>(&mImageWidth), &size);
    size = 4;
    ZKFPM_GetParameters(mDevice, 2, reinterpret_cast<unsigned char *>(&mImageHeight), &size);
    if(mImageWidth <= 0 || mImageHeight <= 0) {
        closeDevice();
        emit error(tr("Invalid fingerprint reader parameters"));
        return false;
    }

    mImageBuffer = new unsigned char[static_cast<size_t>(mImageWidth * mImageHeight)];
    mDeviceOpen = true;
    return true;
}

void ZkfingerprintReader::closeDevice()
{
    cancelCapture();

    delete[] mImageBuffer;
    mImageBuffer = nullptr;

    if(mDbCache) {
        ZKFPM_DBFree(mDbCache);
        mDbCache = nullptr;
    }
    if(mDevice) {
        ZKFPM_CloseDevice(mDevice);
        mDevice = nullptr;
    }
    if(mDeviceOpen) {
        ZKFPM_Terminate();
    }
    mDeviceOpen = false;
}

void ZkfingerprintReader::startEnrollment()
{
    if(!mDeviceOpen) {
        emit error(tr("Fingerprint reader is not connected"));
        return;
    }

    cancelCapture();

    mStopCapture = false;
    auto *thread = new CaptureThread();
    mCaptureThread = thread;
    thread->owner = this;
    thread->device = mDevice;
    thread->dbCache = mDbCache;
    thread->imageBuffer = mImageBuffer;
    thread->imageWidth = mImageWidth;
    thread->imageHeight = mImageHeight;
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    connect(thread, &QThread::finished, this, [this]() {
        mCaptureThread = nullptr;
    });
    thread->start();
    emit progress(0, kTotalSteps, tr("Press finger 1 of %1").arg(EnrollScans));
}

void ZkfingerprintReader::startIdentification()
{
    if(!mDeviceOpen) {
        emit error(tr("Fingerprint reader is not connected"));
        return;
    }

    cancelCapture();

    mStopCapture = false;
    auto *thread = new CaptureThread();
    mCaptureThread = thread;
    thread->owner = this;
    thread->device = mDevice;
    thread->dbCache = mDbCache;
    thread->imageBuffer = mImageBuffer;
    thread->imageWidth = mImageWidth;
    thread->imageHeight = mImageHeight;
    thread->identifyOnly = true;
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    connect(thread, &QThread::finished, this, [this]() {
        mCaptureThread = nullptr;
    });
    thread->start();
    emit progress(0, 1, tr("Place your finger on the scanner"));
}

void ZkfingerprintReader::cancelCapture()
{
    if(!mCaptureThread) {
        return;
    }

    mStopCapture = true;
    mCaptureThread->wait(3000);
    mCaptureThread = nullptr;
    mStopCapture = false;
}

int ZkfingerprintReader::identifyProbe(const QJsonArray &registeredUsers, const QByteArray &probe, int *scoreOut)
{
    if(!mDeviceOpen || !mDbCache || probe.isEmpty()) {
        return 0;
    }

    ZKFPM_DBClear(mDbCache);

    for(const QJsonValue &value : registeredUsers) {
        const QJsonObject item = value.toObject();
        const int userId = item.value(QStringLiteral("user_id")).toInt();
        int templateSize = item.value(QStringLiteral("f_size")).toInt();
        const QByteArray registered = QByteArray::fromBase64(
            item.value(QStringLiteral("template")).toString().toLatin1());
        if(templateSize <= 0 || templateSize > registered.size()) {
            templateSize = registered.size();
        }
        if(userId <= 0 || templateSize <= 0) {
            continue;
        }

        if(ZKFPM_DBAdd(
               mDbCache,
               static_cast<unsigned int>(userId),
               reinterpret_cast<unsigned char *>(const_cast<char *>(registered.constData())),
               static_cast<unsigned int>(templateSize))
           != ZKFP_ERR_OK) {
            continue;
        }
    }

    unsigned int matchedId = 0;
    unsigned int score = 0;
    const int ret = ZKFPM_DBIdentify(
        mDbCache,
        reinterpret_cast<unsigned char *>(const_cast<char *>(probe.constData())),
        static_cast<unsigned int>(probe.size()),
        &matchedId,
        &score);

    if(scoreOut) {
        *scoreOut = static_cast<int>(score);
    }

    if(ret != ZKFP_ERR_OK || matchedId == 0) {
        return 0;
    }

    return static_cast<int>(matchedId);
}

bool ZkfingerprintReader::matchTemplates(const QByteArray &registered, const QByteArray &probe, int registeredSize, int probeSize, int *scoreOut)
{
    if(registered.isEmpty() || probe.isEmpty()) {
        return false;
    }

    if(registeredSize <= 0 || registeredSize > registered.size()) {
        registeredSize = registered.size();
    }
    if(probeSize <= 0 || probeSize > probe.size()) {
        probeSize = probe.size();
    }

    if(ZKFPM_Init() != ZKFP_ERR_OK) {
        return false;
    }

    void *dbCache = ZKFPM_DBInit();
    if(!dbCache) {
        ZKFPM_Terminate();
        return false;
    }

    const int score = ZKFPM_DBMatch(
        dbCache,
        reinterpret_cast<unsigned char *>(const_cast<char *>(registered.constData())),
        static_cast<unsigned int>(registeredSize),
        reinterpret_cast<unsigned char *>(const_cast<char *>(probe.constData())),
        static_cast<unsigned int>(probeSize));

    ZKFPM_DBFree(dbCache);
    ZKFPM_Terminate();

    if(scoreOut) {
        *scoreOut = score;
    }
    return score > 0;
}

#else

ZkfingerprintReader::ZkfingerprintReader(QObject *parent) : QObject(parent) {}
ZkfingerprintReader::~ZkfingerprintReader() {}
bool ZkfingerprintReader::isAvailable() const { return false; }
bool ZkfingerprintReader::openDevice() { emit error(tr("Fingerprint reader is supported on Windows only")); return false; }
void ZkfingerprintReader::closeDevice() {}
void ZkfingerprintReader::startEnrollment() { emit error(tr("Fingerprint reader is supported on Windows only")); }
void ZkfingerprintReader::startIdentification() { emit error(tr("Fingerprint reader is supported on Windows only")); }
void ZkfingerprintReader::cancelCapture() {}
bool ZkfingerprintReader::matchTemplates(const QByteArray &, const QByteArray &, int, int, int *) { return false; }
int ZkfingerprintReader::identifyProbe(const QJsonArray &, const QByteArray &, int *) { return 0; }

#endif
