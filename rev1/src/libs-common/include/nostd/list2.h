/*
 * list2.h
 *
 *  Created on: 2 дек. 2018 г.
 *      Author: mastersan
 */

#ifndef SRC_INCLUDE_NOSTD_LIST2_H_
#define SRC_INCLUDE_NOSTD_LIST2_H_

/*
example:

struct type_s
{
    LIST2_FIELD(type_s, fieldtype_s) field
    data_t data;
}

LIST2_LIST(type_s, fieldtype_s) list;

void main()
{
    LIST2_HEAD_INIT(type_s, list);
}

 */

#define NOSTD_LIST2_FIELD(xtype, xfieldtype) \
        struct xfieldtype \
        { \
            struct xtype * prev; \
            struct xtype * next; \
        }

#define NOSTD_LIST2_LIST(xtype, xfieldtype) \
    NOSTD_LIST2_FIELD(xtype, xfieldtype)

#define NOSTD_LIST2_EMPTY(xtype, xlist) \
        ( (xlist).next == (struct xtype *)&(xlist) )

#define NOSTD_LIST2_HEAD_INIT(xtype, xlist) \
        do { \
            (xlist).prev = (struct xtype*)&(xlist); \
            (xlist).next = (struct xtype*)&(xlist); \
        } while(0)

#define NOSTD_LIST2_INSERT_HEAD(xtype, xfield, xlist, xitem) \
        do { \
            (xitem)->xfield.next = (xlist).next; \
            (xitem)->xfield.next->xfield.prev = (xitem); \
            (xitem)->xfield.prev = (struct xtype*)&(xlist); \
            (xlist).next = (xitem); \
        } while (0)

#define NOSTD_LIST2_INSERT_TAIL(xtype, xfield, xlist, xitem) \
        do { \
            (xitem)->xfield.prev = (xlist).prev; \
            (xitem)->xfield.prev->xfield.next = (xitem); \
            (xitem)->xfield.next = (struct xtype*)&(xlist); \
            (xlist).prev = (xitem); \
        } while (0)

/**
 * @param xitem     insertable item
 */
#define NOSTD_LIST2_INSERT_BEFORE(xfield, xbefore, xitem) \
        do { \
            (xitem)->xfield.prev = (xbefore)->xfield.prev; \
            (xitem)->xfield.next = (xbefore); \
            (xbefore)->xfield.prev->field.next = (xitem); \
            (xbefore)->xfield.prev = (xitem); \
        } while (0)

/**
 * @param xitem     insertable item
 */
#define NOSTD_LIST2_INSERT_AFTER(xfield, xafter, xitem) \
        do { \
            (xitem)->xfield.next = (xafter)->xfield.next; \
            (xitem)->xfield.prev = (xafter); \
            (xafter)->xfield.next->xfield.prev = (xitem); \
            (xafter)->xfield.next = (xitem); \
        } while (0)

#define NOSTD_LIST2_REMOVE(xfield, xitem) \
        do { \
            (xitem)->xfield.prev->xfield.next = (xitem)->xfield.next; \
            (xitem)->xfield.next->xfield.prev = (xitem)->xfield.prev; \
        } while (0)

#define NOSTD_LIST2_END(xtype, xlist) \
        (struct xtype *)&(xlist)

#define NOSTD_LIST2_HEAD(xlist) \
        (xlist).next

#define NOSTD_LIST2_TAIL(xlist) \
        (xlist).prev

#define NOSTD_LIST2_NEXT(xfield, xitem) \
        (xitem)->xfield.next

#define NOSTD_LIST2_PREV(xfield, xitem) \
        (xitem)->xfield.prev

#define NOSTD_LIST2_FOREACH(xtype, xfield, xlist, xitem) \
        for(xitem = NOSTD_LIST2_HEAD(xlist); xitem != NOSTD_LIST2_END(xtype, xlist); xitem = NOSTD_LIST2_NEXT(xfield, xitem))

#define NOSTD_LIST2_FOREACH_REV(xtype, xfield, xlist, xitem) \
        for(xitem = NOSTD_LIST2_TAIL(xlist); xitem != NOSTD_LIST2_END(xtype, xlist); xitem = NOSTD_LIST2_PREV(xfield, xitem))

#endif /* SRC_INCLUDE_NOSTD_LIST2_H_ */
