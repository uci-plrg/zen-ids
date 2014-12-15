#ifndef COMPILE_CONTEXT_H
#define COMPILE_CONTEXT_H 1

#include "php.h"

void push_compilation_unit(const char *path);
void pop_compilation_unit();
void push_compilation_function(const char *function_name);
void pop_compilation_function();

const char *get_compilation_unit_path();
uint get_compilation_unit_hash();
const char *get_compilation_function_name();
uint get_compilation_function_hash();

#endif
