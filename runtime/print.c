#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

int32_t __garter_print(int32_t nargs, ...)
{
	va_list va;
	char format[nargs * 3 + 2];
	char *p;
	int32_t i;
	int32_t retval;

	va_start(va, nargs);

	p = format;
	for (i = 0; i < nargs - 1; i++) {
		*p++ = '%';
		*p++ = 'd';
		*p++ = ' ';
	}
	for (; i < nargs; i++) {
		*p++ = '%';
		*p++ = 'd';
	}
	*p++ = '\n';
	*p++ = '\0';

	retval = vprintf(format, va);
		
	va_end(va);

	return retval;
}
