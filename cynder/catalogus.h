#ifndef CATALOGUS_H
#define CATALOGUS_H

#include "engine.h"

class Catalogus : public AbstractEngine {

  public:
	Catalogus() : AbstractEngine("cat", false) {}

};

#endif // CATALOGUS_H
