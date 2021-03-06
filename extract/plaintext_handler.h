#ifndef PLAINTEXT_HANDLER_H
#define PLAINTEXT_HANDLER_H

#include "parse_observer.h"

class PlainTextHandler: public ParseObserver {

  public:
	PlainTextHandler(Detect *mod) : ParseObserver(mod, "text/plain") {}

	void handle();
};

#endif // PLAINTEXT_HANDLER_H
