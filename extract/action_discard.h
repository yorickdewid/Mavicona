#ifndef ACTION_DISCARD_H
#define ACTION_DISCARD_H

#include <fstream>

#include "action.h"

class Discard : public Action {

  public:
	Discard(const ScrapeData *payload) : Action(payload) { }

	bool run();

};

#endif // ACTION_DISCARD_H
