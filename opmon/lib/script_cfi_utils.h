#ifndef _SCRIPT_CFI_UTILS_H_
#define _SCRIPT_CFI_UTILS_H_ 1

#define PRINT(...) fprintf(stderr, "\t> "__VA_ARGS__)
#define PX "0x%llx"

#define EVAL_FLAG 0x80000000U
#define UNKNOWN_CONTEXT_ID 0xffffffffU

#define HASH_STRING(str) hash_string(str) & ~EVAL_FLAG

#define ASSERT(b) do { \
  if (!(b)) PRINT("Assert failure: %s", #b); \
} while (0);

typedef unsigned long long uint64;

uint hash_string(const char *string);

#endif
