#include "api.h"
#include "core.h"



bool cfg_set_data(struct h_cfg * cfg,h_position pos,const char data[]){
    if(cfg_verify_pos(cfg,pos)){
        switch (cfg->data[pos[POS_SEG]].data.header.data.type)
        {
        case CFG_TYPE_BYTE:
        cfg->data[pos[POS_SEG]].data.record.raw[pos[POS_ELT]] =  atoi(data);
        return TRUE;
            break;

        case CFG_TYPE_INT:
        cfg->data[pos[POS_SEG]].data.record.num[pos[POS_ELT]] =  atoi(data);
        return TRUE;
            break;

        case CFG_TYPE_TEXT:
        cfg; // F.U. C
        size_t len =strlen(data);
        if(len <= 15){
            for(size_t i=0;i<len+1;i++)
             cfg->data[pos[POS_SEG]].data.record.text[pos[POS_ELT]][i] = data[i];
        return TRUE;
        }
        return FALSE;
            break;
        
        default:
        return FALSE;
            break;
        }
    }
    return FALSE;
}

bool cfg_load(const char *path, struct h_cfg *cfg)
{
    int n = pread(path, cfg->data, sizeof(h_cfg_dat));
    if(n<0) return FALSE;
    cfg->n = n / sizeof(struct h_seg);
    return TRUE;
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


int cfg_size(struct h_cfg *cfg){
    return cfg->n * sizeof(struct h_seg);
}

bool cfg_save(const char *path, const struct h_cfg *cfg)
{
    if(pprintf(path, "%Z", cfg) == cfg_size(cfg)) return TRUE;
    return FALSE;
}

extern bool cfg_verify_pos(struct h_cfg *cfg, h_position position){
    if(cfg->n > position[POS_SEG]){
        switch(cfg->data[position[POS_SEG]].data.header.data.type){
            case CFG_TYPE_BYTE:
            return position[POS_ELT] <= 56;
            break;
            case CFG_TYPE_INT:
            return position[POS_ELT] <= 14;
            break;
            case CFG_TYPE_TEXT:
            return position[POS_ELT] <= 3;
            break;
            default:
            return FALSE;
            break;
        }
    }

    return FALSE;
}


bool cfg_enabled(const struct h_cfg *cfg, h_position pos)
{
    if(cfg_verify_pos(cfg,pos)){
        return seg_enabled(&cfg->data[pos[POS_SEG]],pos[POS_ELT]);
    }
    return FALSE;
}

inline bool seg_enabled(const struct h_seg *seg, byte bit)
{
    return (seg->data.header.data.flags[bit >> 3] & ((1 << 7) >> (bit % 8)));
}

/* Too lazy to load from config */
static const char* params[] ={
    "device_name",
    "rom_revision",
    "serial_number",

    "bd_addr_part0",
    "bd_addr_part1",
    "bd_pass_part0",

    "serial_baudrate",
    "serial_baudrate",
    "sleep_period",

    "serial_parity",
    "serial_data_bit",
    "serail_stop_bit",

    "bd_pass_part1",
    "rom_checksum_part0",
    "rom_checksum_part1"
};

static h_position data[]={
    {0,0},
    {0,1},
    {0,2},

    {1,0},
    {1,1},
    {1,2},
    
    {2,0},
    {2,1},
    {2,2},

    {3,0},
    {3,1},
    {3,2},

    {4,0},
    {4,1},
    {4,3}
};
/*   */

bool cfg_get_pos(h_position pos,const char name[]){
    
    for(size_t i = 0;i< sizeof(params)/sizeof(char*);i++){
        if(strcmp(name,params[i]) == 0){
            
            pos[0] = data[i][POS_SEG];
            pos[1] = data[i][POS_ELT];
            return TRUE;
        }
    }

    return FALSE;
}

/**Handler for binary write to stream**/
static int __printf_handler_Z(fd fd, va_list *args)
{
    struct h_cfg *cfg = va_arg(*args, void *); // faster with pointers, better for binary file operations

    size_t len = cfg_size(cfg);

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
                             seg_enabled(&cfg.data[i], j) ? 'E' : 'D',
                             j + 1, cfg.data[i].data.record.text[j]))
            }
            break;
        case CFG_TYPE_INT:
            __INC(printf(fd, "INTEGER\n"))
            __INC(printf(fd, "CONTENT: \n"))
            for (int j = 0; j < 14; j++)
            {
                __INC(printf(fd, "%c INTEGER: %d: %d\n",
                             seg_enabled(&cfg.data[i], j) ? 'E' : 'D',
                             j + 1, cfg.data[i].data.record.num[j]))
            }
            break;
        case CFG_TYPE_BYTE:
            __INC(printf(fd, "BYTE\n"))
            __INC(printf(fd, "CONTENT: \n"))
            for (int j = 0; j < 56; j++)
            {
                __INC(printf(fd, "%c BYTE %d: %b\n",
                             seg_enabled(&cfg.data[i], j) ? 'E' : 'D',
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

