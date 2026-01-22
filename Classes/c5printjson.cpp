#include "c5printjson.h"
#include <QJsonObject>
#include <QDebug>

C5PrintJson::C5PrintJson()
{
}

C5PrintJson::~C5PrintJson()
{
    qDebug() << "C5PRINTJSON DESCTRUCTOR";
}

void C5PrintJson::parse(C5Printing &p, const QJsonArray &cmd)
{
    for(int i = 0; i < cmd.size(); i++) {
        QJsonObject jo = cmd.at(i).toObject();

        if(jo["cmd"].toString() == "line") {
            p.line(jo["x1"].toDouble(), jo["y1"].toDouble(), jo["x2"].toDouble(), jo["y2"].toDouble());
        } else if(jo["cmd"].toString() == "simpleline") {
            p.line();
        } else if(jo["cmd"].toString() == "ltext") {
            p.ltext(jo["text"].toString(), jo["x"].toDouble(), jo["textwidth"].toInt());
        } else if(jo["cmd"].toString() == "ctext") {
            p.ctext(jo["text"].toString());
        } else if(jo["cmd"].toString() == "rtext") {
            p.rtext(jo["text"].toString());
        } else if(jo.value("cmd").toString() == "lrtext") {
            p.lrtext(jo.value("left").toString(), jo.value("right").toString());
        } else if(jo["cmd"].toString() == "fontsize") {
            p.setFontSize(jo["size"].toInt());
        } else if(jo["cmd"].toString() == "fontbold") {
            p.setFontBold(jo["bold"].toBool());
        } else if(jo["cmd"].toString() == "br") {
            p.br(jo["height"].toDouble());
        } else if(jo["cmd"].toString() == "fontstrikeout") {
            p.setFontStrike(jo["strike"].toBool());
        } else if(jo["cmd"].toString() == "scene") {
            p.setSceneParams(jo["width"].toDouble(), jo["height"].toDouble(), jo["mm"].toDouble());
        } else if(jo["cmd"].toString() == "font") {
            QFont font(jo["family"].toString(), jo["size"].toInt());
            font.setBold(jo["bold"].toBool());
            p.setFont(font);
        } else if(jo["cmd"].toString() == "print") {
            //   p.print(jo["printer"].toString(), QPageSize(static_cast<QPageSize::PageSizeId>(jo["pagesize"].toInt())));
        } else if(jo["cmd"].toString() == "line1") {
            p.line(jo["x1"].toDouble(), jo["y1"].toDouble(), jo["x2"].toDouble(), jo["y2"].toDouble(), jo["width"].toInt());
        } else if(jo["cmd"].toString() == "line2") {
            p.line(jo["width"].toInt());
        }
    }
}
