#ifndef _CFG_HANDLER_H_
#define _CFG_HANDLER_H_ 1

#include "httpd.h"

#include "cfg.h"

void init_cfg_handler();
void close_cfg_files(application_t *app);

void starting_script(const char *script_path);
void worker_startup();
void server_startup();
void cfg_initialize_application(application_t *app);
void cfg_request(bool start);
void cfg_form(const char *form_data, int length);

void write_node(application_t *app, uint routine_hash, cfg_opcode_t *opcode, uint index);
void write_op_edge(application_t *app, uint routine_hash, uint from_index, uint to_index,
                   user_level_t user_level);
void write_routine_edge(bool is_new_in_process, application_t *app, uint from_routine_hash,
                        uint from_index, uint to_routine_hash, uint to_index,
                        user_level_t user_level);
void write_routine_catalog_entry(application_t *app, uint routine_hash, const char *unit_path,
                                 const char *routine_name);

void flush_all_outputs(application_t *app);

#endif
