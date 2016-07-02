#include <dlfcn.h>
#include <cassert>
#include <iostream>
#include <stdio.h>
#include <stdio.h>
#include <sys/stat.h>
#include "ace/interface.h"

typedef int (*regclass_t)();
typedef Ace::Job *(*retrieve_t)();

void load(const char *file) {

	struct stat buffer;
	if (stat(file, &buffer) != 0) {
		fprintf(stderr, "%s not found\n", file);
		return;
	}

	void *handle = dlopen("cache/a7b281e46da3f2e9bdcab91ed46ab72c4ba8c275", RTLD_LAZY);
	if (!handle) {
		std::cerr << "Cannot open library: " << dlerror() << std::endl;
		return;
	}

	regclass_t exec_register = (int (*)()) dlsym(handle, "register_class");
	retrieve_t exec_retrieve = (Ace::Job * (*)()) dlsym(handle, "retrieve_object");

	assert(exec_register() == ACE_MAGIC);
	Ace::Job *jobObject = (Ace::Job *)exec_retrieve();
	jobObject->Setup();
	jobObject->Run();
	jobObject->Teardown();

	dlclose(handle);

	return;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "%s [FILE]\n", argv[0]);
		return 1;
	}

	load(argv[1]);
	return 0;
}