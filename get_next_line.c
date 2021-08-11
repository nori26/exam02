#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#ifndef BUFFER_SIZE
# define BUFFER_SIZE 100
#endif
# define ASCII_001 0x0101010101010101
# define SIGN_MASK 0x8080808080808080
typedef struct s_list	t_list;
struct		s_list
{
	void	*content;
	t_list	*next;
};
typedef struct	s_buf
{
	char	buf[BUFFER_SIZE];
	ssize_t	len;
	size_t	total_len;
	ssize_t	n_len;
	t_list	*start;
}t_buf;

void	*ft_memchr(const void *s, int c, size_t n)
{
	uint64_t const	*longptr;
	uint64_t		char_mask;

	while ((uint64_t)s & 0b111 && n)
	{
		if (*(char *)s++ == (unsigned char)c)
			return ((void *)--s);
		--n;
	}
	longptr = (uint64_t *)s;
	char_mask = c | (c << 8);
	char_mask |= char_mask << 16;
	char_mask |= char_mask << 32;
	while (n > 8)
	{
		if (((*longptr ^ char_mask) - ASCII_001) & ~*longptr & SIGN_MASK)
			break ;
		longptr++;
		n -= 8;
	}
	s = (const char *)longptr;
	while (n--)
		if (*(char *)s++ == (unsigned char)c)
			return ((void *)--s);
	return (NULL);
}

int	ft_setptr(void *dest, void *p)
{
	*(uintptr_t *)dest = (uintptr_t)p;
	return (!!p);
}

t_list	*ft_lstnew(void *content)
{
	t_list	*lst;

	if (!ft_setptr(&lst, malloc(sizeof(t_list))))
		return (NULL);
	lst->content = content;
	lst->next = NULL;
	return (lst);
}

void	ft_lstdelone(t_list *lst, void (*del)(void*))
{
	if (!lst || !del)
		return ;
	if (lst->content)
		del(lst->content);
	free(lst);
}

t_list	*del_set(t_list *lst, t_list *next, void (*del)(void*))
{
	ft_lstdelone(lst, del);
	return (next);
}

void	ft_lstclear(t_list **lst, void (*del)(void*))
{
	if (!lst)
		return ;
	while (*lst)
		*lst = del_set(*lst, (*lst)->next, del);
}

void	del_content(void *content)
{
	free(content);
}

static int	exit_gnl(t_list **lst, t_list **save)
{
	t_list *tmp;

	tmp = *lst;
	ft_lstclear(&tmp, del_content);
	ft_lstdelone(*save, del_content);
	*save = NULL;
	return (-1);
}

static int	lst_init(t_list **lst, t_list *start)
{
	t_buf	*content;

	content = malloc(sizeof(t_buf));
	*lst = ft_lstnew(content);
	if (!content || !lst)
		return (1);
	((t_buf *)((*lst)->content))->start = start;
	return (0);
}

static size_t	bufs_len(t_list *end, char *n_ptr)
{
	size_t	size;
	t_buf	*content;

	content = end->content;
	size = content->total_len;
	if (!n_ptr)
		return (size);
	content->n_len = n_ptr - content->buf + 1;
	size -= content->len - (content->n_len - 1);
	return (size);
}

static size_t	line_size(t_list *end, t_list *save, char *n_ptr)
{
	size_t	size;

	size = bufs_len(end, n_ptr);
	if (save)
		size += ((t_buf *)(save->content))->len
				- ((t_buf *)(save->content))->n_len;
	return (size);
}

static char	*till_ln(char *line, t_list *ln_buf, char *n_ptr, size_t start_offset)
{
	size_t		i;
	uint64_t	*longline;
	t_buf		*content;
	char		*buf;

	content = ln_buf->content;
	buf = content->buf + start_offset;
	if (!n_ptr)
		n_ptr = content->buf + content->len - 1;
	i = 0;
	if (!((uintptr_t)buf & 0b111))
	{
		longline = (uint64_t *)line;
		while (buf +  i * 8 <= n_ptr - 7)
			*longline++ = ((uint64_t *)buf)[i++];
		line = (char *)longline;
		i *= 8;
	}
	while (buf + i <= n_ptr)
		*line++ = buf[i++];
	return (line);
}

static void	copy_byte(char **line, t_buf *content)
{
	ssize_t	i;

	i = 0;
	while (i < content->len)
		*(*line)++ = content->buf[i++];
}

static void copy_qword(char **line, t_buf *content)
{
	ssize_t		i;
	uint64_t	*longline;

	i = 0;
	longline = (uint64_t *)*line;
	while (content->len - i * 8 > 7)
		*longline++ = ((uint64_t *)content->buf)[i++];
	*line = (char *)longline;
	i *= 8;
	while (i < content->len)
		*(*line)++ = content->buf[i++];
}

static void	buf_to_line(char *line, t_list *end, char *n_ptr, void (*copy)())
{
	t_list	*lst;
	t_list	*prev;

	prev = NULL;
	lst = ((t_buf *)(end->content))->start;
	if (!n_ptr)
		end = end->next;
	while (lst != end)
	{
		copy(&line, lst->content);
		prev = lst;
		lst = lst->next;
	}
	if (!n_ptr)
		return ;
	till_ln(line, end, n_ptr, 0);
	if (prev)
		prev->next = end->next;
	else
		((t_buf *)end->content)->start = ((t_buf *)end->content)->start->next;
}

static void	lst_to_line(char *line, t_list *end, t_list *save, char *n_ptr)
{
	if (save)
		line = till_ln(line, save, NULL, ((t_buf *)(save->content))->n_len);
	if ((uintptr_t)line & 0b111)
		buf_to_line(line, end, n_ptr, copy_byte);
	else
		buf_to_line(line, end, n_ptr, copy_qword);
}

static int	make_line(char **line, t_list *end, t_list **save, char *n_ptr)
{
	size_t	size;

	size = line_size(end, *save, n_ptr);
	*line = malloc(size + 1);
	if (!*line)
		return (exit_gnl(&((t_buf *)((end)->content))->start, save));
	lst_to_line(*line, end, *save, n_ptr);
	(*line)[size] = '\0';
	if (n_ptr)
	{
		ft_lstdelone(*save, del_content);
		*save = end;
	}
	else
		exit_gnl(&((t_buf *)((end)->content))->start, save);
	return (!!n_ptr);
}

static int	save_to_line(char **line, t_list **save, char *n_ptr)
{
	size_t	size;
	size_t	new_nlen;

	new_nlen = n_ptr - ((t_buf *)((*save)->content))->buf + 1;
	size = new_nlen - ((t_buf *)((*save)->content))->n_len - 1;
	*line = malloc(size + 1);
	if (!*line)
		return (exit_gnl(&((t_buf *)((*save)->content))->start, save));
	till_ln(*line, *save, n_ptr, ((t_buf *)((*save)->content))->n_len);
	(*line)[size] = '\0';
	((t_buf *)((*save)->content))->n_len = new_nlen;
	return (1);
}

void	lst_set_value(t_buf *content, size_t len, t_list *start)
{
	content->total_len = len;
	content->start = start;
}

int	read_and_makeline(int fd, char **line, t_list **save, t_list **lst)
{
	char	*n_ptr;
	t_buf	*content;

	while (1)
	{
		content = (*lst)->content;
		content->len = read(fd, content->buf, BUFFER_SIZE);
		if (content->len == -1)
			break ;
		if (!content->len)
			return (make_line(line, *lst, save, NULL));
		content->total_len += content->len;
		if (ft_setptr(&n_ptr, ft_memchr(content->buf, '\n', content->len)))
			return (make_line(line, *lst, save, n_ptr));
		if (!(*lst)->next && lst_init(&((*lst)->next), content->start))
			break ;
		*lst = (*lst)->next;
		lst_set_value((*lst)->content, content->total_len, content->start);
	}
	return (exit_gnl(&((t_buf *)((*lst)->content))->start, save));
}

int	gnl_init(t_list **save, t_list **lst, char **line)
{
	t_buf	*content;
	char	*n_ptr;

	if (!line)
	{
		if (*save)
			exit_gnl(&((t_buf *)((*save)->content))->start, save);
		return (-1);
	}
	if (*save)
	{
		content = (*save)->content;
		n_ptr = ft_memchr(content->buf + content->n_len, '\n',
				content->len - content->n_len);
		if (n_ptr)
			return (save_to_line(line, save, n_ptr));
		*lst = ((t_buf *)((*save)->content))->start;
	}
	if ((!*save || !*lst) && lst_init(lst, NULL))
		return (-1);
	return (0);
}

int	get_next_line(int fd, char **line)
{
	static 	t_list	*save;
	t_list			*lst;
	int				ret;

	ret = gnl_init(&save, &lst, line);
	if (ret)
		return (ret);
	lst_set_value(lst->content, 0, lst);
	return (read_and_makeline(fd, line, &save, &lst));
}
