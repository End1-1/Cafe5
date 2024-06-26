#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>

class ImageLoader : public QObject
{
    Q_OBJECT
public:
    explicit ImageLoader(int id, QObject *parent = nullptr);

    void start();

signals:
    void imageLoaded(const QPixmap&);

    void noImage();

    void finished();

private:
    int fId;

private slots:
    void loadImage();
};

#endif // IMAGELOADER_H
