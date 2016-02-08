#include "lib/script_cfi_utils.h"
#include "cfg_handler.h"
#include "taint.h"

static sctable_t taint_table;

void init_taint_tracker()
{
  taint_table.hash_bits = 6;
  sctable_init(&taint_table);
}

void destroy_taint_tracker()
{
  sctable_destroy(&taint_table);
}

taint_variable_t *create_taint_variable(const zval *value, zend_op_array *stack_frame,
                                        const zend_op *tainted_at, taint_type_t type, void *taint)
{
  taint_variable_t *var = malloc(sizeof(taint_variable_t));
  var->value = value;
  var->tainted_at_file = strdup(stack_frame->filename->val);
  var->tainted_at = tainted_at;
  var->type = type;
  var->taint = taint;
  return var;
}

void destroy_taint_variable(taint_variable_t *taint_var)
{
  free((char *) taint_var->tainted_at_file); // ok for object owner at end-of-life
  free(taint_var);
}

/*
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
  var->first_tainted_op = tainted_op;
  var->stack_frame = op_array;
  var->type = type;
  var->taint = taint;
  return var;
}
*/

/* perfect hash for taint_variable_t * /
static uint64 hash_taint_fields(taint_variable_type_t var_type, taint_variable_id_t var_id,
                                zend_op *stack_frame_id)
{
  uint64 var_hash, hash, context_hash = ((uint64) stack_frame_id ^ var_type);

  if (var_type == TAINT_VAR_TEMP)
    var_hash = var_id.temp_id;
  else
    var_hash = hash_string(var_id.var_name);

  hash = ((context_hash >> 0x20) ^ var_hash) ^ (context_hash << 0x20);
  return hash;
}

static inline uint64 hash_taint_var(taint_variable_t *var)
{
  return hash_taint_fields(var->var_type, var->var_id, var->stack_frame->opcodes);
}
*/

void taint_var_add(application_t *app, taint_variable_t *var)
{
  uint64 hash = (uint64) var->value;

  plog_taint_var(app, var);

  if (sctable_lookup(&taint_table, hash) != NULL)
    destroy_taint_variable(var);
  else
    sctable_add(&taint_table, hash, var);
}

taint_variable_t *taint_var_get(const zval *value)
{
  uint64 hash = (uint64) value;
  return (taint_variable_t *) sctable_lookup(&taint_table, hash);
}

taint_variable_t *taint_var_remove(const zval *value)
{
  uint64 hash = (uint64) value;
  return (taint_variable_t *) sctable_remove(&taint_table, hash);
}

/******************************************************************************************
 * Propagation
 */

typedef enum _taint_operand_index_t {
  TAINT_OPERAND_RESULT,
  TAINT_OPERAND_VALUE = TAINT_OPERAND_RESULT,
  TAINT_OPERAND_1,
  TAINT_OPERAND_MAP = TAINT_OPERAND_1,
  TAINT_OPERAND_2,
  TAINT_OPERAND_KEY = TAINT_OPERAND_2,
} taint_operand_index_t;

typedef enum _taint_value_type_t {
  TAINT_VALUE_TYPE_NONE,
  TAINT_VALUE_TYPE_CONST,
  TAINT_VALUE_TYPE_JMP,
  TAINT_VALUE_TYPE_VAR,
  TAINT_VALUE_TYPE_THIS,
  TAINT_VALUE_TYPE_TEMP,
  TAINT_VALUE_TYPE_INCLUDE,
  TAINT_VALUE_TYPE_FCALL,
} taint_value_type_t;

typedef struct _taint_value_t {
  taint_value_type_t type;
  union {
    uint temp_id;
    uint jmp_target;
    const char *var_name;
    zval *constant;
    zend_class_entry *class;
  };
} taint_value_t;

/*
static void initilize_taint_operand(taint_operand_t *operand, zend_op_array *zops,
                                       znode_op *znode, zend_uchar type)
{
  switch (type) {
    case IS_CONST:
      operand->value.type = TAINT_VALUE_TYPE_CONST;
      operand->value.constant = znode->zv;
      break;
    case IS_VAR:
    case IS_TMP_VAR:
      operand->value.type = TAINT_VALUE_TYPE_TEMP;
      operand->value.temp_id = (uint) (EX_VAR_TO_NUM(znode->var) - zops->last_var);
      break;
    case IS_CV:
      operand->value.var_name = zops->vars[EX_VAR_TO_NUM(znode->var)]->val;
      if (strcmp("this", operand->value.var_name) == 0)
        operand->value.type = TAINT_VALUE_TYPE_THIS;
      else
        operand->value.type = TAINT_VALUE_TYPE_VAR;
      break;
    case IS_UNUSED:
    default:
      operand->value.type = TAINT_VALUE_TYPE_NONE;
  }
}

static void propagate_operand_taint(taint_opcode_t *op, taint_operand_index_t src_index,
                                    taint_operand_index_t dst_index)
{
  char var_name[256];
  uint lookup_index = 0;
  taint_operand_t *src = get_taint_operand(op, src_index);
  taint_operand_t *dst = get_taint_operand(op, dst_index);

  if (src_index != TAINT_OPERAND_SOURCE) {
    taint_live_variable_t *src_var;

    get_var_name(src, var_name, 256);
    while ((src_var = lookup_live_variable(live_variables, &lookup_index, src)) != NULL) {
      if (IS_LIVE_GLOBAL(src_var)) {
        taint_operand_t *global_dst = (dst == NULL ? src : dst);

        fprintf(opcode_dump_file, "Attempting to link global source %s to 0x%x:%d\n", var_name,
                op->id.routine_hash, op->id.op_index);

        if (add_predecessor(&global_dst->predecessor, &src_var->global_id,
                            TAINT_OPERAND_SOURCE)) {
          fprintf(opcode_dump_file, "Link global variable '%s': "
                  "0x%x:%d.source -%d-> 0x%x:%d.%c\n", var_name,
                  src_var->global_id.routine_hash, src_var->global_id.op_index, src_index,
                  op->id.routine_hash, op->id.op_index,
                  get_operand_index_code(dst_index));
        }
      }
    }
  }

  lookup_index = 0;
  if (dst_index != TAINT_OPERAND_SINK) {
    taint_live_variable_t *dst_var;

    get_var_name(dst, var_name, 256);
    while ((dst_var = lookup_live_variable(live_variables, &lookup_index, dst)) != NULL) {
      if (IS_LIVE_GLOBAL(dst_var)) {
        taint_routine_t *sink_routine = sctable_lookup(&dg->routine_table,
                                                          dst_var->global_id.routine_hash);
        taint_opcode_t *sink_op = (taint_opcode_t *) sink_routine->opcodes.data[dst_var->global_id.op_index];

        fprintf(opcode_dump_file, "Attempting to link 0x%x:%d to global sink of variable %s\n",
                op->id.routine_hash, op->id.op_index, var_name);

        if (add_predecessor(&sink_op->sink.predecessor, &dst_var->src.opcode_id,
                            dst_var->src.operand_index)) {
          fprintf(opcode_dump_file, "Link to global variable '%s': 0x%x:%d.%c -%d-> 0x%x:%d.sink\n",
                  var_name, op->id.routine_hash, op->id.op_index,
                  get_operand_index_code(src_index),
                  dst_index, sink_op->id.routine_hash, sink_op->id.op_index);
        }
      }
    }
  }
  lookup_index = 0;

  //fprintf(opcode_dump_file, "   [ propagate_operand_taint %d.(%d->%d)) ]\n",
  //        op->id.op_index, src_index, dst_index);

  if (src_index == TAINT_OPERAND_SOURCE) {
    taint_operand_t *dst = get_taint_operand(op, dst_index);

    if (add_predecessor(&dst->predecessor, &op->id, src_index)) {
      fprintf(opcode_dump_file, "Link source type %d into 0x%x:%d.%d\n", op->source.type,
              op->id.routine_hash, op->id.op_index, dst_index);
    }

  } else if (dst_index == TAINT_OPERAND_SINK) {
    taint_operand_t *src = get_taint_operand(op, src_index);
    taint_live_variable_t *src_var;

    switch (src->value.type) {
      case TAINT_VALUE_TYPE_VAR:
        if (op->sink.type == SINK_TYPE_GLOBAL) { / * create an empty live_variable flagged global * /
          taint_live_variable_t *global_var = malloc(sizeof(taint_live_variable_t));
          memset(global_var, 0, sizeof(taint_live_variable_t));
          global_var->global_id = op->id;
          global_var->var_name = src->value.var_name;
          global_var->src.opcode_id = op->id;
          global_var->src.operand_index = src_index;
          scarray_append(live_variables, global_var);

          fprintf(opcode_dump_file, "Generate global var %s at 0x%x:%d\n", global_var->var_name,
                  op->id.routine_hash, op->id.op_index);
        }
      case TAINT_VALUE_TYPE_THIS: / * FT * /
      case TAINT_VALUE_TYPE_TEMP:
        get_var_name(src, var_name, 256);
        //fprintf(opcode_dump_file, "   [ searching live variables for %s (in operand %d.(%d->%d)) ]\n",
        //        var_name, op->id.op_index, src_index, dst_index);
        while ((src_var = lookup_live_variable(live_variables, &lookup_index, src)) != NULL)
          add_predecessor(&src->predecessor, &src_var->src.opcode_id, src_var->src.operand_index);
      default: / *no* /;
    }

    if (add_predecessor(&op->sink.predecessor, &op->id, src_index)) {
      fprintf(opcode_dump_file, "Link operand 0x%x:%d.%d into sink of type %d\n",
              op->id.routine_hash, op->id.op_index, src_index, op->sink.type);
    }
  } else {
    bool found_any;
    taint_operand_t *src = get_taint_operand(op, src_index);
    taint_operand_t *dst = get_taint_operand(op, dst_index);
    taint_live_variable_t *src_var;

    switch (src->value.type) {
      case TAINT_VALUE_TYPE_NONE:
      case TAINT_VALUE_TYPE_CONST:
      case TAINT_VALUE_TYPE_JMP: // it's a sink, though
      case TAINT_VALUE_TYPE_INCLUDE:
      case TAINT_VALUE_TYPE_FCALL:
        fprintf(opcode_dump_file, "Nothing to link for 0x%x:%d.(%c->%c)\n", op->id.routine_hash,
                op->id.op_index, get_operand_index_code(src_index),
                get_operand_index_code(dst_index));
        break;
      case TAINT_VALUE_TYPE_VAR:
      case TAINT_VALUE_TYPE_THIS:
      case TAINT_VALUE_TYPE_TEMP:
        found_any = false;
        get_var_name(src, var_name, 256);
        //fprintf(opcode_dump_file, "   [ searching live variables for %s (in operand %d.(%d->%d)) ]\n",
        //        var_name, op->id.op_index, src_index, dst_index);
        while ((src_var = lookup_live_variable(live_variables, &lookup_index, src)) != NULL) {
          found_any = true;
          // add_predecessor(&src->predecessor, &src_var->src.opcode_id, src_var->src.operand_index);
          if (add_predecessor(&dst->predecessor, &src_var->src.opcode_id,
                              src_var->src.operand_index)) {
            fprintf(opcode_dump_file, "Link variable '%s': 0x%x:%d.%c -%d-> 0x%x:%d.%c\n",
                    var_name, src_var->src.opcode_id.routine_hash,
                    src_var->src.opcode_id.op_index,
                    get_operand_index_code(src_var->src.operand_index),
                    src_index, op->id.routine_hash, op->id.op_index,
                    get_operand_index_code(dst_index));
          }
        }
        if (!found_any)
          fprintf(opcode_dump_file, "Can't find var '%s'\n", var_name);
        break;
    }
  }
}
*/

static inline znode_op *get_operand(zend_op *op, taint_operand_index_t index)
{
  switch (index) {
    case TAINT_OPERAND_RESULT:
      return &op->result;
    case TAINT_OPERAND_1:
      return &op->op1;
    case TAINT_OPERAND_2:
      return &op->op2;
    default:
      return NULL; /* error */
  }
}

static inline zend_uchar get_operand_type(zend_op *op, taint_operand_index_t index)
{
  switch (index) {
    case TAINT_OPERAND_RESULT:
      return op->result_type;
    case TAINT_OPERAND_1:
      return op->op1_type;
    case TAINT_OPERAND_2:
      return op->op2_type;
    default:
      return -1;
  }
}

static const char *get_operand_index_name(taint_operand_index_t index)
{
  switch (index) {
    case TAINT_OPERAND_RESULT:
      return "result/value";
    case TAINT_OPERAND_1:
      return "1/map";
    case TAINT_OPERAND_2:
      return "2/key";
    default:
      return "<unknown>";
  }
}

static inline const zval *get_operand_zval(zend_execute_data *execute_data, zend_op *op,
                                    taint_operand_index_t index)
{
  return get_zval(execute_data, get_operand(op, index), get_operand_type(op, index));
}

static bool propagate_zval_taint(application_t *app, zend_execute_data *execute_data,
                                 zend_op_array *stack_frame, zend_op *op,
                                 const zval *src, const zval *dst)
{
  taint_variable_t *taint_var = taint_var_get(src);
  if (taint_var == NULL) {
    return false;
  } else {
    taint_variable_t *propagation = create_taint_variable(dst, stack_frame, op,
                                                          taint_var->type, taint_var->taint);
    taint_var_add(app, propagation);
    return true;
  }
}

static void propagate_operand_taint(application_t *app, zend_execute_data *execute_data,
                                    zend_op_array *stack_frame, zend_op *op,
                                    taint_operand_index_t src, taint_operand_index_t dst)
{
  if (propagate_zval_taint(app, execute_data, stack_frame, op,
                           get_operand_zval(execute_data, op, src),
                           get_operand_zval(execute_data, op, dst))) {
    SPOT("Propagate taint to %s at %s:%d(%d)\n", get_operand_index_name(dst),
         stack_frame->filename->val, op->lineno, (uint) (op - (zend_op *) stack_frame->opcodes));
  }
}

void propagate_taint(application_t *app, zend_execute_data *execute_data,
                     zend_op_array *stack_frame, zend_op *op)
{
  switch (op->opcode) {
    case ZEND_ASSIGN:
      propagate_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_1);
      break;
    case ZEND_FETCH_DIM_R: {
      zval *map = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_MAP);
      zval *key = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_KEY);
      const zval *dst = get_operand_zval(execute_data, op, TAINT_OPERAND_RESULT);
      zval *src;

      // if object map: Z_OBJ_HT_P(map)->read_dimension(map, key, IS_CONST, &dst_indirect);
      if (Z_TYPE_P(map) == IS_ARRAY) {
        zend_string *key_string = Z_STR_P(key); // check type though...
        src = zend_hash_find(Z_ARRVAL_P(map), key_string);
        propagate_zval_taint(app, execute_data, stack_frame, op, src, dst);
      }
    }
    case ZEND_ADD: /* FT */
    case ZEND_SUB:
    case ZEND_MUL:
    case ZEND_DIV:
    case ZEND_MOD:
    case ZEND_POW:
    case ZEND_SL:
    case ZEND_SR:
    case ZEND_BW_OR:
    case ZEND_BW_AND:
    case ZEND_BW_XOR:
    case ZEND_BW_NOT:
    case ZEND_CONCAT:
    case ZEND_BOOL_XOR:
    case ZEND_INSTANCEOF:
    case ZEND_IS_IDENTICAL:
    case ZEND_IS_NOT_IDENTICAL:
    case ZEND_IS_EQUAL:
    case ZEND_IS_NOT_EQUAL:
    case ZEND_IS_SMALLER:
    case ZEND_IS_SMALLER_OR_EQUAL:
    case ZEND_CASE:
    case ZEND_FETCH_CONSTANT:
      propagate_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_RESULT);
    case ZEND_TYPE_CHECK: /* FT */
    case ZEND_DEFINED:
    case ZEND_CAST:
    case ZEND_QM_ASSIGN: /* ? */
    case ZEND_STRLEN:
    case ZEND_FE_RESET:
    case ZEND_BOOL:
    case ZEND_BOOL_NOT:
    case ZEND_CLONE: // TODO: propagate into clone function (if any)
      propagate_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      break;
    case ZEND_ADD_VAR:
    case ZEND_ADD_STRING:
      propagate_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_RESULT);
      break;
    case ZEND_FETCH_DIM_W: // like fetch dim r?
    case ZEND_FETCH_DIM_RW:
    case ZEND_FETCH_DIM_IS:
    case ZEND_FETCH_OBJ_R:
    case ZEND_FETCH_OBJ_W:
    case ZEND_FETCH_OBJ_RW:
    case ZEND_FETCH_OBJ_IS:
      break;
    case ZEND_FETCH_DIM_UNSET: /* also {} => {1.2}, which is not modelled yet */
    case ZEND_FETCH_OBJ_UNSET:
      break;
    case ZEND_ASSIGN_REF:
      propagate_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_1);
      propagate_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_RESULT);
      break;
    case ZEND_ASSIGN_OBJ:
    case ZEND_ASSIGN_DIM: {
      zval *map = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_MAP);
      zval *key = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_KEY);
      const zval *src = get_operand_zval(execute_data, op+1, TAINT_OPERAND_1);
      zval *dst;

      // if object map: Z_OBJ_HT_P(map)->read_dimension(map, key, IS_CONST, &dst_indirect);
      if (Z_TYPE_P(map) == IS_ARRAY) {
        zend_string *key_string = Z_STR_P(key); // check type though...
        SEPARATE_ARRAY(map);
        dst = zend_hash_find(Z_ARRVAL_P(map), key_string);
        propagate_zval_taint(app, execute_data, stack_frame, op, src, dst);
      }
    } break;
    case ZEND_ADD_CHAR:
      propagate_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_RESULT);
      propagate_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_RESULT, TAINT_OPERAND_RESULT);
      break;
    case ZEND_ADD_ARRAY_ELEMENT:
      // if (op->op2_type != IS_UNUSED) /* when modelled, use the key in this case */
      propagate_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      break;
    case ZEND_FE_FETCH:
      propagate_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_MAP, TAINT_OPERAND_VALUE);
      propagate_zval_taint(app, execute_data, stack_frame, op,
                           get_operand_zval(execute_data, op, TAINT_OPERAND_MAP),
                           get_operand_zval(execute_data, op+1, TAINT_OPERAND_VALUE));
      break;
    case ZEND_PRE_INC:
    case ZEND_PRE_DEC:
    case ZEND_POST_INC:
    case ZEND_POST_DEC:
      propagate_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_1);
      propagate_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      break;
    case ZEND_FETCH_UNSET:
      // TODO: remove taint from map
      break;
    case ZEND_UNSET_VAR:
    case ZEND_UNSET_DIM: /* FT */
    case ZEND_UNSET_OBJ:
      // TODO: remove taint
      break;
    case ZEND_ISSET_ISEMPTY_VAR:
    case ZEND_ISSET_ISEMPTY_DIM_OBJ:
    case ZEND_ISSET_ISEMPTY_PROP_OBJ:
    case ZEND_FETCH_R:  /* fetch a superglobal */
    case ZEND_FETCH_W:
    case ZEND_FETCH_RW:
    case ZEND_FETCH_IS:
      propagate_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      propagate_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_RESULT);
      break;
    case ZEND_RECV:
    case ZEND_RECV_VARIADIC:
      // TODO: arg receiver
      break;
    case ZEND_PRINT:
      propagate_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      break;
    case ZEND_DECLARE_FUNCTION:
      // TODO: taint the function?
      break;
    /****************** branches *****************/
    case ZEND_RETURN:
    case ZEND_RETURN_BY_REF:
      // TODO: return
      break;
    case ZEND_THROW: /* {1} =d=> {fast-ret,thrown} */
      break;
    case ZEND_HANDLE_EXCEPTION: /* {thrown} =i=> {opline} */
      break;
    case ZEND_DISCARD_EXCEPTION: /* {thrown} =d=> {thrown} */
      break;
    case ZEND_FAST_CALL: /* {thrown} =d=> {fast-ret,opline} */
      break;
    case ZEND_FAST_RET: /* {fast-ret} =i=> {opline} */
      break;
    case ZEND_JMP_SET: /* nop if op1 is false, but can't see from here */
      propagate_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      break;
    case ZEND_COALESCE: /* nop if op1 is null, but can't see from here */
      propagate_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      break;
    /****************** internal ***************/
    case ZEND_FREE: {
      const zval *value = get_zval(execute_data, &op->op1, op->op1_type);
      SPOT("Free zval 0x%llx and remove any taint\n", (uint64) value);
      taint_variable_t *taint_var = taint_var_remove(value);
      if (taint_var != NULL)
        destroy_taint_variable(taint_var);
    } break;
  }

  {
    taint_variable_t *taint_var;
    const zval *operand = get_zval(execute_data, &op->op1, op->op1_type);
    if (operand != NULL) {
      taint_var = taint_var_get(operand);
      if (taint_var != NULL) {
        SPOT("<taint> propagated to 1/map of %s:%d(%d)\n", stack_frame->filename->val, op->lineno,
             (uint) (op - (zend_op *) stack_frame->opcodes));
      }
    }
    operand = get_zval(execute_data, &op->op2, op->op2_type);
    if (operand != NULL) {
      taint_var = taint_var_get(operand);
      if (taint_var != NULL) {
        SPOT("<taint> propagated to 2/key of %s:%d(%d)\n", stack_frame->filename->val, op->lineno,
             (uint) (op - (zend_op *) stack_frame->opcodes));
      }
    }
    operand = get_zval(execute_data, &op->result, op->result_type);
    if (operand != NULL) {
      taint_var = taint_var_get(operand);
      if (taint_var != NULL) {
        SPOT("<taint> propagated to result/value of %s:%d(%d)\n", stack_frame->filename->val,
             op->lineno, (uint) (op - (zend_op *) stack_frame->opcodes));
      }
    }
  }
}

