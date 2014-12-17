#ifndef COMPILE_CONTEXT_H
#define COMPILE_CONTEXT_H 1

#include "php.h"

void push_compilation_unit(const char *path);
routine_cfg_t *pop_compilation_unit();
void push_compilation_function(const char *function_name);
void pop_compilation_function();
void push_eval(uint eval_id);

const char *get_compilation_unit_path();
uint get_compilation_unit_hash();
const char *get_compilation_function_name();
uint get_compilation_function_hash();

const char *get_function_declaration_path(const char *function_name);
routine_cfg_t *get_cfg(const char *function_name);

void add_compiled_opcode(zend_uchar opcode);
void add_compiled_edge(uint from_index, uint to_index);

#endif
