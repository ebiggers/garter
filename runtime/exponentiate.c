#include <inttypes.h>

static int32_t __garter_exponentiate_recursive(int32_t base, int32_t exponent)
{
	if (exponent == 1)
		return base;
	else
		return __garter_exponentiate_recursive(base, exponent / 2) *
		       __garter_exponentiate_recursive(base, (exponent + 1) / 2);
}

int32_t __garter_exponentiate(int32_t base, int32_t exponent)
{
	if (exponent <= 0)
		return !exponent;
	else
		return __garter_exponentiate_recursive(base, exponent);
}
