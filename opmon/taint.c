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

/* perfect hash for taint_variable_t */
static uint64 hash_taint_fields(taint_variable_type_t var_type, taint_variable_id_t var_id,
                                zend_op *stack_frame_id)
{
  uint64 upper_bits, hash;

  if (var_type == TAINT_VAR_TEMP)
    upper_bits = var_id.temp_id;
  else
    upper_bits = hash_string(var_id.var_name);

  hash = ((uint_ptr_t) stack_frame_id | (upper_bits << 0x20) | var_type);
  return hash;
}

static inline uint64 hash_taint_var(taint_variable_t *var)
{
  return hash_taint_fields(var->var_type, var->var_id, var->stack_frame->opcodes);
}

void taint_var_add(application_t *app, taint_variable_t *var)
{
  uint64 hash = hash_taint_var(var);

  plog_taint_var(app, var);

  if (sctable_lookup(&taint_table, hash) != NULL)
    free(var);
  else
    sctable_add(&taint_table, hash, var);
}

void *taint_var_get(taint_variable_type_t var_type, taint_variable_id_t var_id,
                    zend_op *stack_frame_id)
{
  return NULL;
}

void *taint_var_remove(taint_variable_type_t var_type, taint_variable_id_t var_id,
                       zend_op *stack_frame_id)
{
  return NULL;
}

typedef enum _dataflow_value_type_t {
  DATAFLOW_VALUE_TYPE_NONE,
  DATAFLOW_VALUE_TYPE_CONST,
  DATAFLOW_VALUE_TYPE_JMP,
  DATAFLOW_VALUE_TYPE_VAR,
  DATAFLOW_VALUE_TYPE_THIS,
  DATAFLOW_VALUE_TYPE_TEMP,
  DATAFLOW_VALUE_TYPE_INCLUDE,
  DATAFLOW_VALUE_TYPE_FCALL,
} dataflow_value_type_t;

typedef struct _dataflow_value_t {
  dataflow_value_type_t type;
  union {
    uint temp_id;
    uint jmp_target;
    const char *var_name;
    zval *constant;
    zend_class_entry *class;
  };
} dataflow_value_t;

static void initilize_taint_operand(dataflow_operand_t *operand, zend_op_array *zops,
                                       znode_op *znode, zend_uchar type)
{
  switch (type) {
    case IS_CONST:
      operand->value.type = DATAFLOW_VALUE_TYPE_CONST;
      operand->value.constant = znode->zv;
      break;
    case IS_VAR:
    case IS_TMP_VAR:
      operand->value.type = DATAFLOW_VALUE_TYPE_TEMP;
      operand->value.temp_id = (uint) (EX_VAR_TO_NUM(znode->var) - zops->last_var);
      break;
    case IS_CV:
      operand->value.var_name = zops->vars[EX_VAR_TO_NUM(znode->var)]->val;
      if (strcmp("this", operand->value.var_name) == 0)
        operand->value.type = DATAFLOW_VALUE_TYPE_THIS;
      else
        operand->value.type = DATAFLOW_VALUE_TYPE_VAR;
      break;
    case IS_UNUSED:
    default:
      operand->value.type = DATAFLOW_VALUE_TYPE_NONE;
  }
}

static void propagate_operand_taint(scarray_t *live_variables, dataflow_opcode_t *dop,
                         dataflow_operand_index_t src_index, dataflow_operand_index_t dst_index)
{
  char var_name[256];
  uint lookup_index = 0;
  dataflow_operand_t *src = get_dataflow_operand(dop, src_index);
  dataflow_operand_t *dst = get_dataflow_operand(dop, dst_index);

  /* Global src flows from all global bind points in scope. */
  if (src_index != DATAFLOW_OPERAND_SOURCE) {
    dataflow_live_variable_t *src_var;

    get_var_name(src, var_name, 256);
    while ((src_var = lookup_live_variable(live_variables, &lookup_index, src)) != NULL) {
      if (IS_LIVE_GLOBAL(src_var)) {
        dataflow_operand_t *global_dst = (dst == NULL ? src : dst);

        fprintf(opcode_dump_file, "Attempting to link global source %s to 0x%x:%d\n", var_name,
                dop->id.routine_hash, dop->id.op_index);

        if (add_predecessor(&global_dst->predecessor, &src_var->global_id,
                            DATAFLOW_OPERAND_SOURCE)) {
          fprintf(opcode_dump_file, "Link global variable '%s': "
                  "0x%x:%d.source -%d-> 0x%x:%d.%c\n", var_name,
                  src_var->global_id.routine_hash, src_var->global_id.op_index, src_index,
                  dop->id.routine_hash, dop->id.op_index,
                  get_operand_index_code(dst_index));
        }
      }
    }
  }

  /* Global dst flows to all global bind points in scope. */
  lookup_index = 0;
  if (dst_index != DATAFLOW_OPERAND_SINK) {
    dataflow_live_variable_t *dst_var;

    get_var_name(dst, var_name, 256);
    while ((dst_var = lookup_live_variable(live_variables, &lookup_index, dst)) != NULL) {
      if (IS_LIVE_GLOBAL(dst_var)) {
        dataflow_routine_t *sink_routine = sctable_lookup(&dg->routine_table,
                                                          dst_var->global_id.routine_hash);
        dataflow_opcode_t *sink_op = (dataflow_opcode_t *) sink_routine->opcodes.data[dst_var->global_id.op_index];

        fprintf(opcode_dump_file, "Attempting to link 0x%x:%d to global sink of variable %s\n",
                dop->id.routine_hash, dop->id.op_index, var_name);

        if (add_predecessor(&sink_op->sink.predecessor, &dst_var->src.opcode_id,
                            dst_var->src.operand_index)) {
          fprintf(opcode_dump_file, "Link to global variable '%s': 0x%x:%d.%c -%d-> 0x%x:%d.sink\n",
                  var_name, dop->id.routine_hash, dop->id.op_index,
                  get_operand_index_code(src_index),
                  dst_index, sink_op->id.routine_hash, sink_op->id.op_index);
        }
      }
    }
  }
  lookup_index = 0;

  //fprintf(opcode_dump_file, "   [ propagate_operand_taint %d.(%d->%d)) ]\n",
  //        dop->id.op_index, src_index, dst_index);

  if (src_index == DATAFLOW_OPERAND_SOURCE) {
    dataflow_operand_t *dst = get_dataflow_operand(dop, dst_index);

    if (add_predecessor(&dst->predecessor, &dop->id, src_index)) {
      fprintf(opcode_dump_file, "Link source type %d into 0x%x:%d.%d\n", dop->source.type,
              dop->id.routine_hash, dop->id.op_index, dst_index);
    }

  } else if (dst_index == DATAFLOW_OPERAND_SINK) {
    dataflow_operand_t *src = get_dataflow_operand(dop, src_index);
    dataflow_live_variable_t *src_var;

    switch (src->value.type) {
      case DATAFLOW_VALUE_TYPE_VAR:
        if (dop->sink.type == SINK_TYPE_GLOBAL) { /* create an empty live_variable flagged global */
          dataflow_live_variable_t *global_var = malloc(sizeof(dataflow_live_variable_t));
          memset(global_var, 0, sizeof(dataflow_live_variable_t));
          global_var->global_id = dop->id;
          global_var->var_name = src->value.var_name;
          global_var->src.opcode_id = dop->id;
          global_var->src.operand_index = src_index;
          scarray_append(live_variables, global_var);

          fprintf(opcode_dump_file, "Generate global var %s at 0x%x:%d\n", global_var->var_name,
                  dop->id.routine_hash, dop->id.op_index);
        }
      case DATAFLOW_VALUE_TYPE_THIS: /* FT */
      case DATAFLOW_VALUE_TYPE_TEMP:
        get_var_name(src, var_name, 256);
        //fprintf(opcode_dump_file, "   [ searching live variables for %s (in operand %d.(%d->%d)) ]\n",
        //        var_name, dop->id.op_index, src_index, dst_index);
        while ((src_var = lookup_live_variable(live_variables, &lookup_index, src)) != NULL)
          add_predecessor(&src->predecessor, &src_var->src.opcode_id, src_var->src.operand_index);
      default: /*no*/;
    }

    if (add_predecessor(&dop->sink.predecessor, &dop->id, src_index)) {
      fprintf(opcode_dump_file, "Link operand 0x%x:%d.%d into sink of type %d\n",
              dop->id.routine_hash, dop->id.op_index, src_index, dop->sink.type);
    }
  } else {
    bool found_any;
    dataflow_operand_t *src = get_dataflow_operand(dop, src_index);
    dataflow_operand_t *dst = get_dataflow_operand(dop, dst_index);
    dataflow_live_variable_t *src_var;

    switch (src->value.type) {
      case DATAFLOW_VALUE_TYPE_NONE:
      case DATAFLOW_VALUE_TYPE_CONST:
      case DATAFLOW_VALUE_TYPE_JMP: // it's a sink, though
      case DATAFLOW_VALUE_TYPE_INCLUDE:
      case DATAFLOW_VALUE_TYPE_FCALL:
        fprintf(opcode_dump_file, "Nothing to link for 0x%x:%d.(%c->%c)\n", dop->id.routine_hash,
                dop->id.op_index, get_operand_index_code(src_index),
                get_operand_index_code(dst_index));
        break;
      case DATAFLOW_VALUE_TYPE_VAR:
      case DATAFLOW_VALUE_TYPE_THIS:
      case DATAFLOW_VALUE_TYPE_TEMP:
        found_any = false;
        get_var_name(src, var_name, 256);
        //fprintf(opcode_dump_file, "   [ searching live variables for %s (in operand %d.(%d->%d)) ]\n",
        //        var_name, dop->id.op_index, src_index, dst_index);
        while ((src_var = lookup_live_variable(live_variables, &lookup_index, src)) != NULL) {
          found_any = true;
          // add_predecessor(&src->predecessor, &src_var->src.opcode_id, src_var->src.operand_index);
          if (add_predecessor(&dst->predecessor, &src_var->src.opcode_id,
                              src_var->src.operand_index)) {
            fprintf(opcode_dump_file, "Link variable '%s': 0x%x:%d.%c -%d-> 0x%x:%d.%c\n",
                    var_name, src_var->src.opcode_id.routine_hash,
                    src_var->src.opcode_id.op_index,
                    get_operand_index_code(src_var->src.operand_index),
                    src_index, dop->id.routine_hash, dop->id.op_index,
                    get_operand_index_code(dst_index));
          }
        }
        if (!found_any)
          fprintf(opcode_dump_file, "Can't find var '%s'\n", var_name);
        break;
    }
  }
}

void propagate_taint(zend_op_array *stack_frame, zend_op *op)
{
  bool end_pass = false;
  dataflow_opcode_t *dop;

  switch (cop->opcode) {
    case ZEND_ASSIGN_ADD:
    case ZEND_ASSIGN_SUB:
    case ZEND_ASSIGN_MUL:
    case ZEND_ASSIGN_DIV:
    case ZEND_ASSIGN_MOD:
    case ZEND_ASSIGN_POW:
    case ZEND_ASSIGN_SL:
    case ZEND_ASSIGN_SR:
    case ZEND_ASSIGN_BW_OR:
    case ZEND_ASSIGN_BW_AND:
    case ZEND_ASSIGN_BW_XOR:
    case ZEND_ASSIGN_CONCAT:
    case ZEND_PRE_INC_OBJ: /* structural effect on 1.2; for now modeling as {1.2} => {1.2,r} */
    case ZEND_PRE_DEC_OBJ:
    case ZEND_POST_INC_OBJ:
    case ZEND_POST_DEC_OBJ:
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_1, false);
    case ZEND_ASSIGN: /* FT */
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_2, DATAFLOW_OPERAND_1, true);
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
    case ZEND_FETCH_DIM_R: /* {1.2} => {result} */
    case ZEND_FETCH_DIM_W:
    case ZEND_FETCH_DIM_RW:
    case ZEND_FETCH_DIM_IS:
    case ZEND_FETCH_OBJ_R:
    case ZEND_FETCH_OBJ_W:
    case ZEND_FETCH_OBJ_RW:
    case ZEND_FETCH_OBJ_IS:
    case ZEND_FETCH_CONSTANT:
    case ZEND_FETCH_DIM_UNSET: /* also {} => {1.2}, which is not modelled yet */
    case ZEND_FETCH_OBJ_UNSET:
    case ZEND_ADD_VAR:
    case ZEND_ADD_STRING:
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_2, DATAFLOW_OPERAND_RESULT, false);
    case ZEND_TYPE_CHECK: /* FT */
    case ZEND_DEFINED:
    case ZEND_CAST:
    case ZEND_QM_ASSIGN: /* ? */
    case ZEND_STRLEN:
    case ZEND_FE_RESET:
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_RESULT, true);
      break;
    case ZEND_BOOL:
    case ZEND_BOOL_NOT:
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_RESULT, true);
      break;
    case ZEND_ASSIGN_REF:
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_2, DATAFLOW_OPERAND_1, false);
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_2, DATAFLOW_OPERAND_RESULT, true);
      break;
    case ZEND_ASSIGN_OBJ:
    case ZEND_ASSIGN_DIM: /* ignoring key for now */
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_VALUE, DATAFLOW_OPERAND_MAP, true);
      break;
    case ZEND_ADD_CHAR:
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_2, DATAFLOW_OPERAND_RESULT, false);
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_RESULT, DATAFLOW_OPERAND_RESULT, true);
      break;
    case ZEND_ADD_ARRAY_ELEMENT:
      // if (op->op2_type != IS_UNUSED) /* when modelled, use the key in this case */
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_RESULT, true);
      break;
    case ZEND_FE_FETCH:
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_MAP, DATAFLOW_OPERAND_KEY, true);
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_MAP, DATAFLOW_OPERAND_VALUE, true);
      break;
    case ZEND_CLONE: /* also {1} => {opline} (potentially calls a clone() function) */
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_RESULT, true);
      break;
    case ZEND_PRE_INC:
    case ZEND_PRE_DEC:
    case ZEND_POST_INC:
    case ZEND_POST_DEC:
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_1, false);
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_RESULT, true);
      break;
    case ZEND_FETCH_UNSET:
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_RESULT, false);
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_2, DATAFLOW_OPERAND_RESULT, true);
      /* also unsets {1.2} */
      break;
    case ZEND_UNSET_VAR:
      if (dop->op2.value.type == DATAFLOW_VALUE_TYPE_NONE)
        break; /* {} => {1} */
    case ZEND_UNSET_DIM: /* FT */
    case ZEND_UNSET_OBJ:
        break; /* {} => {1.2}, which is not modelled yet */
    case ZEND_ISSET_ISEMPTY_VAR:
      if (dop->op2.value.type != DATAFLOW_VALUE_TYPE_NONE)
        propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_2, DATAFLOW_OPERAND_RESULT, false);
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_RESULT, true);
      break;
    case ZEND_ISSET_ISEMPTY_DIM_OBJ:
    case ZEND_ISSET_ISEMPTY_PROP_OBJ:
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_RESULT, false);
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_2, DATAFLOW_OPERAND_RESULT, true);
      break;
    /****************** sources *****************/
    case ZEND_FETCH_R:  /* fetch a superglobal */
    case ZEND_FETCH_W:
    case ZEND_FETCH_RW:
    case ZEND_FETCH_IS:
      if (dop->op1.value.type != DATAFLOW_VALUE_TYPE_NONE) {
        if (dop->source.id.routine_hash != 0)
          propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_SOURCE, DATAFLOW_OPERAND_RESULT, true);
        else
          propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_RESULT, true);
      } else {
        propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_2, DATAFLOW_OPERAND_RESULT, false);
        propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_RESULT, true);
      }
      break;
    case ZEND_RECV:
    case ZEND_RECV_VARIADIC:
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_SOURCE, DATAFLOW_OPERAND_RESULT, true);
      break;
    /****************** sinks *****************/
    case ZEND_PRINT:
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_RESULT, false);
    case ZEND_ECHO:
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_SINK, true);
      break;
    case ZEND_FREE:
      remove_live_variable(pass->live_variables, &dop->op1);
      break;
    case ZEND_ADD_INTERFACE: /* also {2.zv+1} => {code} */
    case ZEND_ADD_TRAIT:     /* also {2.zv+1} => {code} */
    case ZEND_DECLARE_INHERITED_CLASS_DELAYED:
    case ZEND_DECLARE_CONST:
    case ZEND_BIND_GLOBAL:
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_2, DATAFLOW_OPERAND_SINK, false);
    case ZEND_SEND_VAL: /* FT */
    case ZEND_SEND_VAL_EX:
    case ZEND_SEND_VAR:
    case ZEND_SEND_VAR_NO_REF:
    case ZEND_SEND_REF:
    case ZEND_SEND_USER:
    case ZEND_SEND_ARRAY:
    case ZEND_DECLARE_CLASS:
    case ZEND_DECLARE_INHERITED_CLASS:
    case ZEND_BIND_TRAITS:
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_SINK, true);
      break;
    case ZEND_DECLARE_FUNCTION: /* {compiler} =i=> {code} */
      break;
    /****************** branches *****************/
    case ZEND_JMP:
      enqueue_dataflow_link_pass(pass->droutine, dop->op1.value.jmp_target,
                                 pass->live_variables);
      end_pass = true;
      break;
    case ZEND_JMPZ:
    case ZEND_JMPNZ:
    case ZEND_JMPZNZ:
    case ZEND_JMPZ_EX:
    case ZEND_JMPNZ_EX:
      enqueue_dataflow_link_pass(pass->droutine, dop->op2.value.jmp_target,
                                 pass->live_variables);
      break;
    case ZEND_BRK:
    case ZEND_CONT:
    case ZEND_GOTO:
      end_pass = true;
      break;
    case ZEND_INIT_METHOD_CALL: /* {1.2} => {fcall-stack} */
      break;
    case ZEND_INIT_STATIC_METHOD_CALL: /* {1.2} => {fcall-stack} */
      break;
    case ZEND_INIT_FCALL: /* {2} => {fcall-stack} */
    case ZEND_INIT_FCALL_BY_NAME:
    case ZEND_INIT_USER_CALL:
    case ZEND_INIT_NS_FCALL_BY_NAME:
      break;
    case ZEND_NEW: /* {1} =d=> {fcall-stack} (or skip via 2 => opline if no ctor) */
      break;
    case ZEND_DO_FCALL:
      // if (is_db_sink_function(NULL, id.call_target))    /* {fcall-stack} => {db,opline,result} */
      // else if (is_file_sink_function(id.call_target))   /* {fcall-stack} => {file,opline,result} */
      // else if (is_system_sink_function(id.call_target)) /* {fcall-stack} => {system,opline,result} */
      // else                                              /* {fcall-stack} =d=> {opline,result} */
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_SOURCE, DATAFLOW_OPERAND_RESULT, true);
      break;
    case ZEND_RETURN:
    case ZEND_RETURN_BY_REF:
      propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_SINK, true);
      break;
    case ZEND_INCLUDE_OR_EVAL:
      switch (cop->extended_value) {
        case ZEND_EVAL: /* {1} =d=> {code,opline} */
          break;
        case ZEND_INCLUDE:
        case ZEND_REQUIRE: {
          dataflow_routine_t *included_routine = sctable_lookup(&dg->routine_table,
                                                                dop->source.include.routine_hash);
          dataflow_link_pass_t include_pass = { false, included_routine, 0, NULL, 0,
                                                pass->live_variables };
          propagate_operand_taint_dataflow(&include_pass);
        } break;
        case ZEND_INCLUDE_ONCE: /* ??? {1} =d=> {opline}, {file} =d=> {code} */
        case ZEND_REQUIRE_ONCE: {
          uint resume_index = scarray_iterator_index(&pass->droutine->opcodes, i);
          dataflow_routine_t *included_routine = sctable_lookup(&dg->routine_table,
                                                                dop->source.include.routine_hash);
          enqueue_dataflow_link_include_pass(included_routine, pass->droutine,
                                             resume_index, pass->live_variables);
        } break;
      }
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
    case ZEND_EXIT:
      if (dop->op1.value.type == DATAFLOW_VALUE_TYPE_NONE)
        break; /* {} => {opline} */
      else
        propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_SINK, true);
        /* also {} =d=> {opline} */
      break;
    case ZEND_JMP_SET: /* nop if op1 is false, but can't see from here */
      if (dop->op1.value.type != DATAFLOW_VALUE_TYPE_CONST ||
          dop->op1.value.constant->u1.v.type != IS_FALSE) {
        propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_RESULT, true);
        /* also {1} => {opline} */
      }
      break;
    case ZEND_COALESCE: /* nop if op1 is null, but can't see from here */
      if (dop->op1.value.type != DATAFLOW_VALUE_TYPE_CONST ||
          dop->op1.value.constant->u1.v.type != IS_NULL) {
        propagate_operand_taint(pass->live_variables, dop, DATAFLOW_OPERAND_1, DATAFLOW_OPERAND_RESULT, true);
        /* also {1} => {opline} */
      }
      break;
    case ZEND_BEGIN_SILENCE:
    case ZEND_END_SILENCE:
      print_sink("sink(internal) {} =d=> {error-reporting}");
      break;
  }
}
