#include "menu.h"
#include "api.h"

#define ASSERT_MENU(c,t) if(!(c)) {err(t);exit(1);}
void exec_menu(size_t count, const char *args[])
{
    struct h_cfg cfg;
    
    ASSERT_MENU(count>=2,"Invalid argument count !")

    char* path = args[0];

    ASSERT_MENU(cfg_load(path, &cfg),"Invalid config !")
    ASSERT_MENU(strlen(args[1])==2 && args[1][0]=='-',"Invalid command !")

    char cmd = args[1][1];
    count -= 2;
    args += 2;

    /**Could do printf way handlers for better scaleablility **/
    switch (cmd)
    {
    case 's':
        menu_option_s(&cfg, count, args);
        break;
    default: 
    ASSERT_MENU(FALSE,"Invalid command !");
        break;
    }

    out("%z",cfg);
    ASSERT_MENU(cfg_save(path, &cfg),"Couldn't save config.")

}

void menu_option_s(struct h_cfg *config, size_t count, const char *args[])
{
    h_position pos;
    ASSERT_MENU(count == 2, "Invalid argument count !")    
    char* param = args[0];
    char* data = args[1];
    
    ASSERT_MENU(cfg_get_pos(pos, param),"Invalid param name !")
    ASSERT_MENU(cfg_set_data(config,pos,data),"Cannot set data !")
    if(!cfg_enabled(config,pos))  ASSERT_MENU(cfg_switch_bit(config,pos),"Could not set bit !")
    
}

#undef ASSERT_MENU