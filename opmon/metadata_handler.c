#include "php.h"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "lib/script_cfi_utils.h"
#include "dataset.h"
#include "metadata_handler.h"

static size_t dataset_size;
//static void *dataset;

static uint eval_id = 0;

void init_metadata_handler()
{
}

void destroy_metadata_handler()
{
  // TODO: unmap me!
  //if (dataset != NULL)
  //  munmap(dataset, dataset_size);
}

void *load_dataset(const char *script_path)
{
  char dataset_path[256] = {0};
  struct stat fileinfo;
  int dataset_file;
  void *map, *dataset;

  setup_base_path(dataset_path, "sets", script_path);
  strcat(dataset_path, ".set");

  if (stat(dataset_path, &fileinfo) != 0) {
    WARN("Failed to obtain file info for path %s. Skipping dataset operations.\n", dataset_path);
    return NULL;
  }
  dataset_size = fileinfo.st_size;

  dataset_file = open(dataset_path, O_RDONLY);
  if (dataset_file == -1) {
    WARN("Failed to open the dataset at path %s. Skipping dataset operations.\n", dataset_path);
    return NULL;
  }

  map = mmap(NULL, dataset_size, PROT_READ, MAP_SHARED, dataset_file, 0);
  if (map == NULL) {
    WARN("Failed to map the dataset at path %s. Skipping dataset operations.\n", dataset_path);
    return NULL;
  }
  SPOT("Mapped %d bytes from dataset at path %s.\n", (int) dataset_size, dataset_path);

  dataset = install_dataset(map);
  eval_id = dataset_get_eval_count(dataset);
  return dataset;
}

uint get_next_eval_id()
{
  // todo: thread safety?
  return eval_id++;
}
