#ifndef AUTOPID_H
#define AUTOPID_H

#include <nostd/slidingwindow/slidingwindow>

#include <cstdlib>

#define AUTOPID_SIZE 50

class autopid
{
public:
    autopid();

    void valueAppend(double x);

    std::size_t length();

private:
    typedef nostd::SlidingWindow< double, AUTOPID_SIZE > Items;

    Items items;

};

#endif // AUTOPID_H
