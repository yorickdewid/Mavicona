#include <dlfcn.h>
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>

#include <iostream>
#include "ace/interface.h"

typedef int (*regclass_t)();
typedef Ace::Job *(*retrieve_t)();

extern "C" int load(const char *file) {
	struct stat buffer;
	if (stat(file, &buffer) != 0) {
		fprintf(stderr, "%s not found\n", file);
		return 1;
	}

	void *handle = dlopen(file, RTLD_LAZY);
	if (!handle) {
		fprintf(stderr, "Cannot open library: %s\n ", dlerror());
		return 1;
	}

	regclass_t exec_register = (int (*)()) dlsym(handle, "register_class");
	retrieve_t exec_retrieve = (Ace::Job * (*)()) dlsym(handle, "retrieve_object");

	assert(exec_register() == ACE_MAGIC);
	Ace::Job *jobObject = (Ace::Job *)exec_retrieve();
	jobObject->Setup();
	jobObject->Run();
	jobObject->Teardown();

	dlclose(handle);

	return 0;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "%s [FILE]\n", argv[0]);
		return 1;
	}

	return load(argv[1]);
}
