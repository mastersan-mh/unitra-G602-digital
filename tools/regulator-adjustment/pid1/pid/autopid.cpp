#include "autopid.hpp"

#include <array>

autopid::autopid()
{

}

void autopid::valueAppend(double x)
{
    std::array< double, AUTOPID_SIZE > vec;

    items.append(x);

    for(
        Items::const_iterator item = items.cbegin();
        item < items.cend();
        ++item
        )
    {
        double y = (*item);

    }

}

std::size_t autopid::length()
{
    return items.length();
}
