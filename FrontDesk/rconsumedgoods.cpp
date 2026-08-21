#include "rconsumedgoods.h"

#include "c5message.h"
#include "c5user.h"
#include "ninterface.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDialog>
#include <QDir>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>
#include <QProgressDialog>
#include <QTemporaryFile>
#include <QTextStream>
#include <QToolBar>
#include <QUrl>
#include <memory>

RConsumedGoods::RConsumedGoods(const QString &title, QIcon icon, const QString &editorName)
    : RAbstractEditorReport(title, icon, editorName)
{}

QToolBar *RConsumedGoods::toolBar()
{
    const bool firstBuild = (fToolBar == nullptr);
    QToolBar *tb = RAbstractEditorReport::toolBar();
    if (firstBuild && tb) {
        tb->addAction(QIcon(QStringLiteral(":/goods_store.png")),
                      tr("Recalculate\nstore output"),
                      this,
                      [this] { recalculateStoreOutput(); });
    }
    return tb;
}

bool RConsumedGoods::syncQuery(const QString &route, const QJsonObject &params, QJsonObject &response, QString &error)
{
    QEventLoop loop;
    bool ok = false;
    NInterface::query(route,
                      mUser->mSessionKey,
                      this,
                      params,
                      [&](const QJsonObject &jdoc) {
                          response = jdoc;
                          ok = true;
                          loop.quit();
                      },
                      [&](const QJsonObject &jerr) {
                          error = jerr.value(QStringLiteral("message")).toString();
                          if (error.isEmpty()) {
                              error = jerr.value(QStringLiteral("errorMessage")).toString();
                          }
                          loop.quit();
                          return true;
                      },
                      false,
                      120000,
                      true);
    loop.exec();
    if (!ok && error.isEmpty()) {
        error = tr("Request failed");
    }
    return ok;
}

void RConsumedGoods::recalculateStoreOutput()
{
    if (C5Message::question(tr("Recalculate store output for the selected period?\n"
                               "Existing write-offs will be removed and sales will be posted again."))
        != QDialog::Accepted) {
        return;
    }

    auto webSuppress = std::make_unique<NInterfaceProgressSuppressor>();

    QJsonObject listResponse;
    QString error;
    if (!syncQuery(QStringLiteral("/engine/v2/officen/editors/list-sale-docs"),
                   {{QStringLiteral("editor"), mEditorName},
                    {QStringLiteral("filter"), reportFilterValues()}},
                   listResponse,
                   error)) {
        webSuppress.reset();
        C5Message::error(error);
        return;
    }

    const QJsonArray docs = listResponse.value(QStringLiteral("docs")).toArray();
    if (docs.isEmpty()) {
        webSuppress.reset();
        C5Message::info(tr("No closed sales found for the selected period"));
        return;
    }

    QProgressDialog progress(tr("Recalculating store output…"), tr("Cancel"), 0, docs.size(), this);
    progress.setWindowModality(Qt::ApplicationModal);
    progress.setMinimumDuration(0);
    progress.setValue(0);
    progress.show();
    QCoreApplication::processEvents();

    QStringList errors;
    int okCount = 0;
    for (int i = 0; i < docs.size(); ++i) {
        if (progress.wasCanceled()) {
            break;
        }
        const QJsonObject doc = docs.at(i).toObject();
        const QString id = doc.value(QStringLiteral("f_id")).toString();
        const QString prefix = doc.value(QStringLiteral("f_prefix")).toString();
        const QString label = prefix.isEmpty() ? id : prefix;

        progress.setLabelText(tr("Processing %1 (%2 / %3)").arg(label).arg(i + 1).arg(docs.size()));
        progress.setValue(i);
        QCoreApplication::processEvents();

        if (id.isEmpty()) {
            errors.append(tr("Empty document id"));
            continue;
        }

        QJsonObject oneResponse;
        QString oneError;
        if (!syncQuery(QStringLiteral("/engine/v2/officen/editors/recalc-sale"),
                       {{QStringLiteral("editor"), mEditorName}, {QStringLiteral("id"), id}},
                       oneResponse,
                       oneError)) {
            errors.append(QStringLiteral("%1 — %2").arg(label, oneError.isEmpty() ? tr("Failed") : oneError));
            continue;
        }
        ++okCount;
    }
    progress.setValue(docs.size());
    progress.setLabelText(tr("Done"));
    QCoreApplication::processEvents();
    progress.close();
    webSuppress.reset();

    if (!errors.isEmpty()) {
        QTemporaryFile tmp(QDir::temp().filePath(QStringLiteral("consumed_goods_recalc_XXXXXX.txt")));
        tmp.setAutoRemove(false);
        if (tmp.open()) {
            QTextStream out(&tmp);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            out.setCodec("UTF-8");
#endif
            out << tr("Consumed goods — store output recalculation errors") << QLatin1Char('\n');
            out << tr("Done: %1 / %2").arg(okCount).arg(docs.size()) << QLatin1Char('\n');
            out << QString(60, QLatin1Char('-')) << QLatin1Char('\n');
            for (const QString &line : errors) {
                out << line << QLatin1Char('\n');
            }
            const QString errPath = tmp.fileName();
            tmp.close();
            if (!QProcess::startDetached(QStringLiteral("notepad.exe"), {errPath})) {
                QDesktopServices::openUrl(QUrl::fromLocalFile(errPath));
            }
        } else {
            C5Message::error(tr("Done: %1 / %2\n%3")
                                 .arg(okCount)
                                 .arg(docs.size())
                                 .arg(errors.join(QLatin1Char('\n'))));
        }
    }

    getData();
}
