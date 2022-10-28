#ifndef STDIO_H
#define STDIO_H

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define KBDIN 3

#define MAX_CHARS 256

int putchar(char c);
int puts(const char * s);
int getchar();
int scanf(char * fmt, ...);
int printf(char * fmt, ...);

#endif
