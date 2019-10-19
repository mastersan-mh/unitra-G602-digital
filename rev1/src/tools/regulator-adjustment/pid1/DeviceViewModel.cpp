#include "DeviceViewModel.hpp"

#include <QIcon>

DeviceViewModel::DeviceViewModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_prev_size(0)
    , m_data()
    , m_index()
{

}

DeviceViewModel::~DeviceViewModel()
{

}

QModelIndex DeviceViewModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return createIndex(row, column);
}

QModelIndex DeviceViewModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

int DeviceViewModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.size();
}

int DeviceViewModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant DeviceViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if (index.row() >= m_data.size())
    {
        return QVariant();
    }

    switch(role)
    {
        case Qt::DisplayRole:
        {
            static const QVector<QString> strs =
            {
                [Device::FuncId::FUNC_00_PULSES_R     ] = "pulses",
                [Device::FuncId::FUNC_01_MODE_R       ] = "mode_r",
                [Device::FuncId::FUNC_02_KOEF_R       ] = "koef_r",
                [Device::FuncId::FUNC_03_KOEF_W       ] = "koef_w",
                [Device::FuncId::FUNC_04_SPEED_SP_R   ] = "speed_SP_r",
                [Device::FuncId::FUNC_05_SPEED_SP_W   ] = "speed_SP_w",
                [Device::FuncId::FUNC_06_SPEED_PV_R   ] = "speed_PV_r",
                [Device::FuncId::FUNC_07_PROCESS_START] = "proc start",
                [Device::FuncId::FUNC_08_PROCESS_STOP ] = "proc stop",
                [Device::FuncId::FUNC_09_CONF_STORE   ] = "Conf store",
            };

            static const QVector<QString> reqModeStr =
            {
                [Device::ReqMode::AUTO  ] = "A",
                [Device::ReqMode::MANUAL] = "M",
            };

            int row = (m_index.size() - 1) - index.row();
            uint16_t ruid = m_index.at(row);
            const struct Device::req_status &stat = m_data[ruid];

            return QString("[%2] %3 %4")
                    .arg(ruid, 4, 16, QLatin1Char('0'))
                    .arg(strs[stat.funcId])
                    .arg(reqModeStr[static_cast<int>(stat.reqmode)]);

        }
        case Qt::DecorationRole:
        {
            int row = (m_index.size() - 1) - index.row();
            uint16_t ruid = m_index.at(row);
            const struct Device::req_status &stat = m_data[ruid];

            QString name;
            switch(stat.res)
            {
                case Device::ReqResult::AWAITING: name = ":/images/uploading.png"; break;
                case Device::ReqResult::SUCCESS : name = ":/images/success.png"; break;
                case Device::ReqResult::TIMEOUT : name = ":/images/timeout.png"; break;
                case Device::ReqResult::ERROR   : name = ":/images/error.png"; break;
            }
            return QIcon(name);
        }
        default: ;
    }

    return QVariant();
}

void DeviceViewModel::update(const Device::ReqStatuses & data)
{
    m_data = data;
    int size = m_data.size();
    m_index.clear();
    m_index.reserve(size);

    for(Device::ReqStatuses::const_iterator it = m_data.cbegin(); it != m_data.cend(); ++it)
    {
        m_index.append(it.key());
    }

    int affect_size = (size > m_prev_size ? size : m_prev_size);

    if(affect_size > 0)
    {
        static const QVector<int> icon_roles = { Qt::DecorationRole };
        static const QVector<int> text_roles = { Qt::DisplayRole };

        emit dataChanged(
                    createIndex(0, 0),
                    createIndex(affect_size - 1, 0),
                    icon_roles
                    );

        emit dataChanged(
                    createIndex(0, 1),
                    createIndex(affect_size - 1, 1),
                    text_roles
                    );
    }

    m_prev_size = size;

}
