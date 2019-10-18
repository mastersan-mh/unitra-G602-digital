
#include "fan.h"

namespace fan
{

Extreme::Extreme()
    : m_valid(false)
    , m_value()
{
}

Extreme::~Extreme()
{
}

void Extreme::reset()
{
    m_valid = false;
}

bool Extreme::setMin(double value)
{
    bool extreme = (m_valid ? (value < m_value) : true);
    if(extreme)
    {
        m_value = value;
    }
    m_valid = true;
    return extreme;
}

bool Extreme::setMax(double value)
{
    bool extreme = (m_valid ? (value > m_value) : true);
    if(extreme)
    {
        m_value = value;
    }
    m_valid = true;
    return extreme;
}

bool Extreme::get(double &value)
{
    value = m_value;
    return m_valid;
}


int pointsCompare(double a, double b)
{
    return (
                (a < b)
                ? -1
                : (a == b) ? 0 : 1
                             );
}

PointsOrigins pointsOriginsGet(const QVector<double> points3)
{
    static const PointsOrigins table[3 * 3] =
    {
        PointsOrigins::MONOTONICALLY_GROWING    , PointsOrigins::NON_MONOTONICALLY_GROWING   , PointsOrigins::MIDDLE_MAXIMUM,
        PointsOrigins::NON_MONOTONICALLY_GROWING, PointsOrigins::NON_DECREASING_NON_GROWING  , PointsOrigins::NON_MONOTONICALLY_DECREASING,
        PointsOrigins::MIDDLE_MINIMUM           , PointsOrigins::NON_MONOTONICALLY_DECREASING, PointsOrigins::MONOTONICALLY_DECREASING,
    };
    int c0 = pointsCompare(points3[0], points3[1]) + 1;
    int c1 = pointsCompare(points3[1], points3[2]) + 1;
    int index = c0 * 3 + c1;
    return table[index];
}

FuncDescr::FuncDescr()
    : m_ftype(FuncType::NON_MONOTONIC)
    , m_pointDescrs()
{

}

FuncDescr::~FuncDescr()
{

}

FuncDescr::FuncType FuncDescr::typeGet() const
{
    return m_ftype;
}

const FuncDescr::PointsDescr & FuncDescr::pointsDescrGet() const
{
    return m_pointDescrs;
}

void functionAnalysis(const QVector<double> &values, FuncDescr &fdescr)
{
    QVector<double> points;

    size_t src_global_min_index = 0;
    size_t src_global_max_index = 0;

    Extreme global_min;
    Extreme global_max;

    fdescr.m_ftype = fan::FuncDescr::FuncType::NON_MONOTONIC;

    if(values.size() == 0)
    {
        return;
    }

    fdescr.m_pointDescrs.clear();

    size_t src_index = 0;

    foreach(double value, values)
    {
        if(global_min.setMin(value))
        {
            src_global_min_index = src_index;
        }

        if(global_max.setMax(value))
        {
            src_global_max_index = src_index;
        }

        points.push_back(value);
        if(points.size() > 3)
        {
            points.pop_front();
        }

        if(points.size() == 3)
        {
            PointsOrigins orig = pointsOriginsGet(points);
            switch(orig)
            {
                case PointsOrigins::MONOTONICALLY_DECREASING:
                {
                    break;
                }
                case PointsOrigins::NON_MONOTONICALLY_DECREASING:
                {
                    break;
                }
                case PointsOrigins::MIDDLE_MINIMUM:
                {
                    fdescr.m_pointDescrs[src_index - 1] = FuncDescr::PointDescr::LOCAL_MINIMUM;
                    break;
                }
                case PointsOrigins::NON_DECREASING_NON_GROWING:
                {
                    break;
                }
                case PointsOrigins::MIDDLE_MAXIMUM:
                {
                    fdescr.m_pointDescrs[src_index - 1] = FuncDescr::PointDescr::LOCAL_MAXIMUM;
                    break;
                }
                case PointsOrigins::NON_MONOTONICALLY_GROWING:
                {
                    break;
                }
                case PointsOrigins::MONOTONICALLY_GROWING:
                {
                    break;
                }

            }
        }

        ++src_index;
    }

    fdescr.m_pointDescrs[src_global_min_index] = FuncDescr::PointDescr::GLOBAL_MINIMUM;
    fdescr.m_pointDescrs[src_global_max_index] = FuncDescr::PointDescr::GLOBAL_MAXIMUM;

    if(fdescr.m_pointDescrs.size() == 2)
    {
        if(fdescr.m_pointDescrs.first() == FuncDescr::PointDescr::GLOBAL_MINIMUM)
        {
            fdescr.m_ftype = FuncDescr::FuncType::MONOTONICALLY_GROWING;
        }
        else
        {
            fdescr.m_ftype = FuncDescr::FuncType::MONOTONICALLY_DECREASING;
        }
    }

}

void functionExtremumsGet(
        const FuncDescr::PointsDescr & pointsDescr,
        FuncDescr::PointsDescr::const_iterator &min,
        FuncDescr::PointsDescr::const_iterator &max
        )
{
    min = pointsDescr.constEnd();
    max = pointsDescr.constEnd();

    for (FuncDescr::PointsDescr::const_iterator it = pointsDescr.constBegin(); it != pointsDescr.constEnd(); ++it)
    {
        if(it.value() == FuncDescr::PointDescr::GLOBAL_MINIMUM)
        {
            min = it;
        }
        if(it.value() == FuncDescr::PointDescr::GLOBAL_MAXIMUM)
        {
            max = it;
        }
        if(min != pointsDescr.constEnd() && max != pointsDescr.constEnd())
        {
            return;
        }
    }
}

size_t functionAmountOfVibrations(
        const FuncDescr::PointsDescr & pointsDescr
        )
{
    size_t minimums = 0;
    size_t maximums = 0;
    for (FuncDescr::PointsDescr::const_iterator it = pointsDescr.constBegin(); it != pointsDescr.constEnd(); ++it)
    {
        FuncDescr::PointDescr descr = it.value();
        switch(descr)
        {
            case FuncDescr::PointDescr::LOCAL_MINIMUM : /* fall */
            case FuncDescr::PointDescr::GLOBAL_MINIMUM: ++minimums; break;
            case FuncDescr::PointDescr::LOCAL_MAXIMUM : /* fall */
            case FuncDescr::PointDescr::GLOBAL_MAXIMUM: ++maximums; break;
        }
    }
    return (minimums < maximums ? minimums : maximums);
}

void functionMinimumsVectorGet(
        const FuncDescr::PointsDescr & pointsDescr,
        const QVector<double> &source,
        QVector<double> &maximums
        )
{
    for (FuncDescr::PointsDescr::const_iterator it = pointsDescr.constBegin(); it != pointsDescr.constEnd(); ++it)
    {
        FuncDescr::PointDescr descr = it.value();
        if(
           descr == FuncDescr::PointDescr::LOCAL_MINIMUM ||
           descr == FuncDescr::PointDescr::GLOBAL_MINIMUM
           )
        {
            maximums.push_back(source[it.key()]);
        }
    }
}

void functionMaximumsVectorGet(
        const FuncDescr::PointsDescr & pointsDescr,
        const QVector<double> &source,
        QVector<double> &maximums
        )
{
    for (FuncDescr::PointsDescr::const_iterator it = pointsDescr.constBegin(); it != pointsDescr.constEnd(); ++it)
    {
        FuncDescr::PointDescr descr = it.value();
        if(
           descr == FuncDescr::PointDescr::LOCAL_MAXIMUM ||
           descr == FuncDescr::PointDescr::GLOBAL_MAXIMUM
           )
        {
            maximums.push_back(source[it.key()]);
        }
    }
}


#define TEST_ASSERT(x) if(!(x)) return -1
int test()
{
    PointsOrigins orig;
    QVector<double> points1 = { -1, 0,  1};
    QVector<double> points2 = {  0, 0,  1};
    QVector<double> points3 = {  1, 0,  1};
    QVector<double> points4 = { -1, 0,  0};
    QVector<double> points5 = {  0, 0,  0};
    QVector<double> points6 = {  1, 0,  0};
    QVector<double> points7 = { -1, 0, -1};
    QVector<double> points8 = {  0, 0, -1};
    QVector<double> points9 = {  1, 0, -1};

    orig = pointsOriginsGet(points1); TEST_ASSERT(orig == PointsOrigins::MONOTONICALLY_GROWING);
    orig = pointsOriginsGet(points2); TEST_ASSERT(orig == PointsOrigins::NON_MONOTONICALLY_GROWING);
    orig = pointsOriginsGet(points3); TEST_ASSERT(orig == PointsOrigins::MIDDLE_MINIMUM);
    orig = pointsOriginsGet(points4); TEST_ASSERT(orig == PointsOrigins::NON_MONOTONICALLY_GROWING);
    orig = pointsOriginsGet(points5); TEST_ASSERT(orig == PointsOrigins::NON_DECREASING_NON_GROWING);
    orig = pointsOriginsGet(points6); TEST_ASSERT(orig == PointsOrigins::NON_MONOTONICALLY_DECREASING);
    orig = pointsOriginsGet(points7); TEST_ASSERT(orig == PointsOrigins::MIDDLE_MAXIMUM);
    orig = pointsOriginsGet(points8); TEST_ASSERT(orig == PointsOrigins::NON_MONOTONICALLY_DECREASING);
    orig = pointsOriginsGet(points9); TEST_ASSERT(orig == PointsOrigins::MONOTONICALLY_DECREASING);


    const QVector<double> values1 = {0, 1, 2, 3};
    FuncDescr fdescr1;
    functionAnalysis(values1, fdescr1);

    const QVector<double> values2 = {3, 2, 1, 0};
    FuncDescr fdescr2;
    functionAnalysis(values2, fdescr2);

    const QVector<double> values3 = {0, 1, 2, 1, 0, -1, -2, -1, 10};
    FuncDescr fdescr3;
    functionAnalysis(values3, fdescr3);

    return 0;
}

}
