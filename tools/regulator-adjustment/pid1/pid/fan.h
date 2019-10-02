#ifndef FAN_H
#define FAN_H

#include <QVector>
#include <QMap>

namespace fan
{

template<typename value_type>
class ValidValue
{
public:
    ValidValue()
        : m_valid(false)
        , m_value()
    {
    }

    ~ValidValue()
    {
    }

    void reset()
    {
        m_valid = false;
    }

    void set(value_type value)
    {
        m_valid = true;
        m_value = value;
    }

    value_type get()
    {
        return m_value;
    }

    bool get(value_type &value)
    {
        value = m_value;
        return m_valid;
    }

    bool isValid()
    {
        return m_valid;
    }

    ValidValue& operator= (const value_type value)
    {
        m_valid = true;
        m_value = value;
        return *this;
    }

private:
    bool m_valid;
    value_type m_value;

};

class Extreme
{
public:
    Extreme();
    ~Extreme();
    void reset();
    /**
     * @return New value is more exreme, than stored
     */
    bool setMin(double value);
    /**
     * @return New value is more exreme, than stored
     */
    bool setMax(double value);
    bool get(double &value);

private:
    bool m_valid;
    double m_value;
};

enum class PointsOrigins
{
    MONOTONICALLY_DECREASING    , /**< монотонно убывающие */
    NON_MONOTONICALLY_DECREASING, /**< немонотонно убывающие */
    MIDDLE_MINIMUM              , /**< Средняя точка - минимум */
    NON_DECREASING_NON_GROWING  , /**< неубывающие-невозрастающие */
    MIDDLE_MAXIMUM              , /**< Средняя точка - максимум */
    NON_MONOTONICALLY_GROWING   , /**< немонотонно возрстающие */
    MONOTONICALLY_GROWING       , /**< монотонно возрастающие */
};

int pointsCompare(double a, double b);
PointsOrigins pointsOriginsGet(const QVector<double> points3);



/**
 * @brief Описание графика функции после анализа
 */
class FuncDescr
{
public:
    enum class PointDescr
    {
        LOCAL_MINIMUM,
        LOCAL_MAXIMUM,
        GLOBAL_MINIMUM,
        GLOBAL_MAXIMUM,
    };
    enum class FuncType
    {
        MONOTONICALLY_DECREASING, /**< монотонно убывающая */
        MONOTONICALLY_GROWING   , /**< монотонно возрастающая */
        NON_MONOTONIC,                  /**< невозрастающая, неубывающая */
    };

    typedef QMap< size_t, PointDescr > PointsDescr;
    FuncDescr();
    ~FuncDescr();

    FuncType typeGet() const;
    const PointsDescr & pointsDescrGet() const;

private:
    FuncType m_ftype;
    PointsDescr m_pointDescrs;/** pair = <index, descr>  */

    friend void functionAnalysis(const QVector<double> &values, FuncDescr &fdescr);
};

void functionAnalysis(const QVector<double> &values, FuncDescr &fdescr);

void functionExtremumsGet(
        const FuncDescr::PointsDescr & pointsDescr,
        FuncDescr::PointsDescr::const_iterator &min,
        FuncDescr::PointsDescr::const_iterator &max
        );

/**
 * @brief Количество колебаний (количество переходов min-max/max-min )
 */
size_t functionAmountOfVibrations(
        const FuncDescr::PointsDescr & pointsDescr
        );

void functionMinimumsVectorGet(
        const FuncDescr::PointsDescr & pointsDescr,
        const QVector<double> &source,
        QVector<double> &maximums
        );

void functionMaximumsVectorGet(
        const FuncDescr::PointsDescr & pointsDescr,
        const QVector<double> &source,
        QVector<double> &maximums
        );

int test();

}

#endif // FAN_H

