#include "php.h"
#include "script_cfi_utils.h"

#define IS_SESSION(vars) (Z_ISREF_P(&vars) && Z_TYPE_P(Z_REFVAL(vars)) == IS_ARRAY)

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

uint hash_addr(void *addr)
{
  uint64 addr_bits = p2int(addr);
  uint hash = ((uint)(addr_bits >> 0x20)) ^ ((uint)(addr_bits & 0xffffffffU));
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
    mkdir(path, 0700);

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

// unused
zval *php_session_lookup_var(zend_string *key)
{
  if (!IS_SESSION(PS(http_session_vars)))
    return NULL;
  
  HashTable *session_table = Z_ARRVAL_P(Z_REFVAL(PS(http_session_vars)));
  return zend_hash_find(session_table, key);
}

zval *php_session_set_var(zend_string *key, zval *value)
{
  zval *cell;
  
  if (!IS_SESSION(PS(http_session_vars)))
    return;
  
  HashTable *session_table = Z_ARRVAL_P(Z_REFVAL(PS(http_session_vars)));
  cell = zend_hash_add_new(session_table, key, &EG(uninitialized_zval));
  ZVAL_COPY_VALUE(cell, value);
  return cell;
}
