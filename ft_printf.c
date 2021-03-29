#include <stdarg.h>
#include <stdint.h>
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

int put_nbr(uint32_t u, char *num, uint32_t base, int len)
{
	if (u >= base)
		len = put_nbr(u / base, num, base, len);
	num[len++] = "0123456789abcdef"[u % base];
	return (len);
}

int itoa_base(int n, char *flag, char *num)
{
	uint32_t base;
	uint32_t u;

	base = 10 + 6 * flag['x'];
	u = flag['x'] || n > 0 ? n : -n;
	return (put_nbr(u, num, base, 0));
}

int integer(va_list *ap, char *flag, int field, int prec)
{
	int n;
	int zero;
	int space;
	int digit;
	int len;
	char num[30] = {};

	n = va_arg(*ap, int);
	digit = itoa_base(n, flag, num);
	if (!prec && !n && flag['.'])
		digit = 0;
	zero = prec > digit ? prec - digit : 0;
	len = zero + digit;
	if (flag['d'] && n < 0)
		len++;
	space = field > len ? field - len : 0;
	while (space--)
		write(1, " ", 1);
	if (flag['d'] && n < 0)
		write(1, "-", 1);
	while (zero--)
		write(1, "0", 1);
	write(1, num, digit);
	return (field > len ? field : len);
}

int put_str(char *s, char *flag, int field, int prec)
{
	int i;
	int len;
	int space;

	if (!s)
		return (put_str("(null)", flag, field, prec));
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

int character(va_list *ap, char *flag, int field, int prec)
{
	char *s;

	s = va_arg(*ap, void *);
	return (put_str(s, flag, field, prec));
}

int convert(va_list *ap, char **s)
{
	int field;
	int prec;
	char flag[255] = {};

	while (**s == '%' || **s == '0' || **s == ' ' || **s == '-')
		(*s)++;
	field = field_width(s);
	prec = precision(s, flag);
	conversion(s, flag);
	if (flag['d'] || flag['x'])
		return (integer(ap, flag, field, prec));
	else if (flag['s'])
		return (character(ap, flag, field, prec));
	return (0);
}

int till_per(char **s)
{
	char *tmp;

	tmp = *s;
	while (**s && **s != '%')
		(*s)++;
	return (write(1, tmp, *s - tmp));
}

int print_va(va_list *ap, char *format)
{
	int ret;

	ret = 0;
	while (*format)
	{
		ret += till_per(&format);
		ret += convert(ap, &format);
	}
	return (ret);
}

int ft_printf(char *format, ...)
{
	va_list ap;
	int		ret;

	va_start(ap, format);
	ret = print_va(&ap, format);
	va_end(ap);
	return (ret);
}