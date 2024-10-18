#include "ce5editor.h"
#include "c5lineeditwithselector.h"
#include "c5checkbox.h"
#include "c5dateedit.h"
#include "c5cache.h"
#include "c5combobox.h"
#include "c5selector.h"
#include <QValidator>
#include <QPushButton>
#include <QPlainTextEdit>

CE5Editor::CE5Editor(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent)
{
    fRememberFields = false;
    fEditor = nullptr;
}

CE5Editor::~CE5Editor()
{
    if (b1()) {
        b1()->deleteLater();
    }
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
                    if (le->cacheId() == 0) {
                        switch (le->property("Type").toInt()) {
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
                    } else {
                        le->setValue(db.getString(i));
                    }
                    if (le->property("SetColor") != QVariant::Invalid) {
                        if (le->isEmpty()) {
                            le->setInteger(-1);
                        }
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
            if (!found) {
                foreach (C5ComboBox *cb, fCombos) {
                    if (cb->property("Field").toString() == colName) {
                        if (cb->property("ItemData").toBool()) {
                            cb->setCurrentIndex(cb->findData(db.getString(i)));
                        } else {
                            cb->setCurrentText(db.getString(i));
                        }
                        found = true;
                        break;
                    }
                }
            }
            if (!found) {
                foreach (QPlainTextEdit *pt, fPlainText) {
                    if (pt->property("Field").toString() == colName) {
                        pt->setPlainText(db.getString(i));
                        found = true;
                        break;
                    }
                }
            }
        }
        for (C5LineEditWithSelector *l : qAsConst(fLines)) {
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
        if (leField->property("SetColor") != QVariant::Invalid) {
            if (leField->isEmpty()) {
                leField->setInteger(-1);
            }
            if (leField->property("SetColor").toBool()) {
                leField->setColor(leField->getInteger());
            }
        }
        QString fieldName = leField->property("Field").toString();
        QVariant value;
        switch (leField->property("Type").toInt()) {
            case 0:
                value = leField->text();
                if (value.toString().isEmpty()) {
                    value = QVariant(QVariant::String);
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
    foreach (C5ComboBox *cb, fCombos) {
        if (cb->property("Field") == QVariant::Invalid) {
            continue;
        }
        if (cb->currentData().toInt() == 0) {
            db[":" + cb->property("Field").toString()] = cb->currentText();
            row[cb->property("Field").toString()] = cb->currentText();
        } else {
            db[":" + cb->property("Field").toString()] = cb->currentData();
            row[cb->property("Field").toString()] = cb->currentData();
        }
    }
    foreach (QPlainTextEdit *pt, fPlainText) {
        db[":" + pt->property("Field").toString()] = pt->toPlainText();
        row[pt->property("Field").toString()] = pt->toPlainText();
    }
    if (leId->getInteger() == 0) {
        db[":f_id"] = 0;
        leId->setInteger(db.insert(table()));
        if (leId->getInteger() < 1) {
            err = dbError(db.fLastError);
        }
    } else {
        if (leId->property("Manual") != QVariant::Invalid) {
            if (leId->isEnabled()) {
                C5Database dbcheck(fDBParams);
                dbcheck[":f_id"] = leId->getInteger();
                dbcheck.exec("select * from " + table() + " where f_id=:f_id");
                if (dbcheck.nextRow()) {
                    err = tr("Duplicate id");
                    db.rollback();
                    return false;
                }
                leId->setEnabled(false);
                db[":f_id"] = leId->getInteger();
                if (!db.insert(table(), false)) {
                    err = dbError(db.fLastError);
                }
            } else {
                if (!db.update(table(), where_id(leId->getInteger()))) {
                    err = dbError(db.fLastError);
                }
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
    C5Cache *cache = C5Cache::cache(fDBParams, 0);
    int cacheid = cache->idForTable(table());
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
                    le->setText(le->text().trimmed());
                    if (le->isEmpty()) {
                        err += QString("%1 %2<br>")
                               .arg(rule.mid(rule.indexOf("=") + 1, rule.length() - rule.indexOf("=")))
                               .arg(tr("cannot be empty"));
                    }
                } else if (rule.mid(0, 7) == "nonzero") {
                    if (le->getDouble() < 0.0001) {
                        err += QString("%1 %2<br>")
                               .arg(rule.mid(rule.indexOf("=") + 1, rule.length() - rule.indexOf("=")))
                               .arg("cannot be zero");
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
        if (le->property("Default").isValid()) {
            le->setText(le->property("Default").toString());
        }
    }
    foreach (C5ComboBox *cb, fCombos) {
        cb->setCurrentIndex(-1);
    }
    foreach (QPlainTextEdit *pt, fPlainText) {
        pt->clear();
    }
    foreach (C5CheckBox *c, fChecks) {
        c->setChecked(false);
        if (c->property("Default").isValid()) {
            c->setChecked(c->property("Default").toInt() > 0);
        }
    }
    focusFirst();
}

QPushButton *CE5Editor::b1()
{
    return nullptr;
}

C5LineEditWithSelector *CE5Editor::findLineEditWithId()
{
    foreach (C5LineEditWithSelector *le, fLines) {
        if (le->property("Primary") != QVariant::Invalid) {
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
    foreach (QObject *o, ol) {
        if (o->property("nouse").toBool()) {
            continue;
        }
        if (o->children().count() > 0) {
            getLineEdit(o);
        }
        le = dynamic_cast<C5LineEditWithSelector *>(o);
        if (le) {
            QVariant dataType = le->property("Type");
            int decimalPlaces = 3;
            if (le->property("DecimalPlaces").isValid()) {
                decimalPlaces = le->property("DecimalPlaces").toInt();
            }
            if (dataType != QVariant::Invalid) {
                switch (dataType.toInt()) {
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
            if (le->property("Default").isValid()) {
                QVariant v = le->property("Default");
                le->setText(v.toString());
            }
            fLines << le;
        } else if (ch = dynamic_cast<C5CheckBox * >(o)) {
            fChecks << ch;
        } else if (de = dynamic_cast<C5DateEdit * >(o)) {
            fDates << de;
        } else if (cb = dynamic_cast<C5ComboBox * >(o)) {
            fCombos << cb;
        } else if (pt = dynamic_cast<QPlainTextEdit * >(o)) {
            fPlainText << pt;
        }
    }
}

void CE5Editor::focusFirst()
{
    foreach (C5LineEditWithSelector *l, fLines) {
        if (l->property("First") != QVariant::Invalid) {
            if (l->property("First").toBool()) {
                l->setFocus();
            }
            return;
        }
    }
    foreach (C5CheckBox *c, fChecks) {
        if (c->property("First") != QVariant::Invalid) {
            if (c->property("First").toBool()) {
                c->setFocus();
            }
            return;
        }
    }
    foreach (C5DateEdit *d, fDates) {
        if (d->property("First") != QVariant::Invalid) {
            if (d->property("First").toBool()) {
                d->setFocus();
            }
            return;
        }
    }
    foreach (C5ComboBox *c, fCombos) {
        if (c->property("First") != QVariant::Invalid) {
            if (c->property("First").toBool()) {
                c->setFocus();
            }
            return;
        }
    }
}

void CE5Editor::setDatabase(const QStringList &dbParams)
{
    fDBParams = dbParams;
}

QJsonObject CE5Editor::makeJsonObject()
{
    return fJsonData;
}

bool CE5Editor::acceptOnSave() const
{
    return false;
}
