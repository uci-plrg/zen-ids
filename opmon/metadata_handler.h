#ifndef _METADATA_HANDLER_H_
#define _METADATA_HANDLER_H_ 1

#include "cfg.h"
#include "dataset.h"

typedef struct _control_flow_metadata_t {
  routine_cfg_t *cfg;
  dataset_routine_t *dataset;
} control_flow_metadata_t;

void init_metadata_handler();
void destroy_metadata_handler();

void load_dataset(const char *script_path);

#endif
