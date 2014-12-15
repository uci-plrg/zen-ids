#include "php.h"
#include "lib/script_cfi_utils.h"
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

static compilation_unit_t unit_stack[256];
static compilation_unit_t *unit_frame;
static compilation_unit_t *current_unit;

static compilation_function_t function_stack[256];
static compilation_function_t *function_frame;
static compilation_function_t *current_function;

void init_compile_context()
{
  unit_frame = unit_stack;
  unit_frame->path = NULL;
  current_unit = NULL;
  
  function_frame = function_stack;
  function_frame->name = NULL;
  current_function = NULL;
}

void push_compilation_unit(const char *path)
{
  unit_frame->path = path;
  unit_frame->hash = HASH_STRING(path);
  current_unit = unit_frame;
  unit_frame++;
}

void pop_compilation_unit()
{
  unit_frame--;
  current_unit = unit_frame - 1;
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
  if (function_name == NULL)
    function_frame->name = EVAL_ID;
  else
    function_frame->name = function_name;
  unit_frame->hash = HASH_STRING(function_name);
  function_frame++;
}

void pop_compilation_function()
{
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
