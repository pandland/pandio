#include <stdint.h>
#include "common.h"
#include "heap.h"

struct pnd_io 
{
  uint64_t now;
  struct heap timers;
};

typedef struct pnd_io pnd_io_t;
