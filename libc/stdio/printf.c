#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static bool print(const char* data, size_t length) 
{
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
	{
		if (putchar(bytes[i]) == EOF)
		{
			return false;
		}
	}

	return true;
}

char tbuf[32];
char bchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void itoa(unsigned i,unsigned base,char* buf) 
{
	int pos = 0;
	int opos = 0;
	int top = 0;

	if (i == 0 || base > 16) 
	{
		buf[0] = '0';
		buf[1] = '\0';
		return;
	}

	while (i != 0) 
	{
		tbuf[pos] = bchars[i % base];
		pos++;
		i /= base;
	}

	top=pos--;

	for (opos=0; opos<top; pos--,opos++) 
	{
		buf[opos] = tbuf[pos];
	}

	buf[opos] = 0;
}

void itoa_s(int i,unsigned base,char* buf) 
{
	if (base > 16)
	{ 
		return;
	}

	if (i < 0) 
	{
		*buf++ = '-';
		i *= -1;
	}

	itoa(i, base,buf);
}

long strtol(const char* nptr, char** endptr, int base)
{
	const char *s = nptr;
	unsigned long acc;
	int c;
	unsigned long cutoff;
	int neg = 0, any, cutlim;

	do
	{
		c = *s++;
	} while (isspace(c));

	if (c == '-')
	{
		neg = 1;
		c = *s++;
	} 
	else if (c == '+')
	{
		c = *s++;
	}

	if ((base == 0 || base == 16) 
		&& c == '0' && (*s == 'x' || *s == 'X')) 
	{
		c = s[1];
		s += 2;
		base = 16;
	} 
	else if ((base == 0 || base == 2) 
		&& c == '0' && (*s == 'b' || *s == 'B')) 
	{
		c = s[1];
		s += 2;
		base = 2;
	}

	if (base == 0)
	{
		base = c == '0' ? 8 : 10;
	}

	cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
	cutlim = cutoff % (unsigned long)base;
	cutoff /= (unsigned long)base;

	for (acc = 0, any = 0;; c = *s++)
	{
		if (isdigit(c))
		{
			c -= '0';
		}
		else if (isalpha(c))
		{
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		}
		else
		{
			break;
		}

		if (c >= base)
		{
			break;
		}

		if (any < 0 || acc > cutoff || acc == cutoff && c > cutlim)
		{
			any = -1;
		}
		else 
		{
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) 
	{
		acc = neg ? LONG_MIN : LONG_MAX;
	} 
	else if (neg)
	{
		acc = -acc;
	}

	if (endptr != 0)
	{
		*endptr = (char *)(any ? s - 1 : nptr);
	}

	return (acc);
}

int atoi(const char* str)
{
	return (int)strtol(str, 0, 10);
}

int printf(const char* __restrict__ format, ...) 
{
	va_list parameters;
	va_start(parameters, format);

	int written = 0;

	while (*format != '\0') 
	{
		size_t maxrem = INT_MAX - written;

		if (format[0] != '%' || format[1] == '%') 
		{
			if (format[0] == '%')
			{
				format++;
			}

			size_t amount = 1;
			while (format[amount] && format[amount] != '%')
			{
				amount++;
			}

			if (maxrem < amount) 
			{
				return -1;
			}

			if (!print(format, amount))
			{
				return -1;
			}

			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format++;

		if (*format == 'c') 
		{
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);

			if (!maxrem) 
			{
				return -1;
			}

			if (!print(&c, sizeof(c)))
			{
				return -1;
			}

			written++;
		} 
		else if (*format == 'd' || *format == 'i') 
		{
			format++;
			int d = va_arg(parameters, int);
			char str[32]={0};
			itoa_s (d, 10, str);
			size_t len = strlen(str);

			if (!maxrem) 
			{
				return -1;
			}

			if (!print(str, len))
			{
				return -1;
			}

			written++;
		} 
		else if (*format == 's') 
		{
			format++;
			const char* str = va_arg(parameters, const char*);
			size_t len = strlen(str);

			if (maxrem < len) 
			{
				return -1;
			}

			if (!print(str, len))
			{
				return -1;
			}

			written += len;
		}
		else if(*format == 'X' || *format == 'x')
		{
			format++;
			unsigned int d = va_arg (parameters, unsigned int);
			char str[32]={0};
			itoa(d,16,str);
			size_t len = strlen(str);

			if (!maxrem) 
			{
				return -1;
			}

			if (!print(str, len))
			{
				return -1;
			}

			written+=len;
		}
		else 
		{
			format = format_begun_at;
			size_t len = strlen(format);
			if (maxrem < len) 
			{
				return -1;
			}

			if (!print(format, len))
			{
				return -1;
			}
			
			written += len;
			format += len;
		}
	}

	va_end(parameters);
	return written;
}
