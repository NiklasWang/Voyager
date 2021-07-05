#include <stdio.h>
#include <unistd.h>

#define RED    "\e[0;31m"
#define BLUE   "\e[0;34m"
#define NONE   "\e[0m"
#define GREEN  "\e[0;32m"
#define YELLOW "\e[0;33;1m"
#define PURPLE "\e[0;35m"
#define GR     "\e[0;36m"

static const char * const gColors[] = {
    NONE,
    RED,
    BLUE,
    GREEN,
    YELLOW,
    PURPLE,
    GR
};

int main()
{
    int i = 0;
    char bar[120];
    const char lable[] = "|/-\\";

    while (i <= 100) {
        printf("%s[%-100s][%d%%][%c]\r%s",
            gColors[i % (sizeof(gColors) / sizeof(gColors[0]))],
            bar, i, lable[i % 4],
            gColors[0]);
        fflush(stdout);
        bar[i] = '#';
        i++;
        bar[i] = 0;
        usleep(100 * 1000);
    }
    printf("\n");

    return 0;
}