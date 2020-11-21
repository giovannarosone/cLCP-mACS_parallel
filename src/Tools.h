#ifndef TOOLS_H_
#define TOOLS_H_

#include <string>
#include <sstream>
#include <omp.h>

namespace multi_acs {

// Utility class for files name management
class FileName {
private:
	std::string full_name_str;
public:
	FileName(const std::string &str);
	FileName(const char *file_name, const std::string &ext);
	FileName(const std::string &file_name, const std::string &ext);
	FileName(const std::string &file_name, const int i, const std::string &ext);
	const std::string str() const;
	const char* c_str() const;
	static std::string removeExtension(const std::string &file_name);
	static std::string removePath(const std::string &file_name);
	static std::string getPath(const std::string &file_name);
};

// Utility classes for Errors and Warnings Log
class ErrorMessage {
public:
	ErrorMessage(const std::string error_message) :
	error_message(error_message) { }
private:
	const std::string error_message;
};

class Error {
public:
	static void stopWithError(const char* class_name, const char* funcion_name,
			const std::string &message);
	static void continueWithWarning(const char* class_name, const char* funcion_name,
			const std::string &message);
};

} /* namespace multi_acs */

#endif /* TOOLS_H_ */
