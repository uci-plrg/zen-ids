#include "lib/script_cfi_utils.h"
#include "ext/standard/php_array.h"
#include "cfg_handler.h"
#include "taint.h"

#define OP_LINE(stack_frame, op) ((uint) ((op) - (zend_op *) (stack_frame)->opcodes))

static sctable_t taint_table;

static taint_variable_t *pending_return_taint = NULL;

typedef struct _taint_call_t {
  taint_variable_t *taint_send[0x10];
  uint arg_count;
} taint_call_t;

void init_taint_tracker()
{
  taint_table.hash_bits = 6;
  sctable_init(&taint_table);
}

void destroy_taint_tracker()
{
  sctable_destroy(&taint_table);
}

taint_variable_t *create_taint_variable(const char *file_path, const zend_op *tainted_at,
                                        taint_type_t type, void *taint)
{
  taint_variable_t *var = malloc(sizeof(taint_variable_t));
  // var->value = value;
  var->tainted_at_file = strdup(file_path);
  var->tainted_at = tainted_at;
  var->type = type;
  var->taint = taint;
  return var;
}

void destroy_taint_variable(taint_variable_t *taint_var)
{
  // pool them instead
  //free((char *) taint_var->tainted_at_file);
  //free(taint_var);
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

void taint_var_add(application_t *app, const zval *taintee, taint_variable_t *taint)
{
  uint64 hash = (uint64) taintee;

  plog_taint_var(app, taint);

  if (sctable_lookup(&taint_table, hash) != NULL)
    destroy_taint_variable(taint);
  else
    sctable_add(&taint_table, hash, taint);
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

static const char *get_operand_index_name(zend_op *op, taint_operand_index_t index)
{
  switch (op->opcode) {
    case ZEND_FETCH_DIM_R:
    case ZEND_FETCH_DIM_W:
    case ZEND_FETCH_DIM_RW:
    case ZEND_FETCH_DIM_IS:
    case ZEND_FETCH_OBJ_R:
    case ZEND_FETCH_OBJ_W:
    case ZEND_FETCH_OBJ_RW:
    case ZEND_FETCH_OBJ_IS:
      switch (index) {
        case TAINT_OPERAND_RESULT:
          return "R";
        case TAINT_OPERAND_1:
          return "M";
        case TAINT_OPERAND_2:
          return "K";
        default:
          return "<unknown>";
      }
    case ZEND_ISSET_ISEMPTY_DIM_OBJ:
    case ZEND_ISSET_ISEMPTY_PROP_OBJ:
    case ZEND_FETCH_DIM_UNSET:
    case ZEND_FETCH_OBJ_UNSET:
    case ZEND_ASSIGN_OBJ:
    case ZEND_ASSIGN_DIM:
    case ZEND_FE_RESET:
    case ZEND_FE_FETCH:
      switch (index) {
        case TAINT_OPERAND_RESULT:
          return "V";
        case TAINT_OPERAND_1:
          return "M";
        case TAINT_OPERAND_2:
          return "K";
        default:
          return "<unknown>";
      }
    default:
      switch (index) {
        case TAINT_OPERAND_RESULT:
          return "R";
        case TAINT_OPERAND_1:
          return "1";
        case TAINT_OPERAND_2:
          return "2";
        default:
          return "<unknown>";
      }
  }
}

static inline const zval *get_operand_zval(zend_execute_data *execute_data, zend_op *op,
                                           taint_operand_index_t index)
{
  return get_zval(execute_data, get_operand(op, index), get_operand_type(op, index));
}

static bool propagate_zval_taint(application_t *app, zend_execute_data *execute_data,
                                 zend_op_array *stack_frame, zend_op *op, bool clobber,
                                 const zval *src, const zval *dst, const char *dst_name)
{
  taint_variable_t *taint_var = taint_var_get(src);

  if (clobber) {
    sctable_remove(&taint_table, (uint64) dst);

    plog(app, "<taint> remove %s at %04d(L%04d)%s\n", dst_name, OP_LINE(stack_frame, op),
         op->lineno, site_relative_path(app, stack_frame));
  }

  if (taint_var == NULL) {
    return false;
  } else {
    //taint_variable_t *propagation = create_taint_variable(dst, stack_frame, op,
    //                                                      taint_var->type, taint_var->taint);
    plog(app, "<taint> write %s at %04d(L%04d)%s\n",
         dst_name, OP_LINE(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));
    taint_var_add(app, dst, taint_var); // propagation); /* why make a new one? could just attach the existing one */
    return true;
  }
}

static void propagate_operand_taint(application_t *app, zend_execute_data *execute_data,
                                    zend_op_array *stack_frame, zend_op *op, bool clobber,
                                    taint_operand_index_t src, taint_operand_index_t dst)
{
  propagate_zval_taint(app, execute_data, stack_frame, op, clobber,
                       get_operand_zval(execute_data, op, src),
                       get_operand_zval(execute_data, op, dst), get_operand_index_name(op, dst));
}

static void clobber_operand_taint(application_t *app, zend_execute_data *execute_data,
                                  zend_op_array *stack_frame, zend_op *op,
                                  taint_operand_index_t src, taint_operand_index_t dst)
{
  propagate_operand_taint(app, execute_data, stack_frame, op, true, src, dst);
}

static void merge_operand_taint(application_t *app, zend_execute_data *execute_data,
                                zend_op_array *stack_frame, zend_op *op,
                                taint_operand_index_t src, taint_operand_index_t dst)
{
  propagate_operand_taint(app, execute_data, stack_frame, op, false, src, dst);
}

static bool is_derived_class(zend_class_entry *child_class, zend_class_entry *parent_class)
{
  child_class = child_class->parent;
  while (child_class) {
    if (child_class == parent_class) {
      return true;
    }
    child_class = child_class->parent;
  }

  return false;
}

void propagate_taint_from_array(application_t *app, zend_execute_data *execute_data,
                                zend_op_array *stack_frame, zend_op *op)
{
  zval *map = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_MAP);
  zval *key = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_KEY);
  const zval *dst = get_operand_zval(execute_data, op, TAINT_OPERAND_RESULT);
  zval *src;

  Z_UNWRAP_P(map);
  if (Z_TYPE_P(map) == IS_ARRAY) {
    if (Z_TYPE_P(key) == IS_LONG) {
      zend_ulong key_long = Z_LVAL_P(key);
      src = zend_hash_index_find(Z_ARRVAL_P(map), key_long);
      propagate_zval_taint(app, execute_data, stack_frame, op, true, src, dst, "R");
    } else if (Z_TYPE_P(key) == IS_STRING) {
      zend_string *key_string = Z_STR_P(key);
      src = zend_hash_find(Z_ARRVAL_P(map), key_string);
      propagate_zval_taint(app, execute_data, stack_frame, op, true, src, dst, "R");
    }
  } else {
    plog(app, "<taint> Error in %s: found an unknown array type %d at %04d(L%04d)%s\n",
         zend_get_opcode_name(op->opcode), Z_TYPE_P(map),
         OP_LINE(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));
  }

  merge_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_MAP, TAINT_OPERAND_RESULT);
}

void propagate_taint_into_array(application_t *app, zend_execute_data *execute_data,
                                zend_op_array *stack_frame, zend_op *op)
{
  zval *map = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_MAP);
  const zval *src = get_operand_zval(execute_data, op+1, TAINT_OPERAND_1);
  zval *dst;

  Z_UNWRAP_P(map);
  if (Z_TYPE_P(map) == IS_ARRAY) {
    zval *key, temp_key;

    if (op->op2_type == IS_UNUSED) { /* it's an append, so point key at the appended element */
      zend_long count = php_count_recursive(map, COUNT_NORMAL);

      if (count <= 0) {
        plog(app, "<taint> Error in %s: php array is empty after append at %04d(L%04d)%s\n",
         zend_get_opcode_name(op->opcode),
         OP_LINE(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));
      } else {
        ZVAL_LONG(&temp_key, count-1);
        key = &temp_key;
      }
    } else {
      key = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_KEY);
    }

    if (Z_TYPE_P(key) == IS_LONG) {
      zend_ulong key_long = Z_LVAL_P(key);
      SEPARATE_ARRAY(map);
      dst = zend_hash_index_find(Z_ARRVAL_P(map), key_long);
      propagate_zval_taint(app, execute_data, stack_frame, op, true, src, dst, "A[i]");
    } else if (Z_TYPE_P(key) == IS_STRING) {
      zend_string *key_string = Z_STR_P(key);
      SEPARATE_ARRAY(map);
      dst = zend_hash_find(Z_ARRVAL_P(map), key_string);
      propagate_zval_taint(app, execute_data, stack_frame, op, true, src, dst, "A[i]");
    }
  } else {
    plog(app, "<taint> Error in %s: found an unknown array type %d at %04d(L%04d)%s\n",
         zend_get_opcode_name(op->opcode), Z_TYPE_P(map),
         OP_LINE(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));
  }
}

void propagate_taint_from_object(application_t *app, zend_execute_data *execute_data,
                                 zend_op_array *stack_frame, zend_op *op)
{
  const zval *map, *key = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_KEY);
  const zval *src, *dst;

  if (op->op1_type == IS_UNUSED) {
    map = &execute_data->This;
    dst = get_operand_zval(execute_data, op, TAINT_OPERAND_RESULT);
  } else {
    map = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_MAP);
    dst = get_operand_zval(execute_data, op, TAINT_OPERAND_1);
  }

  Z_UNWRAP_P(map);
  if (Z_TYPE_P(map) == IS_OBJECT) {
    if (Z_TYPE_P(key) == IS_LONG) { /* is this possible for an object? */
      zend_ulong key_long = Z_LVAL_P(key);
      zend_object *object = Z_OBJ_P(map);
      void **cache_slot = execute_data->run_time_cache + Z_CACHE_SLOT_P(key);
      zend_property_info *p = NULL;

      if (cache_slot != NULL && object->ce == CACHED_PTR_EX(cache_slot)) {
        p = CACHED_PTR_EX(cache_slot + 1);
      } else {
        zval *pp = zend_hash_index_find(&object->ce->properties_info, key_long);
        if (pp != NULL)
          p = (zend_property_info *) Z_PTR_P(pp);
      }

      if (p == NULL && EG(scope) != object->ce && EG(scope) &&
          is_derived_class(object->ce, EG(scope))) {
        zval *pp = zend_hash_index_find(&EG(scope)->properties_info, key_long);
        if (pp != NULL && ((zend_property_info*) Z_PTR_P(pp))->flags & ZEND_ACC_PRIVATE)
          p = (zend_property_info *) Z_PTR_P(pp);
      }

      if (p == NULL && cache_slot != NULL)
        CACHE_POLYMORPHIC_PTR_EX(cache_slot, object->ce, NULL);

      if (p != NULL) {
        src = OBJ_PROP(object, p->offset);
        if (Z_TYPE_P(src) == IS_UNDEF)
          src = NULL;
      } else if (object->properties != NULL) {
        src = zend_hash_index_find(object->properties, key_long);
      } else if (object->ce->__get != NULL) {
        plog(app, "<taint> Can't find property #%d, but have a magic getter.\n", key_long);
      }

      if (src != NULL)
        propagate_zval_taint(app, execute_data, stack_frame, op, true, src, src, "P");
      else
        plog(app, "<taint> Can't find property #%d\n", key_long);
    } else if (Z_TYPE_P(key) == IS_STRING) {
      zend_string *key_string = Z_STR_P(key);
      zend_object *object = Z_OBJ_P(map);
      void **cache_slot = execute_data->run_time_cache + Z_CACHE_SLOT_P(key);
      zend_property_info *p = NULL;

      if (cache_slot != NULL && object->ce == CACHED_PTR_EX(cache_slot)) {
        p = CACHED_PTR_EX(cache_slot + 1);
      } else {
        zval *pp = zend_hash_find(&object->ce->properties_info, key_string);
        if (pp != NULL)
          p = (zend_property_info *) Z_PTR_P(pp);
      }

      if (p == NULL && EG(scope) != object->ce && EG(scope) &&
          is_derived_class(object->ce, EG(scope))) {
        zval *pp = zend_hash_find(&EG(scope)->properties_info, key_string);
        if (pp != NULL && ((zend_property_info*) Z_PTR_P(pp))->flags & ZEND_ACC_PRIVATE)
          p = (zend_property_info *) Z_PTR_P(pp);
      }

      if (p == NULL && cache_slot != NULL)
        CACHE_POLYMORPHIC_PTR_EX(cache_slot, object->ce, NULL);

      if (p != NULL) {
        src = OBJ_PROP(object, p->offset);
        if (Z_TYPE_P(src) == IS_UNDEF)
          src = NULL;
      } else if (object->properties != NULL) {
        src = zend_hash_find(object->properties, key_string);
      } else if (object->ce->__get != NULL) {
        plog(app, "<taint> Can't find property %s, but have a magic getter.\n", Z_STRVAL_P(key));
      }

      if (src != NULL)
        propagate_zval_taint(app, execute_data, stack_frame, op, true, src, dst, "R");
      else
        plog(app, "<taint> Can't find property %s\n", Z_STRVAL_P(key));
    }
  } else {
    plog(app, "<taint> Error in %s: found an unknown object type %d at %04d(L%04d)%s\n",
         zend_get_opcode_name(op->opcode), Z_TYPE_P(map),
         OP_LINE(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));
  }

  merge_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_MAP, TAINT_OPERAND_RESULT);
}

void propagate_taint_into_object(application_t *app, zend_execute_data *execute_data,
                                 zend_op_array *stack_frame, zend_op *op)
{
  zval *map, *key = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_KEY);
  const zval *src = get_operand_zval(execute_data, op+1, TAINT_OPERAND_1);
  zval *dst;

  if (op->op1_type == IS_UNUSED)
    map = &execute_data->This;
  else
    map = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_MAP);

  Z_UNWRAP_P(map);
  if (Z_TYPE_P(map) == IS_OBJECT) {
    if (Z_TYPE_P(key) == IS_LONG) { /* is this possible for an object? */
      zend_ulong key_long = Z_LVAL_P(key);
      zend_object *object = Z_OBJ_P(map);
      void **cache_slot = execute_data->run_time_cache + Z_CACHE_SLOT_P(key);
      zend_property_info *p = NULL;

      if (cache_slot != NULL && object->ce == CACHED_PTR_EX(cache_slot)) {
        p = CACHED_PTR_EX(cache_slot + 1);
      } else {
        zval *pp = zend_hash_index_find(&object->ce->properties_info, key_long);
        if (pp != NULL)
          p = (zend_property_info *) Z_PTR_P(pp);
      }

      if (p == NULL && EG(scope) != object->ce && EG(scope) &&
          is_derived_class(object->ce, EG(scope))) {
        zval *pp = zend_hash_index_find(&EG(scope)->properties_info, key_long);
        if (pp != NULL && ((zend_property_info*) Z_PTR_P(pp))->flags & ZEND_ACC_PRIVATE)
          p = (zend_property_info *) Z_PTR_P(pp);
      }

      if (p == NULL && cache_slot != NULL)
        CACHE_POLYMORPHIC_PTR_EX(cache_slot, object->ce, NULL);

      if (p != NULL) {
        dst = OBJ_PROP(object, p->offset);
        if (Z_TYPE_P(dst) == IS_UNDEF)
          dst = NULL;
      } else if (object->properties != NULL) {
        dst = zend_hash_index_find(object->properties, key_long);
      } else if (object->ce->__get != NULL) {
        plog(app, "<taint> Can't find property #%d, but have a magic getter.\n", key_long);
      }

      if (dst != NULL)
        propagate_zval_taint(app, execute_data, stack_frame, op, true, src, dst, "A[i]");
      else
        plog(app, "<taint> Can't find property #%d\n", key_long);
    } else if (Z_TYPE_P(key) == IS_STRING) {
      zend_string *key_string = Z_STR_P(key);
      zend_object *object = Z_OBJ_P(map);
      void **cache_slot = execute_data->run_time_cache + Z_CACHE_SLOT_P(key);
      zend_property_info *p = NULL;

      if (cache_slot != NULL && object->ce == CACHED_PTR_EX(cache_slot)) {
        p = CACHED_PTR_EX(cache_slot + 1);
      } else {
        zval *pp = zend_hash_find(&object->ce->properties_info, key_string);
        if (pp != NULL)
          p = (zend_property_info *) Z_PTR_P(pp);
      }

      if (p == NULL && EG(scope) != object->ce && EG(scope) &&
          is_derived_class(object->ce, EG(scope))) {
        zval *pp = zend_hash_find(&EG(scope)->properties_info, key_string);
        if (pp != NULL && ((zend_property_info*) Z_PTR_P(pp))->flags & ZEND_ACC_PRIVATE)
          p = (zend_property_info *) Z_PTR_P(pp);
      }

      if (p == NULL && cache_slot != NULL)
        CACHE_POLYMORPHIC_PTR_EX(cache_slot, object->ce, NULL);

      if (p != NULL) {
        dst = OBJ_PROP(object, p->offset);
        if (Z_TYPE_P(dst) == IS_UNDEF)
          dst = NULL;
      } else if (object->properties != NULL) {
        dst = zend_hash_find(object->properties, key_string);
      } else if (object->ce->__get != NULL) {
        plog(app, "<taint> Can't find property %s, but have a magic getter.\n", Z_STRVAL_P(key));
      }

      if (dst != NULL)
        propagate_zval_taint(app, execute_data, stack_frame, op, true, src, dst, "A[i]");
      else
        plog(app, "<taint> Can't find property %s\n", Z_STRVAL_P(key));
    }
  } else {
    plog(app, "<taint> Error in %s: found an unknown object type %d at %04d(L%04d)%s\n",
         zend_get_opcode_name(op->opcode), Z_TYPE_P(map),
         OP_LINE(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));
  }
}

void propagate_taint(application_t *app, zend_execute_data *execute_data,
                     zend_op_array *stack_frame, zend_op *op)
{
  switch (op->opcode) {
    case ZEND_ASSIGN:
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_1);
      break;
    case ZEND_ADD:
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
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_RESULT);
    case ZEND_TYPE_CHECK: /* FT */
    case ZEND_DEFINED:
    case ZEND_CAST:
    case ZEND_QM_ASSIGN: /* ? */
    case ZEND_STRLEN:
    case ZEND_FE_RESET:
    case ZEND_BOOL:
    case ZEND_BOOL_NOT:
    case ZEND_CLONE: // TODO: propagate into clone function (if any)
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      break;
    case ZEND_ADD_VAR:
    case ZEND_ADD_STRING:
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_RESULT);
      break;
    case ZEND_FETCH_DIM_UNSET: /* also {} => {1.2}, which is not modelled yet */
    case ZEND_FETCH_OBJ_UNSET:
      break;
    case ZEND_ASSIGN_REF:
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_1);
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_RESULT);
      break;
    case ZEND_ADD_CHAR:
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_RESULT);
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_RESULT, TAINT_OPERAND_RESULT);
      break;
    case ZEND_ADD_ARRAY_ELEMENT:
      // if (op->op2_type != IS_UNUSED) /* when modelled, use the key in this case */
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      break;
    case ZEND_FE_FETCH:
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_MAP, TAINT_OPERAND_VALUE);
      propagate_zval_taint(app, execute_data, stack_frame, op, true,
                           get_operand_zval(execute_data, op, TAINT_OPERAND_MAP),
                           get_operand_zval(execute_data, op+1, TAINT_OPERAND_VALUE), "iter");
      break;
    case ZEND_PRE_INC:
    case ZEND_PRE_DEC:
    case ZEND_POST_INC:
    case ZEND_POST_DEC:
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_1);
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      break;
    case ZEND_PRINT:
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      break;
    case ZEND_DECLARE_FUNCTION:
      // TODO: taint the function?
      break;
    case ZEND_FETCH_R:  /* fetch a superglobal */
    case ZEND_FETCH_W:
    case ZEND_FETCH_RW:
    case ZEND_FETCH_IS:
      merge_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      // clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_RESULT);
      break;
    /****************** arrays and objects *****************/
    case ZEND_FETCH_DIM_R:
    case ZEND_FETCH_DIM_W:
    case ZEND_FETCH_DIM_RW:
    case ZEND_FETCH_DIM_IS:
      propagate_taint_from_array(app, execute_data, stack_frame, op);
      break;
    case ZEND_ASSIGN_DIM:
      propagate_taint_into_array(app, execute_data, stack_frame, op);
      break;
    case ZEND_FETCH_OBJ_R:
    case ZEND_FETCH_OBJ_W:
    case ZEND_FETCH_OBJ_RW:
    case ZEND_FETCH_OBJ_IS:
      propagate_taint_from_object(app, execute_data, stack_frame, op);
      break;
    case ZEND_ASSIGN_OBJ:
      propagate_taint_into_object(app, execute_data, stack_frame, op);
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
      merge_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      // clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_RESULT);
      break;
    /****************** calls *****************/
    case ZEND_DO_FCALL:
      // pop args
      break;
    /****************** branches *****************/
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
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      break;
    case ZEND_COALESCE: /* nop if op1 is null, but can't see from here */
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      break;
    /****************** internal ***************/
    case ZEND_FREE: {
      const zval *value = get_zval(execute_data, &op->op1, op->op1_type);
      plog(app, "Free zval 0x%llx and remove any taint\n", (uint64) value);
      taint_variable_t *taint_var = taint_var_remove(value);
      if (taint_var != NULL)
        destroy_taint_variable(taint_var);
    } break;
  }

  if (op < &stack_frame->opcodes[stack_frame->last]) {
    switch ((op+1)->opcode) {
      case ZEND_RETURN:
      case ZEND_RETURN_REF: {
        zend_op *return_op = (op+1);
        const zval *return_value = get_operand_zval(execute_data, return_op, TAINT_OPERAND_1);
        if (return_value != NULL) {
          pending_return_taint = taint_var_get(return_value);
          if (pending_return_taint != NULL) {
            plog(app, "<taint> return 1 of %04d(L%04d)%s\n",
                 OP_LINE(stack_frame, return_op), return_op->lineno, site_relative_path(app, stack_frame));
          }
        }
      }
    }
  }

  {
    taint_variable_t *taint_var;
    const zval *operand = get_zval(execute_data, &op->op1, op->op1_type);
    if (operand != NULL) {
      taint_var = taint_var_get(operand);
      if (taint_var != NULL) {
        plog(app, "<taint> on %s of %04d(L%04d)%s\n", get_operand_index_name(op, TAINT_OPERAND_1),
             OP_LINE(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));
      }
    }
    operand = get_zval(execute_data, &op->op2, op->op2_type);
    if (operand != NULL) {
      taint_var = taint_var_get(operand);
      if (taint_var != NULL) {
        plog(app, "<taint> on %s of %04d(L%04d)%s\n", get_operand_index_name(op, TAINT_OPERAND_2),
             OP_LINE(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));
      }
    }
    operand = get_zval(execute_data, &op->result, op->result_type);
    if (operand != NULL) {
      taint_var = taint_var_get(operand);
      if (taint_var != NULL) {
        plog(app, "<taint> on %s of %04d(L%04d)%s\n", get_operand_index_name(op, TAINT_OPERAND_RESULT),
             OP_LINE(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));
      }
    }
  }
}

void taint_prepare_call(zend_execute_data *execute_data, zend_op **args, uint arg_count)
{
  zend_op_array *stack_frame = &EX(call)->func->op_array;
  taint_call_t *call = NULL;
  uint i;

  for (i = 0; i < arg_count; i++) {
    const zval *send_val = get_operand_zval(execute_data, args[i], TAINT_OPERAND_1);
    taint_variable_t *taint_send = taint_var_get(send_val);
    if (taint_send != NULL) {
      if (call == NULL) {
        call = malloc(sizeof(taint_call_t));
        memset(call, 0, sizeof(taint_call_t));
        call->arg_count = arg_count;
        sctable_add(&taint_table, (uint64) stack_frame, call);
      }
      call->taint_send[i] = taint_send;
    }
  }
}

void taint_proagate_into_arg_receivers(application_t *app, zend_execute_data *execute_data,
                                       zend_op_array *stack_frame, zend_op *op)
{
  uint64 hash = (uint64) stack_frame;
  taint_call_t *call = (taint_call_t *) sctable_lookup(&taint_table, hash);

  plog(app, "<taint> looking for call at %04d(L%04d)%s\n",
       OP_LINE(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));

  if (call != NULL) {
    uint i;

    plog(app, "<taint> found call at %04d(L%04d)%s\n",
         OP_LINE(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));

    for (i = 0; i < call->arg_count; i++) {
      if (call->taint_send[i] != NULL) {
        zend_op *receive = (op - (i + 1));
        if (receive->opcode == ZEND_RECV) {
          const zval *receive_val = get_operand_zval(execute_data, receive, TAINT_OPERAND_RESULT);
          taint_var_add(app, receive_val, call->taint_send[i]);
          plog(app, "<taint> write arg receive R at %04d(L%04d)%s\n",
               OP_LINE(stack_frame, receive), receive->lineno, site_relative_path(app, stack_frame));
        } else {
          plog(app, "<taint> Error! Found opcode 0x%x but ZEND_RECV was expected at %04d(L%04d)%s\n",
               OP_LINE(stack_frame, receive), receive->lineno, site_relative_path(app, stack_frame));
        }
      }
    }
    sctable_remove(&taint_table, hash);
    free(call);
  }
}

void taint_propagate_return(application_t *app, zend_execute_data *execute_data,
                            zend_op_array *stack_frame, zend_op *call_op)
{
  if (pending_return_taint != NULL) {
    const zval *call_result = get_operand_zval(execute_data, call_op, TAINT_OPERAND_RESULT);
    if (call_result != NULL) {
      taint_var_add(app, call_result, pending_return_taint);
      plog(app, "<taint> write call result R at %04d(L%04d)%s\n",
           OP_LINE(stack_frame, call_op), call_op->lineno, site_relative_path(app, stack_frame));
    }
    pending_return_taint = NULL;
  }
}
