#ifndef SLIDINGWINDOW_H
#define SLIDINGWINDOW_H

#include <QVector>

class CSlidingWindow
{
public:
    CSlidingWindow(int capacity);
    ~CSlidingWindow() = default;
    void sizeSet(int size);
    int sizeGet() const;
    bool append(double val);
    bool isFull() const;
    void clear();
    const QVector <double> &get();
private:
    int m_size;
    QVector <double> m_vec{};
};

#endif // SLIDINGWINDOW_H
