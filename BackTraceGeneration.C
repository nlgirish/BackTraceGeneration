// g++ BackTraceGeneration.C -rdynamic
#include <iostream>
#include <csignal>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <cxxabi.h>

void signal_handler(int signal)
{
    std::cerr << "Received signal:" << signal << std::endl;

    void *callStack[100];
    size_t size;
    char **symbols;
    size_t i;

    size = backtrace (callStack, 100);
    symbols = backtrace_symbols (callStack, size);

    for (i = 0; i < size; i++)
    {
        char *begin_name = 0, *begin_offset = 0, *end_offset = 0;
        for (char *p = symbols[i]; *p; ++p)
        {
            if (*p == '(')
                begin_name = p;
            else if (*p == '+')
                begin_offset = p;
            else if (*p == ')' && begin_offset)
            {
                end_offset = p;
                break;
            }
        }
        if (begin_name && begin_offset && end_offset
                && begin_name < begin_offset)
        {
            *begin_name++ = '\0';
            *begin_offset++ = '\0';
            *end_offset = '\0';
            int status;
            char* demangled_func = abi::__cxa_demangle(begin_name,
                    NULL, NULL, &status);
            if (status == 0)
            {
                std::cerr<<"Backtrace #"<<i-1<<" : " << demangled_func <<"+"<<begin_offset << " in " << symbols[i]<<" ("<<begin_name<<"+"<<begin_offset<<")"<<std::endl;
            }
            else
            {
                std::cerr<<"Backtrace #"<<i-1<<" : " << begin_name<<"()+"<<begin_offset<< " in " << symbols[i]<<" ("<<begin_name<<"+"<<begin_offset<<")"<<std::endl;
            }
            if(demangled_func != NULL)
            {
                free (demangled_func);
            }
        }
        else
        {
            std::cerr<<"Backtrace #"<<i-1<<" : " << symbols[i] << std::endl;
        }
    }

    free (symbols);
    exit(1);
}

void func() {
    int *ptr = NULL;
    *ptr = 10;
}

int main()
{
    std::signal(SIGABRT, signal_handler);
    std::signal(SIGSEGV, signal_handler);
    func();
    return 0;
}
