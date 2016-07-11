#ifndef _SERVICE_HPP_
#define _SERVICE_HPP_

#include <map>
#include <string>

class CSerivce {
  public:

	std::string GetAllProcedures();

	std::string CallProcedure(const std::string& data);
};

#endif	/* _SERVICE_HPP_ */
