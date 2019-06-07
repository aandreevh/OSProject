#ifndef H_API_TYPES
#define H_API_TYPES
#include "core.h"

/** definitions **/
#define CFG_MAX_SEG 64

#define CFG_TYPE_TEXT 0
#define CFG_TYPE_INT  1
#define CFG_TYPE_BYTE 2

#define POS_SEG 0
#define POS_ELT 1

typedef byte h_position[2];
typedef char h_text[16];

/** segment header**/
struct h_seg_header
{
    union {

        byte raw[8];

        struct
        {
            byte type;
            byte flags[7];
        } data;
    };
};

/**segment metadata record **/
struct h_seg_record
{

    union {
        byte raw[56];
        h_text text[3];
        int num[14];
    };
};

/**segment itself**/
struct h_seg
{
    union {

        byte raw[64];
        struct
        {

            struct h_seg_header header;
            struct h_seg_record record;

        } data;
    };
};

/*
Config segments. Prefer using static memory and optimizing copy later.
Will be slower with dynamic mem and more complex for MAX_CFG_SEG = 64
 */
typedef struct h_seg h_cfg_dat[CFG_MAX_SEG];

/**
 *  config structure.
 *  Data to be stored is h_cfg_dat.
 *  n is used to store actual count of segments in config
 * */
typedef struct h_cfg
{
    h_cfg_dat data;
    byte n;
};

#endif