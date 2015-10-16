#include "php.h"
#include "cfg.h"
#include "cfg_handler.h"
#include "lib/script_cfi_utils.h"
#include "operand_resolver.h"

typedef struct _application_list_t {
  application_t app;
  uint root_length;
  struct _application_list_t *next;
} application_list_t;

static application_list_t *application_list = NULL;
static application_t unknown_app = { "unknown", "/", NULL, NULL, NULL };

#define SITE_ROOTS_FILENAME "opmon.site.roots"
#define SITE_ROOTS_FILENAME_LEN 17

void init_operand_resolver()
{
  unknown_app.cfg = cfg_new();
  cfg_initialize_application(&unknown_app);
}

void destroy_operand_resolver()
{
  application_list_t *next_app, *app = application_list;
  while (app != NULL) {
    next_app = app->next;
    cfg_destroy_application(&app->app);
    free((char *) app->app.root);
    free((char *) app->app.name);
    cfg_free(app->app.cfg);
    free(app);
    app = next_app;
  }
  cfg_free(unknown_app.cfg);
}

const char *resolve_constant_include(zend_op *op)
{
	zval *inc_filename = op->op1.zv;
	zval tmp_inc_filename;
  char *resolved_path, *return_path;

	ZVAL_UNDEF(&tmp_inc_filename);
	if (Z_TYPE_P(inc_filename) != IS_STRING) {
		ZVAL_STR(&tmp_inc_filename, zval_get_string(inc_filename));
		inc_filename = &tmp_inc_filename;
	}

  resolved_path = zend_resolve_path(Z_STRVAL_P(inc_filename), (int)Z_STRLEN_P(inc_filename)); // TSRMLS_CC);
  if (resolved_path) {
    return_path = malloc(strlen(resolved_path) + 1);
    strcpy(return_path, resolved_path);
    efree(resolved_path);

    SPOT("malloc %ld bytes for return_path %s\n",
         strlen(resolved_path) + 1, return_path);
  } else {
    const char *val = Z_STRVAL_P(inc_filename);
    size_t length = Z_STRLEN_P(inc_filename) + 1;
    return_path = malloc(length);
    strcpy(return_path, val);

    PRINT("malloc %ld bytes for return_path %s\n",
          length, return_path);
  }

	if (Z_TYPE(tmp_inc_filename) != IS_UNDEF) {
		zval_ptr_dtor(&tmp_inc_filename);
	}

  return return_path;
}

char *resolve_eval_body(zend_op *op)
{
	zval *eval_body_z;
	zval tmp_eval_body_z;
  char *eval_body;

	eval_body_z = op->op1.zv;

	ZVAL_UNDEF(&tmp_eval_body_z);
	if (Z_TYPE_P(eval_body_z) != IS_STRING) {
		ZVAL_STR(&tmp_eval_body_z, zval_get_string(eval_body_z));
		eval_body_z = &tmp_eval_body_z;
	}

  eval_body = malloc(Z_STRLEN_P(eval_body_z) + 1);
  strcpy(eval_body, Z_STRVAL_P(eval_body_z));

	if (Z_TYPE(tmp_eval_body_z) != IS_UNDEF) {
		zval_ptr_dtor(&tmp_eval_body_z);
	}

  return eval_body;
}

static application_t *new_site_app(char *buffer)
{
  application_list_t *new_app;
  const char *app_name;
  char *new_app_name;

  new_app = malloc(sizeof(application_list_t));

  app_name = strrchr(buffer, '/');
  if (app_name == NULL)
    app_name = buffer;
  else
    app_name++;
  new_app_name = malloc(strlen(app_name) + 1);
  strcpy(new_app_name, app_name);
  new_app->app.name = new_app_name;

  if (buffer[strlen(buffer) - 1] != '/')
    strcat(buffer, "/");
  new_app->app.root = buffer;
  new_app->root_length = strlen(buffer);

  new_app->app.cfg = cfg_new();
  cfg_initialize_application(&new_app->app);

  new_app->next = application_list;
  application_list = new_app;
  return &new_app->app;
}

static application_t *lookup_application(const char *filename /*absolute path*/)
{
  uint length = strlen(filename);
  application_list_t *next_app = application_list;

  // return existing app if there is one
  while (next_app != NULL) {
    if (length > next_app->root_length &&
        strncmp(next_app->app.root, filename, next_app->root_length) == 0)
      return &next_app->app;
    next_app = next_app->next;
  }

  return NULL;
}

application_t *locate_application(const char *filename /*absolute path*/)
{
  char *buffer, *parent_dir;
  uint length = strlen(filename);
  struct stat file_info;

  application_t *app = lookup_application(filename);
  if (app != NULL)
    return app;

  buffer = malloc(length + SITE_ROOTS_FILENAME_LEN + 1);
  strcpy(buffer, filename);

  if (strcmp(buffer + length - 5, ".phar") == 0) {
    strcpy(buffer, "phar://");
    strcat(buffer, filename);
    return new_site_app(buffer);
  }

  while ((parent_dir = strrchr(buffer, '/')) != NULL) {
    parent_dir[1] = '\0'; // truncate after trailing slash
    strcat(buffer, SITE_ROOTS_FILENAME);
    if (stat(buffer, &file_info) == 0) {
      FILE *roots = fopen(buffer, "rt");
      char line[128], *path;
      int line_length;

      parent_dir[0] = '\0'; // remove filename and trailing slash from `buffer`
      while (fgets(line, 128, roots) != NULL) {
        line_length = strlen(line);
        if (line_length == 0)
          continue;

        line[line_length-1] = '\0';
        if (strcmp(line, ".") == 0) {
          path = strdup(buffer);
        } else {
          path = malloc(strlen(buffer) + line_length + 2);
          strcpy(path, buffer);
          if (path[strlen(path)-1] != '/')
            strcat(path, "/");
          strcat(path, line);
          if (path[strlen(path)-1] == '/')
            path[strlen(path)-1] = '\0';
        }
        new_site_app(path);
      }
      fclose(roots);
      break;
    }
    parent_dir[0] = '\0'; // remove filename and trailing slash
  }

  free(buffer);

  app = lookup_application(filename);
  if (app != NULL)
    return app;

  return &unknown_app;
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

void dump_operand(FILE *file, char index, znode_op *operand, zend_uchar type)
{
  fprintf(file, "[%c|", index);
  switch (type) {
    case IS_CONST:
      switch (operand->zv->u1.v.type) {
        case IS_UNDEF:
          fprintf(file, "const <undefined-type>");
          break;
        case IS_NULL:
          fprintf(file, "const");
          break;
        case IS_FALSE:
          fprintf(file, "const");
          break;
        case IS_TRUE:
          fprintf(file, "const true");
          break;
        case IS_LONG:
          fprintf(file, "const 0x%lx", operand->zv->value.lval);
          break;
        case IS_DOUBLE:
          fprintf(file, "const %f", operand->zv->value.dval);
          break;
        case IS_STRING: {
          uint i, j;
          char buffer[32] = {0};
          const char *str = Z_STR_P(operand->zv)->val;

          for (i = 0, j = 0; i < 31; i++) {
            if (str[i] == '\0')
              break;
            if (str[i] != '\n')
              buffer[j++] = str[i];
          }
          fprintf(file, "\"%s\"", buffer);
        } break;
        case IS_ARRAY:
          fprintf(file, "const array? (zv:"PX")", p2int(operand->zv));
          break;
        case IS_OBJECT:
          fprintf(file, "const object? (zv:"PX")", p2int(operand->zv));
          break;
        case IS_RESOURCE:
          fprintf(file, "const resource? (zv:"PX")", p2int(operand->zv));
          break;
        case IS_REFERENCE:
          fprintf(file, "const reference? (zv:"PX")", p2int(operand->zv));
          break;
        default:
          fprintf(file, "const what?? (zv:"PX")", p2int(operand->zv));
          break;
      }
      break;
    case IS_TMP_VAR:
    case IS_VAR:
    case IS_CV:
      fprintf(file, "var #%d", (uint)(operand->var / sizeof(zval *)));
      break;
    default:
      fprintf(file, "?");
  }
  fprintf(file, "]");
}

static void dump_opcode_header(FILE *file, zend_op *op)
{
  fprintf(file, "\t%04d: 0x%02x %s  ", op->lineno, op->opcode,
          zend_get_opcode_name(op->opcode));
}

void dump_fcall_opcode(FILE *file, zend_op *op, const char *routine_name)
{
  dump_opcode_header(file, op);
  if (uses_return_value(op)) {
    dump_operand(file, 'r', &op->result, op->result_type);
    fprintf(file, " = ");
  }
  fprintf(file, "%s\n", routine_name);
}

void dump_fcall_arg(FILE *file, zend_op *op, const char *routine_name)
{
  dump_opcode_header(file, op);
  dump_operand(file, 'a', &op->op1, op->op1_type);
  if (op->opcode == ZEND_SEND_ARRAY)
    fprintf(file, " -*-> %s\n", routine_name);
  else
    fprintf(file, " -%d-> %s\n", op->op2.num, routine_name);
}

void dump_map_assignment(FILE *file, zend_op *op, zend_op *next_op)
{
  dump_opcode_header(file, op);
  if (op->op1_type == IS_UNUSED)
    fprintf(file, "$this");
  else
    dump_operand(file, 'b', &op->op1, IS_CV);
  fprintf(file, ".");
  dump_operand(file, 'f', &op->op2, op->op2_type);
  fprintf(file, " = ");
  dump_operand(file, 'v', &next_op->op1, next_op->op1_type);
  fprintf(file, "\n");
}

void dump_foreach_fetch(FILE *file, zend_op *op, zend_op *next_op)
{
  dump_opcode_header(file, op);
  fprintf(file, " in ");
  dump_operand(file, 'a', &op->op1, op->op1_type);
  fprintf(file, ": ");
  dump_operand(file, 'k', &next_op->result, next_op->result_type);
  fprintf(file, ", ");
  dump_operand(file, 'v', &op->result, op->result_type);
  fprintf(file, "\n");
}

void dump_opcode(FILE *file, zend_op *op)
{
  zend_op *jump_target = NULL;
  const char *jump_reason = NULL;

  dump_opcode_header(file, op);

  if (uses_return_value(op)) {
    dump_operand(file, 'r', &op->result, op->result_type);
    fprintf(file, " = ");
    if (op->op1_type == IS_UNUSED && op->op2_type == IS_UNUSED) {
      switch (op->opcode) {
        case ZEND_RECV:
        case ZEND_RECV_INIT:
        case ZEND_RECV_VARIADIC:
          fprintf(file, "(arg)");
          break;
        default:
          fprintf(file, "(?)");
      }
    }
  }
  if (op->op1_type != IS_UNUSED) {
    dump_operand(file, '1', &op->op1, op->op1_type);
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
        fprintf(file, "$this.");
        break;
    }
  }
  if (op->op1_type != IS_UNUSED && op->op2_type != IS_UNUSED)
    fprintf(file, " ? ");
  if (op->op2_type != IS_UNUSED) {
    dump_operand(file, '2', &op->op2, op->op2_type);
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
    fprintf(file, " @ %s%d %s", delta > 0 ? "+" : "", delta, jump_reason);
  }
  fprintf(file, "\n");
}

static void print_sink(FILE *file, const char *sink_details)
{
  fprintf(file, "\t      %s\n", sink_details);
}

void identify_sink_operands(FILE *file, zend_op *op, sink_identifier_t id)
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
      print_sink(file, "sink(number) {op1,op2} => {result}");
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
      print_sink(file, "sink(local:number) {op1,op2} => {op1,result}");
      break;
    case ZEND_CONCAT:
      print_sink(file, "sink(local:string) {op1,op2} => {result}");
      break;
    case ZEND_ASSIGN_CONCAT:
      print_sink(file, "sink(local:string) {op1,op2} => {op1,result}");
      break;
    case ZEND_BOOL_XOR:
      print_sink(file, "sink(local:bool) {op1,op2} => {result}");
      break;
    case ZEND_INSTANCEOF: /* op1 instanceof op2 */
      print_sink(file, "sink(local:bool) {op1,op2,class-hierarchy} => {result}");
      break;
    case ZEND_TYPE_CHECK:
      print_sink(file, "sink(local:bool) {op1,class-hierarchy} => {result}");
      break;
    case ZEND_DEFINED:
      print_sink(file, "sink(local:bool) {op1,constants} => {result}");
      break;
    case ZEND_IS_IDENTICAL:
    case ZEND_IS_NOT_IDENTICAL:
    case ZEND_IS_EQUAL:
    case ZEND_IS_NOT_EQUAL:
    case ZEND_IS_SMALLER:
    case ZEND_IS_SMALLER_OR_EQUAL:
      print_sink(file, "sink(condition) {op1,op2} => {branch-condition}");
      break;
    case ZEND_PRE_INC_OBJ:
    case ZEND_PRE_DEC_OBJ:
    case ZEND_POST_INC_OBJ:
    case ZEND_POST_DEC_OBJ:
      print_sink(file, "sink(local:number) {op1.op2} => {op1.op2,result}");
      break;
    case ZEND_PRE_INC:
    case ZEND_PRE_DEC:
    case ZEND_POST_INC:
    case ZEND_POST_DEC:
      print_sink(file, "sink(local:number) {op1} => {op1,result}");
      break;
    case ZEND_ECHO:
      print_sink(file, "sink(local:number) {op1} => {output}");
      break;
    case ZEND_PRINT:
      print_sink(file, "sink(local:number) {op1} => {output,result}");
      break;
    case ZEND_FETCH_R:
    case ZEND_FETCH_W:
    case ZEND_FETCH_RW:
    case ZEND_FETCH_IS:
      print_sink(file, "sink(global) {op1(type).op2} => {result}");
      break;
    case ZEND_FETCH_UNSET:
      print_sink(file, "sink(global) {} => {op1(type).op2}");
      break;
    case ZEND_FETCH_DIM_R:
    case ZEND_FETCH_DIM_W:
    case ZEND_FETCH_DIM_RW:
    case ZEND_FETCH_DIM_IS:
    case ZEND_FETCH_OBJ_R:
    case ZEND_FETCH_OBJ_W:
    case ZEND_FETCH_OBJ_RW:
    case ZEND_FETCH_OBJ_IS:
      print_sink(file, "sink(map) {op1.op2} => {result}");
      break;
    case ZEND_FETCH_DIM_UNSET:
    case ZEND_FETCH_OBJ_UNSET:
      print_sink(file, "sink(map) {} => {op1.op2,result}");
      break;
    case ZEND_FETCH_CONSTANT:
      print_sink(file, "sink(local) {op1.op2} => {result}");
      break;
    case ZEND_UNSET_VAR:
      if (op->op2_type == IS_UNUSED)
        print_sink(file, "sink(local) {} => {op1}");
      else
        print_sink(file, "sink(map) {} => {op2.op1}"); /* op2 must be static */
      break;
    case ZEND_UNSET_DIM:
    case ZEND_UNSET_OBJ:
      print_sink(file, "sink(map) {} => {op1.op2}");
      break;
    case ZEND_ASSIGN:
      print_sink(file, "sink(local) {op2} => {op1,result}");
      break;
    case ZEND_ASSIGN_OBJ:
    case ZEND_ASSIGN_DIM:
      print_sink(file, "sink(map) {value} => {base.field}");
      break;
    case ZEND_JMPZ:
    case ZEND_JMPNZ:
    case ZEND_JMPZNZ:
    case ZEND_JMPZ_EX:
    case ZEND_JMPNZ_EX:
      print_sink(file, "sink(branch) {branch-condition} => {opline}");
      break;
    /* opcode has misleading name: ADD means APPEND */
    case ZEND_ADD_CHAR:   /* (op2 must be a const char) */
    case ZEND_ADD_STRING: /* (op2 must be a const string) */
    case ZEND_ADD_VAR:    /* (may convert op2 to string) */
      print_sink(file, "sink(local:string) {op2,result} => {result}");
      break;
    case ZEND_ADD_ARRAY_ELEMENT: /* insert or append */
      print_sink(file, "sink(map) {op1} => {result.op2}");
      break;
    case ZEND_INIT_METHOD_CALL:
      print_sink(file, "sink(edge) {op1.op2} => {fcall-stack}");
      break;
    case ZEND_INIT_STATIC_METHOD_CALL:
      print_sink(file, "sink(edge) {op1(type).op2} => {fcall-stack}");
      break;
    case ZEND_INIT_FCALL:
    case ZEND_INIT_FCALL_BY_NAME:
    case ZEND_INIT_USER_CALL:
    case ZEND_INIT_NS_FCALL_BY_NAME:
      print_sink(file, "sink(edge) {op2} => {fcall-stack}");
      break;
    case ZEND_NEW:
      print_sink(file, "sink(edge) {op1(,op2)} => {fcall-stack(,opline)}");
      break;
    case ZEND_DO_FCALL:
      if (is_db_sink_function(NULL, id.call_target))
        print_sink(file, "sink(edge) {fcall-stack} => {db,opline}");
      else if (is_file_sink_function(id.call_target))
        print_sink(file, "sink(edge) {fcall-stack} => {file,opline}");
      else if (is_system_sink_function(id.call_target))
        print_sink(file, "sink(edge) {fcall-stack} => {system,opline}");
      else
        print_sink(file, "sink(edge) {fcall-stack} => {opline}");
      break;
    case ZEND_INCLUDE_OR_EVAL:
      if (op->extended_value == ZEND_EVAL)
        print_sink(file, "sink(edge) {op1} => {code,opline}");
      else
        print_sink(file, "sink(edge) {op1} => {opline}");
      break;
    case ZEND_CLONE:
      print_sink(file, "sink(edge) {op1} => {opline}");
      break;
    case ZEND_THROW:
      print_sink(file, "sink(edge) {op1} => {thrown}");
      break;
    case ZEND_HANDLE_EXCEPTION:
      print_sink(file, "sink(edge) {thrown} => {opline}");
      break;
    case ZEND_DISCARD_EXCEPTION:
      print_sink(file, "sink(edge) {thrown} => {thrown}");
      break;
    case ZEND_FAST_CALL: /* call finally via no-arg fastcall */
      print_sink(file, "sink(edge) {thrown} => {fast-ret,opline}");
      break;
    case ZEND_FAST_RET: /* return from fastcall (usually finally) */
      print_sink(file, "sink(edge) {fast-ret} => {opline}");
      break;
    case ZEND_SEND_VAL:
    case ZEND_SEND_VAL_EX:
    case ZEND_SEND_VAR:
    case ZEND_SEND_VAR_NO_REF:
    case ZEND_SEND_REF:
    case ZEND_SEND_USER:
      print_sink(file, "sink(local) {op1} => {arg(# in op2)}");
      break;
    case ZEND_SEND_ARRAY:
      print_sink(file, "sink(local) {op1} => {all-args}");
      break;
    case ZEND_RECV:
    case ZEND_RECV_VARIADIC:
      print_sink(file, "sink(local) {arg} => {result}");
      break;
    case ZEND_BOOL:
    case ZEND_BOOL_NOT:
      print_sink(file, "sink(local) {op1} => {result}");
      break;
    case ZEND_BRK:
    case ZEND_CONT:
    case ZEND_GOTO:
      print_sink(file, "sink(branch) {op2} => {opline}");
      break;
    case ZEND_CASE:
      print_sink(file, "sink(branch) {op1,op2} => {opline}");
      break;
    case ZEND_CAST:
      print_sink(file, "sink(local) {op1,ext} => {result}"); /* ext specifies cast dest type */
      break;
    case ZEND_FE_RESET:
      print_sink(file, "sink(map) {op1} => {op1(,opline)}"); /* skips foreach if empty */
      break;
    case ZEND_FE_FETCH:
      /* pulls two results: first the key, second the value; or skips foreach if empty */
      print_sink(file, "sink(map) {op1} => {result => next_op.result(,opline)}");
      break;
    case ZEND_ISSET_ISEMPTY_VAR:
      if (op->op2_type == IS_UNUSED)
        print_sink(file, "sink(local:bool) {op1} => {result}");
      else
        print_sink(file, "sink(local:bool) {op2.op1} => {result}"); /* op2 static */
      break;
    case ZEND_ISSET_ISEMPTY_DIM_OBJ:
    case ZEND_ISSET_ISEMPTY_PROP_OBJ:
      print_sink(file, "sink(local:bool) {op1.op2} => {result}");
      break;
    case ZEND_EXIT:
      if (op->op1_type == IS_UNUSED)
        print_sink(file, "sink(local:bool) {} => {opline}");
      else
        print_sink(file, "sink(edge) {op1} => {opline,exit-code}");
      break;
    case ZEND_BEGIN_SILENCE:
    case ZEND_END_SILENCE:
      print_sink(file, "sink(internal) {} => {error-reporting}");
      break;
    case ZEND_TICKS:
      print_sink(file, "sink(internal) {ext} => {timer}");
      break;
    case ZEND_JMP_SET:
      print_sink(file, "sink(branch) {op1} => {result,opline}"); /* NOP if op1 is false */
      break;
    case ZEND_COALESCE:
      print_sink(file, "sink(branch) {op1} => {result,opline}"); /* NOP if op1 is NULL */
      break;
    case ZEND_QM_ASSIGN:
      print_sink(file, "sink(local) {op1} => {result}");
      break;
    case ZEND_DECLARE_CLASS:
    case ZEND_DECLARE_INHERITED_CLASS:
      print_sink(file, "sink(code) {op1} => {class-hierarchy}");
      break;
    case ZEND_DECLARE_INHERITED_CLASS_DELAYED: /* bind op1 only if op2 is unbound */
      print_sink(file, "sink(code) {op1,op2} => {class-hierarchy}");
      break;
    case ZEND_DECLARE_FUNCTION:
      print_sink(file, "sink(code) {compiler} => {functions}");
      break;
    case ZEND_ADD_INTERFACE:
    case ZEND_ADD_TRAIT: /* add interface/trait op2 (with zv+1) to class op1 */
      print_sink(file, "sink(code) {op1.(op2,op2.zv+1)} => {class-hierarchy}");
      break;
    case ZEND_BIND_TRAITS:
      print_sink(file, "sink(code) {op1} => {class-hierarchy}"); /* bind pending traits to op1 */
      break;
    case ZEND_SEPARATE:
      print_sink(file, "sink(local) {op1} => {op1}"); /* unbinds op1 somehow */
      break;
    case ZEND_DECLARE_CONST:
      print_sink(file, "sink(global) {op1.op2} => {constants}");
      break;
    case ZEND_BIND_GLOBAL: /* binds value in op1 to global named op2 */
      print_sink(file, "sink(global) {op1,op2} => {global}");
      break;
    case ZEND_STRLEN:
      print_sink(file, "sink(local:number) {op1} => {result}");
      break;

/* ======== SINK TODO ======= *

  ZEND_CAST
  ZEND_QM_ASSIGN

  ZEND_FETCH_FUNC_ARG?
  ZEND_FETCH_DIM_FUNC_ARG?
  ZEND_FETCH_OBJ_FUNC_ARG?

  ZEND_FETCH_LIST

  ZEND_ASSIGN_REF

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
#define DB_STMT_PREFIX "mysqli_stmt_"
#define DB_STMT_PREFIX_LEN (strlen(DB_STMT_PREFIX))

static bool is_db_stmt_function_base(const char *type, const char *name)
{
  return (type == NULL && strncmp(name, DB_STMT_PREFIX, DB_STMT_PREFIX_LEN) == 0) ||
         (type != NULL && strcmp(type, DB_STMT_TYPE) == 0);
}

#define DB_SQL_TYPE "mysqli"
#define DB_SQL_PREFIX "mysqli_"
#define DB_SQL_PREFIX_LEN (strlen(DB_SQL_PREFIX))

static bool is_db_sql_function_base(const char *type, const char *name)
{
  return ((type == NULL && strncmp(name, DB_SQL_PREFIX, DB_SQL_PREFIX_LEN) == 0) ||
          (type != NULL && strcmp(type, DB_SQL_TYPE) == 0)) &&
         !is_db_stmt_function_base(type, name);
}

static bool is_db_sql_source_function(const char *name)
{ /* separate cases to allow for a possible enum mapping later on */
  if (strcmp(name, "multi_query") == 0) {
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
  } else if (strcmp(name, "query") == 0) {
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
  //} else if (strcmp(name, "") == 0) {
  }
  return false;
}
