#include "axreporting.h"
#include "c5database.h"
#include "c5utils.h"
#include "c5message.h"
#include "c5config.h"
#include "c5user.h"
#include <windows.h>
#include <QFile>
#include <QDir>
#include <QAxWidget>
#include <QAxObject>
#include <QApplication>

AXReporting::AXReporting(QObject *parent)
    : QObject{parent}
{
}

void AXReporting::printReservation(const QJsonObject &jo)
{
    QString filename = QDir::tempPath() + "/" + C5Database::uuid() + "_waiter.docx";
    QString srcFile = qApp->applicationDirPath() + "/templates/reservation_voucher_en.docx";
    if (!QFile::copy(srcFile,  filename)) {
        C5Message::error("Cannot copy template file.");
        return;
    }
    auto wApp = new QAxObject("Word.Application");
    auto docs = wApp->querySubObject("Documents");
    auto doc  = docs->querySubObject("Open(QString)", filename);
    if (nullptr == doc) {
        qFatal("File not found");
    }
    auto active  = wApp->querySubObject("ActiveDocument()");
    auto content = active->querySubObject("Content()");
    auto find   = content->querySubObject("Find");
    QJsonObject jheader = jo["header"].toObject();
    QJsonObject jpreorder = jo["preorder"].toObject();
    QJsonObject jhotel = jo["hoteldata"].toObject();
    QJsonObject jconfig = jo["config"].toObject();
    replaceString(find, "#voucher_number", jheader["f_prefix"].toString() + QString::number(jheader["f_hallid"].toInt()));
    replaceString(find, "#date", QDate::currentDate().toString(FORMAT_DATE_TO_STR));
    replaceString(find, "#guest", jpreorder["f_guestname"].toString());
    replaceString(find, "#arrival_date", QDate::fromString(jhotel["f_checkin"].toString(), FORMAT_DATE_TO_STR_MYSQL)
                  .toString(FORMAT_DATE_TO_STR));
    replaceString(find, "#arrival_time", jpreorder["f_timefor"].toString());
    replaceString(find, "#departure_date", QDate::fromString(jhotel["f_checkout"].toString(), FORMAT_DATE_TO_STR_MYSQL)
                  .toString(FORMAT_DATE_TO_STR));
    replaceString(find, "#departure_time", "12:00");
    replaceString(find, "#number_of_nights", QString::number(jhotel["f_nights"].toInt()));
    replaceString(find, "#category", jheader["f_hallname"].toString());
    replaceString(find, "#room", jheader["f_tablename"].toString());
    replaceString(find, "#price_per_night", float_str(jhotel["f_roomrate"].toDouble(), 2));
    replaceString(find, "#meal_plan", jhotel["f_mealplanname"].toString());
    replaceString(find, "#operator", __user->fullName());
    replaceString(find, "#printed", QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    active->dynamicCall("Activate()");
    wApp->setProperty("Visible", true);
    wApp->dynamicCall("Activate()");
    doc->dynamicCall("Save()");
    QAxObject *story = doc->querySubObject("StoryRanges");
    QAxObject  *range = story->querySubObject("Item( wdPrimaryFooterStory )" );
    QAxObject  *findString =  range->querySubObject("Find");
    replaceString(findString, "#companydata", jconfig["companydata"].toString());
    delete findString;
    delete range;
    delete story;
    delete find;
    delete content;
    delete active;
    delete doc;
    delete docs;
    delete wApp;
    for (HWND hwnd = GetTopWindow(NULL); hwnd != NULL; hwnd = GetNextWindow(hwnd, GW_HWNDNEXT)) {
        if (!IsWindowVisible(hwnd))
            continue;
        int length = GetWindowTextLength(hwnd);
        if (length == 0)
            continue;
        wchar_t *title = new wchar_t[length + 1];
        GetWindowText(hwnd, title, length + 1);
        if (QString::fromWCharArray(title).contains(filename)) {
            BringWindowToTop(hwnd);
            break;
        }
    }
}

void AXReporting::replaceString(QAxObject *find, const QString &oldStr, const QString &newStr)
{
    bool bMatchCase      = false;
    bool bMatchWholeWord = false;
    bool bMatchWildCards = false;
    bool bReplaceAll     = true;
    QVariantList vl = { oldStr, bMatchCase, bMatchWholeWord, bMatchWildCards,
                        false, false, true, 1, false, newStr, bReplaceAll ? "2" : "1"
                      };
    find->dynamicCall("Execute(QString,bool,bool,bool,bool,bool,bool,int,bool,QString,int)", vl);
}
