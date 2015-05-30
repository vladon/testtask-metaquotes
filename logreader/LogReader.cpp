#include "LogReader.h"

namespace
{
	// from http://www.codeproject.com/Articles/1088/Wildcard-string-compare-globbing
	bool IsStringMatchedFilter(const char *str, const char *filter)
	{
		const char *cp = nullptr;
		const char *mp = nullptr;

		while ((*str) && (*filter != '*')) {
			if ((*filter != *str) && (*filter != '?')) {
				return false;
			}
			++str;
			++filter;
		}

		while (*str) {
			if (*filter == '*') {
				if (!*++filter) {
					return true;
				}

				mp = filter;
				cp = str + 1;
			}
			else if ((*filter == *str) || (*filter == '?')) {
				++filter;
				++str;
			}
			else {
				filter = mp;
				str = cp++;
			}
		}

		while (*filter == '*') {
			++filter;
		}

		return static_cast<bool>(!*filter);
	}
}

CLogReader::CLogReader(const char *filename)
	: filename_(filename),
	file_(nullptr),
	fileOpened_(false),
	filter_(nullptr)
{

}

CLogReader::~CLogReader()
{
	Close();

	free(filter_);
}

bool CLogReader::Open()
{
	bool result;

	errno_t error = fopen_s(&file_, filename_, "rb");
	if (error == 0) {
		result = true;
	}
	else {
		file_ = nullptr;
		result = false;
	}

	return result;
}

void CLogReader::Close()
{
	if (file_ != nullptr) {
		fclose(file_);
	}
}

bool CLogReader::SetFilter(const char *a_filter)
{
	bool result;

	if (a_filter != nullptr) {
		size_t length = strlen(a_filter) + 1;
		// malloc instead of new[] (new[] can throw exceptions)
		filter_ = static_cast<char *>(malloc(length + 1));

		if (filter_ != nullptr) {
			strcpy_s(filter_, length, a_filter);
			filter_[length - 1] = '\0';

			result = true;
		}
		else {
			result = false; // false if cannot allocate
		}
	}
	else {
		result = false; // nullptr passed
	}

	return result;
}

#define LINE_BUFFER_SIZE 4096

char * CLogReader::ReadNextLineDynamic()
{
	char * result = nullptr;
	size_t maxlength = 0;
	
	while (true) {
		char *crlf, *block;

		maxlength += LINE_BUFFER_SIZE;
		if ((result = static_cast<char *>(realloc(result, maxlength + 1))) == nullptr) {
			// error alloc memory
			break;
		}
		block = result + maxlength - LINE_BUFFER_SIZE;

		// +1 for trailing \0
		if (fgets(block, LINE_BUFFER_SIZE + 1, file_) == nullptr) {
			if (block == result) {
				// Error.
				free(result); 
				result = nullptr;
			}
			break;
		}

		// if we found CRLF then break
		if (nullptr != (crlf = strchr(block, '\n'))) {
			*crlf = '\0';
			if (crlf != block) {
				if ('\r' == *(--crlf))
					*crlf = '\0';
			}
			break;
		} 
	} // while

	return result;
}

bool CLogReader::GetNextLine(char *buf, const size_t bufsize)
{
	bool result = false;
	char * line = nullptr;
	
	while ((line = ReadNextLineDynamic()) != nullptr) {
		if (IsStringMatchedFilter(line, filter_)) {
			break;
		}

		free(line);
		line = nullptr;
	}

	if (line != nullptr) {
		size_t lineLength = strlen(line) + 1; // +1 because including trailing \0
		size_t sizeToWrite = (lineLength <= bufsize) ? lineLength : bufsize;

		errno_t error = strcpy_s(buf, sizeToWrite, line);

		if (error == 0) {
			result = true;
		}
		
		free(line);
	}

	return result;
}
