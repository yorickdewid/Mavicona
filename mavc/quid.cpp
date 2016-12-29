#include <vector>
#include <quidpp.h>
#include "quid.h"

/* Module specific commands */
void ModQuid::generateQuid(int paramc, const std::vector<std::string>& params) {
	int count = 0;
	if (paramc < 1)
		throw TooFewParameters(1);

	try {
		count = std::stoi(params[0]);
	} catch (std::invalid_argument&) {
		throw InvalidParameter("Not an integer");
	}

	for (int i = 0; i < count; ++i) {
		std::cout << quidpp::Quid() << std::endl;
	}
}
