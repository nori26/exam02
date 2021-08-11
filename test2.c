#include <stdio.h>

struct foo {
unsigned int base:19;
};

int main()
{
	struct foo a;
	unsigned long lu;

	a.base = 1;
	lu = a.base << 31;
	printf("%#x\n0x%lx\n", a.base, lu);
	lu = (unsigned int)(a.base << 13);
	printf("0x%lx\n", lu);
}
