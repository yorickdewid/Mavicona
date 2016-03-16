#ifndef DETECT_H
#define DETECT_H

#include <cstddef>
#include <iostream>

class Detect {
  public:
	Detect(const std::string& s);

	Detect(const Detect &) = delete;
	Detect &operator= (const Detect &) = delete;

  private:
	const char *magicFromBuffer(const char *buffer, std::size_t sz);
	const char *parseMagic(const char *magicrs);
};

#endif // DETECT_H
