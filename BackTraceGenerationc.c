//gcc BackTraceGeneration.c -rdynamic
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <execinfo.h>

void print_trace(int signum)
{
    FILE *fp;
    void *callStack[12];
    int size,i;
    char** symbols = NULL;
    fp = fopen("/tmp/crash_dump.txt", "a");
    if (fp != NULL)
    {
        fseek(fp, 0, SEEK_END);
        size = backtrace(callStack, 12);
        symbols = backtrace_symbols(callStack, size);
        fprintf(fp, "signal received: %d\n", signum);
        printf("signal received: %d\n", signum);
        for (i = 0; i < size; i++)
        {
            fprintf(fp, "Backtrace # %d: %s\n", i, symbols[i]);
            printf("Backtrace # %d: %s\n",i, symbols[i]);
        }

        fclose(fp);
        free(symbols);
    }
}

void signal_handler(int signum)
{
    printf("Signal %d caught",signum);
    print_trace (signum);
    exit(signum);
}

void func()
{
    int *ptr = NULL;
    *ptr = 10;
}

int main()
{
    signal(SIGABRT, signal_handler);
    signal(SIGSEGV, signal_handler);
    func();
    return 0;
}
