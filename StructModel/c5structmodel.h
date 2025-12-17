#pragma once

#include "c5structtraits.h"
#include <QAbstractTableModel>
#include <QVector>
#include <QVariant>

template<typename T>
class C5StructModel : public QAbstractTableModel
{
public:
    explicit C5StructModel(QObject *parent = nullptr)
        : QAbstractTableModel(parent)
    {
    }

    void setData(QVector<T>&& data)
    {
        beginResetModel();
        mData = std::move(data);
        mChecked.fill(false, mData.size());
        endResetModel();
    }

    QVector<T> selectedItems() const
    {
        QVector<T> result;
        result.reserve(mData.size());

        for(int i = 0; i < mData.size(); ++i) {
            if(mChecked.at(i))
                result.append(mData.at(i));
        }

        return result;
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent)
        return mData.size();
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent)
        return StructTraits<T>::columnCount() + 1;
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        if(!index.isValid())
            return {};

        int row = index.row();

        if(row < 0 || row >= mData.size())
            return {};

        if(index.column() == 0 && role == Qt::CheckStateRole)
            return mChecked[index.row()] ? Qt::Checked : Qt::Unchecked;

        if(role == Qt::DisplayRole && index.column() > 0)
            return StructTraits<T>::data(mData[index.row()], index.column() - 1);

        return {};
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role) override
    {
        if(!index.isValid())
            return false;

        if(index.column() == 0 && role == Qt::CheckStateRole) {
            const bool checked = (value.toInt() == Qt::Checked);

            if(!mMultiSelection && checked) {
                bool anyChanged = false;

                for(int i = 0; i < mChecked.size(); ++i) {
                    if(mChecked[i]) {
                        mChecked[i] = false;
                        anyChanged = true;
                    }
                }

                if(anyChanged) {
                    emit dataChanged(
                        this->index(0, 0),
                        this->index(mChecked.size() - 1, 0),
                    { Qt::CheckStateRole }
                    );
                }
            }

            if(mChecked[index.row()] != checked) {
                mChecked[index.row()] = checked;
                emit dataChanged(index, index, { Qt::CheckStateRole });
            }

            return true;
        }

        return false;
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        if(!index.isValid())
            return Qt::NoItemFlags;

        Qt::ItemFlags f = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

        if(index.column() == 0)
            f |= Qt::ItemIsUserCheckable;

        return f;
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        if(role != Qt::DisplayRole)
            return {};

        if(orientation == Qt::Vertical)
            return section + 1;

        if(section == 0)
            return {};

        return StructTraits<T>::header(section - 1);
    }

    void setMultiSelect(bool v)
    {
        mMultiSelection = v;
    }

    bool hasSelectedData()
    {
        return mChecked.contains(true);
    }

    QVector<T> selectedData()
    {
        QVector<T> out;
        out.reserve(mData.size());

        for(int i = 0; i < mData.size(); ++i)
            if(mChecked.at(i))
                out.append(mData.at(i));

        return out;
    }

protected:
    QVector<T> mData;

    QVector<bool> mChecked;

private:
    bool mMultiSelection = false;
};
