#include "php.h"

#include "zend.h"
#include <zend_language_parser.h>
#include "zend_compile.h"
//#include "zend_highlight.h"
#include "zend_ptr_stack.h"
#include "zend_globals.h"

#include "script_cfi_array.h"
#include "script_cfi_utils.h"

#define IS_SESSION(vars) (Z_ISREF_P(&vars) && Z_TYPE_P(Z_REFVAL(vars)) == IS_ARRAY)

static scarray_t request_allocations;

void init_utils()
{
  scarray_init(&request_allocations);
}

void destroy_utils()
{
  scarray_destroy(&request_allocations);
}

uint hash_string(const char *string)
{
  uint four;
  uint hash = 0;
  uint i = strlen(string);

  while (i > 3) {
    four = *(uint *)string;
    i -= 4;
    string += 4;
    hash = hash ^ (hash << 5) ^ four;
  }

  if (i > 0) {
    four = 0;
    while (i-- > 0)
      four = (four << 8) | *string++;
    hash = hash ^ (hash << 5) ^ four;
  }

  return hash;
}

uint64 hash_addrs(void *first, void *second)
{
  uint64 hash;
  uint64 first_bits = p2int(first);
  uint64 second_bits = p2int(second);
  second_bits = (second_bits << 0x20) | (second_bits >> 0x20); // flip dwords for distribution
  hash = (first_bits ^ second_bits);
  return hash;
}

void setup_base_path(char *path, const char *category, const char *app_path)
{
  char *path_truncate;
  const char *script_filename;
  struct stat dirinfo;

  strcat(path, OPMON_G(dataset_dir));

  uint len = strlen(path);
  if (path[len-1] != '/')
    path[len] = '/';

  strcat(path, category);
  strcat(path, "/");

  if (stat(path, &dirinfo) != 0)
    mkdir(path, 0777);

  script_filename = strrchr(app_path, '/');
  if (script_filename == NULL)
    script_filename = app_path;
  else
    script_filename++;
  if ((strlen(path) + strlen(script_filename)) > 250)
    PRINT("Error: path %s%s will exceed buffer length of 256!\n", path, script_filename);
  strcat(path, script_filename);

  // .../script.2.1.php -> .../script.2.1
  path_truncate = strrchr(path, '.');
  if (path_truncate != NULL)
    *path_truncate = '\0';
}

bool is_php_session_active()
{
  return PS(id) != NULL && IS_SESSION(PS(http_session_vars));
}

zend_dataflow_monitor_t *dataflow_monitor = NULL;

zval *php_session_set_var(zend_string *key, zval *value)
{
  zval *cell;

  if (!IS_SESSION(PS(http_session_vars)))
    return NULL;

  HashTable *session_table = Z_ARRVAL_P(Z_REFVAL(PS(http_session_vars)));
  cell = zend_hash_add_new(session_table, key, &EG(uninitialized_zval));
  ZVAL_COPY_VALUE(cell, value);
  return cell;
}

char *request_strdup(const char *src)
{
  char *dst = REQUEST_ALLOC(strlen(src) + 1);
  strcpy(dst, src);
  return dst;
}

const char *operand_strdup(zend_execute_data *execute_data, const znode_op *operand, zend_uchar type)
{
  switch (type) {
    case IS_CONST:
      if (0 /* alpha: operand->zv->u1.v.type */ == IS_STRING)
        return request_strdup("alpha"); // Z_STRVAL_P(operand->zv));
      break;
    case IS_VAR:
    case IS_TMP_VAR:
    case IS_CV:
      return request_strdup(Z_STRVAL_P(EX_VAR(operand->var)));
  }

  return NULL;
}

const zval *get_zval(zend_execute_data *execute_data, const znode_op *operand, zend_uchar type)
{
  switch (type) {
    case IS_CONST:
      return NULL; // alpha: operand->zv;
    case IS_VAR:
    case IS_TMP_VAR:
    case IS_CV:
      return EX_VAR(operand->var);
    default:
      return NULL;
  }
}

const zval *get_arg_zval(zend_execute_data *execute_data, const zend_op *arg /* ZEND_SEND_* */)
{
  return get_zval(execute_data, &arg->op1, arg->op1_type);
}

char *get_resource_filename(const zval *value)
{
  if (value != NULL) {
    int res_type = Z_RES_TYPE_P(value);
    if (res_type == php_file_le_stream() || res_type == php_file_le_pstream()) {
      php_stream *stream = (php_stream *) Z_RES_VAL_P(value);
      return stream->orig_path;
    }
  }
  return NULL;
}

void tokenize_file(void)
{
	zval token;
	int token_type;
	int prev_space = 0;

	ZVAL_UNDEF(&token);
	while ((token_type=lex_scan(&token TSRMLS_CC))) {
		switch (token_type) {
			case T_WHITESPACE:
				if (!prev_space) {
					zend_write(" ", sizeof(" ") - 1);
					prev_space = 1;
				}
						/* lack of break; is intentional */
			case T_COMMENT:
			case T_DOC_COMMENT:
				ZVAL_UNDEF(&token);
				continue;

			case T_END_HEREDOC:
				zend_write((char*)LANG_SCNG(yy_text), LANG_SCNG(yy_leng));
				/* read the following character, either newline or ; */
				if (lex_scan(&token TSRMLS_CC) != T_WHITESPACE) {
					zend_write((char*)LANG_SCNG(yy_text), LANG_SCNG(yy_leng));
				}
				zend_write("\n", sizeof("\n") - 1);
				prev_space = 1;
				ZVAL_UNDEF(&token);
				continue;

			default:
				zend_write((char*)LANG_SCNG(yy_text), LANG_SCNG(yy_leng));
				break;
		}

		if (Z_TYPE(token) == IS_STRING) {
			switch (token_type) {
				case T_OPEN_TAG:
				case T_OPEN_TAG_WITH_ECHO:
				case T_CLOSE_TAG:
				case T_WHITESPACE:
				case T_COMMENT:
				case T_DOC_COMMENT:
					break;

				default:
					zend_string_release(Z_STR(token));
					break;
			}
		}
		prev_space = 0;
		ZVAL_UNDEF(&token);
	}
}

void *scalloc(size_t size, scalloc_lifespan_t lifespan)
{
  void *p;

  if (lifespan == ALLOC_REQUEST) {
    p = malloc(size);
    scarray_append(&request_allocations, p);
  } else {
    p = malloc(size);
  }

  return p;
}

void scfree_process(void *p)
{
  free(p);
}

void scfree_request()
{
  uint i;

  for (i = 0; i < request_allocations.size; i++)
    free(request_allocations.data[i]);

  request_allocations.size = 0;
}

