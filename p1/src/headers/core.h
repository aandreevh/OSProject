#ifndef H_CORE
#define H_CORE

/** standard defininitions **/

#define TRUE 1
#define FALSE 0
#define NULL ((void*)0)

typedef char bool;
typedef unsigned long long size_t;
typedef unsigned char byte;
typedef int fd;

#define MAX_INT 2147483647
#define MAX_BYTE 255
#define MAX_INT_LEN 11 // intlen(-MAX_INT)

/** using builtin-s **/
#define msb(x) __builtin_clz(x)
#define lsb(x) __builtin_ctz(x)
#define va_start(v, n) __builtin_va_start(v, n)
#define va_list __builtin_va_list
#define va_arg(l, t) __builtin_va_arg(l, t)
#define va_end(l) __builtin_va_end(l)

/** std io **/
#define STDIN 0
#define STDOUT 1
#define STDERR 2

/** standart syscall ids **/

#define SYS_EXIT 60 // _NR_EXIT
#define SYS_READ 0  // _NR_READ
#define SYS_WRITE 1 // _NR_WRITE
#define SYS_OPEN 2  // _NR_OPEN
#define SYS_CLOSE 3 // _NR_CLOSE

/** syscall **/
#define exit(s) syscall(SYS_EXIT, s)
#define write(f, p, n) syscall(SYS_WRITE, f, p, n)
#define read(f, b, c) syscall(SYS_READ, f, b, c)
#define open(p, f) syscall(SYS_OPEN, p, f)
#define openm(p, f,m) syscall(SYS_OPEN, p, f,m)
#define close(f) syscall(SYS_CLOSE, f)

/** file access ( copied from fcntl.h ) **/
#define O_RDONLY 00000000
#define O_WRONLY 00000001
#define O_RDWR 00000002
#define O_CREAT 00000100
#define O_EXCL 00000200
#define O_NOCTTY 00000400
#define O_TRUNC 00001000
#define O_APPEND 00002000

/** utils **/
extern int atoi(const char *text);
extern bool itoi(int val, char *buf, size_t len);
extern void memset(void *, byte, size_t);
extern size_t strlen(const char *text);
extern size_t intlen(int val);
extern int strcmp(const char* left,const char* right);

/*Function type definition for printf handlers*/
typedef int PFHANDLER(fd, va_list *);

/**
 * Registers handlers for printf.
 * Polymorphism at lower level.
 * **/
extern void printf_register_handler(byte h, PFHANDLER *handler);

/**printf functions **/
extern int printf(fd fd, const char *text, ...);

/**useful for va warping **/
extern int vprintf(fd fd, const char *text, va_list *vlist);

/* faster macros */
#define out(t, ...) printf(STDOUT, t, ##__VA_ARGS__)

#define err(t, ...) printf(STDERR, t, ##__VA_ARGS__)

/** file based io // last io abstraction needed for core.h **/
extern int pmprintf(const char *path, size_t mode, const char *text, ...);
extern int pprintf(const char *path, const char *text, ...);

extern int pread(const char *path, void *buf, size_t n);

#endif