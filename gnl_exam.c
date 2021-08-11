int		check_buf(char *buf, char **line, char *p)
{
	if (p)
	{
		*p = '\0';
		if (!(*line = free_set(*line, ft_strjoin(*line, buf))))
			return (-1));
		while (*++p)
			*buf++ = *p;
		*buf = '\0';
		return (1);
	}
	else if (!(*line = free_set(*line, ft_strjoin(*line, buf))))
		return (-1));
	return (0);
}

int		get_next_line(int fd, char **line)
{
	int			ret;
	char		*p;
	int64_t		len;
	static char	buf[(uint64_t)BUFFER_SIZE + 1];

	*line = NULL;
	if ((ret = check_buf(&buf, line, ft_strchr(buf, '\n'))))
		return (ret);
	while (1)
	{
		if ((len = read(fd, buf, BUFFER_SIZE)) < 1)
			break ;
		buf[len] = '\0';
		if ((p = ft_strchr(buf, '\n')))
			return (check_buf(&buf, line, p));
		if (!(*line = free_set(*line, ft_strjoin(*line, buf))))
			return (freeturn(&buf, -1));
	}
	return (len ? -1 : 0);
}
