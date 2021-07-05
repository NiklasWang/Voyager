#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#define FORMAT_MAX_LEN          4096
#define MAX_LEN_EXCEPT_PROGRESS 11
#define MIN_PROGRESS_LEN        10

int main()
{
    int i = 0;
    char bar[120];
    const char lable[] = "|/-\\";
    struct winsize w;
    char format[FORMAT_MAX_LEN] = { '\0' };

    while (i <= 100) {
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        int progresslen = w.ws_col - MAX_LEN_EXCEPT_PROGRESS;
        progresslen = progresslen < MIN_PROGRESS_LEN ? MIN_PROGRESS_LEN : progresslen;
        sprintf(format, "[%%-%ds][%%d%%][%%c]\r", progresslen);
        printf(format, bar, i, lable[i % 4]);
        fflush(stdout);
        bar[i] = '#';
        i++;
        bar[i] = 0;
        usleep(100 * 1000);
    }
    printf("\n");

    return 0;
}
