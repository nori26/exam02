#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int precision(char **s, va_list *ap, char *flag)
{
	int prec;

	prec = 0;
	if (**s != '.')
		return (0);
	(*s)++;
	flag['.']++;
	while ('0' <= **s && **s <= '9')
		prec = prec * 10 + *(*s)++ - '0';
	if (**s == '*')
	{
		prec = va_arg(*ap, int);
		(*s)++;
	}
	return (prec);
}

int field_width(char **s, va_list *ap)
{
	int field;

	field = 0;
	while ('0' <= **s && **s <= '9')
		field = field * 10 + *(*s)++ - '0';
	if (**s == '*')
	{
		field = va_arg(*ap, int);
		(*s)++;
	}
	return (field);
}

void conversion(char **s, char *flag)
{
	if (**s == 'd' || **s == 'x' || **s == 's')
		flag[(int)*(*s)++]++;
}

int nbr_base(uint32_t n, int base, int len, char *num)
{
	if (n >= (uint32_t)base)
		len = nbr_base(n / base, base, len, num);
	num[len++] = "0123456789abcdef"[n % base];
	return (len);
}

int itoa_base(int32_t n, char *flag, char *num)
{
	int			base;
	uint32_t	u;

	base = flag['d'] ? 10 : 16;
	u = n > 0 || flag['x'] ? n : -n;
	return (nbr_base(n, base, 0, num));
}

int integer(va_list *ap, int field, int prec, char *flag)
{
	int		n;
	int		zero;
	int		digit;
	int		space;
	int		len;
	char	num[30] = {};

	n = va_arg(*ap, int);
	digit = itoa_base(n, flag, num);
	if (!prec && !n && flag['.'])
		digit = 0;
	len = digit;
	zero = prec > digit ? prec - digit : 0;
	if (flag['d'] && n < 0)
		len++;
	len += zero;
	space = len > field ? 0 : field - len;
	while (space--)
		write(1, " ", 1);
	if (flag['d'] && n < 0)
		write(1, "-", 1);
	while (zero--)
		write(1, "0", 1);
	write(1, num, digit);
	return (field > len ? field : len);
}

int put_str(char *s, int32_t field, int32_t prec, char *flag)
{
	int	i;
	int	len;
	int	space;

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

int character(va_list *ap, int32_t field, int32_t prec, char *flag)
{
	char *s;

	s = va_arg(*ap, void *);
	return (put_str(s, field, prec, flag));
}

int convert(char **s, va_list *ap)
{
	int		field;
	int		prec;
	char	flag[150] = {};

	if (!**s)
		return (0);
	(*s)++;
	field = field_width(s, ap);
	prec = precision(s, ap, flag);
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

int print_va(char *s, va_list *ap)
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

int ft_printf(const char *format, ...)
{
	int		ret;
	va_list ap;

	va_start(ap, format);
	ret = print_va((char *)format, &ap);
	va_end(ap);
	return (ret);
}

int main()
{
	ft_printf("%5.d", 0);
}