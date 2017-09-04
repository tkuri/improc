
#pragma once

#include <string>

class FilenameParser
{
public:
	//! Remove folder part from the path and returns filename only.
	static std::string getFilename(const std::string& path);
	//! Return folder name only. Returned string does not include last \ or /.
	static std::string getFolder(const std::string& path);
	//! Return number part of the file.
	static std::string getNumber(const std::string& path);
	//! Return extension of the file.
	static std::string getExtension(const std::string& path);
	//! Return path without extension.
	static std::string removeExtension(const std::string& path);

protected:
	FilenameParser(void);
	virtual ~FilenameParser(void);
};
