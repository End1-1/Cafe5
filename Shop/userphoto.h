#ifndef USERPHOTO_H
#define USERPHOTO_H

#include <QWidget>

namespace Ui {
class UserPhoto;
}

class UserPhoto : public QWidget
{
    Q_OBJECT

public:
    explicit UserPhoto(QWidget *parent = nullptr);

    ~UserPhoto();

    void setImage(const QPixmap &p, const QString &name, const QString &scancode);

private:
    Ui::UserPhoto *ui;
};

#endif // USERPHOTO_H
