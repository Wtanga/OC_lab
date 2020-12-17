#include <stdio.h>

#include <time.h>
void cookie() {
    FILE *stat;
    stat = fopen("/home/nikolay/Рабочий стол/OS_labs/lab6/cookie.c.txt", "a+");
    long int ttime;

    ttime = time (NULL);
    fprintf(stat, "connected: ");
    fprintf(stat, ctime(&ttime));
    fprintf(stat, "\n");
    fclose(stat);
}