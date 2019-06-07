#ifndef H_API
#define H_API
#include "core.h"
#include "api_types.h"

extern void init_cfg(struct h_cfg *cfg);

/** cfg io operations needed**/
extern bool cfg_load(const char *path, struct h_cfg *cfg);
extern bool cfg_save(const char *path, const struct h_cfg *cfg);

extern int cfg_size(struct h_cfg *cfg);
extern bool cfg_verify_pos(struct h_cfg *cfg, h_position position);

extern bool seg_enabled(const struct h_seg *cfg, byte bit);
extern bool cfg_enabled(const struct h_cfg *cfg, h_position pos);

extern bool cfg_get_pos(h_position pos,const char name[]);
extern bool cfg_set_data(struct h_cfg * cfg,h_position pos,const char data[]);

extern void cfg_merge(struct h_cfg *dest, const struct h_cfg **cfg_list, size_t count);
#endif