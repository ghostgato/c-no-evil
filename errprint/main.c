#include "errprint.h"
// #include <stdio.h>

int main(void)
{
    int mynum = 5;
    int yournum = 10;
    int hisnum = 7;
    char myinfo[] = "his num is ";

    errprint(ERR, "test #%d", mynum);
    errprint(USAGE, "yournum is #%d", yournum);
    errprint(WARN, "%s%d", myinfo, hisnum);
    errprint(USAGE, "arg with no variable attached %s", "THIS IS MY ATTACHMENT");
    // printf("testing %d");
    errprint(ERR, "hello!");

    return 0;
}