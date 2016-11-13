#include <limits.h>
#include <stdio.h>

int main() {
	printf("INT_MAX: %d\n", INT_MAX);
	printf("INT_MIN: %d\n", INT_MIN);
	printf("UINT_MAX: %u\n", UINT_MAX);
	printf("UINT_MAX(wrongly using %%d): %d\n", UINT_MAX);
	printf("LONG_MAX: %ld\n", LONG_MAX);
	printf("LONG_MIN: %ld\n", LONG_MIN);
	printf("ULONG_MAX: %lu\n", ULONG_MAX);
	printf("ULONG_MAX(wrongly using %%ld): %ld\n", ULONG_MAX);
	printf("-1 using %%u: %u\n", (unsigned int) -1);
	printf("-1L using %%lu: %lu\n", (unsigned long) -1L);
	printf("u = %u, tu = %d\n", 4294967295u, (int) 4294967295u);
}