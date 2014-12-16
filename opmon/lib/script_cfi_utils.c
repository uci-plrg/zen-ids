
#include "php.h"

uint hash_string(const char *string)
{
  uint four;
  uint hash = 0;
  uint i = strlen(string);

  while (i > 3) {
    four = *(uint *)string;
    i -= 4;
    string += 4;
    hash = hash ^ (hash << 5) ^ four;
  }
  
  if (i > 0) {
    four = 0;
    while (i-- > 0)
      four = (four << 8) | *string++;
    hash = hash ^ (hash << 5) ^ four;
  }
  
  return hash;
}
