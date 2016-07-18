#ifndef COMPILE_CONTEXT_H
#define COMPILE_CONTEXT_H 1

#include "php.h"
#include "metadata_handler.h"

#define COMPILED_EDGE_UNKNOWN_TARGET 0xffffffffU
#define COMPILED_ROUTINE_DEFAULT_SCOPE "<default>"
#define COMPILED_ROUTINE_DEFAULT_SCOPE_LEN strlen(COMPILED_ROUTINE_DEFAULT_SCOPE)

typedef enum _compiled_edge_type_t {
  COMPILED_EDGE_NONE,
  COMPILED_EDGE_DIRECT,
  COMPILED_EDGE_INDIRECT,
  COMPILED_EDGE_CALL
} _compiled_edge_type_t;

typedef struct _compiled_edge_target_t {
  _compiled_edge_type_t type;
  uint index;
} compiled_edge_target_t;

void init_compile_context();

void function_created(zend_op *src, zend_op_array *f);

control_flow_metadata_t *get_cfm_by_name(const char *routine_name);
control_flow_metadata_t *get_cfm_by_opcodes_address(zend_op *opcodes);
control_flow_metadata_t *get_last_eval_cfm();

compiled_edge_target_t get_compiled_edge_target(zend_op *op, uint op_index);
#endif
