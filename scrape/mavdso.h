#ifndef MAVICONA_DSO_H
#define MAVICONA_DSO_H

#include <stdio.h>
#include <stdlib.h>

#define MAGIC_CHECK 0xe37abb23

typedef struct  {
	char *name;
	void *data;
	size_t size;
} item_t;

int stack_size = 0;
item_t *data_array = (item_t *) calloc(sizeof(item_t), 10);

void *mav_copy(const char *s, size_t n) {
  char *result;
  size_t len = strlen (s);

  if (n < len)
    len = n;

  result = (char *) malloc (len + 1);
  if (!result)
    return 0;

  result[len] = '\0';
  return memcpy (result, s, len);
}

#ifdef __cplusplus
extern "C" {
#endif

struct datastack {
	unsigned int magic;
	item_t *data;
	size_t size;
};

struct datastack *mav_commit() {
	static struct datastack interface;

	interface.magic = MAGIC_CHECK;
	interface.data = data_array;
	interface.size = stack_size;

	return &interface;
}

int mav_main(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

void push(const char *name, const void *data, size_t size) {
	data_array[stack_size].name = (char *)mav_copy(name, 8);
	data_array[stack_size].data = mav_copy((const char *)data, size);
	data_array[stack_size++].size = size;
}

#endif // MAVICONA_DSO_H
