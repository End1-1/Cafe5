#include "ce5editor.h"
#include "c5lineeditwithselector.h"
#include "c5checkbox.h"
#include "c5dateedit.h"
#include "c5cache.h"
#include "c5selector.h"
#include <QValidator>

CE5Editor::CE5Editor(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent)
{

}

void CE5Editor::setId(int id)
{
    C5LineEditWithSelector *le = findLineEditWithId();
    if (!le) {
        C5Message::error(tr("Program error. Cannot find field with id property"));
        return;
    }
    le->setEnabled(false);
    if (id == 0) {
        if (le->property("Manual") != QVariant::Invalid) {
            le->setEnabled(true);
        }
        return;
    }
    le->setInteger(id);
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select * from " + table() + " where f_id=:f_id ");
    if (db.nextRow()) {
        for (int i = 0; i < db.columnCount(); i++) {
            QString colName = db.columnName(i);
            bool found = false;
            foreach (C5LineEditWithSelector *le, fLines) {
                if (le->property("Field").toString() == colName) {
                    if (le->cacheId() == 0){
                        le->setValue(db.getString(i));
                    } else {
                        le->setValue(db.getString(i));
                    }
                    if (le->property("SetColor") != QVariant::Invalid) {
                        if (le->property("SetColor").toBool()) {
                            le->setColor(le->getInteger());
                        }
                    }
                    found = true;
                    break;
                }
            }
            if (!found) {
                foreach (C5CheckBox *ch, fChecks) {
                    if (ch->property("Field").toString() == colName) {
                        ch->setChecked(db.getInt(i) == 1);
                        found = true;
                        break;
                    }
                }
            }
            if (!found) {
                foreach (C5DateEdit *de, fDates) {
                    if (de->property("Field").toString() == colName) {
                        de->setDate(db.getDate(i));
                        found = true;
                        break;
                    }
                }
            }
        }
    } else {
        C5Message::error(tr("Invalid code"));
    }
}

bool CE5Editor::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    C5Database db(fDBParams);
    C5LineEditWithSelector *leId = findLineEditWithId();
    if (!leId) {
        C5Message::error(tr("Program error. Cannot find field with id property"));
        return false;
    }
    QMap<QString, QVariant> row;
    foreach (C5LineEditWithSelector *leField, fLines) {
        if (leField->property("FieldRef") != QVariant::Invalid) {
            row[leField->property("FieldRef").toString()] = leField->text();
            continue;
        }
        if (leField->property("Field") == QVariant::Invalid) {
            continue;
        }
        if (leField->property("Primary") != QVariant::Invalid) {
            continue;
        }
        QString fieldName = leField->property("Field").toString();
        QVariant value;
        switch (leField->property("Type").toInt()) {
        case 0:
            value = leField->text();
            break;
        case 1:
            value = leField->getInteger();
            break;
        case 2:
            value = leField->getDouble();
            break;
        }
        db[":" + fieldName] = value;
        row[fieldName] = value;
    }
    foreach (C5CheckBox *chField, fChecks) {
        if (chField->property("Field") == QVariant::Invalid) {
            continue;
        }
        db[":" + chField->property("Field").toString()] = (chField->isChecked() ? 1 : 0);
        row[chField->property("Field").toString()] = (chField->isChecked() ? 1 : 0);
    }
    foreach (C5DateEdit *deField, fDates) {
        if (deField->property("Field") == QVariant::Invalid) {
            continue;
        }
        db[":" + deField->property("Field").toString()] = deField->date();
        row[deField->property("Field").toString()] = deField->date();
    }
    if (leId->getInteger() == 0) {
        db[":f_id"] = 0;
        leId->setInteger(db.insert(table()));
        if (leId->getInteger() < 1) {
            err = db.fLastError;
        }
    } else {
        if (leId->property("Manual") != QVariant::Invalid) {
            if (leId->isEnabled()) {
                C5Database dbcheck(fDBParams);
                dbcheck[":f_id"] = leId->getInteger();
                dbcheck.exec("select * from c_partners where f_id=:f_id");
                if (dbcheck.nextRow()) {
                    err = tr("Duplicate id");
                    db.rollback();
                    return false;
                }
                leId->setEnabled(false);
                db[":f_id"] = leId->getInteger();
                db.insert(table(), false);
            }
        } else {
            if (!db.update(table(), where_id(leId->getInteger()))) {
                err = db.fLastError;
            }
        }
    }
    row[leId->property("Field").toString()] = leId->getInteger();
    data.append(row);
    db.startTransaction();
    int cacheid = C5Cache::idForTable(table());
    if (cacheid == 0) {
        C5Message::error(QString("No cache for %1").arg(table()));
    }
    db[":f_id"] = cacheid;
    db.exec("select f_version from s_cache where f_id=:f_id for update");
    if (db.nextRow()) {
        db[":f_version"] = db.getInt("f_version") + 1;
        db.update("s_cache", where_id(C5Cache::idForTable(table())));
    }
    db.commit();
    C5Cache::resetCache(fDBParams, table());
    C5Selector::resetCache(fDBParams, cacheid);
    return err.isEmpty();
}

bool CE5Editor::checkData(QString &err)
{
    foreach (C5LineEdit *le, fLines) {
        if (le->property("Check") != QVariant::Invalid) {
            QStringList rules = le->property("Check").toString().split(";", QString::SkipEmptyParts);
            foreach (QString rule, rules) {
                if (rule.mid(0, 5) == "empty") {
                    if (le->isEmpty()) {
                        err += QString("%1 %2<br>")
                                .arg(rule.mid(rule.indexOf("=") + 1, rule.length() - rule.indexOf("=")))
                                .arg(tr("cannot be empty"));
                    }
                }
            }
        }
    }
    return err.isEmpty();
}

void CE5Editor::clear()
{
    foreach (C5LineEditWithSelector *le, fLines) {
        if(le->cacheId() == 0) {
            le->clear();
        } else {
            le->setValue("");
            if (le->property("SetColor") != QVariant::Invalid) {
                le->setColor(-1);
                le->setInteger(-1);
            }
        }
    }
}

C5LineEditWithSelector *CE5Editor::findLineEditWithId()
{
    foreach (C5LineEditWithSelector *le, fLines) {
        if (le->property("Primary") != QVariant::Invalid) {
            return le;
        }
    }
    return 0;
}

void CE5Editor::getLineEdit(QObject *parent)
{
    C5LineEditWithSelector *le = 0;
    C5CheckBox *ch = 0;
    C5DateEdit *de = 0;
    QObjectList ol = parent->children();
    foreach (QObject *o, ol) {
        if (o->children().count() > 0) {
            getLineEdit(o);
        }
        le = dynamic_cast<C5LineEditWithSelector*>(o);
        if (le) {
            QVariant dataType = le->property("Type");
            if (dataType != QVariant::Invalid) {
                switch (dataType.toInt()) {
                case 0:
                    break;
                case 1:
                    le->setValidator(new QIntValidator());
                    break;
                case 2:
                    le->setValidator(new QDoubleValidator(0, 99999999999, 3));
                    break;
                }
            }
            fLines << le;
        } else {
            ch = dynamic_cast<C5CheckBox*>(o);
            if (ch) {
                fChecks << ch;
            } else {
                de = dynamic_cast<C5DateEdit*>(o);
                if (de) {
                    fDates << de;
                }
            }
        }
    }
}

void CE5Editor::focusFirst()
{
    foreach (C5LineEditWithSelector *l, fLines) {
        if (l->property("First") != QVariant::Invalid) {
            l->setFocus();
            return;
        }
    }
    foreach (C5CheckBox *c, fChecks) {
        if (c->property("First") != QVariant::Invalid) {
            c->setFocus();
            return;
        }
    }
    foreach (C5DateEdit *d, fDates) {
        if (d->property("First") != QVariant::Invalid) {
            d->setFocus();
            return;
        }
    }
}