/**
 * @file schedulersoft.hpp
 * @brief Soft time simple scheduler
 */

#ifndef NOSTD_SCHEDULERSOFT_SCHEDULERSOFT_HPP_
#define NOSTD_SCHEDULERSOFT_SCHEDULERSOFT_HPP_

extern "C"
{
#include <stddef.h>
#include <stdlib.h>
#include <nostd/list2.h>
}

#include <nostd/types>

/* #define NOSTD_SCHEDULERSOFT_DEBUG */

#ifdef NOSTD_SCHEDULERSOFT_DEBUG
#   include <iostream>

#   define NOSTD_SCHEDULERSOFT_DEBUG_PRINT(x) (x).debugPrint()
#else
#   define NOSTD_SCHEDULERSOFT_DEBUG_PRINT(x)
#endif


namespace nostd
{
/**
 * @brief The very simple scheduler with external timer
 * @param scheduledTasksMax     Maximal amount of scheduled tasks
 * @param time_type             The time type
 *
 */
template < nostd::size_t scheduledTasksMax, class time_type >
class SchedulerSoft
{
private:
    struct task_s
    {
        NOSTD_LIST2_FIELD(task_s, field_s) field;

        nostd::size_t id;
        time_type time;
        /* if handler == nullptr, then free */
        void (*handler)(nostd::size_t id, time_type time, time_type now, SchedulerSoft & sched, void * args);
        void * args;
    };

public:

    enum class Error
    {
        OK = 0,
        OUT_OF_RESOURCES = -1,
        INVALID_HANDLER = -2
    };

    SchedulerSoft()
    {
        NOSTD_LIST2_HEAD_INIT(task_s, scheduled);

        nostd::size_t i = 0;
        for(i = 0; i < scheduledTasksMax; ++i)
        {
            tasks[i].handler = nullptr;
        }
    }

    ~SchedulerSoft() noexcept
    {
    }

    /**
     * @brief Schedule the task
     * @note The task is scheduled to only one time. If you want to task run more than one time,
     * @note you should schedule it again in <handler> function.
     * @param id            The task identification. Can be any, it only to you to differ they.
     * @param time          Time, when task being handled.
     * @param handler       The task handler.
     * @param     id        The current task identification.
     * @param     time      Time, when task should being handled.
     * @param     now       Current time, can be lower than <time>.
     * @param     sched     Scheduler. Can be used to schedule task again.
     */
    Error shedule(
            nostd::size_t id,
            time_type time,
            void (*handler)(nostd::size_t id, time_type time, time_type now, SchedulerSoft & sched, void * args),
            void * args
    )
    {
        if(handler == nullptr)
        {
            return Error::INVALID_HANDLER;
        }
        struct task_s * task = task_alloc();
        if(task == nullptr)
        {
            return Error::OUT_OF_RESOURCES;
        }
        task->id = id;
        task->time = time;
        task->handler = handler;
        task->args = args;

        struct task_s * tmp;

        /*
         * Inserting in tail can be more faster,
         * because time of most scheduled tasks,
         * is later than current and later than already added tasks.
         */
        NOSTD_LIST2_FOREACH_REV(task_s, field, scheduled, tmp)
        {
            if(time >= tmp->time)
            {
                NOSTD_LIST2_INSERT_AFTER(field, tmp, task);
                return Error::OK;
            }
        }

        NOSTD_LIST2_INSERT_HEAD(task_s, field, scheduled, task);

        return Error::OK;
    }

    /**
     * @brief remove the scheduled task
     * @param id            The task identification.
     */
    Error unshedule(
            nostd::size_t id
    )
    {

        bool removed;
        do
        {
            removed = false;
            struct task_s * tmp;
            NOSTD_LIST2_FOREACH(task_s, field, scheduled, tmp)
            {
                if(id == tmp->id)
                {
                    NOSTD_LIST2_REMOVE(field, tmp);
                    tmp->handler = nullptr;
                    removed = true;
                    break;
                }
            }
        } while(removed);


        return Error::OK;
    }

    /**
     * @brief Handle the tasks, whose time has come.
     * @param now       Current time.
     */
    void handle(time_type now)
    {
        void (*handler)(nostd::size_t id, time_type time, time_type now, SchedulerSoft & sched, void * args);
        struct task_s * tmp;

        tmp = NOSTD_LIST2_HEAD(scheduled);
        while(tmp != NOSTD_LIST2_END(task_s, scheduled) && tmp->time <= now)
        {
            NOSTD_LIST2_REMOVE(field, tmp);

            handler = tmp->handler;
            tmp->handler = nullptr;

            handler(tmp->id, tmp->time, now, *this, tmp->args);

            tmp = NOSTD_LIST2_HEAD(scheduled);

        }
    }

    /**
     * @brief Get the near future when we can handle the immediate tasks.
     * @return time
     * @return      time == 0   No scheduled tasks.
     */
    inline time_type nearestTime() const
    {
        if(NOSTD_LIST2_EMPTY(task_s, scheduled))
        {
            return time_type();
        }
        return NOSTD_LIST2_HEAD(scheduled)->time;
    }

#ifdef NOSTD_SCHEDULERSOFT_DEBUG
    void debugPrint() const
    {
        struct task_s * tmp;
        LIST2_FOREACH(task_s, field, scheduled, tmp)
        {
            std::cout
            << "SchedulerSoft::debugPrint(): id = " << tmp->id
            << ";time = " << tmp->time
            << std::endl;
        }
    }
#endif

private:

    struct task_s * task_alloc()
    {
        struct task_s * task;
        nostd::size_t i;
        for(i = 0; i < scheduledTasksMax; ++i)
        {
            task = &tasks[i];
            if(task->handler == nullptr)
            {
                return task;
            }
        }
        return nullptr;
    }

    /* scheduled tasks */
    NOSTD_LIST2_LIST(task_s, field_s) scheduled;
    /* pool for the tasks */
    struct task_s tasks[scheduledTasksMax];

};

}

#endif /* NOSTD_SCHEDULERSOFT_SCHEDULERSOFT_HPP_ */
