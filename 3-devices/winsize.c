#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(void) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        perror("ioctl(TIOCGWINSZ)");
        return 1;
    }
    printf("Terminal size: %u rows, %u columns\n",
           (unsigned)ws.ws_row, (unsigned)ws.ws_col);
    return 0;
}
