#include "ce5editor.h"
#include "c5lineeditwithselector.h"
#include "c5checkbox.h"
#include "c5dateedit.h"
#include "c5cache.h"
#include "c5combobox.h"
#include "c5selector.h"
#include "c5message.h"
#include "c5database.h"
#include <QValidator>
#include <QPushButton>
#include <QPlainTextEdit>

CE5Editor::CE5Editor(QWidget *parent) :
    C5Widget(parent)
{
    fRememberFields = false;
    fEditor = nullptr;
}

CE5Editor::~CE5Editor()
{
    if(b1()) {
        b1()->deleteLater();
    }
}

void CE5Editor::setId(int id)
{
    C5LineEditWithSelector *le = findLineEditWithId();

    if(!le) {
        C5Message::error(tr("Program error. Cannot find field with id property"));
        return;
    }

    le->setEnabled(false);

    if(id == 0) {
        if(le->property("Manual").isValid()) {
            le->setEnabled(true);
        }

        return;
    }

    le->setInteger(id);
    C5Database db;
    db[":f_id"] = id;
    db.exec("select * from " + table() + " where f_id=:f_id ");

    if(db.nextRow()) {
        for(int i = 0; i < db.columnCount(); i++) {
            QString colName = db.columnName(i);
            bool found = false;

            foreach(C5LineEditWithSelector *le, fLines) {
                if(le->property("Field").toString() == colName) {
                    if(le->property("Primary").isValid()) {
                        le->setInteger(db.getInt(i));
                    } else {
                        switch(le->property("Type").toInt()) {
                        case 0:
                            le->setText(db.getString(i));
                            break;

                        case 1:
                            le->setInteger(db.getInt(i));
                            break;

                        case 2:
                        case 3:
                            le->setDouble(db.getDouble(i));
                            break;

                        default:
                            le->setText(db.getString(i));
                            break;
                        }
                    }

                    if(le->property("SetColor").isValid()) {
                        if(le->isEmpty()) {
                            le->setInteger(-1);
                        }

                        if(le->property("SetColor").toBool()) {
                            le->setColor(le->getInteger());
                        }
                    }

                    found = true;
                    break;
                }
            }

            if(!found) {
                foreach(C5CheckBox *ch, fChecks) {
                    if(ch->property("Field").toString() == colName) {
                        ch->setChecked(db.getInt(i) == 1);
                        found = true;
                        break;
                    }
                }
            }

            if(!found) {
                foreach(C5DateEdit *de, fDates) {
                    if(de->property("Field").toString() == colName) {
                        de->setDate(db.getDate(i));
                        found = true;
                        break;
                    }
                }
            }

            if(!found) {
                foreach(C5ComboBox *cb, fCombos) {
                    if(cb->property("Field").toString() == colName) {
                        if(cb->property("ItemData").toBool()) {
                            cb->setCurrentIndex(cb->findData(db.getInt(i)));
                        } else {
                            cb->setCurrentText(db.getString(i));
                        }

                        found = true;
                        break;
                    }
                }
            }

            if(!found) {
                foreach(QPlainTextEdit *pt, fPlainText) {
                    if(pt->property("Field").toString() == colName) {
                        pt->setPlainText(db.getString(i));
                        found = true;
                        break;
                    }
                }
            }
        }

        for(C5LineEditWithSelector *l : qAsConst(fLines)) {
            l->fixValue();
        }
    } else {
        C5Message::error(tr("Invalid code"));
    }
}

QString CE5Editor::dbError(QString err)
{
    return err;
}

bool CE5Editor::save(QString &err, QList<QMap<QString, QVariant> >& data)
{
    C5Database db;
    C5LineEditWithSelector *leId = findLineEditWithId();

    if(!leId) {
        C5Message::error(tr("Program error. Cannot find field with id property"));
        return false;
    }

    QMap<QString, QVariant> row;

    foreach(C5LineEditWithSelector *leField, fLines) {
        if(leField->property("FieldRef").isValid()) {
            row[leField->property("FieldRef").toString()] = leField->text();
            continue;
        }

        if(!leField->property("Field").isValid()) {
            continue;
        }

        if(leField->property("Primary").isValid()) {
            continue;
        }

        if(leField->property("SetColor").isValid()) {
            if(leField->isEmpty()) {
                leField->setInteger(-1);
            }

            if(leField->property("SetColor").toBool()) {
                leField->setColor(leField->getInteger());
            }
        }

        QString fieldName = leField->property("Field").toString();
        QVariant value;

        switch(leField->property("Type").toInt()) {
        case 0:
            value = leField->text();

            if(value.toString().isEmpty()) {
                value = QVariant();
            }

            break;

        case 1:
            value = leField->getInteger();
            break;

        case 2:
        case 3:
        case 4:
            value = leField->getDouble();
            break;
        }

        db[":" + fieldName] = value;
        row[fieldName] = value;
    }

    foreach(C5CheckBox *chField, fChecks) {
        if(!chField->property("Field").isValid()) {
            continue;
        }

        db[":" + chField->property("Field").toString()] = (chField->isChecked() ? 1 : 0);
        row[chField->property("Field").toString()] = (chField->isChecked() ? 1 : 0);
    }

    foreach(C5DateEdit *deField, fDates) {
        if(!deField->property("Field").isValid()) {
            continue;
        }

        db[":" + deField->property("Field").toString()] = deField->date();
        row[deField->property("Field").toString()] = deField->date();
    }

    foreach(C5ComboBox *cb, fCombos) {
        if(!cb->property("Field").isValid()) {
            continue;
        }

        if(cb->currentData().toInt() == 0) {
            db[":" + cb->property("Field").toString()] = cb->currentText();
            row[cb->property("Field").toString()] = cb->currentText();
        } else {
            db[":" + cb->property("Field").toString()] = cb->currentData();
            row[cb->property("Field").toString()] = cb->currentData();
        }
    }

    foreach(QPlainTextEdit *pt, fPlainText) {
        db[":" + pt->property("Field").toString()] = pt->toPlainText();
        row[pt->property("Field").toString()] = pt->toPlainText();
    }

    if(leId->getInteger() == 0) {
        db[":f_id"] = 0;
        leId->setInteger(db.insert(table()));

        if(leId->getInteger() < 1) {
            err = dbError(db.fLastError);
        }
    } else {
        if(leId->property("Manual").isValid()) {
            if(leId->isEnabled()) {
                C5Database dbcheck;
                dbcheck[":f_id"] = leId->getInteger();
                dbcheck.exec("select * from " + table() + " where f_id=:f_id");

                if(dbcheck.nextRow()) {
                    err = tr("Duplicate id");
                    return false;
                }

                leId->setEnabled(false);
                db[":f_id"] = leId->getInteger();

                if(!db.insert(table(), false)) {
                    err = dbError(db.fLastError);
                }
            } else {
                if(!db.update(table(), where_id(leId->getInteger()))) {
                    err = dbError(db.fLastError);
                }
            }
        } else {
            if(!db.update(table(), where_id(leId->getInteger()))) {
                err = db.fLastError;
            }
        }
    }

    row[leId->property("Field").toString()] = leId->getInteger();
    data.append(row);
    C5Cache *cache = C5Cache::cache(0);
    int cacheid = cache->idForTable(table());

    if(cacheid == 0) {
        C5Message::error(QString("No cache for %1").arg(table()));
    }

    db[":f_id"] = cacheid;
    db.exec("select f_version from s_cache where f_id=:f_id for update");

    if(db.nextRow()) {
        db[":f_version"] = db.getInt("f_version") + 1;
        db.update("s_cache", where_id(C5Cache::idForTable(table())));
    }

    C5Cache::resetCache(table());
    C5Selector::resetCache(cacheid);
    return err.isEmpty();
}

bool CE5Editor::checkData(QString &err)
{
    foreach(C5LineEdit *le, fLines) {
        if(le->property("Check").isValid()) {
            QStringList rules = le->property("Check").toString().split(";", Qt::SkipEmptyParts);

            foreach(QString rule, rules) {
                if(rule.mid(0, 5) == "empty") {
                    le->setText(le->text().trimmed());

                    if(le->isEmpty()) {
                        err += QString("%1 %2<br>")
                               .arg(rule.mid(rule.indexOf("=") + 1, rule.length() - rule.indexOf("=")))
                               .arg(tr("cannot be empty"));
                    }
                } else if(rule.mid(0, 7) == "nonzero") {
                    if(le->getDouble() < 0.0001) {
                        err += QString("%1 %2<br>")
                               .arg(rule.mid(rule.indexOf("=") + 1, rule.length() - rule.indexOf("=")))
                               .arg("cannot be zero");
                    }
                }
            }
        }
    }

    foreach(C5ComboBox *cb, fCombos) {
        if(cb->property("Check").isValid()) {
            QStringList rules = cb->property("Check").toString().split(";", Qt::SkipEmptyParts);

            foreach(QString rule, rules) {
                if(rule.mid(0, 5) == "empty") {
                    if(cb->currentData().toInt() == 0) {
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

bool CE5Editor::isOnline()
{
    return false;
}

void CE5Editor::clear()
{
    setProperty("saveandnew", false);

    foreach(C5LineEditWithSelector *le, fLines) {
        if(le->cacheId() == 0) {
            le->clear();
        } else {
            le->setValue("");

            if(le->property("SetColor").isValid()) {
                le->setColor(-1);
                le->setInteger(-1);
            }
        }

        if(le->property("Default").isValid()) {
            le->setText(le->property("Default").toString());
        }
    }

    foreach(C5ComboBox *cb, fCombos) {
        cb->setCurrentIndex(-1);

        if(cb->property("default").toInt() > 0) {
            cb->setCurrentIndex(cb->findData(cb->property("default")));
        }
    }

    foreach(QPlainTextEdit *pt, fPlainText) {
        pt->clear();
    }

    foreach(C5CheckBox *c, fChecks) {
        c->setChecked(false);

        if(c->property("Default").isValid()) {
            c->setChecked(c->property("Default").toInt() > 0);
        }
    }

    focusFirst();
}

bool CE5Editor::canCopy()
{
    return false;
}

void CE5Editor::copyObject()
{
}

QPushButton* CE5Editor::b1()
{
    return nullptr;
}

C5LineEditWithSelector* CE5Editor::findLineEditWithId()
{
    foreach(C5LineEditWithSelector *le, fLines) {
        if(le->property("Primary").isValid()) {
            return le;
        }
    }

    return nullptr;
}

void CE5Editor::getLineEdit(QObject *parent)
{
    C5LineEditWithSelector *le = nullptr;
    C5CheckBox *ch = nullptr;
    C5DateEdit *de = nullptr;
    C5ComboBox *cb = nullptr;
    QPlainTextEdit *pt = nullptr;
    QObjectList ol = parent->children();

    foreach(QObject *o, ol) {
        if(o->property("nouse").toBool()) {
            continue;
        }

        if(o->children().count() > 0) {
            getLineEdit(o);
        }

        le = dynamic_cast<C5LineEditWithSelector*>(o);

        if(le) {
            QVariant dataType = le->property("Type");
            int decimalPlaces = 3;

            if(le->property("DecimalPlaces").isValid()) {
                decimalPlaces = le->property("DecimalPlaces").toInt();
            }

            if(dataType.isValid()) {
                switch(dataType.toInt()) {
                case 0:
                    break;

                case 1:
                    le->setValidator(new QIntValidator());
                    break;

                case 2:
                    le->setValidator(new QDoubleValidator(0, 99999999999, decimalPlaces));
                    break;

                case 3:
                    le->setValidator(new QDoubleValidator(-99999999, 999999999, decimalPlaces));
                    break;

                case 4:
                    le->setValidator(new QDoubleValidator(-1, 999999999, decimalPlaces));
                    break;
                }
            }

            if(le->property("Default").isValid()) {
                QVariant v = le->property("Default");
                le->setText(v.toString());
            }

            fLines << le;
        } else if((ch = dynamic_cast<C5CheckBox* >(o))) {
            fChecks << ch;
        } else if((de = dynamic_cast<C5DateEdit* >(o))) {
            fDates << de;
        } else if((cb = dynamic_cast<C5ComboBox* >(o))) {
            fCombos << cb;
        } else if((pt = dynamic_cast<QPlainTextEdit* >(o))) {
            fPlainText << pt;
        }
    }
}

void CE5Editor::focusFirst()
{
    foreach(C5LineEditWithSelector *l, fLines) {
        if(l->property("First").isValid()) {
            if(l->property("First").toBool()) {
                l->setFocus();
            }

            return;
        }
    }

    foreach(C5CheckBox *c, fChecks) {
        if(c->property("First").isValid()) {
            if(c->property("First").toBool()) {
                c->setFocus();
            }

            return;
        }
    }

    foreach(C5DateEdit *d, fDates) {
        if(d->property("First").isValid()) {
            if(d->property("First").toBool()) {
                d->setFocus();
            }

            return;
        }
    }

    foreach(C5ComboBox *c, fCombos) {
        if(c->property("First").isValid()) {
            if(c->property("First").toBool()) {
                c->setFocus();
            }

            return;
        }
    }
}

QJsonObject CE5Editor::makeJsonObject()
{
    return fJsonData;
}

bool CE5Editor::acceptOnSave() const
{
    return false;
}
