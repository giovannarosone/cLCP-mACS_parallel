#include "Tools.h"
#include <iostream>
#include <string>
#include <sstream>

namespace multi_acs {

FileName::FileName(const std::string& str) : full_name_str(str) {

}

FileName::FileName(const char* file_name, const std::string& ext) {
	std::ostringstream fn;
	fn << file_name << ext;
	full_name_str = fn.str();
}

FileName::FileName(const std::string& file_name,
		const std::string& ext) {
	full_name_str = file_name + ext;
}

FileName::FileName(const std::string& file_name, const int i,
		const std::string& ext) {
	std::ostringstream fn;
	fn << file_name << i << ext;
	full_name_str = fn.str();
}

const std::string FileName::str() const {
	return full_name_str;
}

const char* multi_acs::FileName::c_str() const {
	return full_name_str.c_str();
}

std::string FileName::removeExtension(const std::string& full_name) {
	return full_name.substr(0, full_name.find_last_of("."));
}

std::string FileName::removePath(const std::string& full_name) {
	return full_name.substr(full_name.find_last_of("/") + 1, full_name.length());
}

std::string FileName::getPath(const std::string& full_name) {
	size_t i = full_name.find_last_of("/");
	if(i != std::string::npos)
		return full_name.substr(0, full_name.find_last_of("/"));
	else
		return ".";
}


void Error::stopWithError(const char* class_name,
		const char* function_name, const std::string& message) {
	std::ostringstream stream;
	stream << class_name << "::" << function_name << " ERROR: " << message;
	std::cerr << stream.str() << std::endl;
	exit(EXIT_FAILURE);
}

void Error::continueWithWarning(const char* class_name,
		const char* funcion_name, const std::string& message) {
	std::ostringstream stream;
	stream << class_name << "::" << funcion_name << " WARNING: " << message;
	std::cerr << stream.str() << std::endl;
}

} /* namespace multi_acs */




