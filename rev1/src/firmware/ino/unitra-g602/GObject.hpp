#ifndef GOBJECT_H_
#define GOBJECT_H_

#ifndef NOEXCEPT
#   define NOEXCEPT noexcept
#endif

class GObject
{
public:
    GObject() NOEXCEPT = default;
    virtual ~GObject() NOEXCEPT = default;
};

#endif /* GOBJECT_H_ */
