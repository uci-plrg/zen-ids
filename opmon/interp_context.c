#include "php.h"
#include "php_opcode_monitor.h"
#include "lib/script_cfi_utils.h"
#include "metadata_handler.h"
#include "cfg_handler.h"
#include "compile_context.h"
#include "interp_context.h"

#define MAX_STACK_FRAME 256
#define ROUTINE_NAME_LENGTH 256

#define INCREMENT_STACK(base, ptr) \
do { \
  (ptr)++; \
  if (((ptr) - (base)) >= MAX_STACK_FRAME) \
    PRINT("Error: "#ptr" exceeds max stack frame!\n"); \
} while (0)

#define DECREMENT_STACK(base, ptr) \
do { \
  (ptr)--; \
  if ((ptr) <= (base)) \
    PRINT("Error: "#ptr" hit stack bottom!\n"); \
} while (0)

typedef struct _shadow_frame_t {
  zend_op *opcodes;
  uint last_index;
  control_flow_metadata_t cfm;
} shadow_frame_t;

static cfg_t *app_cfg;

static shadow_frame_t shadow_stack[MAX_STACK_FRAME];
static shadow_frame_t *shadow_frame;

#define CONTEXT_ENTRY 0xffffffffU
//static const cfg_node_t context_entry_node = { CONTEXT_ENTRY, 0xffffffff };

extern cfg_files_t cfg_files;

static void app_cfg_add_edge(control_flow_metadata_t *from_cfm, 
                             routine_cfg_t *to_cfg, cfg_node_t from_node)
{
  bool write_edge = true;
  cfg_add_routine(app_cfg, to_cfg);
  cfg_add_routine_edge(app_cfg, from_node, from_cfm->cfg, to_cfg);
  
  if (from_cfm->dataset != NULL) {
    if (dataset_verify_routine_edge(from_cfm->dataset, from_node.index, 
                                    to_cfg->unit_hash, to_cfg->routine_hash)) {
      write_edge = false;
      PRINT("<MON> Verified routine edge [0x%x|0x%x|%d -> 0x%x|0x%x]\n", 
            from_cfm->cfg->unit_hash,
            from_cfm->cfg->routine_hash, from_node.index, 
            to_cfg->unit_hash, to_cfg->routine_hash);
    }
  }
  
  if (write_edge) {
    PRINT("<MON> New routine edge [0x%x|0x%x|%d -> 0x%x|0x%x]\n", 
          from_cfm->cfg->unit_hash, 
          from_cfm->cfg->routine_hash, from_node.index, 
          to_cfg->unit_hash, to_cfg->routine_hash);
    write_routine_edge(from_cfm->cfg->unit_hash, from_cfm->cfg->routine_hash, from_node.index, 
                       to_cfg->unit_hash, to_cfg->routine_hash, 0 /* durf */);
  }
}

static bool is_alias(zend_uchar first_opcode, zend_uchar second_opcode) 
{
  switch (first_opcode) {
    case ZEND_FETCH_IS:
    case ZEND_FETCH_FUNC_ARG:
      return second_opcode == ZEND_FETCH_R;
    case ZEND_FETCH_DIM_FUNC_ARG:
      return ZEND_FETCH_DIM_R;
  }
  return false;
}

void initialize_interp_context()
{
  app_cfg = cfg_new();
  memset(shadow_stack, 0, 2 * sizeof(shadow_frame_t));
  shadow_frame = shadow_stack + 1;
}

const char *get_current_interp_context_name()
{
  return "<unavailable>"; // todo: put on cfm
}

routine_cfg_t *get_current_interp_routine_cfg()
{
  return shadow_frame->cfm.cfg;
}

static bool is_eval(zend_op_array *op_array)
{
  if (op_array->type == ZEND_EVAL_CODE)
    return true;
  
  if (op_array->scope == NULL && op_array->function_name != NULL) {
    if (strcmp(op_array->function_name->val, "create_function") == 0)
      return true;
    if (strcmp(op_array->function_name->val, "__lambda_func") == 0)
      return true;
  }
  
  return false;
}

void routine_call(zend_execute_data *call)
{
  control_flow_metadata_t cfm;
  zend_op_array *op_array = &call->func->op_array;
  
  // create/evaluate a call edge
  
  if (is_eval(op_array)) {
    // eval or lambda
    cfm.cfg = NULL;
    cfm.dataset = NULL;
    
    PRINT("--- Routine call to eval with opcodes at "PX" and cfg "PX"\n",
          p2int(op_array->opcodes), p2int(cfm.cfg));
  } else {
    char routine_name[ROUTINE_NAME_LENGTH];
    const char *classname, *function_name;
    control_flow_metadata_t *monitored_cfm;
    
    if (op_array->function_name == NULL) {     // eval
      classname = (strrchr(op_array->filename->val, '/') + 1);
      function_name = "<script-body>";
    } else if (op_array->scope == NULL) {      // function call
      classname = "<default>";
      function_name = op_array->function_name->val;
    } else if (call->This.value.obj == NULL) { // static method call
      classname = op_array->scope->name->val;
      function_name = op_array->function_name->val;
    } else {                                   // instance method call
      classname = call->This.value.obj->ce->name->val;
      function_name = op_array->function_name->val;
    }
    sprintf(routine_name, "%s:%s", classname, function_name);
    monitored_cfm = get_cfm(routine_name);
    
    if (monitored_cfm == NULL) {
      PRINT("--- Routine call to builtin function %s\n", routine_name);
      return;
    } else {
      cfm = *monitored_cfm;
      PRINT("--- Routine call to %s with opcodes at "PX" and cfg "PX"\n",
            routine_name, p2int(op_array->opcodes), p2int(cfm.cfg));
    }
  }
  
  shadow_frame++;
  shadow_frame->opcodes = op_array->opcodes;
  shadow_frame->last_index = CONTEXT_ENTRY;
  shadow_frame->cfm = cfm;
}

void push_interp_context(zend_op* op_array, uint branch_index, control_flow_metadata_t cfm)
{
  if (branch_index == 0xffffffffU)
    branch_index = shadow_frame->last_index + 1; // todo: usually...
  
  if (cfm.cfg != NULL)
    PRINT("# Push interp context 0x%x|0x%x (from index %d)\n", 
          cfm.cfg->unit_hash, cfm.cfg->routine_hash, branch_index);
  else
    PRINT("# Push interp context (null) (from index %d)\n", branch_index);
  
  if (cfm.cfg != NULL && shadow_frame->cfm.cfg != NULL) { // current_context is not always the right one? 
    if (branch_index < shadow_frame->cfm.cfg->opcodes.size) {
      cfg_node_t from_node = { 
        routine_cfg_get_opcode(shadow_frame->cfm.cfg, branch_index)->opcode, 
        branch_index 
      };
      app_cfg_add_edge(&shadow_frame->cfm, cfm.cfg, from_node);
    } else {
      PRINT("Error: branch_index %d exceeds last context cfg size %d\n", 
            branch_index, shadow_frame->cfm.cfg->opcodes.size);
    }
  }
  
  if (shadow_frame->opcodes == NULL)
    shadow_frame->opcodes = op_array;
  shadow_frame->last_index = branch_index + 1;
  
  INCREMENT_STACK(shadow_stack, shadow_frame);
  shadow_frame->cfm = cfm;
  shadow_frame->opcodes = NULL;
}

// in general, do popped objects need to be freed?
void pop_interp_context()
{
  DECREMENT_STACK(shadow_stack, shadow_frame);
  
  if (shadow_frame->cfm.cfg == NULL) {
    PRINT("# Pop interp context to null\n");
  } else {
    PRINT("# Pop interp context to 0x%x|0x%x\n", 
          shadow_frame->cfm.cfg->unit_hash, shadow_frame->cfm.cfg->routine_hash);
  }
}

void opcode_executing(const zend_op *op)
{
  if (true) return;
  
  shadow_frame_t *verify_frame;
  cfg_node_t from_node; // ???
  cfg_node_t node = { op->opcode, 0 };
  
  if (shadow_frame->opcodes != EG(current_execute_data)->func->op_array.opcodes) {
    PRINT("Error: expected opcode array at "PX", but the current opcodes are at "PX"\n",
          p2int(shadow_frame->opcodes), 
          p2int(EG(current_execute_data)->func->op_array.opcodes));
    return;
  }
  
  node.index = (uint)(op - shadow_frame->opcodes);
  
  //PRINT("[%s(0x%x):%d, line %d]: 0x%x:%s\n", get_current_interp_context_name(), get_current_interp_context_id(), 
  //  node.index, op->lineno, op->opcode, zend_get_opcode_name(op->opcode));
  
  verify_interp_context(shadow_frame->opcodes, node);
  
  if (shadow_frame->cfm.cfg == NULL) {
    PRINT("  === executing %s at index %d of %s on line %d with ops "PX"\n", 
          zend_get_opcode_name(node.opcode), 
          node.index, get_current_interp_context_name(), op->lineno,
          p2int(shadow_frame->opcodes));
  } else {
    PRINT("  === executing %s at index %d of %s(0x%x|0x%x) on line %d with ops "PX"\n", 
          zend_get_opcode_name(node.opcode), node.index, 
          get_current_interp_context_name(), shadow_frame->cfm.cfg->unit_hash, 
          shadow_frame->cfm.cfg->routine_hash, op->lineno, p2int(shadow_frame->opcodes));
  }
  
  if (shadow_frame->cfm.cfg == NULL && node.opcode == CONTEXT_ENTRY) {
    cfg_opcode_t *last_call_node = routine_cfg_get_opcode(shadow_frame->cfm.cfg, node.index-1);
    if (last_call_node->opcode == ZEND_INCLUDE_OR_EVAL && 
        (last_call_node->extended_value == ZEND_INCLUDE_ONCE ||
         last_call_node->extended_value == ZEND_REQUIRE_ONCE)) {
      pop_interp_context(); // target file has already been included once
    }
  }         
  
  if (/*last_pop == NULL*/false /* ?? */) {
    cfg_opcode_t *expected_opcode;
    if (from_node.opcode != CONTEXT_ENTRY && node.index != (from_node.index + 1) &&
        from_node.opcode != ZEND_BRK && from_node.opcode != ZEND_CONT) { // todo: security of BRK/CONT?
      bool found = false;
      uint i;
      for (i = 0; i < shadow_frame->cfm.cfg->edges.size; i++) {
        cfg_opcode_edge_t *edge = routine_cfg_get_edge(shadow_frame->cfm.cfg, i);
        if (edge->from_index == from_node.index) {
          if (edge->to_index == node.index) {
            found = true;
            break;
          } else if (edge->to_index != (edge->from_index + 1)) {
            PRINT("Error! Opcode edge from %d was compiled with target %d, but executed with target %d\n",
                  from_node.index, edge->to_index, node.index);
          }
        }
      }
      if (!found) 
        PRINT("Error! Opcode edge from %d to %d not found\n", from_node.index, node.index);
    }
    
    if (node.index < shadow_frame->cfm.cfg->opcodes.size) {
      expected_opcode = routine_cfg_get_opcode(shadow_frame->cfm.cfg, node.index);
      if (node.opcode != expected_opcode->opcode && !is_alias(node.opcode, expected_opcode->opcode)) {
        PRINT("Error! Expected opcode %s at index %d, but found opcode %s\n", 
              zend_get_opcode_name(routine_cfg_get_opcode(shadow_frame->cfm.cfg, node.index)->opcode), 
              node.index, zend_get_opcode_name(node.opcode));
      }
    } else {
      PRINT("Error! The executing node has index %d, but the current context has only %d opcodes\n",
            node.index, shadow_frame->cfm.cfg->opcodes.size);
    }
    return;
  }

  if (verify_frame->opcodes != shadow_frame->opcodes) {
    PRINT("Error! Returned to op_array "PX" but expected op_array "PX"!\n", 
          (uint64) shadow_frame->opcodes, (uint64) verify_frame->opcodes);
    return;
  }
  if (verify_frame->last_index != node.index) {
    PRINT("Error! Returned to index %d but expected index %d!\n",
          node.index, verify_frame->last_index);
    return;
  }
  
  PRINT("Verified return to index %d\n", node.index);
}
