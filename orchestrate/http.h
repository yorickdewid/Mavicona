#ifndef _HTTP_HPP_
#define _HTTP_HPP_

#include <map>
#include <string>

/* Header size */
#define HDR_MAXLEN	2048

/* Reply codes */
#define REPLY_OK	200
#define REPLY_BADREQ	400
#define REPLY_NOENT	404
#define REPLY_INTERR	500
#define REPLY_NOTIMP	501


/*!
	HTTP static class.
	\author Miguel Boton Campo
*/
class CHTTP {
	static std::map<int, const char*> replyMap;	//!< HTTP reply map.

  public:
	/*!
		Initialize the class.
	*/
	static void Init(void);

	/*!
		Get the reply number description.
		\param reply the reply number.
		\returns the description string.
	*/
	static std::string GetDescription(int reply);
};

#endif	/* _HTTP_HPP_ */
