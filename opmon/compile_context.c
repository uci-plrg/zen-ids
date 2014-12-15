#include "compile_context.h"

typedef struct _compilation_unit_t {
  const char *path;
} compilation_unit_t;

typedef struct _compilation_function_t {
  const char *name;
} compilation_function_t;

static compilation_unit_t unit_stack[256];
static compilation_unit_t *unit_frame;

static compilation_function_t function_stack[256];
static compilation_function_t *function_frame;

void init_compile_context()
{
  unit_frame = unit_stack;
  unit_frame->path = NULL;
  
  function_frame = function_stack;
  function_frame->name = NULL;
}

void push_compilation_unit(const char *path)
{
  unit_frame->path = path;
  unit_frame++;
}

void pop_compilation_unit()
{
  unit_frame--;
}

void push_compilation_function(const char *function_name)
{
  function_frame->name = function_name;
  function_frame++;
}

void pop_compilation_function()
{
  function_frame--;
}

