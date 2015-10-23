#ifndef _CFG_H_
#define _CFG_H_ 1

#include "php.h"
#include "lib/script_cfi_hashtable.h"
#include "lib/script_cfi_array.h"
#include "lib/script_cfi_utils.h"

typedef enum _dataflow_sink_type_t {
  SINK_TYPE_VAR_NUM,          /* modifies an int or float variable */
  SINK_TYPE_VAR_BOOL,         /* modifies a bool variable */
  SINK_TYPE_VAR_STRING,       /* modifies a string */
  SINK_TYPE_VAR_COND,         /* can change a branch condition */
  SINK_TYPE_SESSION,          /* modifies a session variable */
  SINK_TYPE_OUTPUT,           /* specifies output text */
  SINK_TYPE_EDGE,             /* specifies a control flow target, e.g. call function by name */
  SINK_TYPE_SQL,              /* specifies an SQL insert or update (possibly a fragment) */
  /* Various functions like file_put_contents(), fopen(), fwrite(), etc. It might be a good idea
   * to monitor system calls in the process, though not sure how (hook?)
   */
  SINK_TYPE_FILE,
  SINK_TYPE_CODE,             /* eval() or create_function() */
  SINK_TYPE_CLASS_HIERARCHY,  /* add subclass, method, interface, trait */
  SINK_TYPE_INCLUDE,          /* include() or require() */
  SINK_TYPE_NONE,             /* no sink here */
} dataflow_sink_type_t;

typedef enum _dataflow_source_type_t {
  SOURCE_TYPE_HTTP,      /* HTTP parameters and cookies */
  SOURCE_TYPE_SESSION,   /* session variables */
  SOURCE_TYPE_SQL,       /* database query results */
  SOURCE_TYPE_FILE,      /* file access such as fread() */
  SOURCE_TYPE_SYSTEM,    /* server and environment configuration */
  SOURCE_TYPE_NONE,      /* no source here */
} dataflow_source_type_t;

typedef struct _dataflow_source_t {
  uint routine_hash;
  uint op_index;
  dataflow_source_type_t type;
} dataflow_source_t;

typedef struct _dataflow_sink_t {
  uint routine_hash;
  uint op_index;
  dataflow_sink_type_t type;
} dataflow_sink_t;

typedef struct _dataflow_influence_t {
  dataflow_source_t source;
  struct _dataflow_influence_t *next;
} dataflow_influence_t;

typedef struct _dataflow_var_t {
  bool is_temp;
  union {
    uint temp_id;
    const char *name;
  };
} dataflow_var_t;

typedef struct _dataflow_op_t {
  dataflow_var_t var;
  dataflow_influence_t *influence;
} dataflow_op_t;

/*
typedef struct _cfg_node_t {
  zend_uchar opcode;
  uint index;
} cfg_node_t;
*/

typedef struct _cfg_opcode_edge_t {
  uint from_index;
  uint to_index;
  user_level_t user_level;
} cfg_opcode_edge_t;

typedef struct _cfg_opcode_t {
  zend_uchar opcode;
  ushort line_number;
  zend_ulong extended_value;
  dataflow_source_t source;
  dataflow_sink_t sink;
  dataflow_op_t op1;
  dataflow_op_t op2;
  dataflow_op_t result;
} cfg_opcode_t;

typedef struct _routine_cfg_t {
  uint routine_hash;
  scarray_t opcodes;        // cfg_opcode_t *
  scarray_t opcode_edges;   // cfg_opcode_edge_t *
} routine_cfg_t;

// todo: add type { call, exception } ?
typedef struct _cfg_routine_edge_t {
  uint from_index;
  uint to_index;
  user_level_t user_level;
  routine_cfg_t *from_routine;
  routine_cfg_t *to_routine;
} cfg_routine_edge_t;

typedef struct _cfg_t {
  sctable_t routines;       // routine_cfg_t * // TODO: shouldn't I be a hashtable?
  sctable_t routine_edges;  // cfg_routine_edge_t *
} cfg_t;

typedef struct _application_t {
  const char *name;
  const char *root; // ends in '/'
  cfg_t *cfg;
  void *cfg_files;
  void *dataset;
  void *base_frame;
} application_t;

static inline bool
is_same_routine_cfg(routine_cfg_t *first, routine_cfg_t *second)
{
  return first->routine_hash == second->routine_hash;
}

static inline routine_cfg_t *
cfg_routine_lookup(cfg_t *cfg, uint routine_hash)
{
  return sctable_lookup(&cfg->routines, routine_hash);
}

static inline cfg_opcode_t *
routine_cfg_get_opcode(routine_cfg_t *routine, uint index)
{
  return (cfg_opcode_t *) scarray_get(&routine->opcodes, index);
}

static inline cfg_opcode_edge_t *
routine_cfg_get_opcode_edge(routine_cfg_t *routine, uint index)
{
  return (cfg_opcode_edge_t *) scarray_get(&routine->opcode_edges, index);
}

routine_cfg_t *routine_cfg_new(uint routine_hash);
void routine_cfg_free(routine_cfg_t *cfg);
void routine_cfg_assign_opcode(routine_cfg_t *cfg, zend_uchar opcode, zend_ulong extended_value,
                               ushort line_number, uint index);
bool routine_cfg_has_opcode_edge(routine_cfg_t *cfg, uint from_index, uint to_index);
void routine_cfg_add_opcode_edge(routine_cfg_t *cfg, uint from_index, uint to_index,
                                 user_level_t user_level);

cfg_t *cfg_new();
void cfg_free(cfg_t *cfg);
void cfg_add_routine(cfg_t *cfg, routine_cfg_t *routine);
bool cfg_has_routine_edge(cfg_t *cfg, routine_cfg_t *from_routine, uint from_index,
                          routine_cfg_t *to_routine, uint to_index);
void cfg_add_routine_edge(cfg_t *cfg, routine_cfg_t *from_routine, uint from_index,
                          routine_cfg_t *to_routine, uint to_index,
                          user_level_t user_level);

#endif
