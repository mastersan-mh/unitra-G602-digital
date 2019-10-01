#include <stdlib.h>

void setup();
void loop();


int main()
{
    setup();
    while(1)
    {
        loop();
    }
    return EXIT_SUCCESS;
}
