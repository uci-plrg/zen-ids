#include "cfg_handler.h"
#include "taint.h"

taint_variable_t *create_taint_variable(zend_op_array *op_array, const zend_op *tainted_op,
                                        taint_type_t type, void *taint)
{
  taint_variable_type_t var_type;
  taint_variable_id_t var_id;

  // TODO: check for global fetch op

  switch (tainted_op->result_type) {
    case IS_TMP_VAR:
      var_type = TAINT_VAR_TEMP;
      var_id.temp_id = (uint) (EX_VAR_TO_NUM(tainted_op->result.var) - op_array->last_var);
      // print_var_value(EX_VAR(operand->var));
      break;
    case IS_VAR:
      var_type = TAINT_VAR_TEMP;
      var_id.temp_id = (uint) (EX_VAR_TO_NUM(tainted_op->result.var) - op_array->last_var);
      break;
    case IS_CV:
      var_type = TAINT_VAR_LOCAL;
      var_id.var_name = op_array->vars[EX_VAR_TO_NUM(tainted_op->result.var)]->val;
      // print_var_value(EX_VAR(operand->var));
      break;
    default:
      return NULL;
  }

  taint_variable_t *var = malloc(sizeof(taint_variable_t));
  var->var_type = var_type;
  var->var_id = var_id;
  var->tainted_op = tainted_op;
  var->stack_frame = op_array;
  var->type = type;
  var->taint = taint;
  return var;
}

void taint_var_add(application_t *app, taint_variable_t *var)
{
  plog_taint_var(app, var);
}

void *taint_var_get(taint_variable_type_t type, taint_variable_id_t id, zend_op *stack_frame_id)
{
  return NULL;
}

void *taint_var_remove(taint_variable_type_t type, taint_variable_id_t id, zend_op *stack_frame_id)
{
  return NULL;
}

