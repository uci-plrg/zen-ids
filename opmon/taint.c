
#include "taint.h"

void taint_var_add(application_t *app, taint_variable_type_t type, taint_variable_id_t id,
                   zend_op *stack_frame_id, void *taint)
{
  taint_variable_t taint = { type, id, stack_frame_id, taint };

  plog_taint(app, &taint);
}

void *taint_var_get(taint_variable_type_t type, taint_variable_id_t id, zend_op *stack_frame_id)
{
  return NULL;
}

void *taint_var_remove(taint_variable_type_t type, taint_variable_id_t id, zend_op *stack_frame_id)
{
  return NULL;
}

