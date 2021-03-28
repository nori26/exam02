#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	char ascii[255] = {};

	if (argc != 3)
		return (!write(1, "\n", 1));
	while (*argv[1])
		if (!ascii[(int)*argv[1]++])
			ascii[(int)*argv[1]++] += write(1, --argv[1], 1);
	while (*argv[2])
		if (!ascii[(int)*argv[2]++])
			ascii[(int)*argv[2]++] += write(1, --argv[2], 1);
	write(1, "\n", 1);
}