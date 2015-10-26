#include "compile_context.h"
#include "dataflow.h"

static FILE *opcode_dump_file = NULL;

void initialize_opcode_dump(const char *path)
{
    opcode_dump_file = fopen(path, "w");
    if (opcode_dump_file == NULL)
      ERROR("Failed to open the opcode dump file '%s'\n", path);
}

void dump_script_header(const char *routine_name, uint function_hash)
{
  fprintf(opcode_dump_file, " === %s (0x%x)\n", routine_name, function_hash);
}

void dump_function_header(const char *unit_path, const char *routine_name, uint function_hash)
{
  fprintf(opcode_dump_file, " === %s|%s (0x%x)\n", unit_path, routine_name, function_hash);
}

static bool uses_return_value(zend_op *op)
{
  switch (op->opcode) {
    case ZEND_ADD:
    case ZEND_SUB:
    case ZEND_MUL:
    case ZEND_DIV:
    case ZEND_MOD:
    case ZEND_SL:
    case ZEND_SR:
    case ZEND_CONCAT:
    case ZEND_IS_IDENTICAL:
    case ZEND_IS_NOT_IDENTICAL:
    case ZEND_IS_EQUAL:
    case ZEND_IS_NOT_EQUAL:
    case ZEND_IS_SMALLER:
    case ZEND_IS_SMALLER_OR_EQUAL:
    case ZEND_BW_OR:
    case ZEND_BW_AND:
    case ZEND_BW_XOR:
    case ZEND_BOOL_XOR:
    case ZEND_BW_NOT:
    case ZEND_POST_INC_OBJ:
    case ZEND_POST_DEC_OBJ:
    case ZEND_POST_INC:
    case ZEND_POST_DEC:
    case ZEND_PRINT:
    case ZEND_FETCH_R:
    case ZEND_FETCH_W:
    case ZEND_FETCH_RW:
    case ZEND_FETCH_FUNC_ARG:
    case ZEND_FETCH_UNSET:
    case ZEND_FETCH_IS:
    case ZEND_FETCH_DIM_R:
    case ZEND_FETCH_DIM_W:
    case ZEND_FETCH_DIM_RW:
    case ZEND_FETCH_DIM_IS:
    case ZEND_FETCH_DIM_FUNC_ARG:
    case ZEND_FETCH_DIM_UNSET:
    case ZEND_FETCH_OBJ_R:
    case ZEND_FETCH_OBJ_W:
    case ZEND_FETCH_OBJ_RW:
    case ZEND_FETCH_OBJ_IS:
    case ZEND_FETCH_OBJ_FUNC_ARG:
    case ZEND_FETCH_OBJ_UNSET:
    case ZEND_FETCH_LIST:
    case ZEND_JMPZ_EX:
    case ZEND_JMPNZ_EX:
    case ZEND_ADD_CHAR:
    case ZEND_ADD_STRING:
    case ZEND_ADD_VAR:
    case ZEND_FETCH_CLASS:
    case ZEND_RECV:
    case ZEND_RECV_INIT:
    case ZEND_RECV_VARIADIC:
    case ZEND_BOOL:
    case ZEND_CASE:
    case ZEND_FETCH_CONSTANT:
    case ZEND_ADD_ARRAY_ELEMENT:
    case ZEND_INIT_ARRAY:
    case ZEND_CAST:
    case ZEND_FE_RESET:
    case ZEND_FE_FETCH:
    case ZEND_ISSET_ISEMPTY_VAR:
    case ZEND_ISSET_ISEMPTY_DIM_OBJ:
    case ZEND_ISSET_ISEMPTY_PROP_OBJ:
    case ZEND_BEGIN_SILENCE:
    case ZEND_DECLARE_CLASS:
    case ZEND_DECLARE_INHERITED_CLASS:
    case ZEND_INSTANCEOF:
    case ZEND_DECLARE_LAMBDA_FUNCTION:
    case ZEND_POW:
    case ZEND_ASSIGN_POW:
    case ZEND_STRLEN:
    case ZEND_TYPE_CHECK:
    case ZEND_DEFINED:
    case ZEND_QM_ASSIGN:
    case ZEND_CATCH:
      return true;
    case ZEND_ECHO:
    case ZEND_JMP:
    case ZEND_JMPZ:
    case ZEND_JMPNZ:
    case ZEND_JMPZNZ:
    case ZEND_FREE:
    case ZEND_INIT_STATIC_METHOD_CALL:
    case ZEND_INIT_FCALL_BY_NAME:
    case ZEND_INIT_USER_CALL:
    case ZEND_INIT_NS_FCALL_BY_NAME:
    case ZEND_INIT_FCALL:
    case ZEND_INIT_METHOD_CALL:
    case ZEND_GENERATOR_RETURN:
    case ZEND_THROW:
    case ZEND_SEND_VAL:
    case ZEND_SEND_VAL_EX:
    case ZEND_SEND_VAR:
    case ZEND_SEND_VAR_NO_REF:
    case ZEND_SEND_REF:
    case ZEND_SEND_VAR_EX:
    case ZEND_SEND_UNPACK:
    case ZEND_SEND_ARRAY:
    case ZEND_SEND_USER:
    case ZEND_BRK:
    case ZEND_CONT:
    case ZEND_GOTO:
    case ZEND_UNSET_VAR:
    case ZEND_UNSET_DIM:
    case ZEND_UNSET_OBJ:
    case ZEND_EXIT:
    case ZEND_END_SILENCE:
    case ZEND_JMP_SET:
    case ZEND_COALESCE:
    case ZEND_EXT_STMT:
    case ZEND_EXT_FCALL_BEGIN:
    case ZEND_EXT_FCALL_END:
    case ZEND_DECLARE_INHERITED_CLASS_DELAYED:
    case ZEND_DECLARE_FUNCTION:
    case ZEND_TICKS:
    case ZEND_EXT_NOP:
    case ZEND_NOP:
    case ZEND_ADD_INTERFACE:
    case ZEND_ADD_TRAIT:
    case ZEND_BIND_TRAITS:
    case ZEND_VERIFY_ABSTRACT_CLASS:
    case ZEND_DECLARE_CONST:
    case ZEND_SEPARATE:
    case ZEND_DISCARD_EXCEPTION:
    case ZEND_FAST_CALL:
    case ZEND_BIND_GLOBAL:
    case ZEND_RETURN:
      return false;
    default:
      return (op->result_type & EXT_TYPE_UNUSED) == 0;
  }
}

void dump_operand(char index, zend_op_array *ops, znode_op *operand, zend_uchar type)
{
  fprintf(opcode_dump_file, "[%c|", index);
  switch (type) {
    case IS_CONST:
      switch (operand->zv->u1.v.type) {
        case IS_UNDEF:
          fprintf(opcode_dump_file, "const <undefined-type>");
          break;
        case IS_NULL:
          fprintf(opcode_dump_file, "const null");
          break;
        case IS_FALSE:
          fprintf(opcode_dump_file, "const false");
          break;
        case IS_TRUE:
          fprintf(opcode_dump_file, "const true");
          break;
        case IS_LONG:
          fprintf(opcode_dump_file, "const 0x%lx", operand->zv->value.lval);
          break;
        case IS_DOUBLE:
          fprintf(opcode_dump_file, "const %f", operand->zv->value.dval);
          break;
        case IS_STRING: {
          uint i, j;
          char buffer[32] = {0};
          const char *str = Z_STRVAL_P(operand->zv);

          for (i = 0, j = 0; i < 31; i++) {
            if (str[i] == '\0')
              break;
            if (str[i] != '\n')
              buffer[j++] = str[i];
          }
          fprintf(opcode_dump_file, "\"%s\"", buffer);
        } break;
        case IS_ARRAY:
          fprintf(opcode_dump_file, "const array (zv:"PX")", p2int(operand->zv));
          break;
        case IS_OBJECT:
          fprintf(opcode_dump_file, "const object? (zv:"PX")", p2int(operand->zv));
          break;
        case IS_RESOURCE:
          fprintf(opcode_dump_file, "const resource? (zv:"PX")", p2int(operand->zv));
          break;
        case IS_REFERENCE:
          fprintf(opcode_dump_file, "const reference? (zv:"PX")", p2int(operand->zv));
          break;
        default:
          fprintf(opcode_dump_file, "const what?? (zv:"PX")", p2int(operand->zv));
          break;
      }
      break;
    case IS_VAR:
    case IS_TMP_VAR:
      fprintf(opcode_dump_file, "var #%d", (uint) (EX_VAR_TO_NUM(operand->var) - ops->last_var));
      break;
    case IS_CV:
      fprintf(opcode_dump_file, "var $%s", ops->vars[EX_VAR_TO_NUM(operand->var)]->val);
      break;
    case IS_UNUSED:
      fprintf(opcode_dump_file, "-");
      break;
    default:
      fprintf(opcode_dump_file, "?");
  }
  fprintf(opcode_dump_file, "]");
}

static void dump_opcode_header(zend_op *op)
{
  fprintf(opcode_dump_file, "\t%04d: 0x%02x %s  ", op->lineno, op->opcode,
          zend_get_opcode_name(op->opcode));
}

void dump_fcall_opcode(zend_op_array *ops, zend_op *op, const char *routine_name)
{
  dump_opcode_header( op);
  if (uses_return_value(op)) {
    dump_operand('r', ops, &op->result, op->result_type);
    if (is_db_source_function(NULL, routine_name))
      fprintf(opcode_dump_file, " <=db= ");
    else if (is_file_source_function(routine_name))
      fprintf(opcode_dump_file, " <=file= ");
    else if (is_system_source_function(routine_name))
      fprintf(opcode_dump_file, " <=system= ");
    else
      fprintf(opcode_dump_file, " = ");
  }
  fprintf(opcode_dump_file, "%s\n", routine_name);
}

void dump_fcall_arg(zend_op_array *ops, zend_op *op, const char *routine_name)
{
  dump_opcode_header(op);
  dump_operand('a', ops, &op->op1, op->op1_type);
  if (op->opcode == ZEND_SEND_ARRAY)
    fprintf(opcode_dump_file, " -*-> %s\n", routine_name);
  else
    fprintf(opcode_dump_file, " -%d-> %s\n", op->op2.num, routine_name);
}

void dump_map_assignment(zend_op_array *ops, zend_op *op, zend_op *next_op)
{
  dump_opcode_header(op);
  if (op->op1_type == IS_UNUSED)
    fprintf(opcode_dump_file, "$this");
  else
    dump_operand('m', ops, &op->op1, op->op1_type);
  fprintf(opcode_dump_file, ".");
  dump_operand('k', ops, &op->op2, op->op2_type);
  fprintf(opcode_dump_file, " = ");
  dump_operand('v', ops, &next_op->op1, next_op->op1_type);
  fprintf(opcode_dump_file, "\n");
}

void dump_foreach_fetch(zend_op_array *ops, zend_op *op, zend_op *next_op)
{
  dump_opcode_header(op);
  fprintf(opcode_dump_file, " in ");
  dump_operand('m', ops, &op->op1, op->op1_type);
  fprintf(opcode_dump_file, ": ");
  dump_operand('k', ops, &next_op->result, next_op->result_type);
  fprintf(opcode_dump_file, ", ");
  dump_operand('v', ops, &op->result, op->result_type);
  fprintf(opcode_dump_file, "\n");
}

void dump_opcode(zend_op_array *ops, zend_op *op)
{
  zend_op *jump_target = NULL;
  const char *jump_reason = NULL;

  dump_opcode_header(op);

  if (uses_return_value(op)) {
    if (op->result_type != IS_UNUSED) {
      dump_operand('r', ops, &op->result, op->result_type);
      fprintf(opcode_dump_file, " = ");
    }
    if (op->op1_type == IS_UNUSED && op->op2_type == IS_UNUSED) {
      switch (op->opcode) {
        case ZEND_RECV:
        case ZEND_RECV_INIT:
        case ZEND_RECV_VARIADIC:
          fprintf(opcode_dump_file, "(arg)");
          break;
        default:
          fprintf(opcode_dump_file, "(?)");
      }
    }
  }
  if (op->op1_type != IS_UNUSED) {
    dump_operand('1', ops, &op->op1, op->op1_type);
  } else {
    switch (op->opcode) {
      case ZEND_JMP:
      case ZEND_GOTO:
        jump_target = op->op1.jmp_addr;
        jump_reason = "(target)";
        break;
      case ZEND_FAST_CALL:
        jump_target = op->op1.jmp_addr;
        jump_reason = "(?)";
        break;
      case ZEND_FETCH_OBJ_R:
        fprintf(opcode_dump_file, "$this.");
        break;
    }
  }
  if (op->op1_type != IS_UNUSED && op->op2_type != IS_UNUSED)
    fprintf(opcode_dump_file, " ? ");
  if (op->op2_type != IS_UNUSED) {
    dump_operand('2', ops, &op->op2, op->op2_type);
  } else {
    switch (op->opcode) {
      case ZEND_JMPZ:
      case ZEND_JMPZNZ:
      case ZEND_JMPZ_EX:
      case ZEND_JMPNZ_EX:
        jump_target = op->op2.jmp_addr;
        jump_reason = "(target)";
        break;
      case ZEND_COALESCE:
        jump_target = op->op2.jmp_addr;
        jump_reason = "(?)";
        break;
      case ZEND_JMP_SET:
        jump_target = op->op2.jmp_addr;
        jump_reason = "(?)";
        break;
      case ZEND_FE_RESET:
      case ZEND_FE_FETCH:
        jump_target = op->op2.jmp_addr;
        jump_reason = "(if array empty)";
        break;
      case ZEND_NEW:
        jump_target = op->op2.jmp_addr;
        jump_reason = "(if no constructor)";
        break;
    }
  }
  if (jump_target != NULL) {
    int delta = (int) (jump_target - op);
    fprintf(opcode_dump_file, " @ %s%d %s", delta > 0 ? "+" : "", delta, jump_reason);
  }
  fprintf(opcode_dump_file, "\n");
}

static void print_sink(const char *sink_details)
{
  fprintf(opcode_dump_file, "\t      %s\n", sink_details);
}

void identify_sink_operands(zend_op *op, sink_identifier_t id)
{
  switch (op->opcode) {
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
      print_sink("sink(zval:number) {1,2} =d=> {result}");
      break;
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
      print_sink("sink(zval:number) {1,2} =d=> {1,result}");
      break;
    case ZEND_CONCAT:
      print_sink("sink(zval:string) {1,2} =d=> {result}");
      break;
    case ZEND_ASSIGN_CONCAT:
      print_sink("sink(zval:string) {1,2} =d=> {1,result}");
      break;
    case ZEND_BOOL_XOR:
      print_sink("sink(zval:bool) {1,2} =d=> {result}");
      break;
    case ZEND_INSTANCEOF: /* op1 instanceof op2 */
      print_sink("sink(zval:bool) {1,2} =d=> {result}");
      break;
    case ZEND_TYPE_CHECK: /* check metadata consistency for op1 */
      print_sink("sink(zval:bool) {1} =d=> {result}");
      break;
    case ZEND_DEFINED:
      print_sink("sink(zval:bool) {1} =d=> {result}");
      break;
    case ZEND_IS_IDENTICAL:
    case ZEND_IS_NOT_IDENTICAL:
    case ZEND_IS_EQUAL:
    case ZEND_IS_NOT_EQUAL:
    case ZEND_IS_SMALLER:
    case ZEND_IS_SMALLER_OR_EQUAL:
      print_sink("sink(zval:bool) {1,2} =d=> {result}");
      break;
    case ZEND_PRE_INC_OBJ:
    case ZEND_PRE_DEC_OBJ:
    case ZEND_POST_INC_OBJ:
    case ZEND_POST_DEC_OBJ:
      print_sink("sink(zval:number) {1.2} =d=> {1.2,result}");
      break;
    case ZEND_PRE_INC:
    case ZEND_PRE_DEC:
    case ZEND_POST_INC:
    case ZEND_POST_DEC:
      print_sink("sink(zval:number) {1} =d=> {1,result}");
      break;
    case ZEND_ECHO:
      print_sink("sink(output) {1} =d=> {output}");
      break;
    case ZEND_PRINT:
      print_sink("sink(output,zval:number) {1} =d=> {output}, {len(1)} =d=> {result}");
      break;
    case ZEND_FETCH_R:  /* fetch a superglobal */
    case ZEND_FETCH_W:
    case ZEND_FETCH_RW:
    case ZEND_FETCH_IS:
      if (op->op2_type == IS_UNUSED) {
        const char *superglobal_name = Z_STRVAL_P(op->op1.zv);
        if (superglobal_name != NULL) {
          if (strcmp(superglobal_name, "_SESSION") == 0) {
            print_sink("sink(zval) {session} =d=> {result}");
            break;
          } else if (strcmp(superglobal_name, "_REQUEST") == 0 ||
                     strcmp(superglobal_name, "_GET") == 0 ||
                     strcmp(superglobal_name, "_POST") == 0 ||
                     strcmp(superglobal_name, "_COOKIE") == 0 ||
                     strcmp(superglobal_name, "_FILES") == 0) {
            print_sink("sink(zval) {user} =d=> {result}");
            break;
          } else if (strcmp(superglobal_name, "_ENV") == 0 ||
                     strcmp(superglobal_name, "_SERVER") == 0) {
            print_sink("sink(zval) {env} =d=> {result}");
            break;
          }
        }
        print_sink("sink(zval) {1} =d=> {result}");
      } else {
        print_sink("sink(zval) {2.1} =d=> {result}");
      }
      break;
    case ZEND_FETCH_UNSET:
      print_sink("sink(zval,zval) {1.2} =d=> {result}, {} =d=> {1.2}");
      break;
    case ZEND_FETCH_DIM_R:
    case ZEND_FETCH_DIM_W:
    case ZEND_FETCH_DIM_RW:
    case ZEND_FETCH_DIM_IS:
    case ZEND_FETCH_OBJ_R:
    case ZEND_FETCH_OBJ_W:
    case ZEND_FETCH_OBJ_RW:
    case ZEND_FETCH_OBJ_IS:
      print_sink("sink(zval:map) {1.2} =d=> {result}");
      break;
    case ZEND_FETCH_DIM_UNSET:
    case ZEND_FETCH_OBJ_UNSET:
      print_sink("sink(zval:map) {1.2} =d=> {result}, {} =d=> {1.2}");
      break;
    case ZEND_FETCH_CONSTANT:
      print_sink("sink(zval) {1.2} =d=> {result}");
      break;
    case ZEND_UNSET_VAR:
      if (op->op2_type == IS_UNUSED)
        print_sink("sink(zval) {} =d=> {1}");
      else
        print_sink("sink(zval:map) {} =d=> {2.1}"); /* op2 must be static */
      break;
    case ZEND_UNSET_DIM:
    case ZEND_UNSET_OBJ:
      print_sink("sink(zval:map) {} =d=> {1.2}");
      break;
    case ZEND_ASSIGN:
    case ZEND_ASSIGN_REF:
      print_sink("sink(zval) {2} =d=> {1,result}");
      break;
    case ZEND_ASSIGN_OBJ:
    case ZEND_ASSIGN_DIM:
      print_sink("sink(zval:map) {s(value)} =d=> {g(base.field)}");
      break;
    case ZEND_JMPZ:
    case ZEND_JMPNZ:
    case ZEND_JMPZNZ:
    case ZEND_JMPZ_EX:
    case ZEND_JMPNZ_EX:
      print_sink("sink(branch) {1} =l=> {opline}");
      break;
    /* opcode has misleading name: ADD means APPEND */
    case ZEND_ADD_CHAR:   /* (op2 must be a const char) */
    case ZEND_ADD_STRING: /* (op2 must be a const string) */
    case ZEND_ADD_VAR:    /* (may convert op2 to string) */
      print_sink("sink(zval:string) {2,result} =d=> {result}");
      break;
    case ZEND_ADD_ARRAY_ELEMENT: /* insert or append */
      print_sink("sink(zval:map) {1} =d=> {result[2]}");
      break;
    case ZEND_INIT_METHOD_CALL:
      print_sink("sink(edge) {1.2} =d=> {fcall-stack}");
      break;
    case ZEND_INIT_STATIC_METHOD_CALL:
      print_sink("sink(edge) {1.2} =d=> {fcall-stack}");
      break;
    case ZEND_INIT_FCALL:
    case ZEND_INIT_FCALL_BY_NAME:
    case ZEND_INIT_USER_CALL:
    case ZEND_INIT_NS_FCALL_BY_NAME:
      print_sink("sink(edge) {2} =d=> {fcall-stack}");
      break;
    case ZEND_NEW:
      print_sink("sink(edge) {1} =d=> {fcall-stack} (or skip via 2 => opline if no ctor)");
      break;
    case ZEND_DO_FCALL:
      if (is_db_sink_function(NULL, id.call_target))
        print_sink("sink(edge) {fcall-stack} =d=> {db,opline,result}");
      else if (is_file_sink_function(id.call_target))
        print_sink("sink(edge) {fcall-stack} =d=> {file,opline,result}");
      else if (is_system_sink_function(id.call_target))
        print_sink("sink(edge) {fcall-stack} =d=> {system,opline,result}");
      else
        print_sink("sink(edge) {fcall-stack} =d=> {opline,result}");
      break;
    case ZEND_INCLUDE_OR_EVAL:
      if (op->extended_value == ZEND_EVAL)
        print_sink("sink(edge) {1} =d=> {code,opline}");
      else
        print_sink("sink(edge) {1} =d=> {opline}, {file} =d=> {code}");
      break;
    case ZEND_CLONE:
      print_sink("sink(edge) {1} =d=> {opline,result}");
      break;
    case ZEND_THROW:
      print_sink("sink(edge) {1} =d=> {fast-ret,thrown}");
      break;
    case ZEND_HANDLE_EXCEPTION: /* makes fastcall to finally blocks */
      print_sink("sink(edge) {thrown} =i=> {opline}");
      break;
    case ZEND_DISCARD_EXCEPTION:
      print_sink("sink(edge) {thrown} =d=> {thrown}");
      break;
    case ZEND_FAST_CALL: /* call finally via no-arg fastcall (when not handling exception) */
      print_sink("sink(edge) {thrown} =d=> {fast-ret,opline}");
      break;
    case ZEND_FAST_RET: /* return from fastcall */
      print_sink("sink(edge) {fast-ret} =i=> {opline}");
      break;
    case ZEND_SEND_VAL:
    case ZEND_SEND_VAL_EX:
    case ZEND_SEND_VAR:
    case ZEND_SEND_VAR_NO_REF:
    case ZEND_SEND_REF:
    case ZEND_SEND_USER:
      print_sink("sink(zval) {1} =d=> {fcall-stack(2 is arg#)}");
      break;
    case ZEND_SEND_ARRAY:
      print_sink("sink(zval) {1} =d=> {fcall-stack(all args)}");
      break;
    case ZEND_RECV:
    case ZEND_RECV_VARIADIC:
      print_sink("sink(zval) {fcall-stack(arg)} =i=> {result}");
      break;
    case ZEND_BOOL:
    case ZEND_BOOL_NOT:
      print_sink("sink(zval) {1} =d=> {result}");
      break;
    case ZEND_BRK:
    case ZEND_CONT:
    case ZEND_GOTO:
      print_sink("sink(branch) {2} =i=> {opline}");
      break;
    case ZEND_CASE: /* execute case if 1 == 2 (via fast_equal_function) */
      print_sink("sink(zval:bool) {1,2} =l=> {result}");
      break;
    case ZEND_CAST:
      print_sink("sink(zval) {1,ext} =d=> {result}"); /* ext specifies cast dest type */
      break;
    case ZEND_FE_RESET: /* starts an iterator */
      print_sink("sink(zval:map) {1} =d=> {result} (or skips via 2 => opline if 1 is empty)");
      break;
    case ZEND_FE_FETCH: /* advances an iterator */
      print_sink("sink(zval:map) {1} =d=> {result(key), next_op.result(value)} "
                       "(or terminates via 2 => opline}");
      break;
    case ZEND_ISSET_ISEMPTY_VAR:
      if (op->op2_type == IS_UNUSED)
        print_sink("sink(zval:bool) {1} =d=> {result}"); // is this a superglobal source?
      else
        print_sink("sink(zval:bool) {2.1} =d=> {result}"); /* op2 must be static */
      break;
    case ZEND_ISSET_ISEMPTY_DIM_OBJ:
    case ZEND_ISSET_ISEMPTY_PROP_OBJ:
      print_sink("sink(zval:bool) {1.2} =d=> {result}");
      break;
    case ZEND_EXIT:
      if (op->op1_type == IS_UNUSED)
        print_sink("sink(edge) {} =d=> {opline}");
      else
        print_sink("sink(edge) {} =d=> {opline}, {1} =d=> {exit-code}");
      break;
    case ZEND_BEGIN_SILENCE:
    case ZEND_END_SILENCE:
      print_sink("sink(internal) {} =d=> {error-reporting}");
      break;
    case ZEND_TICKS:
      print_sink("sink(internal) {ext} =d=> {timer}");
      break;
    case ZEND_JMP_SET:
      print_sink("sink(branch) {1} =l=> {result,opline}"); /* NOP if op1 is false */
      break;
    case ZEND_COALESCE:
      print_sink("sink(branch) {1} =l=> {result,opline}"); /* NOP if op1 is NULL */
      break;
    case ZEND_QM_ASSIGN:
      print_sink("sink(zval?) {1} =?=> {result}");
      break;
    case ZEND_DECLARE_CLASS:
    case ZEND_DECLARE_INHERITED_CLASS:
      print_sink("sink(code) {1} =i=> {code}");
      break;
    case ZEND_DECLARE_INHERITED_CLASS_DELAYED: /* bind op1 only if op2 is unbound */
      print_sink("sink(code) {1,2} =i=> {code}");
      break;
    case ZEND_DECLARE_FUNCTION:
      print_sink("sink(code) {compiler} =i=> {code}");
      break;
    case ZEND_ADD_INTERFACE:
    case ZEND_ADD_TRAIT: /* add interface/trait op2 (with zv+1) to class op1 */
      print_sink("sink(code) {1(.[2,2.zv+1])} =i=> {code}");
      break;
    case ZEND_BIND_TRAITS:
      print_sink("sink(code) {1} =i=> {code}"); /* bind pending traits of op1 */
      break;
    case ZEND_SEPARATE:
      print_sink("sink(zval) {1} =d=> {op1}"); /* unbinds op1 somehow */
      break;
    case ZEND_DECLARE_CONST:
      print_sink("sink(zval) {1.2} =d=> {global}");
      break;
    case ZEND_BIND_GLOBAL: /* binds value in op2 to global named op1 */
      print_sink("sink(global) {2} =d=> {1}");
      break;
    case ZEND_STRLEN:
      print_sink("sink(zval:number) {1} =d=> {result}");
      break;

/* ======== SINK TODO ======= *

  ZEND_CAST
  ZEND_QM_ASSIGN

  ZEND_FETCH_FUNC_ARG?
  ZEND_FETCH_DIM_FUNC_ARG?
  ZEND_FETCH_OBJ_FUNC_ARG?

  ZEND_FETCH_LIST

  ZEND_RETURN_BY_REF
  ZEND_GENERATOR_RETURN

  ZEND_FETCH_CLASS

  ZEND_SEND_UNPACK

  ZEND_EXT_STMT
  ZEND_EXT_FCALL_BEGIN
  ZEND_EXT_FCALL_END

  ZEND_YIELD

  ZEND_DECLARE_LAMBDA_FUNCTION -- tricky scope rules
*/

/* ======= SINK NOP ======= *

  ZEND_NOP
  ZEND_EXT_NOP
  ZEND_RETURN
  ZEND_JMP
  ZEND_FREE
  ZEND_DO_FCALL
  ZEND_VERIFY_ABSTRACT_CLASS
  ZEND_USER_OPCODE   -- customizable opcode handlers...!
*/

  }
}

#define DB_STMT_TYPE "mysqli_stmt"
#define DB_STMT_PREFIX COMPILED_ROUTINE_DEFAULT_SCOPE":mysqli_stmt_"
#define DB_STMT_PREFIX_LEN (strlen(DB_STMT_PREFIX))

static bool is_db_stmt_function_base(const char *type, const char *name)
{
  if (type == NULL) {
    if (strncmp(name, DB_STMT_PREFIX, DB_STMT_PREFIX_LEN) != 0)
      return false;
  } else {
    if (strcmp(type, DB_STMT_PREFIX) != 0)
      return false;
  }
  return true;
}

#define DB_SQL_TYPE "mysqli"
#define DB_SQL_PREFIX COMPILED_ROUTINE_DEFAULT_SCOPE":mysqli_"
#define DB_SQL_PREFIX_LEN (strlen(DB_SQL_PREFIX))

static bool is_db_sql_function_base(const char *type, const char *name)
{
  if (is_db_stmt_function_base(type, name))
    return false;

  if (type == NULL) {
    if (strncmp(name, DB_SQL_PREFIX, DB_SQL_PREFIX_LEN) != 0)
      return false;
  } else {
    if (strcmp(type, DB_SQL_TYPE) != 0)
      return false;
  }

  return true;
}

static bool is_db_sql_source_function(const char *name)
{ /* separate cases to allow for a possible enum mapping later on */
  if (strcmp(name, "connect") == 0) {
    return true;
  } else if (strcmp(name, "fetch_field") == 0) {
    return true;
  } else if (strcmp(name, "fetch_fields") == 0) {
    return true;
  } else if (strcmp(name, "fetch_field_direct") == 0) {
    return true;
  } else if (strcmp(name, "fetch_lengths") == 0) {
    return true;
  } else if (strcmp(name, "fetch_all") == 0) {
    return true;
  } else if (strcmp(name, "fetch_array") == 0) {
    return true;
  } else if (strcmp(name, "fetch_assoc") == 0) {
    return true;
  } else if (strcmp(name, "fetch_object") == 0) {
    return true;
  } else if (strcmp(name, "fetch_row") == 0) {
    return true;
  } else if (strcmp(name, "field_count") == 0) {
    return true;
  } else if (strcmp(name, "field_seek") == 0) {
    return true;
  } else if (strcmp(name, "field_tell") == 0) {
    return true;
  } else if (strcmp(name, "insert_id") == 0) {
    return true;
  } else if (strcmp(name, "next_result") == 0) {
    return true;
  } else if (strcmp(name, "num_fields") == 0) {
    return true;
  } else if (strcmp(name, "num_rows") == 0) {
    return true;
  } else if (strcmp(name, "reap_async_query") == 0) {
    return true;
  }
  return false;
}

static bool is_db_stmt_source_function(const char *name)
{
  if (strcmp(name, "bind_result") == 0) {
    return true;
  } else if (strcmp(name, "data_seek") == 0) {
    return true;
  } else if (strcmp(name, "fetch") == 0) {
    return true;
  } else if (strcmp(name, "field_count") == 0) {
    return true;
  } else if (strcmp(name, "get_result") == 0) {
    return true;
  } else if (strcmp(name, "insert_id") == 0) {
    return true;
  } else if (strcmp(name, "more_results") == 0) {
    return true;
  } else if (strcmp(name, "next_result") == 0) {
    return true;
  } else if (strcmp(name, "num_rows") == 0) {
    return true;
  } else if (strcmp(name, "param_count") == 0) {
    return true;
  } else if (strcmp(name, "result_metadata") == 0) {
    return true;
  }
  return false;
}

bool is_db_source_function(const char *type, const char *name)
{
  return (is_db_sql_function_base(type, name) &&
          is_db_sql_source_function(type == NULL ? name + DB_SQL_PREFIX_LEN : name)) ||
         (is_db_stmt_function_base(type, name) &&
          is_db_stmt_source_function(type == NULL ? name + DB_STMT_PREFIX_LEN : name));
}

static bool is_db_sql_sink_function(const char *name)
{ /* separate cases to allow for a possible enum mapping later on */
  if (strcmp(name, "multi_query") == 0) {
    return true;
  } else if (strcmp(name, "prepare") == 0) {
    return true;
  } else if (strcmp(name, "query") == 0) { /* not a source: only the fetch functions are sources */
    return true;
  } else if (strcmp(name, "multi_query") == 0) {
    return true;
  } else if (strcmp(name, "select_db") == 0) {
    return true;
  }
  return false;
}

static bool is_db_stmt_sink_function(const char *name)
{
  if (strcmp(name, "attr_get") == 0) {
    return true;
  } else if (strcmp(name, "attr_set") == 0) {
    return true;
  } else if (strcmp(name, "bind_param") == 0) {
    return true;
  } else if (strcmp(name, "prepare") == 0) {
    return true;
  } else if (strcmp(name, "reset") == 0) {
    return true;
  } else if (strcmp(name, "send_long_data") == 0) {
    return true;
  }
  return false;
}

bool is_db_sink_function(const char *type, const char *name)
{
  return (is_db_sql_function_base(type, name) &&
          is_db_sql_sink_function(type == NULL ? name + DB_SQL_PREFIX_LEN : name)) ||
         (is_db_stmt_function_base(type, name) &&
          is_db_stmt_sink_function(type == NULL ? name + DB_STMT_PREFIX_LEN : name));
}

bool is_file_source_function(const char *name)
{
  if (strcmp(name, "dir") == 0) {
    return true;
  } else if (strcmp(name, "getcwd") == 0) {
    return true;
  } else if (strcmp(name, "readdir") == 0) {
    return true;
  } else if (strcmp(name, "scandir") == 0) {
    return true;
  }
  return false;
}

bool is_file_sink_function(const char *name)
{
  if (strcmp(name, "chdir") == 0) {
    return true;
  } else if (strcmp(name, "chroot") == 0) {
    return true;
  } else if (strcmp(name, "closedir") == 0) {
    return true;
  } else if (strcmp(name, "opendir") == 0) {
    return true;
  } else if (strcmp(name, "rewinddir") == 0) {
    return true;
  }
  return false;
}

bool is_system_source_function(const char *name)
{
  if (strcmp(name, "shmop_read") == 0) {
    return true;
  } else if (strcmp(name, "shmop_size") == 0) {
    return true;
  } else if (strcmp(name, "expect_expectl") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_alarm") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_errno") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_get_last_err") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_getpriority") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_strerror") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_wexitstatus") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_wifexited") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_wifsignaled") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_wifstopped") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_wstopsig") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_wtermsig") == 0) {
    return true;
  } else if (strcmp(name, "basename") == 0) {
    return true;
  } else if (strcmp(name, "dirname") == 0) {
    return true;
  } else if (strcmp(name, "disk_free_space") == 0) {
    return true;
  } else if (strcmp(name, "disk_total_space") == 0) {
    return true;
  } else if (strcmp(name, "diskfreespace") == 0) {
    return true;
  } else if (strcmp(name, "feof") == 0) {
    return true;
  } else if (strcmp(name, "fgetc") == 0) {
    return true;
  } else if (strcmp(name, "fgetcsv") == 0) {
    return true;
  } else if (strcmp(name, "fgets") == 0) {
    return true;
  } else if (strcmp(name, "fgetss") == 0) {
    return true;
  } else if (strcmp(name, "file_exists") == 0) {
    return true;
  } else if (strcmp(name, "file_get_contents") == 0) {
    return true;
  } else if (strcmp(name, "file") == 0) {
    return true;
  } else if (strcmp(name, "fileatime") == 0) {
    return true;
  } else if (strcmp(name, "filectime") == 0) {
    return true;
  } else if (strcmp(name, "filegroup") == 0) {
    return true;
  } else if (strcmp(name, "fileinode") == 0) {
    return true;
  } else if (strcmp(name, "filemtime") == 0) {
    return true;
  } else if (strcmp(name, "fileowner") == 0) {
    return true;
  } else if (strcmp(name, "fileperms") == 0) {
    return true;
  } else if (strcmp(name, "filesize") == 0) {
    return true;
  } else if (strcmp(name, "filetype") == 0) {
    return true;
  } else if (strcmp(name, "fnmatch") == 0) {
    return true;
  } else if (strcmp(name, "fread") == 0) {
    return true;
  } else if (strcmp(name, "fscanf") == 0) {
    return true;
  } else if (strcmp(name, "fstat") == 0) {
    return true;
  } else if (strcmp(name, "ftell") == 0) {
    return true;
  } else if (strcmp(name, "glob") == 0) {
    return true;
  } else if (strcmp(name, "is_dir") == 0) {
    return true;
  } else if (strcmp(name, "is_executable") == 0) {
    return true;
  } else if (strcmp(name, "is_file") == 0) {
    return true;
  } else if (strcmp(name, "is_link") == 0) {
    return true;
  } else if (strcmp(name, "is_readable") == 0) {
    return true;
  } else if (strcmp(name, "is_uploaded_file") == 0) {
    return true;
  } else if (strcmp(name, "is_writable") == 0) {
    return true;
  } else if (strcmp(name, "is_writeable") == 0) {
    return true;
  } else if (strcmp(name, "linkinfo") == 0) {
    return true;
  } else if (strcmp(name, "lstat") == 0) {
    return true;
  } else if (strcmp(name, "parse_ini_file") == 0) {
    return true;
  } else if (strcmp(name, "parse_ini_string") == 0) {
    return true;
  } else if (strcmp(name, "pathinfo") == 0) {
    return true;
  } else if (strcmp(name, "readfile") == 0) {
    return true;
  } else if (strcmp(name, "readlink") == 0) {
    return true;
  } else if (strcmp(name, "realpath_cache_get") == 0) {
    return true;
  } else if (strcmp(name, "realpath_cache_size") == 0) {
    return true;
  } else if (strcmp(name, "realpath") == 0) {
    return true;
  } else if (strcmp(name, "stat") == 0) {
  //} else if (strcmp(name, "") == 0) {
  }
  return false;
}

bool is_system_sink_function(const char *name)
{
  if (strcmp(name, "shmop_close") == 0) {
    return true;
  } else if (strcmp(name, "shmop_delete") == 0) {
    return true;
  } else if (strcmp(name, "shmop_open") == 0) {
    return true;
  } else if (strcmp(name, "shmop_write") == 0) {
    return true;
  } else if (strcmp(name, "expect_popen") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_exec") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_fork") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_setpriority") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_signal_dispatch") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_signal") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_sigprocmask") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_sigtimedwait") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_sigwaitinfo") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_wait") == 0) {
    return true;
  } else if (strcmp(name, "pcntl_waitpid") == 0) {
    return true;
  } else if (strcmp(name, "chgrp") == 0) {
    return true;
  } else if (strcmp(name, "chmod") == 0) {
    return true;
  } else if (strcmp(name, "chown") == 0) {
    return true;
  } else if (strcmp(name, "clearstatcache") == 0) {
    return true;
  } else if (strcmp(name, "copy") == 0) {
    return true;
  } else if (strcmp(name, "delete") == 0) {
    return true;
  } else if (strcmp(name, "fclose") == 0) {
    return true;
  } else if (strcmp(name, "fflush") == 0) {
    return true;
  } else if (strcmp(name, "file_put_contents") == 0) {
    return true;
  } else if (strcmp(name, "flock") == 0) {
    return true;
  } else if (strcmp(name, "fopen") == 0) {
    return true;
  } else if (strcmp(name, "fpassthru") == 0) {
    return true;
  } else if (strcmp(name, "fputcsv") == 0) {
    return true;
  } else if (strcmp(name, "fputs") == 0) {
    return true;
  } else if (strcmp(name, "fseek") == 0) {
    return true;
  } else if (strcmp(name, "ftruncate") == 0) {
    return true;
  } else if (strcmp(name, "fwrite") == 0) {
    return true;
  } else if (strcmp(name, "lchgrp") == 0) {
    return true;
  } else if (strcmp(name, "lchown") == 0) {
    return true;
  } else if (strcmp(name, "link") == 0) {
    return true;
  } else if (strcmp(name, "mkdir") == 0) {
    return true;
  } else if (strcmp(name, "move_uploaded_file") == 0) {
    return true;
  } else if (strcmp(name, "pclose") == 0) {
    return true;
  } else if (strcmp(name, "popen") == 0) {
    return true;
  } else if (strcmp(name, "rename") == 0) {
    return true;
  } else if (strcmp(name, "rewind") == 0) {
    return true;
  } else if (strcmp(name, "rmdir") == 0) {
    return true;
  } else if (strcmp(name, "set_file_buffer") == 0) {
    return true;
  } else if (strcmp(name, "symlink") == 0) {
    return true;
  } else if (strcmp(name, "tempnam") == 0) {
    return true;
  } else if (strcmp(name, "tmpfile") == 0) {
    return true;
  } else if (strcmp(name, "touch") == 0) {
    return true;
  } else if (strcmp(name, "umask") == 0) {
    return true;
  } else if (strcmp(name, "unlink") == 0) {
    return true;
  }
  return false;
}

/**************************************************************************************
 Static Dataflow Analysis
 **************************************************************************************/

typedef struct _dataflow_file_t {
  char *path;
  bool analyzed;
  zend_op_array *zops;
  struct _dataflow_file_t *next;
} dataflow_file_t;

typedef struct _dataflow_graph_t {
  sctable_t routines;
} dataflow_graph_t;

typedef struct _dataflow_routine_t {
  uint routine_hash;
  scarray_t opcodes;
} dataflow_routine_t;

static dataflow_file_t *dataflow_file_list = NULL;
static dataflow_graph_t *dg = NULL;
static dataflow_routine_t *assembling_routine = NULL;

static int analyze_file_dataflow(zend_file_handle *in, dataflow_file_t *out)
{
  int retval = FAILURE;

  SPOT("analyze_file_dataflow() for file %s\n", in->filename);

  zend_try {
    out->zops = zend_compile_file(in, ZEND_INCLUDE TSRMLS_CC);
    zend_destroy_file_handle(in TSRMLS_CC);
    if (out->zops != NULL)
      retval = SUCCESS;
  } zend_end_try();

  return retval;
}

static void link_operand_dataflow(dataflow_file_t *top_file)
{
  uint i;
  zend_op *zop;

  for (i = 0; i < top_file->zops->last; i++) {
    switch (
  }
}

int analyze_dataflow(zend_file_handle *file)
{
  int retval;
  dataflow_file_t top_file;
  dataflow_file_t *next_file;

  SPOT("static_dataflow() for file %s\n", file->filename);

  dg = malloc(sizeof(dataflow_graph_t));
  dg->routines.hash_bits = 9;
  sctable_init(&dg->routines);

  retval = analyze_file_dataflow(file, &top_file);

  while (retval == SUCCESS) {
    zend_file_handle file;

    next_file = dataflow_file_list;
    while (next_file != NULL) {
      if (!next_file->analyzed)
        break;
      next_file = next_file->next;
    }

    if (next_file == NULL)
      break;

    next_file->analyzed = true;
    retval = zend_stream_open(next_file->path, &file);
    if (retval != SUCCESS) {
      ERROR("Failed to locate included file '%s'\n", next_file->path);
      continue; // allow bad includes
    }

    retval = analyze_file_dataflow(&file, next_file);
  }

  link_operand_dataflow(&top_file);

  if (top_file.zops != NULL) {
    destroy_op_array(top_file.zops TSRMLS_CC);
    efree(top_file.zops);
  }
  next_file = dataflow_file_list;
  while (next_file != NULL) {
    dataflow_file_list = next_file->next;
    if (next_file->zops != NULL) {
      destroy_op_array(next_file->zops TSRMLS_CC);
      efree(next_file->zops);
    }
    free(next_file->path);
    free(next_file);
    next_file = dataflow_file_list;
  }

  return retval;
}

void destroy_dataflow_analysis()
{
  if (opcode_dump_file != NULL)
    fclose(opcode_dump_file);

  if (dg != NULL) {
    sctable_destroy(&dg->routines);
    free(dg);
  }
}

void add_dataflow_routine(uint routine_hash)
{
  dataflow_routine_t *routine = malloc(sizeof(dataflow_routine_t));
  routine->routine_hash = routine_hash;
  scarray_init(&routine->opcodes);
  sctable_add(&dg->routines, routine_hash, routine);

  assembling_routine = routine;
}

static void initilize_dataflow_operand(dataflow_operand_t *operand, zend_op_array *zops,
                                       znode_op *zop, zend_uchar type)
{
  switch (type) {
    case IS_CONST:
      operand->value.type = DATAFLOW_VALUE_TYPE_CONST;
      operand->value.constant = zop->zv;
      break;
    case IS_VAR:
    case IS_TMP_VAR:
      operand->value.type = DATAFLOW_VALUE_TYPE_TEMP;
      operand->value.temp_id = (uint) (EX_VAR_TO_NUM(zop->var) - zops->last_var);
      break;
    case IS_CV:
      operand->value.var_name = zops->vars[EX_VAR_TO_NUM(zop->var)]->val;
      if (strcmp("this", operand->value.var_name) == 0) // maybe...?
        operand->value.type = DATAFLOW_VALUE_TYPE_THIS;
      else
        operand->value.type = DATAFLOW_VALUE_TYPE_VAR;
      break;
    case IS_UNUSED:
    default:
      operand->value.type = DATAFLOW_VALUE_TYPE_NONE;
  }
}

static void initialize_opcode_index(uint routine_hash, uint index, dataflow_routine_t **routine_out,
                                    dataflow_opcode_t **opcode_out)
{
  dataflow_routine_t *routine;
  dataflow_opcode_t *opcode;

  if (assembling_routine != NULL && assembling_routine->routine_hash == routine_hash)
    routine = assembling_routine;
  else
    routine = sctable_lookup(&dg->routines, routine_hash);

  while (index >= routine->opcodes.size) {
    opcode = malloc(sizeof(dataflow_opcode_t));
    memset(opcode, 0, sizeof(dataflow_opcode_t));
    scarray_append(&routine->opcodes, opcode);
  }

  opcode = (dataflow_opcode_t *) scarray_get(&routine->opcodes, index);
  opcode->id.routine_hash = routine_hash;
  opcode->id.op_index = index;

  *routine_out = routine;
  *opcode_out = opcode;
}

static inline void initialize_sink(dataflow_opcode_t *opcode, dataflow_sink_type_t type)
{
  opcode->sink.id = opcode->id;
  opcode->sink.type = type;
}

static inline void initialize_source(dataflow_opcode_t *opcode, dataflow_source_type_t type)
{
  opcode->source.id = opcode->id;
  opcode->source.type = type;
}

void add_dataflow_opcode(uint routine_hash, uint index, zend_op_array *zops)
{
  dataflow_routine_t *routine;
  dataflow_opcode_t *opcode;
  zend_op *zop = &zops->opcodes[index];

  initialize_opcode_index(routine_hash, index, &routine, &opcode);

  initilize_dataflow_operand(&opcode->op1, zops, &zop->op1, zop->op1_type);
  initilize_dataflow_operand(&opcode->op2, zops, &zop->op2, zop->op2_type);
  initilize_dataflow_operand(&opcode->result, zops, &zop->result, zop->result_type);

  switch (zop->opcode) {
    case ZEND_ECHO:
    case ZEND_PRINT:
      opcode->sink.id = opcode->id;
      opcode->sink.type = SINK_TYPE_OUTPUT;
      break;
    case ZEND_JMPZ:
    case ZEND_JMPNZ:
    case ZEND_JMPZNZ:
    case ZEND_JMPZ_EX:
    case ZEND_JMPNZ_EX:
    case ZEND_BRK:
    case ZEND_CONT:
    case ZEND_GOTO:
      initialize_sink(opcode, SINK_TYPE_EDGE);
      break;
    case ZEND_JMP_SET:
      // dunno how this works
      break;
    case ZEND_COALESCE:
      // dunno how this works
      break;
    case ZEND_INCLUDE_OR_EVAL:
      if (zop->extended_value == ZEND_EVAL) // else handled in add_dataflow_include
        initialize_sink(opcode, SINK_TYPE_CODE);
      break;
    case ZEND_DECLARE_CLASS:
    case ZEND_DECLARE_INHERITED_CLASS:
    case ZEND_DECLARE_INHERITED_CLASS_DELAYED:
    case ZEND_DECLARE_FUNCTION:
    case ZEND_ADD_INTERFACE:
    case ZEND_ADD_TRAIT:
      initialize_sink(opcode, SINK_TYPE_CODE);
      break;
    case ZEND_BIND_TRAITS:
    case ZEND_FETCH_R:  /* fetch a superglobal */
    case ZEND_FETCH_W:
    case ZEND_FETCH_RW:
    case ZEND_FETCH_IS:
      if (zop->op2_type == IS_UNUSED) {
        const char *superglobal_name = Z_STRVAL_P(zop->op1.zv);
        if (superglobal_name != NULL) {
          if (strcmp(superglobal_name, "_SESSION") == 0) {
            initialize_source(opcode, SOURCE_TYPE_SESSION);
            break;
          } else if (strcmp(superglobal_name, "_REQUEST") == 0 ||
                     strcmp(superglobal_name, "_GET") == 0 ||
                     strcmp(superglobal_name, "_POST") == 0 ||
                     strcmp(superglobal_name, "_COOKIE") == 0 ||
                     strcmp(superglobal_name, "_FILES") == 0) {
            initialize_source(opcode, SOURCE_TYPE_HTTP);
            break;
          } else if (strcmp(superglobal_name, "_ENV") == 0 ||
                     strcmp(superglobal_name, "_SERVER") == 0) {
            initialize_source(opcode, SOURCE_TYPE_SYSTEM);
          }
        }
      }
      break;
    case ZEND_RECV:
    case ZEND_RECV_VARIADIC:
      initialize_source(opcode, SOURCE_TYPE_ARG);
      break;
    case ZEND_DECLARE_CONST:
    case ZEND_BIND_GLOBAL:
      initialize_sink(opcode, SINK_TYPE_GLOBAL);
      break;
  }
}

void add_dataflow_fcall(uint routine_hash, uint index, zend_op_array *zops,
                        const char *routine_name)
{
  dataflow_routine_t *routine;
  dataflow_opcode_t *opcode;
  zend_op *zop = &zops->opcodes[index];

  initialize_opcode_index(routine_hash, index, &routine, &opcode);

  if (uses_return_value(zop)) {
    if (is_db_source_function(NULL, routine_name))
      initialize_source(opcode, SOURCE_TYPE_SQL);
    else if (is_db_sink_function(NULL, routine_name))
      initialize_sink(opcode, SINK_TYPE_SQL);
    else if (is_file_source_function(routine_name))
      initialize_source(opcode, SOURCE_TYPE_FILE);
    else if (is_file_sink_function(routine_name))
      initialize_sink(opcode, SINK_TYPE_FILE);
    else if (is_system_source_function(routine_name))
      initialize_source(opcode, SOURCE_TYPE_SYSTEM);
    else if (is_system_sink_function(routine_name))
      initialize_sink(opcode, SINK_TYPE_SYSTEM);
  }
}

void add_dataflow_fcall_arg(uint routine_hash, uint index, zend_op_array *zops,
                        const char *routine_name)
{
  dataflow_routine_t *routine;
  dataflow_opcode_t *opcode;

  initialize_opcode_index(routine_hash, index, &routine, &opcode);

  opcode->sink.id.routine_hash = routine_hash;
  opcode->sink.id.op_index = index;
  opcode->sink.type = SINK_TYPE_CALL;
  opcode->sink.call.type = DATAFLOW_CALL_TYPE_BY_NAME;
  opcode->sink.call.call_by_name.name = routine_name;
}

void add_dataflow_map_assignment(uint routine_hash, uint index, zend_op_array *zops)
{
  dataflow_routine_t *routine;
  dataflow_opcode_t *opcode;
  zend_op *zop1 = &zops->opcodes[index];
  zend_op *zop2 = &zops->opcodes[index+1];

  initialize_opcode_index(routine_hash, index, &routine, &opcode);

  if (zop1->op1_type == IS_UNUSED) {
    opcode->map.value.type = DATAFLOW_VALUE_TYPE_THIS;
    opcode->map.value.class = zops->scope; // do we need zend_op_array.this_var (index of CV)?
  } else {
    initilize_dataflow_operand(&opcode->map, zops, &zop1->op1, zop1->op1_type);
  }
  initilize_dataflow_operand(&opcode->key, zops, &zop1->op2, zop1->op2_type);
  initilize_dataflow_operand(&opcode->value, zops, &zop2->op1, zop2->op1_type);
}

void add_dataflow_foreach_fetch(uint routine_hash, uint index, zend_op_array *zops)
{
  dataflow_routine_t *routine;
  dataflow_opcode_t *opcode;
  zend_op *zop1 = &zops->opcodes[index];
  zend_op *zop2 = &zops->opcodes[index+1];

  initialize_opcode_index(routine_hash, index, &routine, &opcode);

  if (zop1->op1_type == IS_UNUSED) {
    opcode->map.value.type = DATAFLOW_VALUE_TYPE_THIS;
    opcode->map.value.class = zops->scope; // do we need zend_op_array.this_var (index of CV)?
  } else {
    initilize_dataflow_operand(&opcode->map, zops, &zop1->op1, zop1->op1_type);
  }
  initilize_dataflow_operand(&opcode->key, zops, &zop1->op2, zop1->op2_type);
  initilize_dataflow_operand(&opcode->value, zops, &zop2->op1, zop2->op1_type);
}

void add_dataflow_include(uint routine_hash, uint index, zend_op_array *zops,
                          const char *include_path, uint include_hash)
{
  dataflow_routine_t *routine;
  dataflow_opcode_t *opcode;
  dataflow_file_t *file = dataflow_file_list;
  bool found = false;

  initialize_opcode_index(routine_hash, index, &routine, &opcode);
  initialize_source(opcode, SOURCE_TYPE_INCLUDE);
  opcode->source.include.routine_hash = include_hash;

  SPOT("Add static dataflow include %s\n", include_path);

  while (file != NULL) {
    if (strcmp(include_path, file->path) == 0) {
      found = true;
      break;
    }
    file = file->next;
  }

  if (found)
    return;

  file = malloc(sizeof(dataflow_file_t));
  file->path = strdup(include_path);
  file->analyzed = false;
  file->next = dataflow_file_list;
  dataflow_file_list = file;
}
