#ifndef DEVICEVIEWMODEL_H
#define DEVICEVIEWMODEL_H

#include <QAbstractItemModel>

#include <Device.hpp>

class DeviceViewModel : public QAbstractItemModel
{
public:
    explicit DeviceViewModel(QObject *parent = 0)
        : QAbstractItemModel(parent)
    {}
    virtual ~DeviceViewModel() = default;

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex parent(const QModelIndex &child) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void update(const Device::ReqStatuses &data);
private:
    int m_prev_size = 0;
    Device::ReqStatuses m_data{};
    QVector<uint16_t> m_index{};
};

#endif // DEVICEVIEWMODEL_H
