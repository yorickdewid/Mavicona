#ifndef _ERROR_PAGE_HPP_
#define _ERROR_PAGE_HPP_

#include <string>

/*!
	Error pages class.
	\author Miguel Boton Campo
*/
class CErrorPage {
	/*!
		Error 404.
		\returns the page html code.
	*/
	static std::string E404(void);

	/*!
		Error 500.
		\returns the page html code.
	*/
	static std::string E500(void);

	/*!
		Error 501.
		\returns the page html code.
	*/
	static std::string E501(void);

	/*!
		Generate a generic error page.
		\returns the page html code.
	*/
	static std::string Generate(int reply);

  public:
	/*!
		Get the reply error page.
		\param reply the reply number.
		\returns the page html code.
	*/
	static std::string Get(int reply);
};

#endif	/* _ERROR_PAGE_HPP_ */
