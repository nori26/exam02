#include <unistd.h>

int main(int argc, char **s)
{
	char flag[256] = {};

	if (argc != 3)
		return (!write(1, "\n", 1));
	while (*s[1])
		if (!flag[(int)*s[1]++])
			flag[(int)*s[1]++] += write(1, --s[1], 1);
	while (*s[2])
		if (!flag[(int)*s[2]++])
			flag[(int)*s[2]++] += write(1, --s[2], 1);
	write(1, "\n", 1);
}