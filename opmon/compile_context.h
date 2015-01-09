#ifndef COMPILE_CONTEXT_H
#define COMPILE_CONTEXT_H 1

#include "php.h"
#include "metadata_handler.h"

void init_compile_context();

void function_compiled();

const char *get_function_declaration_path(const char *function_name);
control_flow_metadata_t *get_cfm(const char *function_name);

#endif
