#ifndef SLIDINGWINDOW_H
#define SLIDINGWINDOW_H

#include <QVector>

class CSlidingWindow
{
public:
    CSlidingWindow(int capacity);
    ~CSlidingWindow();
    void capacitySet(int capacity);
    bool append(double val);
    bool isFull();
    void clear();
    const QVector <double> &get();
private:
    int m_capacity;
    QVector <double> m_vec;
};

#endif // SLIDINGWINDOW_H
