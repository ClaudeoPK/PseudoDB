#pragma once
#ifndef _WINDOWS_IMPL_
#include <unistd.h>
#include <fcntl.h>
#else
#include <fstream>
#endif


void* uvOpen(const char* path,  bool mRead);
void uvClose(void* f, bool mRead);
void uvRead(void* f, char* buffer, int size);
void uvWrite(void* f, char* buffer, int size);