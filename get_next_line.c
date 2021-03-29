#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>


size_t ft_strlen(char *s)
{
	size_t i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

char *free_set(char *s, char *ret)
{
	free(s);
	return (ret);
}

char *ft_strchr(char *s, char c)
{
	while (*s && *s != (char)c)
		s++;
	return (*s == (char )c ? s : NULL);
}

char *ft_strjoin(char *s1, char *s2)
{
	char *ret;
	char *tmp;
	size_t len1;
	size_t len2;

	len1 = 0;
	len2 = 0;
	if (s1)
		len1 = ft_strlen(s1);
	if (s2)
		len2 = ft_strlen(s2);
	if (!(ret = malloc(len1 + len2 + 1)))
		return (NULL);
	tmp = ret;
	while (s1 && *s1)
		*tmp++ = *s1++;
	while (s2 && *s2)
		*tmp++ = *s2++;
	*tmp = '\0';
	return (ret);
}

int join_buf(char **line, char *buf, char *p)
{
	if (p)
	{
		*p = '\0';
		if (!(*line = free_set(*line, ft_strjoin(*line, buf))))
			return (-1);
		while (*++p)
			*buf++ = *p;
		*buf = '\0';
		return (1);
	}
	else if (!(*line = free_set(*line, ft_strjoin(*line, buf))))
		return (-1);
	return (0);
}

int get_next_line(int fd, char **line)
{
	int		ret;
	int64_t len;
	static char buf[BUFFER_SIZE + 1];

	*line = NULL;
	if ((ret = join_buf(line, buf, ft_strchr(buf, '\n'))))
		return (ret);
	while ((len = read(fd, buf, BUFFER_SIZE)) >= 0)
	{
		buf[len] = '\0';
		if ((ret = join_buf(line, buf, ft_strchr(buf, '\n'))) || !len)
			return (ret);
	}
	*line = free_set(*line, NULL);
	return (-1);
}

int main()
{
	char *line;

	get_next_line(-1, &line);
}