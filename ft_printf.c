#include <libc.h>
#define PC(c) ret += ft_putchar(c)
#define WRITE_W while (0 < w--){PC(' ');}
#define WRITE_STR(s) while(*s){PC(*s++);}
#define WRITE_NEG if (neg){PC('-');}
#define ATOI(n) while('0' <= *f && *f <= '9'){n = n * 10 + *f++ - '0';}
#define BREAK if(!*f){break;}

int  ft_strlen(char *s)
{
	int i = 0;
	while (s[i])
		i++;
	return i;
}

int ft_putchar(int c)
{
	unsigned char uc;

	uc = (unsigned char)c;
	return write(1, &uc, 1);
}

int doprint(va_list ap, const char *f)
{
	int ret = 0;

	while (*f)
	{
		int w = 0;
		int prec = -1;
		while (*f && *f != '%')
			PC(*f++);
		BREAK;
		if (*f == '%' && *(f+1))
		{
			f++;
			ATOI(w);
			BREAK;
			if (*f == '.')
			{
				f++;
				prec = 0;
				ATOI(prec);
				BREAK;
			}
			if (*f == 's')
			{
				char *s;
				s = va_arg(ap, char *);
				if (!s)
					s = "(null)";
				int len = ft_strlen(s);
				if (prec == -1)
				{
					w -= len;
					WRITE_W;
					WRITE_STR(s);
				}
				else
				{
					w -= MIN(len, prec);
					WRITE_W;
					while (0 < prec-- && *s)
					{
						PC(*s++);
					}
				}
			}
			if (*f == 'd' || *f == 'x')
			{
				int neg = 0;
				int base = 10;
				intmax_t n;
				char buf[100] = {0};
				char *p;
				p = buf;
				p++;

				if (*f == 'd')
				{
					n = va_arg(ap, int);
					if (n < 0)
					{
						neg = 1;
						n *= -1;
					}
				}
				else
				{
					n = va_arg(ap, unsigned int);
					base = 16;
				}
				while (1)
				{
					*p = "0123456789abcdef"[n % base];
					n /= base;
					if (n == 0)
						break;
					p++;
				}
				int len = p - buf;
				if (prec == 0 && *p == '0')
				{
					*p = 0;
					w++;
				}
				if (prec == -1)
				{
					w -= len;
					WRITE_W;
					WRITE_NEG;
				}
				else
				{
					w -= MAX(len, prec);
					prec -= len;
					WRITE_W;
					WRITE_NEG;
					while (0 < prec--)
						PC('0');
				}
				while(*p)
					PC(*p--);
			}
		}
		f++;
	}
	return ret;
}

int ft_printf(const char *fmt, ...)
{
	int ret = 0;
	va_list ap;

	va_start(ap, fmt);
	ret = doprint(ap, fmt);
	va_end(ap);
	return ret;
}
