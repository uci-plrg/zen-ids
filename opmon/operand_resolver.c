#include "php.h"
#include "lib/script_cfi_utils.h"
#include "operand_resolver.h"

typedef struct _site_root_list_t {
  const char *site_root;
  uint length;
  struct _site_root_list_t *next;
} site_root_list_t;

static site_root_list_t *site_root_list = NULL;

#define SITE_ROOT_FILENAME "opmon.site.root"
#define SITE_ROOT_FILENAME_LEN 16

void destroy_operand_resolver()
{
  site_root_list_t *next_root, *root = site_root_list;
  while (root != NULL) {
    next_root = root->next;
    free((char *) root->site_root);
    free(root);
    root = next_root;
  }
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

static const char *new_site_root(const char *buffer)
{
  site_root_list_t *new_root;

  SPOT("Found site root %s\n", buffer);

  new_root = malloc(sizeof(site_root_list_t));
  new_root->site_root = buffer;
  new_root->length = strlen(buffer);
  new_root->next = site_root_list;
  site_root_list = new_root;
  return new_root->site_root;
}

const char *locate_site_root(const char *filename /*absolute path*/)
{
  char *buffer, *parent_dir;
  uint length = strlen(filename);
  site_root_list_t *next_root = site_root_list;
  struct stat file_info;

  // return existing root if there is one
  while (next_root != NULL) {
    if (length > next_root->length &&
        strncmp(next_root->site_root, filename, next_root->length) == 0)
      return next_root->site_root;
    next_root = next_root->next;
  }

  buffer = malloc(length + SITE_ROOT_FILENAME_LEN + 1);
  strcpy(buffer, filename);

  if (strcmp(buffer + length - 5, ".phar") == 0) {
    strcpy(buffer, "phar://");
    strcat(buffer, filename);
    if (buffer[strlen(buffer) - 1] != '/')
      strcat(buffer, "/");
    return new_site_root(buffer);
  }

  while ((parent_dir = strrchr(buffer, '/')) != NULL) {
    parent_dir[1] = '\0'; // truncate after last slash
    strcat(buffer, SITE_ROOT_FILENAME);
    if (stat(buffer, &file_info) == 0) {
      parent_dir[1] = '\0'; // remove filename
      return new_site_root(buffer);
    }
    parent_dir[0] = '\0'; // truncate at last slash
  }

  free(buffer);
  return NULL;
}
