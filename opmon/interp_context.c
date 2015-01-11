#include "php.h"
#include "php_opcode_monitor.h"
#include "lib/script_cfi_utils.h"
#include "metadata_handler.h"
#include "cfg_handler.h"
#include "compile_context.h"
#include "interp_context.h"

#define MAX_STACK_FRAME_shadow_stack 0x100000
#define MAX_STACK_FRAME_lambda_stack 0x100
#define ROUTINE_NAME_LENGTH 256

#define INCREMENT_STACK(base, ptr) \
do { \
  (ptr)++; \
  if (((ptr) - (base)) >= MAX_STACK_FRAME_##base) \
    ERROR(#ptr" exceeds max stack frame!\n"); \
} while (0)

#define DECREMENT_STACK(base, ptr) \
do { \
  (ptr)--; \
  if ((ptr) <= (base)) \
    ERROR(#ptr" hit stack bottom!\n"); \
} while (0)

typedef struct _shadow_frame_t {
  zend_op *opcodes;
  uint last_index;
  control_flow_metadata_t cfm;
} shadow_frame_t;

typedef struct _lambda_frame_t {
  const char *name;
} lambda_frame_t;

typedef enum _exception_state_t {
  EXCEPTION_STATE_NONE,
  EXCEPTION_STATE_UNWINDING
} exception_state_t;

typedef struct _exception_context_t {
  exception_state_t state;
  shadow_frame_t *throw_frame;
} exception_context_t;

static cfg_t *app_cfg;

static shadow_frame_t shadow_stack[MAX_STACK_FRAME_shadow_stack];
static shadow_frame_t *shadow_frame;

static lambda_frame_t lambda_stack[MAX_STACK_FRAME_lambda_stack];
static lambda_frame_t *lambda_frame;

static exception_context_t exception_context;

static bool is_initial_entry = true;

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
      PRINT("<MON> Verified routine edge [0x%x|0x%x|%u -> 0x%x|0x%x]\n", 
            from_cfm->cfg->unit_hash,
            from_cfm->cfg->routine_hash, from_node.index, 
            to_cfg->unit_hash, to_cfg->routine_hash);
    }
  }
  
  if (write_edge) {
    PRINT("<MON> New routine edge [0x%x|0x%x|%u -> 0x%x|0x%x]\n", 
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
  
  memset(lambda_stack, 0, 2 * sizeof(lambda_frame_t));
  lambda_frame = lambda_stack + 1;
  
  exception_context.state = EXCEPTION_STATE_NONE;
  exception_context.throw_frame = NULL;
}

const char *get_current_interp_context_name()
{
  return "<unavailable>"; // todo: put on cfm
}

routine_cfg_t *get_current_interp_routine_cfg()
{
  return shadow_frame->cfm.cfg;
}

static bool is_lambda(zend_op_array *op_array)
{
  if (op_array->type == ZEND_USER_FUNCTION && op_array->scope == NULL && 
      op_array->function_name != NULL) {
    return (strcmp(op_array->function_name->val, "__lambda_func") == 0);
  }
  return false;
}

void routine_call(zend_execute_data *call)
{
  control_flow_metadata_t cfm;
  zend_op_array *op_array = &call->func->op_array;
  shadow_frame_t *call_frame = shadow_frame + 1;
  
  if (op_array == NULL || op_array->opcodes == NULL)
    return; // foobar
  
  if (exception_context.state == EXCEPTION_STATE_UNWINDING)
    return;
  
  // create/evaluate a call edge
  
  if (op_array->type == ZEND_EVAL_CODE) { // eval or lambda
    cfm = get_last_eval_cfm();
  } else {
    char routine_name[ROUTINE_NAME_LENGTH];
    const char *classname, *function_name;
    control_flow_metadata_t *monitored_cfm = NULL;
    
    if (op_array->function_name == NULL) {     // script-body
      classname = (strrchr(op_array->filename->val, '/') + 1);
      function_name = "<script-body>";
      monitored_cfm = get_cfm_by_name(routine_name);
    } else if (op_array->scope == NULL || (op_array->fn_flags & ZEND_ACC_CLOSURE)) {
      classname = "<default>";                 // function call
      if (strcmp(op_array->function_name->val, "__lambda_func") == 0) {
        function_name = lambda_frame->name;
        DECREMENT_STACK(lambda_stack, lambda_frame);
      } else {
        function_name = op_array->function_name->val;
      }
    } else if (call->This.value.obj == NULL) { // static method call
      classname = op_array->scope->name->val;
      function_name = op_array->function_name->val;
    } else {                                   // instance method call
      classname = op_array->scope->name->val; //call->This.value.obj->ce->name->val;
      function_name = op_array->function_name->val;
    }
    sprintf(routine_name, "%s:%s", classname, function_name);
    
    if (monitored_cfm == NULL)
      monitored_cfm = get_cfm_by_opcodes_address(op_array->opcodes);
    if (monitored_cfm == NULL)
      monitored_cfm = get_cfm_by_name(routine_name);
    if (monitored_cfm == NULL) {
      char *routine_name_buffer = malloc(strlen(routine_name)+10);
      sprintf(routine_name_buffer, "<missing>%s", routine_name);
      cfm.routine_name = (const char *)routine_name_buffer;
      cfm.cfg = NULL;
      cfm.dataset = NULL;
      //INCREMENT_STACK(shadow_stack, shadow_frame);
      //WARN("--- Routine call to builtin function %s\n", routine_name);
      //return;
    } else {
      cfm = *monitored_cfm;
    }
  }
  
  INCREMENT_STACK(shadow_stack, shadow_frame);
  
  WARN("--- Routine call to %s with opcodes at "PX" and cfg "PX"\n",
        cfm.routine_name, p2int(op_array->opcodes), p2int(cfm.cfg));
  
  call_frame->opcodes = op_array->opcodes;
  call_frame->last_index = CONTEXT_ENTRY;
  call_frame->cfm = cfm;
}

void routine_return()
{
  control_flow_metadata_t cfm;
  
  DECREMENT_STACK(shadow_stack, shadow_frame);
  
  cfm = shadow_frame->cfm;
  WARN("--- Routine return to %s with opcodes at "PX" and cfg "PX"\n",
        cfm.routine_name, p2int(shadow_frame->opcodes), p2int(cfm.cfg));
}

static uint get_next_executable_index(uint from_index)
{
  zend_uchar opcode;
  uint i = from_index;
  while (true) {
    opcode = shadow_frame->opcodes[i].opcode;
    if (zend_get_opcode_name(opcode) != NULL && opcode != ZEND_RECV)
      break;
    i++;
  }
  return i;
}

static bool is_fallthrough(cfg_node_t *to_node)
{
  bool is_context_entry = shadow_frame->last_index == CONTEXT_ENTRY;
  uint next_index;
  
  if (!is_context_entry) {
    switch (shadow_frame->opcodes[shadow_frame->last_index].opcode) {
      case ZEND_RETURN:
      case ZEND_JMP:
      case ZEND_BRK:
      case ZEND_CONT:
        return false;
    }
  }
  
  next_index = (is_context_entry ? 0 : shadow_frame->last_index + 1);
  return to_node->index == get_next_executable_index(next_index);
}

static bool is_lambda_call_init(const zend_op *op)
{
  zend_op_array *op_array = &EG(current_execute_data)->func->op_array;
  
  if (op->opcode != ZEND_INIT_FCALL_BY_NAME || op_array->function_name == NULL)
    return false;
  
  return strcmp(op_array->function_name->val, "__lambda_func") == 0;
}

void opcode_executing(const zend_op *op)
{
  if (op->opcode == ZEND_HANDLE_EXCEPTION) {
    if (exception_context.state == EXCEPTION_STATE_NONE) {
      exception_context.state = EXCEPTION_STATE_UNWINDING;
      exception_context.throw_frame = shadow_frame;
      WARN("Exception thrown at op %d of 0x%x|0x%x\n", shadow_frame->last_index,
           shadow_frame->cfm.cfg->unit_hash, shadow_frame->cfm.cfg->routine_hash);
    } else {
      DECREMENT_STACK(shadow_stack, shadow_frame);
    }
    return;
  }
  
  if (op->opcode == ZEND_CATCH) {
    if (exception_context.state == EXCEPTION_STATE_UNWINDING) {
      WARN("Exception at op %d of 0x%x|0x%x caught at op %d of 0x%x|0x%x\n",
           exception_context.throw_frame->last_index, 
           exception_context.throw_frame->cfm.cfg->unit_hash, 
           exception_context.throw_frame->cfm.cfg->routine_hash,
           shadow_frame->last_index, shadow_frame->cfm.cfg->unit_hash, 
           shadow_frame->cfm.cfg->routine_hash);           
      exception_context.state = EXCEPTION_STATE_NONE;
      exception_context.throw_frame = NULL;
    } else {
      ERROR("Catch executed at op %d of 0x%x|0x%x for unknown exception!\n",
            shadow_frame->last_index, shadow_frame->cfm.cfg->unit_hash, 
            shadow_frame->cfm.cfg->routine_hash);
    }
  } // what about finally?
  
  if (shadow_frame->opcodes != EG(current_execute_data)->func->op_array.opcodes) {
    zend_op_array *op_array = &EG(current_execute_data)->func->op_array;
    ERROR("expected opcode array at "PX", but the current opcodes are at "PX" (function %s)\n",
          p2int(shadow_frame->opcodes), 
          p2int(EG(current_execute_data)->func->op_array.opcodes),
          op_array->function_name == NULL ? "<script-body>" : op_array->function_name->val);
    PRINT("\tOpcode is %s\n", zend_get_opcode_name(op->opcode));
  } else if (shadow_frame->cfm.cfg == NULL) {
    ERROR("No cfg for opcodes at "PX"\n", p2int(EG(current_execute_data)->func->op_array.opcodes));
  } else {
    cfg_node_t executing_node = { op->opcode, op - shadow_frame->opcodes };
    cfg_opcode_t *expected_opcode;
    
    if (executing_node.index > 0x1000 || executing_node.index >= shadow_frame->cfm.cfg->opcodes.size) {
      ERROR("attempt to execute foobar op at index %u in 0x%x|0x%x\n", executing_node.index,
            shadow_frame->cfm.cfg->unit_hash, shadow_frame->cfm.cfg->routine_hash);
    } else {
      PRINT("@ Executing %s at index %u of 0x%x|0x%x\n",
            zend_get_opcode_name(executing_node.opcode), executing_node.index,
            shadow_frame->cfm.cfg->unit_hash, shadow_frame->cfm.cfg->routine_hash);
      
      expected_opcode = routine_cfg_get_opcode(shadow_frame->cfm.cfg, executing_node.index);
      if (executing_node.opcode != expected_opcode->opcode && 
          !is_alias(executing_node.opcode, expected_opcode->opcode)) {
        ERROR("Expected opcode %s at index %u, but found opcode %s\n", 
              zend_get_opcode_name(expected_opcode->opcode), 
              executing_node.index, zend_get_opcode_name(executing_node.opcode));
      }
          
      if (is_fallthrough(&executing_node)) {
        PRINT("@ Verified fall-through %u -> %u in 0x%x|0x%x\n", 
              shadow_frame->last_index, executing_node.index,
              shadow_frame->cfm.cfg->unit_hash, 
              shadow_frame->cfm.cfg->routine_hash);
      } else {
        if (shadow_frame->last_index == CONTEXT_ENTRY) {
          WARN("Context entry reaches index %u, "
               "but the first executable node has index %u!\n",
               executing_node.index, get_next_executable_index(0));
        } else {
          cfg_node_t from_node = { 
            shadow_frame->opcodes[shadow_frame->last_index].opcode,
            shadow_frame->last_index
          };
          
          if (from_node.opcode != ZEND_BRK && from_node.opcode != ZEND_CONT) {
            bool found = false;
            uint i;
            for (i = 0; i < shadow_frame->cfm.cfg->edges.size; i++) {
              cfg_opcode_edge_t *edge = routine_cfg_get_edge(shadow_frame->cfm.cfg, i);
              if (edge->from_index == shadow_frame->last_index) {
                if (edge->to_index == executing_node.index) {
                  found = true;
                  PRINT("@ Verified opcode edge %u -> %u\n", 
                        shadow_frame->last_index, executing_node.index);
                  break;
                } else if (edge->to_index != (edge->from_index + 1)) {
                  ERROR("Opcode edge from %u was compiled with target %u, "
                        "but executed with target %u\n",
                        shadow_frame->last_index, edge->to_index, executing_node.index);
                }
              }
            }
            if (!found) {
              WARN("Opcode edge from %u to %u not found\n", 
                   shadow_frame->last_index, executing_node.index);
            }
          }
        }
      }
      
      shadow_frame->last_index = executing_node.index;
    }
  }
  
  if (is_lambda_call_init(op)) {
    zend_execute_data *execute_data = EG(current_execute_data);
    if ((op->op2_type == IS_CV || op->op2_type == IS_VAR) && 
        *EX_VAR(op->op2.var)->value.str->val == '\0') {
      INCREMENT_STACK(lambda_stack, lambda_frame);
      lambda_frame->name = EX_VAR(op->op2.var)->value.str->val + 1;
      PRINT("@ Push call to %s\n", lambda_frame->name);
    }
  }
  
  //if (op->opcode == ZEND_DO_FCALL) 
  //  INCREMENT_STACK(shadow_stack, shadow_frame);
}

/***** deprecated *****/

void push_interp_context(zend_op* op_array, uint branch_index, control_flow_metadata_t cfm)
{
  if (branch_index == 0xffffffffU)
    branch_index = shadow_frame->last_index + 1; // todo: usually...
  
  if (cfm.cfg != NULL)
    PRINT("# Push interp context 0x%x|0x%x (from index %u)\n", 
          cfm.cfg->unit_hash, cfm.cfg->routine_hash, branch_index);
  else
    PRINT("# Push interp context (null) (from index %u)\n", branch_index);
  
  if (cfm.cfg != NULL && shadow_frame->cfm.cfg != NULL) { // current_context is not always the right one? 
    if (branch_index < shadow_frame->cfm.cfg->opcodes.size) {
      cfg_node_t from_node = { 
        routine_cfg_get_opcode(shadow_frame->cfm.cfg, branch_index)->opcode, 
        branch_index 
      };
      app_cfg_add_edge(&shadow_frame->cfm, cfm.cfg, from_node);
    } else {
      ERROR("branch_index %u exceeds last context cfg size %u\n", 
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

void deprecated_opcode_executing(const zend_op *op)
{
 
  shadow_frame_t *verify_frame;
  cfg_node_t from_node; // ???
  cfg_node_t node = { op->opcode, 0 };
  
  node.index = (uint)(op - shadow_frame->opcodes);
  
  //PRINT("[%s(0x%x):%u, line %u]: 0x%x:%s\n", get_current_interp_context_name(), get_current_interp_context_id(), 
  //  node.index, op->lineno, op->opcode, zend_get_opcode_name(op->opcode));
  
  verify_interp_context(shadow_frame->opcodes, node);
  
  if (shadow_frame->cfm.cfg == NULL) {
    PRINT("  === executing %s at index %u of %s on line %u with ops "PX"\n", 
          zend_get_opcode_name(node.opcode), 
          node.index, get_current_interp_context_name(), op->lineno,
          p2int(shadow_frame->opcodes));
  } else {
    PRINT("  === executing %s at index %u of %s(0x%x|0x%x) on line %u with ops "PX"\n", 
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
            ERROR("Opcode edge from %u was compiled with target %u, but executed with target %u\n",
                  from_node.index, edge->to_index, node.index);
          }
        }
      }
      if (!found) 
        ERROR("Opcode edge from %u to %u not found\n", from_node.index, node.index);
    }
    
    if (node.index < shadow_frame->cfm.cfg->opcodes.size) {
      expected_opcode = routine_cfg_get_opcode(shadow_frame->cfm.cfg, node.index);
      if (node.opcode != expected_opcode->opcode && !is_alias(node.opcode, expected_opcode->opcode)) {
        ERROR("Expected opcode %s at index %u, but found opcode %s\n", 
              zend_get_opcode_name(routine_cfg_get_opcode(shadow_frame->cfm.cfg, node.index)->opcode), 
              node.index, zend_get_opcode_name(node.opcode));
      }
    } else {
      ERROR("The executing node has index %u, but the current context has only %u opcodes\n",
            node.index, shadow_frame->cfm.cfg->opcodes.size);
    }
    return;
  }

  if (verify_frame->opcodes != shadow_frame->opcodes) {
    ERROR("Returned to op_array "PX" but expected op_array "PX"!\n", 
          (uint64) shadow_frame->opcodes, (uint64) verify_frame->opcodes);
    return;
  }
  if (verify_frame->last_index != node.index) {
    ERROR("Returned to index %u but expected index %u!\n",
          node.index, verify_frame->last_index);
    return;
  }
  
  PRINT("Verified return to index %u\n", node.index);
}
