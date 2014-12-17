#ifndef _SCRIPT_CFI_UTILS_H_
#define _SCRIPT_CFI_UTILS_H_ 1

#define PRINT(...) fprintf(stderr, "\t> "__VA_ARGS__)
#define PX "0x%llx"

#define EVAL_PATH "<eval>"
#define EVAL_FUNCTION_NAME "<eval>"
#define EVAL_HASH 0x00000000U

#define UNKNOWN_CONTEXT_ID 0xffffffffU

#define ASSERT(b) do { \
  if (!(b)) PRINT("Assert failure: %s\n", #b); \
} while (0);

typedef unsigned long long uint64;

uint hash_string(const char *string);

#endif
