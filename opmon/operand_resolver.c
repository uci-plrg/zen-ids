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

#define SITE_ROOT_FILENAME "opmon.site.root"
#define SITE_ROOT_FILENAME_LEN 16

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
    close_cfg_files(&next_app->app);
    free((char *) app->app.root);
    free((char *) app->app.name);
    free(app);
    app = next_app;
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

static application_t *new_site_app(char *buffer)
{
  application_list_t *new_app;
  const char *app_name;
  char *new_app_name;

  SPOT("Found application %s\n", buffer);

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

application_t *locate_application(const char *filename /*absolute path*/)
{
  char *buffer, *parent_dir;
  uint length = strlen(filename);
  application_list_t *next_app = application_list;
  struct stat file_info;

  // return existing app if there is one
  while (next_app != NULL) {
    if (length > next_app->root_length &&
        strncmp(next_app->app.root, filename, next_app->root_length) == 0)
      return &next_app->app;
    next_app = next_app->next;
  }

  buffer = malloc(length + SITE_ROOT_FILENAME_LEN + 1);
  strcpy(buffer, filename);

  if (strcmp(buffer + length - 5, ".phar") == 0) {
    strcpy(buffer, "phar://");
    strcat(buffer, filename);
    return new_site_app(buffer);
  }

  while ((parent_dir = strrchr(buffer, '/')) != NULL) {
    parent_dir[1] = '\0'; // truncate after trailing slash
    strcat(buffer, SITE_ROOT_FILENAME);
    if (stat(buffer, &file_info) == 0) {
      parent_dir[0] = '\0'; // remove filename and trailing slash
      return new_site_app(buffer);
    }
    parent_dir[0] = '\0'; // remove filename and trailing slash
  }

  free(buffer);
  return &unknown_app;
}
