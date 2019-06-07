#include "api.h"
#include "core.h"

void cfg_load(const char *path, struct h_cfg *cfg)
{
    int n = pread(path, cfg->data, sizeof(h_cfg_dat));
    cfg->n = n / sizeof(struct h_seg);
}

void cfg_merge(struct h_cfg *dest, const struct h_cfg **cfg_list, size_t count)
{
    for (size_t i = 0; i < count; i++)
        for (size_t j = 0; j < cfg_list[i]->n; j++)
            if (dest->n >= CFG_MAX_SEG)
                return;
            else
            {
                (*dest).data[dest->n++] = cfg_list[i]->data[j];
            }
}

extern void init_cfg(struct h_cfg *cfg)
{
    memset(cfg, 0, sizeof(struct h_cfg));
}

void cfg_save(const char *path, const struct h_cfg *cfg)
{
    pprintf(path, "%Z", cfg);
}

inline bool cfg_enabled(const struct h_seg *seg, byte bit)
{
    return (seg->data.header.data.flags[bit >> 3] & ((1 << 7) >> (bit % 8)));
}

/**Handler for binary write to stream**/
static int __printf_handler_Z(fd fd, va_list *args)
{
    struct h_cfg *cfg = va_arg(*args, void *); // faster with pointers, better for binary file operations

    size_t len = cfg->n * sizeof(struct h_seg);

    if (write(fd, cfg, len) == len)
        return len;
    return -1;
}

/*Handler meant for user debugging, not file writing */
static int __printf_handler_z(fd fd, va_list *args)
{

#define __INC(F)    \
    out = F;        \
    if (out < 0)    \
        return out; \
    else            \
        total_bytes += out;

    int total_bytes = 0, out = 0;
    struct h_cfg cfg = va_arg(*args, struct h_cfg); //slower, but easier for user debugging (won't forget ptr)

    out = __INC(printf(fd, "Number for segments: %d", cfg.n))

        for (int i = 0; i < cfg.n; i++)
    {
        __INC(printf(fd, "\nSegment #%d\n", i + 1))
        __INC(printf(fd, "Type: "))
        switch (cfg.data[i].data.header.data.type)
        {
        case CFG_TYPE_TEXT:
            __INC(printf(fd, "TEXT\n"))
            __INC(printf(fd, "CONTENT: \n"))
            for (int j = 0; j < 3; j++)
            {
                __INC(printf(fd, "%c TEXT %d: %s\n",
                             cfg_enabled(&cfg.data[i], j) ? 'E' : 'D',
                             j + 1, cfg.data[i].data.record.text[j]))
            }
            break;
        case CFG_TYPE_INT:
            __INC(printf(fd, "INTEGER\n"))
            __INC(printf(fd, "CONTENT: \n"))
            for (int j = 0; j < 14; j++)
            {
                __INC(printf(fd, "%c INTEGER: %d: %d\n",
                             cfg_enabled(&cfg.data[i], j) ? 'E' : 'D',
                             j + 1, cfg.data[i].data.record.num[j]))
            }
            break;
        case CFG_TYPE_BYTE:
            __INC(printf(fd, "BYTE\n"))
            __INC(printf(fd, "CONTENT: \n"))
            for (int j = 0; j < 56; j++)
            {
                __INC(printf(fd, "%c BYTE %d: %b\n",
                             cfg_enabled(&cfg.data[i], j) ? 'E' : 'D',
                             j + 1, cfg.data[i].data.record.raw[j]))
            }
            break;
        default:
            __INC(printf(fd, "UNSUPPORTED\n"))
            break;
        }
    }

#undef __INC
    return total_bytes;
}

/**Register printf callbacks for api **/
static __attribute__((constructor)) void __printf_register_api_callbacks()
{

    printf_register_handler('z', __printf_handler_z);
    printf_register_handler('Z', __printf_handler_Z);
}