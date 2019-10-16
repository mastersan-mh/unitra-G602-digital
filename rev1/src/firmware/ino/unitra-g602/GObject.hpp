#ifndef GOBJECT_H_
#define GOBJECT_H_

#ifndef NOEXCEPT
#   define NOEXCEPT noexcept
#endif

class GObject
{
public:
    GObject() NOEXCEPT;
    virtual ~GObject() NOEXCEPT;
};

#endif /* GOBJECT_H_ */
