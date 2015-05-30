#pragma once

#ifndef LOGREADER_H
#define LOGREADER_H

#include <cstdio>
#include <cstdlib>
#include <cstring>

class CLogReader final
{
public:
	// disable default and copy constructor, C++11
	CLogReader() = delete;
	CLogReader(const CLogReader& that) = delete;

	CLogReader(const char *filename);
	~CLogReader();

	bool Open();
	void Close();

	// sets string filter,
	// returns true if ok, false on error
	bool SetFilter(const char *a_filter);

	// sets next line passed by filter in *buf, 
	bool GetNextLine(char *buf, const size_t bufsize);

private:
	const char *filename_;
	FILE *file_;
	bool fileOpened_;

	char *filter_;

	// returns next read line from file in a_result, size in a_size
	size_t GetNextLineSize(bool seekBack = true);
	char * ReadNextLineDynamic();
	
	char * ReadNextLine();
};

#endif // LOGREADER_H