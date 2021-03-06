#ifndef BACKTRACE_H
#define BACKTRACE_H

#ifdef USE_BACKTRACE
#include <execinfo.h>
#include <stdlib.h> /* for free() */
#include <iostream>

using namespace std;
#endif

static void backtrace()
{
#ifdef USE_BACKTRACE
	void *callers[100];
	size_t size = backtrace(callers, 100);
	char **symbols = backtrace_symbols(callers, size);
	for (size_t i = 0; i < size; i++) cerr << symbols[i] << endl;
	free(symbols);
#endif
}

#endif
