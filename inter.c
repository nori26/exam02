#include <stdint.h>
#include <unistd.h>

int main(int argc, char **s)
{
	char ascii[255] = {};

	if (argc != 3)
		return (!write(1, "\n", 1));
	while (*s[2])
		ascii[(int)*s[2]++] = 1;
	while (*s[1])
		if (ascii[(int)*s[1]++]-- > 0)
			write(1, s[1] - 1, 1);
	write(1, "\n", 1);
}