
#include "GBlinker.hpp"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

int main (void)
{
    GBlinker bl;

    unsigned action;
    bool end;
    bool light;
    unsigned long wait_time;

#define TEST_ASSERT(x) if(!(x)) return -1;

    action = bl.typeSet(GBlinker::BlinkType::ON_TOO_LOW_SPEED, true);
    TEST_ASSERT(action == GBLINKER_ACTIONFLAG_SCHEDULE);
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 100);
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 100);
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 100);

    action = bl.typeSet(GBlinker::BlinkType::ON_START, true);
    TEST_ASSERT(action == (GBLINKER_ACTIONFLAG_UNSCHEDULE | GBLINKER_ACTIONFLAG_SCHEDULE));
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 1000);
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 1000);
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 750);
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 750);
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 500);
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 500);
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 250);
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 250);
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 100);

    action = bl.typeSet(GBlinker::BlinkType::ON_AUTO, true);
    TEST_ASSERT(action == 0);

    /* intermission */
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 1000);

    /* on auto */
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 250);
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 250);

    /* intermission */
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 1000);

    /* infinite blinking */
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 100);
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 100);
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 100);
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 100);
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 100);
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == false && wait_time == 100);

    action = bl.typeSet(GBlinker::BlinkType::ON_TOO_HIGH_SPEED, false);
    TEST_ASSERT(action == 0);
    action = bl.typeSet(GBlinker::BlinkType::ON_TOO_LOW_SPEED, false);
    TEST_ASSERT(action == GBLINKER_ACTIONFLAG_UNSCHEDULE);
    bl.scheduledPartGet(&end, &light, &wait_time);
    TEST_ASSERT(end == true && wait_time == 100);
}
