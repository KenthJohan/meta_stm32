#include "str.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

void str_copy_gpioaf(char *dst, int size, char const *src)
{
	if (strncmp(src, "GPIO_", 5) != 0) {
		return;
	}
	src += 5;
	while (*src && size > 1) {
		if (*src == '_') {
			*dst = 0;
			break;
		}
		*dst++ = *src++;
	}
	*dst = 0;
}

void str_copy_gpio_name(char *dst, char const *src)
{
	while (isalnum(*src)) {
		*dst++ = *src++;
	}
	*dst = 0;
}

void str_copy_af(char *dst, char const *src)
{
	while (*src) {
		if (isalnum(*src)) {
			*dst++ = *src;
		} else {
			*dst++ = '_';
		}
		src++;
	}
	*dst = 0;
}


void str_copy_escape(char *dst, int size, const char *src)
{
	while (*src && size > 1) {
		switch (*src) {
		case '\n':
			*dst++ = '\\';
			*dst++ = 'n';
			break;
		case '\r':
			*dst++ = '\\';
			*dst++ = 'r';
			break;
		case '\t':
			*dst++ = '\\';
			*dst++ = 't';
			break;
		case '\\':
			*dst++ = '\\';
			*dst++ = '\\';
			break;
		case '\"':
			*dst++ = '\\';
			*dst++ = '\"';
			break;
		default:
			*dst++ = *src;
			break;
		}
		src++;
		size--;
	}
	*dst = 0;
}


void str_copy_position(char *dst, char const *src)
{
	while (*src) {
		if (isalnum(*src)) {
			*dst++ = *src;
		} else {
			*dst++ = '_';
		}
		src++;
	}
	*dst = 0;
}


const char *find_last_slash(const char *str)
{
	if (str == NULL) {
		return NULL;
	}

	const char *last_slash = NULL;
	for (const char *p = str; *p != '\0'; p++) {
		if (*p == '/') {
			last_slash = p;
		}
	}

	return last_slash;
}

void read_file_line_by_line(const char *filename)
{
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		printf("Error opening file: %s\n", filename);
		return;
	}

	char line[1024];
	while (fgets(line, sizeof(line), file)) {
		printf("%s", line);
	}

	fclose(file);
}
