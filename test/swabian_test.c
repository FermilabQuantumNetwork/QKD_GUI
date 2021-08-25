#include <stdio.h>
#include <unistd.h>
#include "swabian.h"

/* Macro to compute the size of a static C array.
 *
 * See https://stackoverflow.com/questions/1598773. */
#define LEN(x) ((sizeof(x)/sizeof(0[x]))/((size_t)(!(sizeof(x) % sizeof(0[x])))))

int main(int argc, char **argv)
{
    int i;
    Swabian s;

    if (s.connect())
        exit(1);

    int channels[4] = {1,2,3,4};
    double data[4];

    s.get_count_rates(channels,data,LEN(channels));

    for (i = 0; i < LEN(channels); i++) {
        printf("count rate for channel %i is %.2f Hz\n", channels[i], data[i]);
    }

    return 0;
}
