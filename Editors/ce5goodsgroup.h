#pragma once

#include "ce5editor.h"
#include <QMap>

namespace Ui
{
class CE5GoodsGroup;
}

class CE5GoodsGroup : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5GoodsGroup(QWidget *parent = nullptr);

    ~CE5GoodsGroup();

    virtual QString title() { return tr("Goods group"); }

    virtual QString table() { return "c_groups"; }

    virtual void setId(int id) override;

    virtual bool save(QString &err, QList<QMap<QString, QVariant>> &data) override;

    virtual void clear() override;

    virtual bool isOnline() override;

    virtual bool checkData(QString &err) override;

private slots:
    void openResponse(const QJsonObject &jdoc);

    void saveResponse(const QJsonObject &jdoc);

    void setColor();

    void uploadImage();

    void removeImage();

    void on_lbImg_customContextMenuRequested(const QPoint &pos);

    void onLangButtonClicked();

private:
    void applyGroup(const QJsonObject &group, const QString &imageBase64);

    void applyTranslations(const QJsonObject &translations);

    void stashCurrentLangFields();

    void showLangFields(const QString &lang);

    QJsonObject makeSaveJson() const;

    Ui::CE5GoodsGroup *ui;

    QByteArray fPendingImage;

    bool fImageChanged = false;

    bool fRemoveImage = false;

    QString mEditLang = QStringLiteral("hy");

    /** lang -> name (hy is canonical shown in lineEdit_2 when active) */
    QMap<QString, QString> mNamesByLang;
};
