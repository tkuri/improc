
#include <algorithm>
#include "FilenameParser.h"

FilenameParser::FilenameParser(void)
{
}


FilenameParser::~FilenameParser(void)
{
}



std::string FilenameParser::getFilename(const std::string& path)
{
	size_t pos = path.find_last_of("/\\");
	std::string filename("");
	if(pos == std::string::npos){
		filename = path;
	}
	else{
        filename = path.substr(pos+1, path.size()-pos);
	}
	return filename;
	/*
	int pos0 = path.find_last_of('\\');
	int pos1 = path.find_last_of('/');
	std::string filename("");
	if(pos0 == std::string::npos && pos1 == std::string::npos){
		filename = path;
	}
	else{
		int pos = (pos0 > pos1)? pos0 : pos1;
        filename = path.substr(pos+1, path.size()-pos);
	}
	return filename;
	*/
}
			
std::string FilenameParser::getFolder(const std::string& path)
{
	size_t pos = path.find_last_of("/\\");
	std::string foldername("");
	if(pos != std::string::npos){
        foldername = path.substr(0, pos);
	}
	return foldername;
	/*
	int pos0 = path.find_last_of('\\');
	int pos1 = path.find_last_of('/');
	std::string foldername("");
	if(pos0 != std::string::npos || pos1 != std::string::npos){
		int pos = (pos0 > pos1)? pos0 : pos1;
        foldername = path.substr(0, pos-1);
	}
	return foldername;
	*/
}
		
std::string FilenameParser::getNumber(const std::string& path)
{
	size_t pos = path.find_last_of("1234567890");
	if(pos == std::string::npos){
		return "";
	}
	std::string temp = path.substr(0, pos+1);
	pos = temp.find_last_not_of("1234567890");
	return temp.substr(pos+1, temp.size()-pos);
}
		
std::string FilenameParser::getExtension(const std::string& path)
{
	size_t pos = path.find_last_of('.');
	std::string ext("");
	if(pos != std::string::npos){
        ext = path.substr(pos+1, path.size()-pos);
		std::transform(ext.cbegin(), ext.cend(), ext.begin(), tolower);
	}
	return ext;
}

std::string FilenameParser::removeExtension(const std::string& path)
{
	size_t pos = path.find_last_of('.');
	std::string newPath("");
	if(pos != std::string::npos){
        newPath = path.substr(0, pos);
	}
	return newPath;
}
