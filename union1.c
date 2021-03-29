#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
	char	ascii[255] = {};
	uint8_t	**s;

	if (argc != 3)
		return (!write(1, "\n", 1));
	s = (uint8_t **)argv;
	while (*s[1])
		if (!ascii[*s[1]++])
			ascii[*s[1]++] += write(1, --s[1], 1);
	while (*s[2])
		if (!ascii[*s[2]++])
			ascii[*s[2]++] += write(1, --s[2], 1);
	write(1, "\n", 1);
}
