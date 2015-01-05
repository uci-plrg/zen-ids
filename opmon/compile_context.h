#ifndef COMPILE_CONTEXT_H
#define COMPILE_CONTEXT_H 1

#include "php.h"
#include "metadata_handler.h"

void init_compile_context();

void push_compilation_unit(const char *path);
control_flow_metadata_t pop_compilation_unit();
void push_compilation_function(const char *classname, const char *function_name);
void pop_compilation_function();
void push_eval(uint eval_id);

const char *get_compilation_unit_path();
uint get_compilation_unit_hash();
const char *get_compilation_routine_name();
uint get_compilation_routine_hash();

const char *get_function_declaration_path(const char *function_name);
control_flow_metadata_t *get_cfm(const char *function_name);

void add_compiled_op(const zend_op *op, uint index);
void add_compiled_edge(uint from_index, uint to_index);

#endif
