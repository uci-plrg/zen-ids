#include "lib/script_cfi_utils.h"
#include "ext/standard/php_array.h"
#include "cfg_handler.h"
#include "taint.h"

/******************************************************************************************
 * Propagation
 */

static sctable_t taint_table;

#define MAX_TAINT_ARGS 0x10

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

typedef struct _taint_call_t {
  taint_variable_t *taint_send[MAX_TAINT_ARGS];
  uint arg_count;
} taint_call_t;

typedef enum _taint_mapping_flags_t {
  MAPPING_IS_OBJECT = 0x1,
  MAPPING_BY_REF = 0x2,
} taint_mapping_flags_t;

typedef union _taint_cache_slot_t {
  void **pointer;
  uint32_t offset;
} taint_cache_slot_t;

#define CACHE_SLOT(key, flags) \
  (TEST(MAPPING_BY_REF, (flags)) ? \
   (taint_cache_slot_t) (execute_data->run_time_cache + Z_CACHE_SLOT_P(key)) : \
   (taint_cache_slot_t) Z_CACHE_SLOT_P(key))

#define CACHE_SLOT_VALID(slot, ce, flags) \
  ((slot).pointer != NULL && (ce) == (TEST(MAPPING_BY_REF, (flags)) ? \
                                           CACHED_PTR_EX((slot).pointer + 1) : \
                                           CACHED_PTR((slot).offset + 1)))

#define CACHE_LOOKUP(slot, flags) \
  (TEST(MAPPING_BY_REF, (flags)) ? \
   CACHED_PTR_EX((slot).pointer + 1) : \
   CACHED_PTR((slot).offset + 1))

//void propagate_taint_from_object(application_t *app, zend_execute_data *execute_data,
//                                 zend_op_array *stack_frame, const zend_op *op, uint flags);

//void propagate_taint_into_object(application_t *app, zend_execute_data *execute_data,
//                                 zend_op_array *stack_frame, const zend_op *op, uint flags);

static inline const znode_op *get_operand(const zend_op *op, taint_operand_index_t index)
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

static inline const zend_uchar get_operand_type(const zend_op *op, taint_operand_index_t index)
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

static const char *get_operand_index_name(const zend_op *op, taint_operand_index_t index)
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
    case ZEND_FE_RESET_R:
    case ZEND_FE_FETCH_R:
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

static inline const zval *get_operand_zval(zend_execute_data *execute_data, const zend_op *op,
                                           taint_operand_index_t index)
{
  return get_zval(execute_data, get_operand(op, index), get_operand_type(op, index));
}

bool propagate_zval_taint(application_t *app, zend_execute_data *execute_data,
                          zend_op_array *stack_frame, const zend_op *op, bool clobber,
                          const zval *src, const char *src_name,
                          const zval *dst, const char *dst_name)
{
  taint_variable_t *taint_var = taint_var_get(src);

  if (src == NULL || dst == NULL)
    return false; // bug?

  if (clobber) {
    if (taint_var_remove(dst) != NULL) {
      plog(app, PLOG_TYPE_TAINT, "clobber %s (0x%llx) at %04d(L%04d)%s\n", dst_name, (uint64) dst,
           OP_INDEX(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));
    }
  }

  if (taint_var == NULL) {
    if ((op->opcode == ZEND_ASSIGN_DIM || op->opcode == ZEND_ASSIGN_OBJ) && TAINT_ALL) {
      plog(app, PLOG_TYPE_TAINT, "no taint on %s src (0x%llx)\n",
           zend_get_opcode_name(op->opcode), (uint64) src);
    }
    return false;
  } else {
    plog(app, PLOG_TYPE_TAINT, "write %s(0x%llx)->%s(0x%llx) at %04d(L%04d)%s\n",
         src_name, (uint64) src, dst_name, (uint64) dst, OP_INDEX(stack_frame, op), op->lineno,
         site_relative_path(app, stack_frame));
    taint_var_add(app, dst, taint_var);
    return true;
  }
}

bool propagate_zval_taint_quiet(application_t *app, bool clobber,
                                const zval *src, const char *src_name,
                                const zval *dst, const char *dst_name)
{
  taint_variable_t *taint_var = taint_var_get(src);

  if (src == NULL || dst == NULL)
    return false; // bug?

  if (clobber) {
    if (taint_var_remove(dst) != NULL)
      plog(app, PLOG_TYPE_TAINT, "clobber %s (0x%llx) at a builtin\n", dst_name, (uint64) dst);
  }

  if (taint_var == NULL) {
    return false;
  } else {
    plog(app, PLOG_TYPE_TAINT, "write %s(0x%llx)->%s(0x%llx) at a builtin\n",
         src_name, (uint64) src, dst_name, (uint64) dst);
    taint_var_add(app, dst, taint_var);
    return true;
  }
}

static void propagate_operand_taint(application_t *app, zend_execute_data *execute_data,
                                    zend_op_array *stack_frame, const zend_op *op, bool clobber,
                                    taint_operand_index_t src, taint_operand_index_t dst)
{
  propagate_zval_taint(app, execute_data, stack_frame, op, clobber,
                       get_operand_zval(execute_data, op, src), get_operand_index_name(op, src),
                       get_operand_zval(execute_data, op, dst), get_operand_index_name(op, dst));
}

static void clobber_operand_taint(application_t *app, zend_execute_data *execute_data,
                                  zend_op_array *stack_frame, const zend_op *op,
                                  taint_operand_index_t src, taint_operand_index_t dst)
{
  propagate_operand_taint(app, execute_data, stack_frame, op, true, src, dst);
}

static void merge_operand_taint(application_t *app, zend_execute_data *execute_data,
                                zend_op_array *stack_frame, const zend_op *op,
                                taint_operand_index_t src, taint_operand_index_t dst)
{
  propagate_operand_taint(app, execute_data, stack_frame, op, false, src, dst);
}

/*
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
*/

void propagate_taint_from_array(application_t *app, zend_execute_data *execute_data,
                                zend_op_array *stack_frame, const zend_op *op, uint flags)
{
  zval *map = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_MAP);
  zval *key = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_KEY);
  const zval *dst = get_operand_zval(execute_data, op, TAINT_OPERAND_RESULT);
  zend_ulong key_long;
  zval *src;

  if (key == NULL)
    return; // ignore--will handle this via internal callback

  if (true) {
    //plog(app, PLOG_TYPE_TAINT, "Skipping propagation from array at %04d(L%04d)%s\n",
    //      OP_INDEX(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));
    return;
  }

  key_long = Z_LVAL_P(key);
  Z_UNWRAP_P(map);
  switch (Z_TYPE_P(map)) {
    case IS_ARRAY:
      switch (Z_TYPE_P(key)) {
        case IS_STRING: {
          zend_string *key_string = Z_STR_P(key);
          if (op->op2_type == IS_CONST) {
            src = zend_hash_find(Z_ARRVAL_P(map), key_string);
            propagate_zval_taint(app, execute_data, stack_frame, op, true, src, "[mapped]", dst, "R");
            break;
          }
          ZEND_HANDLE_NUMERIC(key_string, key_long);
        }
        case IS_LONG: /* FT */
          src = zend_hash_index_find(Z_ARRVAL_P(map), key_long);
          propagate_zval_taint(app, execute_data, stack_frame, op, true, src, "[mapped]", dst, "R");
      }
      break;
    case IS_OBJECT:
      // propagate_taint_from_object(app, execute_data, stack_frame, op, flags);
      break;
    case IS_STRING:
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_MAP, TAINT_OPERAND_RESULT);
      break;
    case IS_NULL:
    case IS_FALSE:
      break; /* ignore */
    default:
      ERROR("<taint> Error propagating from %s: found an unknown array type %d at %04d(L%04d)%s\n",
            zend_get_opcode_name(op->opcode), Z_TYPE_P(map),
            OP_INDEX(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));
  }

  merge_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_MAP, TAINT_OPERAND_RESULT);
}

/*
void propagate_taint_into_array(application_t *app, zend_execute_data *execute_data,
                                zend_op_array *stack_frame, const zend_op *op, uint flags)
{
  zval *map = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_MAP);
  const zval *src = get_operand_zval(execute_data, op+1, TAINT_OPERAND_1);
  zval *dst;

  Z_UNWRAP_P(map);
  if (Z_TYPE_P(map) == IS_OBJECT) {
    flags |= MAPPING_IS_OBJECT;
    propagate_taint_into_object(app, execute_data, stack_frame, op, flags);
  } else if (Z_TYPE_P(map) == IS_STRING) {
    clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_MAP);
  } else {
    zval *key = NULL, temp_key;

    if (op->op2_type == IS_UNUSED) { / * it's an append, so point key at the appended element * /
      zend_long count = php_count_recursive(map, COUNT_NORMAL);

      if (count <= 0) {
        ERROR("<taint> Error in %s: php array is empty after append at %04d(L%04d)%s\n",
         zend_get_opcode_name(op->opcode),
         OP_INDEX(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));
      } else {
        ZVAL_LONG(&temp_key, count-1);
        key = &temp_key;
      }
    } else {
      key = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_KEY);
    }

    if (Z_TYPE_P(key) == IS_LONG) {
      zend_ulong key_long = Z_LVAL_P(key);
      dst = zend_hash_index_find(Z_ARRVAL_P(map), key_long);
      propagate_zval_taint(app, execute_data, stack_frame, op, true, src, "?", dst, "A[i]");
    } else if (Z_TYPE_P(key) == IS_STRING && (Z_ARRVAL_P(map)->u.flags & HASH_MASK_CONSISTENCY) == 0) {
      zend_string *key_string = Z_STR_P(key);
      dst = zend_hash_find(Z_ARRVAL_P(map), key_string);
      propagate_zval_taint(app, execute_data, stack_frame, op, true, src,
                           op->op2_type == IS_UNUSED ? "?" : "><",
                           dst, "A[i]");
    }
    ERROR("<taint> Error propagating into %s: found an unknown array type %d at %04d(L%04d)%s\n",
          zend_get_opcode_name(op->opcode), Z_TYPE_P(map),
          OP_INDEX(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));
  }
}
*/
/*
void propagate_taint_from_object(application_t *app, zend_execute_data *execute_data,
                                 zend_op_array *stack_frame, const zend_op *op, uint flags)
{
  const zval *map, *key = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_KEY);
  const zval *src = NULL, *dst = NULL;

  if (op->op1_type == IS_UNUSED) {
    map = &execute_data->This;
    dst = get_operand_zval(execute_data, op, TAINT_OPERAND_RESULT);
  } else {
    map = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_MAP);
    dst = get_operand_zval(execute_data, op, TAINT_OPERAND_1);
  }

  Z_UNWRAP_P(map);
  if (Z_TYPE_P(map) == IS_OBJECT) {
    //zval temp;
    zend_string *key_string = Z_STR_P(key);
    zend_object *object = Z_OBJ_P(map);
    taint_cache_slot_t cache_slot;
    zend_property_info *p = NULL;

    if (Z_TYPE_P(key) != IS_STRING) {
      key_string = zval_get_string((zval *) key);
      //ZVAL_STR(&temp, zval_get_string(key));
      //key = &temp;
      cache_slot.pointer = NULL;
    } else if (TEST(MAPPING_IS_OBJECT, flags) && op->op2_type == IS_CONST) {
      cache_slot = CACHE_SLOT(key, flags);
    } else {
      cache_slot.pointer = NULL;
    }

    if (CACHE_SLOT_VALID(cache_slot, object->ce, flags)) {
      p = CACHE_LOOKUP(cache_slot, flags);
    } else if (TEST(MAPPING_BY_REF, flags)) {
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

    if (p == NULL && cache_slot.pointer != NULL && TEST(MAPPING_BY_REF, flags))
      CACHE_POLYMORPHIC_PTR_EX(cache_slot.pointer, object->ce, NULL);

    if (p != NULL) {
      src = OBJ_PROP(object, p->offset);
      if (Z_TYPE_P(src) == IS_UNDEF)
        src = NULL;
    } else if (TEST(MAPPING_BY_REF, flags)) {
      if (object->properties != NULL) {
        src = zend_hash_find(object->properties, key_string);
      } else if (object->ce->__get != NULL) {
        ERROR("<taint> Can't find property %s, but have a magic getter.\n", Z_STRVAL_P(key));
      }
    }

    if (src != NULL)
      propagate_zval_taint(app, execute_data, stack_frame, op, true, src, "[property]", dst, "R");
    else
      WARN("<taint> Can't find property %s\n", Z_STRVAL_P(key));
  } else {
    WARN("<taint> Error in %s: found an unknown object type %d at %04d(L%04d)%s\n",
         zend_get_opcode_name(op->opcode), Z_TYPE_P(map),
         OP_INDEX(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));
  }

  merge_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_MAP, TAINT_OPERAND_RESULT);
}
*/
/*
void propagate_taint_into_object(application_t *app, zend_execute_data *execute_data,
                                 zend_op_array *stack_frame, const zend_op *op, uint flags)
{
  zval *map, *key = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_KEY);
  const zval *src = get_operand_zval(execute_data, op+1, TAINT_OPERAND_1);
  zval *dst = NULL;

  if (op->op1_type == IS_UNUSED)
    map = &execute_data->This;
  else
    map = (zval *) get_operand_zval(execute_data, op, TAINT_OPERAND_MAP);

  Z_UNWRAP_P(map);
  if (Z_TYPE_P(map) == IS_OBJECT) {
    zend_string *key_string = Z_STR_P(key);
    zend_object *object = Z_OBJ_P(map);
    taint_cache_slot_t cache_slot;
    zend_property_info *p = NULL;

    if (Z_TYPE_P(key) != IS_STRING) {
      key_string = zval_get_string((zval *) key);
      //ZVAL_STR(&temp, zval_get_string(key));
      //key = &temp;
      cache_slot.pointer = NULL;
    } else if (op->op2_type == IS_CONST) {
      cache_slot = CACHE_SLOT(key, flags);
    } else {
      cache_slot.pointer = NULL;
    }

    if (CACHE_SLOT_VALID(cache_slot, object->ce, flags)) {
      p = CACHE_LOOKUP(cache_slot, flags);
    } else if (TEST(MAPPING_BY_REF, flags)) {
      zval *pp = zend_hash_find(&object->ce->properties_info, key_string);
      if (pp != NULL)
        p = (zend_property_info *) Z_PTR_P(pp);
    }

    // may need to dereference for IS_INDIR?

    if (p == NULL && EG(scope) != object->ce && EG(scope) &&
        is_derived_class(object->ce, EG(scope))) {
      zval *pp = zend_hash_find(&EG(scope)->properties_info, key_string);
      if (pp != NULL && ((zend_property_info*) Z_PTR_P(pp))->flags & ZEND_ACC_PRIVATE)
        p = (zend_property_info *) Z_PTR_P(pp);
    }

    if (p == NULL && cache_slot.pointer != NULL && TEST(MAPPING_BY_REF, flags)) // verify
      CACHE_POLYMORPHIC_PTR_EX(cache_slot.pointer, object->ce, NULL);

    if (p != NULL) {
      dst = OBJ_PROP(object, p->offset);
      if (Z_TYPE_P(dst) == IS_UNDEF)
        dst = NULL;
    } else if (TEST(MAPPING_BY_REF, flags)) { // verify
      if (object->properties != NULL && TEST(MAPPING_BY_REF, flags)) {
        dst = zend_hash_find(object->properties, key_string);
      } else if (object->ce->__get != NULL) {
        plog(app, PLOG_TYPE_TAINT, "Can't find property %s, but have a magic getter.\n", Z_STRVAL_P(key));
      }
    }

    if (dst != NULL)
      propagate_zval_taint(app, execute_data, stack_frame, op, true, src, "?", dst, "A[i]");
    else
      plog(app, PLOG_TYPE_TAINT, "Can't find property %s\n", Z_STRVAL_P(key));
  } else {
    ERROR("<taint> Error in %s: found an unknown object type %d at %04d(L%04d)%s\n",
          zend_get_opcode_name(op->opcode), Z_TYPE_P(map),
          OP_INDEX(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));
  }
}
*/

static inline void plog_taint_propagation(application_t *app, zend_execute_data *execute_data,
                                          zend_op_array *stack_frame, const zend_op *op)
{
#ifdef PLOG_TAINT
  taint_variable_t *taint_var;
  const zval *operand = get_zval(execute_data, &op->op1, op->op1_type);
  if (operand != NULL) {
    taint_var = taint_var_get(operand);
    if (taint_var != NULL) {
      plog(app, PLOG_TYPE_TAINT, "on %s of %04d(L%04d)%s (0x%llx) | ",
           get_operand_index_name(op, TAINT_OPERAND_1), OP_INDEX(stack_frame, op),
           op->lineno, site_relative_path(app, stack_frame), (uint64) operand);
      plog_taint(app, taint_var);
      plog_append(app, PLOG_TYPE_TAINT, "\n");
    }
  }
  operand = get_zval(execute_data, &op->op2, op->op2_type);
  if (operand != NULL) {
    taint_var = taint_var_get(operand);
    if (taint_var != NULL) {
      plog(app, PLOG_TYPE_TAINT, "on %s of %04d(L%04d)%s (0x%llx) | ",
           get_operand_index_name(op, TAINT_OPERAND_2), OP_INDEX(stack_frame, op),
           op->lineno, site_relative_path(app, stack_frame), (uint64) operand);
      plog_taint(app, taint_var);
      plog_append(app, PLOG_TYPE_TAINT, "\n");
    }
  }
  operand = get_zval(execute_data, &op->result, op->result_type);
  if (operand != NULL) {
    taint_var = taint_var_get(operand);
    if (taint_var != NULL) {
      plog(app, PLOG_TYPE_TAINT, "on %s of %04d(L%04d)%s (0x%llx) | ",
           get_operand_index_name(op, TAINT_OPERAND_RESULT), OP_INDEX(stack_frame, op),
           op->lineno, site_relative_path(app, stack_frame), (uint64) operand);
      plog_taint(app, taint_var);
      plog_append(app, PLOG_TYPE_TAINT, "\n");
    }
  }
#endif
}

void propagate_taint(application_t *app, zend_execute_data *execute_data,
                     zend_op_array *stack_frame, const zend_op *op)
{
  uint mapping_flags = 0;
  // bool skip_arrays = true;

  // if (true) return;

  switch (op->opcode) {
    case ZEND_ASSIGN:
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_1);
      break;
    case ZEND_ASSIGN_CONCAT:
      propagate_zval_taint(app, execute_data, stack_frame, op, false/*merge*/,
                           get_operand_zval(execute_data, op+1, TAINT_OPERAND_1),
                           get_operand_index_name(op+1, TAINT_OPERAND_1),
                           get_operand_zval(execute_data, op, TAINT_OPERAND_2),
                           get_operand_index_name(op, TAINT_OPERAND_2));
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
    case ZEND_FAST_CONCAT:
    case ZEND_BOOL_XOR:
    case ZEND_INSTANCEOF:
    case ZEND_CASE:
    case ZEND_ROPE_END:
    case ZEND_IS_IDENTICAL:
    case ZEND_IS_NOT_IDENTICAL:
    case ZEND_IS_EQUAL:
    case ZEND_IS_NOT_EQUAL:
    case ZEND_IS_SMALLER:
    case ZEND_IS_SMALLER_OR_EQUAL:
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_RESULT);
      merge_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      break;
    case ZEND_TYPE_CHECK:
    case ZEND_DEFINED:
    case ZEND_CAST:
    case ZEND_QM_ASSIGN:
    case ZEND_STRLEN:
    case ZEND_FE_RESET_R:
    case ZEND_BOOL:
    case ZEND_BOOL_NOT:
    case ZEND_CLONE: // TODO: propagate into clone function (if any)
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      break;
    case ZEND_ROPE_INIT:
    case ZEND_ROPE_ADD:
      merge_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_1);
      break;
    case ZEND_FETCH_CONSTANT:
      merge_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_RESULT);
      break;
    case ZEND_FETCH_DIM_UNSET: /* also {} => {1.2}, which is not modelled yet */
    case ZEND_FETCH_OBJ_UNSET:
      break;
    case ZEND_ASSIGN_REF:
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_1);
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_RESULT);
      break;
    case ZEND_ADD_ARRAY_ELEMENT:
      // if (op->op2_type != IS_UNUSED) /* when modelled, use the key in this case */
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      break;
    case ZEND_FE_FETCH_R:
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_MAP, TAINT_OPERAND_VALUE);
      propagate_zval_taint(app, execute_data, stack_frame, op, true,
                           get_operand_zval(execute_data, op, TAINT_OPERAND_MAP), "?",
                           get_operand_zval(execute_data, op+1, TAINT_OPERAND_VALUE), "iter");
      break;
    case ZEND_PRE_INC:
    case ZEND_PRE_DEC:
    case ZEND_POST_INC:
    case ZEND_POST_DEC:
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_1);
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      break;
    case ZEND_DECLARE_FUNCTION:
      // TODO: taint the function?
      break;
    case ZEND_FETCH_W:  /* fetch a superglobal */
    case ZEND_FETCH_RW:
    case ZEND_FETCH_R:
    case ZEND_FETCH_IS:
      merge_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      // clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_2, TAINT_OPERAND_RESULT);
      break;
    case ZEND_ISSET_ISEMPTY_VAR:
      clobber_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_1, TAINT_OPERAND_RESULT);
      break;
    case ZEND_ISSET_ISEMPTY_DIM_OBJ:
    case ZEND_ISSET_ISEMPTY_PROP_OBJ:
      /* both merge because the value itself is propagated to the result internally */
      merge_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_KEY, TAINT_OPERAND_RESULT);
      merge_operand_taint(app, execute_data, stack_frame, op, TAINT_OPERAND_MAP, TAINT_OPERAND_RESULT);
      break;
    //case ZEND_ISSET_ISEMPTY_STATIC_PROP: // flow handled by stack
    //  break;
    case ZEND_ISSET_ISEMPTY_THIS:
      propagate_zval_taint(app, execute_data, stack_frame, op, true, &EX(This), "this",
                           get_operand_zval(execute_data, op, TAINT_OPERAND_RESULT), "R");
      break;
    /****************** arrays and objects *****************/
    case ZEND_FETCH_DIM_W:
    case ZEND_FETCH_DIM_RW:
      mapping_flags |= MAPPING_BY_REF;
    case ZEND_FETCH_DIM_R:
    case ZEND_FETCH_DIM_IS:
      propagate_taint_from_array(app, execute_data, stack_frame, op, mapping_flags);
      break;
    case ZEND_FETCH_DIM_FUNC_ARG:
      if (ARG_SHOULD_BE_SENT_BY_REF(execute_data->call->func, op->extended_value & ZEND_FETCH_ARG_MASK))
        mapping_flags |= MAPPING_BY_REF;
      propagate_taint_from_array(app, execute_data, stack_frame, op, mapping_flags);
      break;
    case ZEND_ASSIGN_DIM:
      // if (!skip_arrays) /* fail here */
        // propagate_taint_into_array(app, execute_data, stack_frame, op, MAPPING_BY_REF);
      break;
    case ZEND_FETCH_OBJ_W:
    case ZEND_FETCH_OBJ_RW:
      mapping_flags |= MAPPING_BY_REF;
    case ZEND_FETCH_OBJ_R:
    case ZEND_FETCH_OBJ_IS:
      mapping_flags |= MAPPING_IS_OBJECT;
      //if (false) propagate_taint_from_object(app, execute_data, stack_frame, op, mapping_flags);
      break;
    case ZEND_FETCH_OBJ_FUNC_ARG:
      if (ARG_SHOULD_BE_SENT_BY_REF(execute_data->call->func, op->extended_value & ZEND_FETCH_ARG_MASK))
        mapping_flags |= MAPPING_BY_REF;
      //if (false) propagate_taint_from_object(app, execute_data, stack_frame, op, mapping_flags);
      break;
    case ZEND_ASSIGN_OBJ:
      // if (!skip_arrays) /* fail here */
      //  propagate_taint_into_object(app, execute_data, stack_frame, op, MAPPING_BY_REF | MAPPING_IS_OBJECT);
      break;
    case ZEND_FETCH_UNSET:
      // TODO: remove taint from map
      break;
    case ZEND_UNSET_VAR:
    case ZEND_UNSET_DIM: /* FT */
    case ZEND_UNSET_OBJ:
      // TODO: remove taint
      break;
    /****************** calls *****************/
    case ZEND_DO_FCALL:
    case ZEND_DO_ICALL:
      // nop
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
      // plog(app, "<taint> remove (0x%llx)\n", (uint64) value);
      taint_var_remove(value);
    } break;
  }

  plog_taint_propagation(app, execute_data, stack_frame, op);
}

void taint_prepare_call(application_t *app, zend_execute_data *execute_data,
                        const zend_op **args, uint arg_count)
{
  const char *callee_name = EX(call)->func->common.function_name->val;
  zend_op_array *stack_frame = &EX(call)->func->op_array;
  uint64 hash = (uint64) stack_frame;
  taint_call_t *call = NULL;
  uint i;

  if (arg_count > MAX_TAINT_ARGS) {
    ERROR("<taint> Error: too many args (%d) in call to %s!\n", arg_count, callee_name);
    return;
  }

  call = (taint_call_t *) sctable_lookup(&taint_table, hash);
  if (call != NULL) {
    ERROR("<taint> Error! Found dangling call.\n");
    memset(call, 0, sizeof(taint_call_t));
    call->arg_count = arg_count;
  }

  for (i = 0; i < arg_count; i++) {
    const zval *send_val = get_operand_zval(execute_data, args[i], TAINT_OPERAND_1);
    taint_variable_t *taint_send = taint_var_get(send_val);
    if (taint_send != NULL) {
      if (call == NULL) {
        call = REQUEST_NEW(taint_call_t);
        memset(call, 0, sizeof(taint_call_t));
        call->arg_count = arg_count;
        sctable_add(&taint_table, hash, call);
      }
      call->taint_send[i] = taint_send;
    }
  }
}

void taint_propagate_into_arg_receivers(application_t *app, zend_execute_data *execute_data,
                                        zend_op_array *stack_frame, zend_op *op)
{
  uint64 hash = (uint64) stack_frame;
  taint_call_t *call = (taint_call_t *) sctable_remove(&taint_table, hash);

  if (call != NULL) {
    uint i;

    //plog(app, "<taint> found call to %s at %04d(L%04d)%s\n", stack_frame->function_name->val,
    //     OP_INDEX(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));

    if ((op - call->arg_count) < (zend_op *) stack_frame->opcodes) {
      uint explicit_parameter_count = (uint) (op - (zend_op *) stack_frame->opcodes);
      plog(app, PLOG_TYPE_WARN, "vararg call to %s:%s is not supported by taint yet.\n",
           stack_frame->filename->val, stack_frame->function_name->val,
           call->arg_count, explicit_parameter_count);
      call->arg_count = explicit_parameter_count;
    }

    for (i = 0; i < call->arg_count; i++) {
      if (call->taint_send[i] != NULL) {
        zend_op *receive = (op - (i + 1));
        if (receive->opcode == ZEND_RECV || receive->opcode == ZEND_RECV_INIT) {
          const zval *receive_val = get_operand_zval(execute_data, receive, TAINT_OPERAND_RESULT);
          taint_var_add(app, receive_val, call->taint_send[i]);
          plog(app, PLOG_TYPE_TAINT, "write arg receive (0x%llx) at %04d(L%04d)%s\n",
               (uint64) receive_val, OP_INDEX(stack_frame, receive), receive->lineno,
               site_relative_path(app, stack_frame));
        } else {
          ERROR("<taint> Error! Found %s but ZEND_RECV was expected at %04d(L%04d)%s\n",
                zend_get_opcode_name(receive->opcode),
                OP_INDEX(stack_frame, receive), receive->lineno, site_relative_path(app, stack_frame));
        }
      }
    }
  } else {
    /*
    plog(app, "<taint> no args tainted in call to %s at %04d(L%04d)%s\n",
         stack_frame->function_name->val,
         OP_INDEX(stack_frame, op), op->lineno, site_relative_path(app, stack_frame));
    */
  }
}

/******************************************************************************************
 * Public Taint API
 */

void init_taint_tracker()
{
  taint_table.hash_bits = 6;
  sctable_init(&taint_table);
  sctable_activate_pool(&taint_table);
}

void destroy_taint_tracker()
{
  sctable_destroy(&taint_table);
}

taint_variable_t *create_taint_variable(const char *file_path, const zend_op *tainted_at,
                                        taint_type_t type, void *taint)
{
  taint_variable_t *var = REQUEST_NEW(taint_variable_t);
  // var->value = value;
  var->tainted_at_file = request_strdup(file_path);
  var->tainted_at = tainted_at;
  var->type = type;
  var->taint = taint;
  return var;
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

  taint_variable_t *var = REQUEST_NEW(taint_variable_t);
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

  if (hash == 0ULL)
    return; // bug?

#ifdef PLOG_TAINT
  plog_taint_var(app, taint, (uint64) taintee);
#endif

  if (sctable_lookup(&taint_table, hash) == NULL)
    sctable_add(&taint_table, hash, taint);
}

void propagate_args_to_result(application_t *app, zend_execute_data *execute_data,
                              const zend_op *op, const zend_op **args, uint arg_count,
                              const char *builtin_name)
{
  zend_op_array *stack_frame = &execute_data->func->op_array;
  taint_variable_t *arg_taint;
  const zval *arg_value, *result = get_operand_zval(execute_data, op, TAINT_OPERAND_RESULT);
  char arg_id[128];
  uint i;

  // todo: what if `result` is `null`, or `true`? Don't taint global constants!

  for (i = 0; i < arg_count; i++) {
    arg_value = get_operand_zval(execute_data, args[i], TAINT_OPERAND_1);
    arg_taint = taint_var_get(arg_value);
    if (arg_taint != NULL) {
      snprintf(arg_id, 128, "%s(#%d)", builtin_name, i);
      propagate_zval_taint(app, execute_data, stack_frame, op, false,
                           arg_value, arg_id, result, "result");
    }
  }
}

taint_variable_t *taint_var_get(const zval *value)
{
  uint64 hash = (uint64) value;
  return (taint_variable_t *) sctable_lookup(&taint_table, hash);
}

taint_variable_t *taint_var_get_arg(zend_execute_data *execute_data, const zend_op *arg)
{
  return taint_var_get(get_operand_zval(execute_data, arg, TAINT_OPERAND_1));
}

taint_variable_t *taint_var_remove(const zval *value)
{
  uint64 hash = (uint64) value;
  return (taint_variable_t *) sctable_remove(&taint_table, hash);
}

void taint_var_free(const zval *value)
{
  taint_variable_t *taint_var = taint_var_remove(value);

  if (taint_var == NULL) {
    PRINT("nothing to remove for 0x%llx\n", (uint64) value);
  } else {
    PRINT("<taint> remove %s:%d (0x%llx)\n", taint_var->tainted_at_file,
          taint_var->tainted_at->lineno, (uint64) value);
  }
}

void taint_clear()
{
  sctable_erase(&taint_table);
}
