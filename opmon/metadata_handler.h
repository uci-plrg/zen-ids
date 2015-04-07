#ifndef _METADATA_HANDLER_H_
#define _METADATA_HANDLER_H_ 1

#include "cfg.h"

typedef struct _dataset_routine_t dataset_routine_t;

// this should always be a simple bundle of pointers, since it is passed by value
typedef struct _control_flow_metadata_t {
  const char *routine_name;
  routine_cfg_t *cfg;
  dataset_routine_t *dataset;
} control_flow_metadata_t;

void init_metadata_handler();
void destroy_metadata_handler();

void load_dataset(const char *script_path);
uint get_next_eval_id();

#endif
