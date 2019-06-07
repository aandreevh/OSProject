#include "core.h"

static PFHANDLER *_printf_handlers[MAX_BYTE];

void memset(void *s, byte c, size_t n)
{
    if (s == NULL)
        return;
    for (size_t i = 0; i < n; i++)
        *(byte *)(s++) = c;
}

size_t strlen(const char *text)
{
    if (text == NULL)
        return 0;

    size_t size = 0;
    for (size = 0; *text != 0; text++, size++)
        ;

    return size;
}

int atoi(const char *text)
{
    if (text == NULL)
        return 0;

    int val = 0;
    int flag = 1;
    for (const char *ntext = text; *ntext != 0; ntext++)
    {
        if (*ntext == '-' && ntext == text)
            flag *= -1;
        else if (*ntext < '0' || *ntext > '9')
            return 0;
        else
            val = 10 * val + (*ntext) - '0';
    }

    return val * flag;
}

size_t intlen(int val)
{
    if (val == 0)
        return 1;

    size_t count = 0;
    if (val < 0)
    {
        val *= -1;
        count++;
    }

    while (val > 0)
    {
        val /= 10;
        count++;
    }
    return count;
}

static int __itoi(int val, char *buf)
{
    if (val == 0)
        return -1;

    int n = 1 + __itoi(val / 10, buf);

    *(buf + n) = (val % 10) + '0';

    return n;
}

bool itoi(int val, char *buf, size_t len)
{
    size_t n = intlen(val);
    if (buf == NULL || n + 1 > len)
        return FALSE;

    buf[n] = '\0';
    if (val == 0)
    {
        buf[0] = '0';
        return TRUE;
    }
    if (val < 0)
        (buf++)[0] = '-';

    __itoi(abs(val), buf);

    return TRUE;
}

int __get_arg_count(const char *text)
{

    int va_count = 0;
    size_t len = strlen(text);
    //test syntax and get arg count
    for (size_t i = 0; i < len; i++)
        if (text[i] == '%'){
            if (i == len - 1)
                return -1;
            else if (text[i + 1] != '%')
                va_count++;
            else
                i++;
        }
        
    return va_count;
}

int printf(fd fd, const char *text, ...)
{
    va_list vlist;

    if (text == NULL)
        return -1;

    int argc = __get_arg_count(text);
    if (argc < 0)
        return -1;

    va_start(vlist, argc);
    int out = vprintf(fd, text, &vlist);
    va_end(vlist);

    return out;
}

int pread(const char *path, void *buf, size_t n)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0)
        return -1;
    int out = read(fd, buf, n);
    close(fd);
    return out;
}

int pprintf(const char *path, const char *text, ...)
{
    va_list vlist;

    int fd = open(path, O_WRONLY | O_CREAT);

    if (fd < 0)
        return -1;
    if (text == NULL)
        return -1;

    int argc = __get_arg_count(text);
    if (argc < 0)
        return -1;

    va_start(vlist, argc);
    int out = vprintf(fd, text, &vlist);
    va_end(vlist);

    close(fd);

    return out;
}
int pmprintf(const char *path, size_t mode, const char *text, ...)
{
    va_list vlist;

    int fd = open(path, O_WRONLY | O_CREAT | mode);

    if (fd < 0)
        return -1;
    if (text == NULL)
        return -1;

    int argc = __get_arg_count(text);
    if (argc < 0)
        return -1;

    va_start(vlist, argc);
    int out = vprintf(fd, text, &vlist);
    va_end(vlist);

    close(fd);

    return out;
}

extern int vprintf(fd fd, const char *text, va_list *vlist)
{

    size_t last = 0, len = strlen(text), byte_count = 0, out = 0;

    //execute printer
    for (size_t i = 0; i < len; i++)
    {
        if (text[i] != '%')
            continue;

        byte handler = (*(text + i + 1));

        out = write(fd, text + last, i - last);
        if (out < 0)
            return out;

        byte_count += out;

        last = (++i) + 1;
        if (handler == '%')
        {
            last--;
            continue;
        }
        else
        {
            int out = _printf_handlers[handler](fd, vlist);
            if (out < 0)
                return out;
            byte_count += out;
        }
    }
    out = write(fd, text + last, len - last);
    if (out < 0)
        return out;
    byte_count += out;

    return byte_count;
}

void printf_register_handler(byte h, PFHANDLER *handler)
{
    _printf_handlers[h] = handler;
}

/**Handler for char pointer(string) print**/
static int __printf_handler_s(fd fd, va_list *args)
{

    char *ss = va_arg(*args, char *);
    size_t len = strlen(ss);

    return write(fd, ss, len) == len ? len : -1;
}

/**Handler for single char print **/
static int __printf_handler_c(fd fd, va_list *args)
{
    int cc = va_arg(*args, int); // illegal instruction if use char bit hacky...
    return write(fd, &cc, 1) == 1 ? 1 : -1;
}

/**Handler for integer print // unsigned and double not supported**/
static int __printf_handler_d(fd fd, va_list *args)
{

    char dnum[MAX_INT_LEN + 1]; //could use dynamic arr, but it's actually slower
    int num = va_arg(*args, int);
    itoi(num, dnum, MAX_INT_LEN + 1);
    size_t llen = strlen(dnum);

    return write(fd, dnum, llen) == llen ? llen : -1;
}

/**Handler for bit print**/
static int __printf_handler_b(fd fd, va_list *args)
{

    char bbuffer[8];
    int num = va_arg(*args, int);
   byte b = num;

    for (int i = 0; i < 8; i++)
        bbuffer[8 - i-1] = b & (1 << i) ? '1' : '0';

    return write(fd, bbuffer, 8) == 8 ? 8 : -1;
    return 0;
}

/**Register default callbacks for printf **/
static __attribute__((constructor)) void __printf_register_default_callbacks()
{

    printf_register_handler('s', __printf_handler_s);
    printf_register_handler('c', __printf_handler_c);
    printf_register_handler('d', __printf_handler_d);
    printf_register_handler('b', __printf_handler_b);
}
