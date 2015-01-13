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
  zend_execute_data *execute_data;
  zend_op *opcodes;
  uint last_index;
  control_flow_metadata_t cfm;
} shadow_frame_t;

typedef struct _lambda_frame_t {
  const char *name;
} lambda_frame_t;

typedef enum _stack_state_t {
  STACK_STATE_NONE,
  STACK_STATE_UNWINDING,
  STACK_STATE_RETURNING
} stack_state_t;

typedef struct _stack_event_t {
  stack_state_t state;
  shadow_frame_t *throw_frame;
} stack_event_t;

static shadow_frame_t shadow_stack[MAX_STACK_FRAME_shadow_stack];
static shadow_frame_t *shadow_frame;

static lambda_frame_t lambda_stack[MAX_STACK_FRAME_lambda_stack];
static lambda_frame_t *lambda_frame;

static stack_event_t stack_event;

static bool is_initial_entry = true;

static uint first_thread_id;

#define CONTEXT_ENTRY 0xffffffffU

extern cfg_files_t cfg_files;
extern cfg_t *app_cfg;

void initialize_interp_context()
{
  memset(shadow_stack, 0, sizeof(shadow_frame_t));
  shadow_frame = shadow_stack;
  
  memset(lambda_stack, 0, 2 * sizeof(lambda_frame_t));
  lambda_frame = lambda_stack + 1;
  
  stack_event.state = STACK_STATE_NONE;
  stack_event.throw_frame = NULL;
  
  first_thread_id = pthread_self();
}

static void generate_routine_edge(control_flow_metadata_t *from_cfm, uint from_index,
                                  routine_cfg_t *to_cfg, uint to_index)
{
  bool write_edge = true;
  cfg_add_routine_edge(app_cfg, from_index, to_index, from_cfm->cfg, to_cfg);
  
  if (from_cfm->dataset != NULL) {
    if (dataset_verify_routine_edge(from_cfm->dataset, from_index, to_index,
                                    to_cfg->unit_hash, to_cfg->routine_hash)) {
      write_edge = false;
      PRINT("<MON> Verified routine edge [0x%x|0x%x|%u -> 0x%x|0x%x]\n", 
            from_cfm->cfg->unit_hash,
            from_cfm->cfg->routine_hash, from_index, 
            to_cfg->unit_hash, to_cfg->routine_hash);
    }
  }
  
  if (write_edge) {
    PRINT("<MON> New routine edge [0x%x|0x%x|%u -> 0x%x|0x%x]\n", 
          from_cfm->cfg->unit_hash, 
          from_cfm->cfg->routine_hash, from_index, 
          to_cfg->unit_hash, to_cfg->routine_hash);
    write_routine_edge(from_cfm->cfg->unit_hash, from_cfm->cfg->routine_hash, from_index, 
                       to_cfg->unit_hash, to_cfg->routine_hash, to_index);
  }
}

static void generate_opcode_edge(control_flow_metadata_t *cfm, uint from_index, uint to_index)
{
  bool write_edge = true;
  cfg_add_opcode_edge(cfm->cfg, from_index, to_index);
  
  if (cfm->dataset != NULL) {
    if (dataset_verify_opcode_edge(cfm->dataset, from_index, to_index)) {
      write_edge = false;
      PRINT("<MON> Verified opcode edge [0x%x|0x%x %u -> %u]\n", 
            cfm->cfg->unit_hash,
            cfm->cfg->routine_hash, from_index, to_index);
    }
  }
  
  if (write_edge) {
    PRINT("<MON> New opcode edge [0x%x|0x%x %u -> %u]\n", 
          cfm->cfg->unit_hash, cfm->cfg->routine_hash, from_index, to_index);
    write_op_edge(cfm->cfg->unit_hash, cfm->cfg->routine_hash, from_index, to_index);
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

static bool shadow_stack_contains_frame(zend_execute_data *execute_data, zend_op *opcodes) {
  shadow_frame_t *walk = shadow_frame;
  do {
    if (walk->execute_data == execute_data && walk->opcodes == opcodes)
      return true;
  } while (--walk > shadow_stack);
  return false;
}

static bool update_shadow_stack() // true if the stack changed
{
  control_flow_metadata_t to_cfm;
  zend_execute_data *execute_data = EG(current_execute_data);
  zend_op_array *op_array = &execute_data->func->op_array;
  
  if (op_array == NULL || op_array->opcodes == NULL)
    return false; // nothing to do
  
  if (stack_event.state == STACK_STATE_UNWINDING)
    return false;
  
  if (execute_data == shadow_frame->execute_data && 
      op_array->opcodes == shadow_frame->opcodes) {
    stack_event.state = STACK_STATE_NONE;
    return false; // nothing to do
  }
  
  if (stack_event.state == STACK_STATE_RETURNING) {
    stack_event.state = STACK_STATE_NONE;
    if (shadow_stack_contains_frame(execute_data, op_array->opcodes)) {
      shadow_frame_t *shadow_prev = shadow_frame;
      do {
        DECREMENT_STACK(shadow_stack, shadow_frame);
      } while (shadow_frame > shadow_stack && 
               (shadow_frame->execute_data != execute_data || 
                op_array->opcodes != shadow_frame->opcodes));
      
      if (shadow_frame == shadow_stack) {
        ERROR("Shadow stack unwound %d frames and "
              "hit bottom looking for execute_data "PX"\n",
              (int)(shadow_prev - shadow_frame), p2int(execute_data));
      }
      
      to_cfm = shadow_frame->cfm;
      WARN("--- Routine return to %s with opcodes at "PX"|"PX" and cfg "PX"\n",
            to_cfm.routine_name, p2int(execute_data), 
            p2int(shadow_frame->opcodes), p2int(to_cfm.cfg));
      return true;
    } else {
      WARN("Invalid return: shadow stack does not contain execute_data "PX". "
           "Calling instead\n", p2int(execute_data));
    }
  }
  
  if (op_array->type == ZEND_EVAL_CODE) { // eval or lambda
    to_cfm = get_last_eval_cfm();
  } else {
    char routine_name[ROUTINE_NAME_LENGTH];
    const char *classname, *function_name;
    control_flow_metadata_t *monitored_cfm = NULL;
    
    if (op_array->function_name == NULL) {     // script-body
      classname = (strrchr(op_array->filename->val, '/') + 1);
      function_name = "<script-body>";
    } else if (op_array->scope == NULL || (op_array->fn_flags & ZEND_ACC_CLOSURE)) {
      classname = "<default>";                 // function call
      /*
      if (strcmp(op_array->function_name->val, "__lambda_func") == 0) {
        function_name = lambda_frame->name;
        DECREMENT_STACK(lambda_stack, lambda_frame);
      } else {
      */
        function_name = op_array->function_name->val;
      //}
    } else if (execute_data->This.value.obj == NULL) { // static method call
      classname = op_array->scope->name->val;
      function_name = op_array->function_name->val;
    } else {                                   // instance method call
      classname = op_array->scope->name->val;
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
      to_cfm.routine_name = (const char *)routine_name_buffer;
      to_cfm.cfg = NULL;
      to_cfm.dataset = NULL;
      ERROR("Failed to find opcodes for function %s\n", routine_name);
    } else {
      to_cfm = *monitored_cfm;
    }
  }
  
  if (shadow_frame > shadow_stack && to_cfm.cfg != NULL) {
    zend_op *op = &shadow_frame->opcodes[shadow_frame->last_index];
    compiled_edge_target_t compiled_target = get_compiled_edge_target(op, shadow_frame->last_index);
                             
    if (compiled_target.type != COMPILED_EDGE_CALL && op->opcode != ZEND_NEW)
      ERROR("Generating call edge for compiled target type %d (opcode 0x%x)\n", 
            compiled_target.type, op->opcode);
                             
    generate_routine_edge(&shadow_frame->cfm, shadow_frame->last_index, 
                          to_cfm.cfg, 0); // 0 even if that opcode is not executable
  }
  
  INCREMENT_STACK(shadow_stack, shadow_frame);
  
  WARN("--- Routine call to %s with opcodes at "PX"|"PX" and cfg "PX"\n",
        to_cfm.routine_name, p2int(execute_data), 
        p2int(op_array->opcodes), p2int(to_cfm.cfg));
  
  {
    shadow_frame->execute_data = execute_data;
    shadow_frame->opcodes = op_array->opcodes;
    shadow_frame->last_index = CONTEXT_ENTRY;
    shadow_frame->cfm = to_cfm;
  }
  
  return true;
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
  zend_execute_data *execute_data = EG(current_execute_data);
  zend_op_array *op_array = &execute_data->func->op_array;
  bool stack_changed, caught_exception = false;
  
  if (pthread_self() != first_thread_id)
    ERROR("Multiple threads are not supported!\n");
  
  stack_changed = update_shadow_stack();
  
  // todo: check remaining state/opcode mismatches
  if (op->opcode == ZEND_RETURN)
    stack_event.state = STACK_STATE_RETURNING;
  
  if (op->opcode == ZEND_HANDLE_EXCEPTION) {
    if (stack_event.state == STACK_STATE_NONE) {
      stack_event.state = STACK_STATE_UNWINDING;
      stack_event.throw_frame = shadow_frame;
      WARN("Exception thrown at op %d in opcodes "PX"|"PX" of routine 0x%x|0x%x\n", 
           shadow_frame->last_index, p2int(execute_data), p2int(execute_data->func->op_array.opcodes),
           shadow_frame->cfm.cfg->unit_hash, shadow_frame->cfm.cfg->routine_hash);
    } else {
      DECREMENT_STACK(shadow_stack, shadow_frame);
    }
    return;
  }
  
  if (op->opcode == ZEND_CATCH) {
    if (stack_event.state == STACK_STATE_UNWINDING) {
      cfg_node_t executing_node = { op->opcode, op - shadow_frame->opcodes };
      if (shadow_frame->execute_data != execute_data && 
          shadow_stack_contains_frame(execute_data, op_array->opcodes)) {
        do {
          DECREMENT_STACK(shadow_stack, shadow_frame);
        } while (shadow_frame->execute_data != execute_data);
      }
      WARN("Exception at op %d of 0x%x|0x%x caught at op %d in opcodes "PX"|"PX" of 0x%x|0x%x\n",
           stack_event.throw_frame->last_index, 
           stack_event.throw_frame->cfm.cfg->unit_hash, 
           stack_event.throw_frame->cfm.cfg->routine_hash,
           executing_node.index, 
           p2int(execute_data), p2int(execute_data->func->op_array.opcodes),
           shadow_frame->cfm.cfg->unit_hash, shadow_frame->cfm.cfg->routine_hash);           
      stack_event.state = STACK_STATE_NONE;
      
      if (stack_event.throw_frame == shadow_frame) {
        generate_opcode_edge(&shadow_frame->cfm, stack_event.throw_frame->last_index, 
                             executing_node.index);
      } else {
        generate_routine_edge(&stack_event.throw_frame->cfm, stack_event.throw_frame->last_index,
                              shadow_frame->cfm.cfg, executing_node.index);
      }
      caught_exception = true;
    } // else... not sure why these get executed
  }
      
  if (shadow_frame->execute_data != execute_data || 
      shadow_frame->opcodes != op_array->opcodes) {
    ERROR("expected opcode array at "PX"|"PX", "
          "but the current opcodes are at "PX"|"PX" (function %s, %s)\n",
          p2int(shadow_frame->execute_data), p2int(shadow_frame->opcodes), 
          p2int(execute_data), p2int(op_array->opcodes),
          op_array->function_name == NULL ? 
            "<script-body>" : op_array->function_name->val,
          shadow_stack_contains_frame(execute_data, op_array->opcodes) ? 
            "on stack" : "not on stack");
    PRINT("\tOpcode at %d is %s\n", (int)(op - op_array->opcodes), 
          zend_get_opcode_name(op->opcode));
    stack_event.state = STACK_STATE_NONE;
  } else if (shadow_frame->cfm.cfg == NULL) {
    ERROR("No cfg for opcodes at "PX"|"PX"\n", p2int(execute_data), 
          p2int(execute_data->func->op_array.opcodes));
  } else {
    cfg_node_t executing_node = { op->opcode, op - shadow_frame->opcodes };
    cfg_opcode_t *expected_opcode;
    
    if (executing_node.index > 0x1000 || 
        executing_node.index >= shadow_frame->cfm.cfg->opcodes.size) {
      ERROR("attempt to execute foobar op %u in opcodes "PX"|"PX" of routine 0x%x|0x%x\n", 
            executing_node.index, p2int(execute_data), p2int(op_array->opcodes),
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
          
      // todo: verify call continuations here too (seemm to be missing)
      if (is_fallthrough(&executing_node)) {
        PRINT("@ Verified fall-through %u -> %u in 0x%x|0x%x\n", 
              shadow_frame->last_index, executing_node.index,
              shadow_frame->cfm.cfg->unit_hash, 
              shadow_frame->cfm.cfg->routine_hash);
      } else if (!caught_exception) {
        if (shadow_frame->last_index == CONTEXT_ENTRY) {
          WARN("Context entry reaches index %u, "
               "but the first executable node has index %u!\n",
               executing_node.index, get_next_executable_index(0));
        } else if (!stack_changed) {
          uint i;
          bool found = false;
          zend_op *from_op = &shadow_frame->opcodes[shadow_frame->last_index];
          cfg_node_t from_node = { op->opcode, shadow_frame->last_index };
          
          for (i = 0; i < shadow_frame->cfm.cfg->edges.size; i++) {
            cfg_opcode_edge_t *edge = routine_cfg_get_edge(shadow_frame->cfm.cfg, i);
            if (edge->from_index == shadow_frame->last_index) {
              if (edge->to_index == executing_node.index) {
                found = true;
                break;
              }
            }
          }
          if (found) {
            PRINT("@ Verified opcode edge %u -> %u\n", 
                  shadow_frame->last_index, executing_node.index);
          } else {
            compiled_edge_target_t compiled_target = get_compiled_edge_target(from_op, shadow_frame->last_index);
            if (compiled_target.type != COMPILED_EDGE_INDIRECT) {
              ERROR("Generating indirect edge from compiled target type %d (opcode 0x%x)\n",
                    compiled_target.type, op->opcode);
            }
            generate_opcode_edge(&shadow_frame->cfm, from_node.index, executing_node.index);
          }
        }
      }
      
      shadow_frame->last_index = executing_node.index;
    }
  }
  
  /*
  if (is_lambda_call_init(op)) {
    if ((op->op2_type == IS_CV || op->op2_type == IS_VAR) && 
        *EX_VAR(op->op2.var)->value.str->val == '\0') {
      INCREMENT_STACK(lambda_stack, lambda_frame);
      lambda_frame->name = EX_VAR(op->op2.var)->value.str->val + 1;
      PRINT("@ Push call to %s\n", lambda_frame->name);
    }
  }
  */
}
