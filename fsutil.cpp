#include "fsutil.h"

void* uvOpen(const char* path, bool mRead) {
#ifndef _WINDOWS_IMPL_
	int fd = open(path, mRead ? O_RDONLY : (O_RDWR | O_CREAT), 0664);
	if (fd != -1) {
		int* pfd = new int;
		*pfd = fd;
		return pfd;
	}
	return NULL;
#else
	if (mRead) {
		std::ifstream* in = new std::ifstream(path, std::ifstream::in | std::ifstream::binary);
		if (in->is_open()) {
			return in;
		}
		delete in;
		return NULL;
	}
	else {
		std::ofstream* out = new std::ofstream(path, std::ofstream::out | std::ofstream::binary);
		if (out->is_open()) {
			return out;
		}
		delete out;
		return NULL;
	}
#endif
}
void uvClose(void* f, bool mRead) {
#ifndef _WINDOWS_IMPL_
	int fd = *(int*)f;
	close(fd);
	delete (int*)f;
	return;
#else
	if (mRead) {
		std::ifstream* out = (std::ifstream*)f;
		out->close();
		delete out;
		return;
	}
	else {
		std::ofstream* out = (std::ofstream*)f;
		out->close();
		delete out;
		return;
	}
#endif
}
void uvRead(void* f, char* buffer, int size) {
#ifndef _WINDOWS_IMPL_
	int fd = *(int*)f;
	read(fd, (void*)buffer, size);
#else
	std::ifstream* in = (std::ifstream*)f;
	in->read(buffer, size);
#endif
}
void uvWrite(void* f, char* buffer, int size) {
#ifndef _WINDOWS_IMPL_
	int fd = *(int*)f;
	write(fd, (void*)buffer, size);
#else
	std::ofstream* out = (std::ofstream*)f;
	out->write(buffer, size);
#endif
}