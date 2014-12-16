#include "php.h"
#include "lib/script_cfi_utils.h"
#include "lib/script_cfi_hashtable.h"
#include "compile_context.h"

#define EVAL_ID "|eval|"

typedef struct _compilation_unit_t {
  const char *path;
  uint hash;
} compilation_unit_t;

typedef struct _compilation_function_t {
  const char *name;
  uint hash;
} compilation_function_t;

typedef struct _function_fqn_t {
  compilation_unit_t unit;
  compilation_function_t function;
} function_fqn_t;

static compilation_unit_t unit_stack[256];
static compilation_unit_t *unit_frame;
static compilation_unit_t *current_unit;

static compilation_function_t function_stack[256];
static compilation_function_t *function_frame;
static compilation_function_t *current_function;
static compilation_function_t no_function = { "<none>", 0 };

static sctable_t function_table;

void init_compile_context()
{
  unit_frame = unit_stack;
  unit_frame->path = NULL;
  current_unit = NULL;
  
  function_frame = function_stack;
  function_frame->name = NULL;
  current_function = &no_function;
  
  function_table.hash_bits = 7;
  sctable_init(&function_table);
}

void push_compilation_unit(const char *path)
{
  ASSERT(path != NULL);
  
  PRINT("> Push compilation unit %s\n", path);
  
  unit_frame->path = path;
  unit_frame->hash = hash_string(path);
  current_unit = unit_frame;
  unit_frame++;
  
  function_frame->name = "<none>";
  function_frame->hash = 0;
  current_function = function_frame;
  function_frame++;
}

void pop_compilation_unit()
{
  PRINT("> Pop compilation unit\n");
  
  unit_frame--;
  current_unit = unit_frame - 1;
  
  pop_compilation_function();
}

const char *get_compilation_unit_path() 
{
  return current_unit->path;
}

uint get_compilation_unit_hash()
{
  return current_unit->hash;
}

void push_compilation_function(const char *function_name)
{
  function_fqn_t *fqn;
  char *buffer;
  
  PRINT("> Push compilation function %s\n", function_name);
  
  function_frame->name = function_name;
  function_frame->hash = hash_string(function_name);
  current_function = function_frame;
  function_frame++;
  
  fqn = malloc(sizeof(function_fqn_t));
  fqn->unit = *current_unit;
  buffer = malloc(strlen(current_unit->path) + 1);
  strcpy(buffer, current_unit->path);
  fqn->unit.path = buffer;
  fqn->function = *current_function;
  buffer = malloc(strlen(current_function->name) + 1);
  strcpy(buffer, current_function->name);
  fqn->function.name = buffer;
  sctable_add(&function_table, current_function->hash, fqn);
}

void pop_compilation_function()
{
  PRINT("> Pop compilation function\n");
  
  function_frame--;
  current_function = function_frame - 1;
}

const char *get_compilation_function_name() 
{
  return current_function->name;
}

uint get_compilation_function_hash()
{
  return current_function->hash;
}

void push_eval(uint eval_id)
{
  PRINT("> Push eval 0x%x\n", eval_id);
  
  unit_frame->path = EVAL_PATH;
  unit_frame->hash = EVAL_HASH;
  current_unit = unit_frame;
  unit_frame++;
  
  function_frame->name = EVAL_FUNCTION_NAME;
  function_frame->hash = eval_id;
  current_function = function_frame;
  function_frame++;
}

const char *get_function_declaration_path(const char *function_name)
{
  function_fqn_t *fqn;

  fqn = (function_fqn_t *) sctable_lookup(&function_table, hash_string(function_name));
  if (fqn == NULL)
    return "unknown";
  else
    return fqn->unit.path;
}
