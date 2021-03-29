#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

int field_width(char **s)
{
	int field;

	field = 0;
	while ('0' <= **s && **s <= '9')
		field = field * 10 + *(*s)++ - '0';
	return (field);
}

int precision(char **s, char *flag)
{
	int prec;

	if (**s != '.')
		return (0);
	(*s)++;
	flag['.']++;
	prec = 0;
	while ('0' <= **s && **s <= '9')
		prec = prec * 10 + *(*s)++ - '0';
	return (prec);
}

void conversion(char **s, char *flag)
{
	if (**s == 'd' || **s == 'x' || **s == 's')
		flag[(int)*(*s)++]++;
}

int put_nbr(uint32_t n, char *num, uint32_t base, int len)
{
	if (n >= base)
		len = put_nbr(n / base, num, base, len);
	num[len++] = "0123456789abcdef"[n % base];
	return (len);
}

int itoa_base(int n, char *flag, char *num)
{
	uint32_t u;
	uint32_t base;

	base = 10 + 6 * flag['x'];
	u = flag['x'] || n > 0 ? n : -n;
	return (put_nbr(u, num, base, 0));
}

int integer(va_list *ap, int field, int prec, char *flag)
{
	int		n;
	int		len;
	int		digit;
	int		zero;
	int		space;
	char	num[30] = {};

	n = va_arg(*ap, int);
	digit = itoa_base(n, flag, num);
	if (!prec && !n && flag['.'])
		digit = 0;
	len = digit;
	zero = prec > len ? prec - len : 0;
	len += zero;
	if (flag['d'] && n < 0)
		len++;
	space = field > len ? field - len : 0;
	while (space--)
		write(1, " ", 1);
	if (flag['d'] && n < 0)
		write(1, "-", 1);
	while (zero--)
		write(1, "0", 1);
	write (1, num, digit);
	return (field > len ? field : len);
}

int put_str(char *s, int field, int prec, char *flag)
{
	int i;
	int space;
	int len;

	if (!s)
		return (put_str("(null)", field, prec, flag));
	i = 0;
	while (s[i])
		i++;
	len = prec < i && flag['.'] ? prec : i;
	space = field > len ? field - len : 0;
	while (space--)
		write(1, " ", 1);
	write(1, s, len);
	return (field > len ? field : len);
}

int character(va_list *ap, int field, int prec, char *flag)
{
	char *s;

	s = va_arg(*ap, void *);
	return (put_str(s, field, prec, flag));
}

int convert(char **s, va_list *ap)
{
	int field;
	int prec;
	char flag[255] = {};

	while (**s == '%' || **s == '0' || **s == '-' || **s == ' ')
		(*s)++;
	field = field_width(s);
	prec = precision(s, flag);
	conversion(s, flag);
	if (flag['d'] || flag['x'])
		return (integer(ap, field, prec, flag));
	else if (flag['s'])
		return (character(ap, field, prec, flag));
	return (0);
}

int till_per(char **s)
{
	char *tmp;

	tmp = *s;
	while (**s != '%' && **s)
		(*s)++;
	return (write(1, tmp, *s - tmp));
}

int print_va(va_list *ap, char *s)
{
	int ret;

	ret = 0;
	while (*s)
	{
		ret += till_per(&s);
		ret += convert(&s, ap);
	}
	return (ret);
}

int ft_printf(char *format, ...)
{
	int ret;
	va_list ap;

	va_start(ap, format);
	ret = print_va(&ap, format);
	va_end(ap);
	return (ret);
}

// int main()
// {
// 	ft_printf("%7d", 33);
// }