#ifndef PNG_HANDLER_H
#define PNG_HANDLER_H

#include <lodepng/lodepng.h>

#include "parse_observer.h"

class PNGHandler: public ParseObserver {

  public:
	PNGHandler(Detect *mod) : ParseObserver(mod, "image/png") {}

	void displayPNGInfo(const LodePNGInfo& info);
	void handle();
};

#endif // PNG_HANDLER_H
