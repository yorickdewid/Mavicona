#ifndef _FILE_HPP_
#define _FILE_HPP_

#include <fstream>
#include <string>

/*!
	File class.
	\author Miguel Boton Campo
*/
class CFile {
	static std::string basepath;		//!< File basepath.

	std::fstream file;			//!< File object.

  public:
	/*!
		Get the basepath.
		\returns the current basepath.
	*/
	static std::string GetPath(void);

	/*!
		Set the basepath.
		\param dirpath the directory to use as base.
	*/
	static void SetPath(std::string dirpath);
	static void SetPath(const char *dirpath);

	/*!
		Open a file.
		\param filepath the file path.
		\returns true if success, otherwise false.
	*/
	bool Open(std::string filepath);
	bool Open(const char *filepath);

	/*!
		Close the file.
	*/
	void Close(void);

	/*!
		Get the file size.
		\returns the file size.
	*/
	size_t Size(void);

	/*!
		Read data from the file.
		\param buffer the buffer to store the data.
		\param len the number of bytes to read.
		\returns true if success, otherwise false.
	*/
	bool Read(void *buffer, size_t len);

	/*!
		Write data to the file.
		\param buffer the buffer to write.
		\param len the buffer length.
		\returns true if success, otherwise false.
	*/
	bool Write(void *buffer, size_t len);

	/*!
		Get buffer with the file contents.
		\returns the buffer pointer.
	*/
	char *Get(void);
};

#endif	/* _FILE_HPP_ */
