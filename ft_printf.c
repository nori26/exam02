#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>

int field_width(char **s)
{
	int field = 0;

	while ('0' <= **s && **s <= '9')
		field = field * 10 + *(*s)++ - '0';
	return (field);
}

int precision(char **s, char *flag)
{
	int prec = 0;

	if (**s != '.')
		return (0);
	(*s)++;
	flag['.']++;
	while ('0' <= **s && **s <= '9')
		prec = prec * 10 + *(*s)++ - '0';
	return (prec);
}

void conversion(char **s, char *flag)
{
	if (**s == 'd' || **s == 'x' || **s == 's')
		flag[(int)*(*s)++]++;
}

int nbr(uint32_t u, uint32_t base, char *num, int len)
{
	if (u >= base)
		len = nbr(u / base, base, num, len);
	num[len++] = "0123456789abcdef"[u % base];
	return (len);
}

int itoa_base(int n, char *num, char *flag)
{
	uint32_t u;
	uint32_t base;

	base = 10 + 6 * flag['x'];
	u = flag['x'] || n > 0 ? n : -n;
	return (nbr(u, base, num, 0));
}

int integer(va_list *ap, char *flag, int prec, int field)
{
	int n;
	int len;
	int zero;
	int space;
	int digit;
	char num[30] = {};

	n = va_arg(*ap, int);
	digit = itoa_base(n, num, flag);
	if (flag['.'] && !prec && !n)
		digit = 0;
	zero = prec > digit ? prec - digit : 0;
	len = digit + zero;
	if (n < 0 && flag['d'])
		len++;
	space = field > len ? field - len : 0;
	while (space--)
		write(1, " ", 1);
	if (n < 0 && flag['d'])
		write(1, "-", 1);
	while (zero--)
		write(1, "0", 1);
	write(1, num, digit);
	return (field > len ? field : len);
}

int put_str(char *s, int prec, int field, char *flag)
{
	int space;
	int len;

	if (!s)
		return (put_str("(null)", prec, field, flag));
	len = 0;
	while (s[len])
		len++;
	len = flag['.'] && prec < len ? prec : len;
	space = field > len ? field - len : 0;
	while (space--)
		write(1, " ", 1);
	write(1, s, len);
	return (field > len ? field : len);
}

int character(va_list *ap, char *flag, int prec, int field)
{
	char *s;

	s = va_arg(*ap, void *);
	return (put_str(s, prec, field, flag));
}

int convert(char **s, va_list *ap)
{
	int field;
	int prec;
	char flag[255] = {};

	while (**s == '%' || **s == ' ' || **s == '-' || **s == '0')
		(*s)++;
	field = field_width(s);
	prec = precision(s, flag);
	conversion(s, flag);
	if (flag['d'] || flag['x'])
		return (integer(ap, flag, prec, field));
	else if (flag['s'])
		return (character(ap, flag, prec, field));
	return (0);
}

int till_per(char **s)
{
	char *tmp = *s;

	while (**s && **s != '%')
		(*s)++;
	return (write(1, tmp, *s - tmp));
}

int print_va(va_list *ap, char *fmt)
{
	int ret;
	
	ret = 0;
	while (*fmt)
	{
		ret += till_per(&fmt);
		ret += convert(&fmt, ap);
	}
	return (ret);
}

int ft_printf(char *fmt, ...)
{
	int ret;
	va_list ap;

	va_start(ap, fmt);
	ret = print_va(&ap, fmt);
	va_end(ap);
	return (ret);
}