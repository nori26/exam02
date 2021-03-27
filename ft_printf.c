#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdint.h>
#ifndef REAL
# define F ft_printf
#else 
# define F printf
#endif

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

int put_nbr(uint32_t u, char *num, int base, int len)
{
	if (u >= (uint32_t)base)
		len = put_nbr(u / base, num, base, len);
	num[len++] = "0123456789abcdef"[u % base];
	return (len);
}

int itoa_base(int n, char *flag, char *num)
{
	uint32_t u;
	int base;

	base = flag['x'] ? 16 : 10;
	u = !flag['x'] && n < 0 ? -n : n;
	return (put_nbr(u, num, base, 0));
}

int integer(va_list *ap, char *flag, int field, int prec)
{
	int n;
	int digit;
	int len;
	int zero;
	int space;
	char num[30] = {};

	n = va_arg(*ap, int);
	digit = itoa_base(n, flag, num);
	if (!prec && flag['.'] && !n)
		digit = 0;
	len = digit;
	zero = prec > len ? prec - len : 0;
	len += zero;
	if (n < 0 && !flag['x'])
		len++;
	space = field > len ? field - len : 0;
	while (space--)
		write(1, " ", 1);
	if (n < 0 && !flag['x'])
		write(1, "-", 1);
	while (zero--)
		write(1, "0", 1);
	write(1, num, digit);
	return (field > len ? field : len);
}

int put_str(char *s, char *flag, int field, int prec)
{
	int space;
	int len;
	int i;

	if (!s)
		return (put_str("(null)", flag, field, prec));
	i = 0;
	while (s[i])
		i++;
	len = i > prec && flag['.'] ? prec : i;
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

int convert(char **s, va_list *ap)
{
	int field;
	int prec;
	char flag[255] ={};

	if (**s != '%')
		return (0);
	(*s)++;
	if (**s == ' ' || **s == '0' || **s == '-')
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
	va_list ap;
	int		ret;

	va_start(ap, format);
	ret = print_va(&ap, format);
	va_end(ap);
	return (ret);
}
// int main()
// {
// 	// int i = 0;

// 	// while (i++ < 100)
// 	// {
// 	// 	printf("%d\n", F("%10d\n", i));
// 	// 	printf("%d\n", F("%10x\n", i));
// 	// 	printf("%d\n", F("%.10d\n", i));
// 	// 	printf("%d\n", F("%10.10d\n", i));
// 	// }
// 	// while (i-- > -100)
// 	// {
// 	// 	printf("%d\n", F("%10d\n", i));
// 	// 	printf("%d\n", F("%10x\n", i));
// 	// 	printf("%d\n", F("%.10d\n", i));
// 	// 	printf("%d\n", F("%10.10d\n", i));
// 	// }
// 		// printf("%d\n", ft_printf("%10s\n", "aiueo"));
// 		// printf("%d\n", ft_printf("%s\n", "aiueo"));
// 		// printf("%d\n", F("%10s\n", "aiueoaiueo"));
// 		// printf("%d\n", F("%.10s\n", "aiueo"));
// 		// printf("%d\n", F("%10.10s\n", "aiueoaiueoa"));
// 		ft_printf("%.5s%7s", "yo", "boi");
// }
