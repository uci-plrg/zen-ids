#ifndef COMPILE_CONTEXT_H
#define COMPILE_CONTEXT_H 1

#include "php.h"

void push_compilation_unit(const char *path);
void pop_compilation_unit();
void push_compilation_function(const char *function_name);
void pop_compilation_function();

#endif
