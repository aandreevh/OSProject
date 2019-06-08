#include "menu.h"
#include "api.h"

#define ASSERT_MENU(c, t) \
    if (!(c))             \
    {                     \
        err(t);           \
        exit(1);          \
    }
void exec_menu(size_t count, const char *args[])
{
    struct h_cfg cfg;

    if(count == 1){
        ASSERT_MENU(strcmp(args[0],"-h") ==0,"Invalid argument count !");
         menu_option_h();
         return;
    }

    ASSERT_MENU(count >= 2, "Invalid argument count !")

    char *path = args[0];

    if(!cfg_load(path, &cfg))
        memset(&cfg,0,sizeof(struct h_cfg));
    
    ASSERT_MENU(strlen(args[1]) == 2 && args[1][0] == '-', "Invalid command !")

    char cmd = args[1][1];
    count -= 2;
    args += 2;

    /**Could do printf way handlers for better scaleablility **/
    switch (cmd)
    {
    case 's':
        menu_option_s(&cfg, count, args, TRUE);
        break;

    case 'S':
        menu_option_s(&cfg, count, args, FALSE);
        break;

    case 'g':
        menu_option_g(&cfg, count, args, TRUE);
        break;

    case 'G':
        menu_option_g(&cfg, count, args, FALSE);
        break;

    case 'l':
        menu_option_l(&cfg, count, args, TRUE);
        break;

    case 'L':
        menu_option_l(&cfg, count, args, FALSE);

    case 'b':
        menu_option_b(&cfg, count, args);
        break;

    case 'c':
        menu_option_c(&cfg, count, args);
        break;
    case 'p':
        out("%z",cfg);
        break;
    default:
        ASSERT_MENU(FALSE, "Invalid command !");
        break;
    }

    ASSERT_MENU(cfg_save(path, &cfg), "Couldn't save config !")
}

void menu_option_h(){
    out("program - operates over bluetooth configuration\n");
    out("program [CONFIG] [OPTION] [ARGUMENTS]... \n");
    out("OPTIONS \n");
    out("\t -s [PARAM] [VALUE] : changes param value and enables it \n");
    out("\t -S [PARAM] [VALUE] : changes param value\n");
    out("\t -g [PARAM] : prints param value if enabled\n");
    out("\t -G [PARAM] : prints param value\n");
    out("\t -l [PARAM]... : prints param values listed or all, if enabled\n");
    out("\t -L [PARAM]... : prints param values listed or all\n");
    out("\t -b [PARAM] [1|0] : enables|disables bit  \n");
    out("\t -c ([SEG] [t|n|b])... : creates new config \n");
    out("\t -p : prints config data\n");
}

void menu_option_c(struct h_cfg *config, size_t count, const char *args[])
{
    ASSERT_MENU(count%2 ==0,"Invalid command count !")
    ASSERT_MENU(!config->n,"Config not empty !")
   
    for(size_t i =0;i<count;i+=2){
        ASSERT_MENU(strlen(args[i+1]) ==1 , "Invalid argument !")
        int type;
        int seg = atoi(args[i]);
        ASSERT_MENU(seg >=0 && seg < CFG_MAX_SEG, "Invalid seg id!")

        switch(args[i+1][0]){
            case 'b':
            type = CFG_TYPE_BYTE;
            break;
            case 't':
            type = CFG_TYPE_TEXT;
            break;
            case 'n':
            type = CFG_TYPE_INT;
            break;
            default:
            ASSERT_MENU(FALSE,"Invalid type !")
            break;
        }

        if(config->n < seg +1)config->n = seg+1;
        config->data[seg].data.header.data.type = type;
    }
}

void menu_option_b(struct h_cfg *config, size_t count, const char *args[])
{
    ASSERT_MENU(count == 2,"Invalid param count !");

    char* param = args[0];
    char* bit = args[1];

    h_position pos;
    ASSERT_MENU(cfg_get_pos(pos,param),"Invalid param !")
    ASSERT_MENU(cfg_verify_pos(config,pos),"Invalid position !")
    ASSERT_MENU(bit[0] == '0' || bit[0] == '1',"Invalid bit !");
    byte bitZ = bit[0]-'0';

    if(bitZ != cfg_enabled(config,pos))cfg_switch_bit(config,pos);
}

void menu_option_s(struct h_cfg *config, size_t count, const char *args[], bool swtch)
{
    h_position pos;
    ASSERT_MENU(count == 2, "Invalid argument count !")
    char *param = args[0];
    char *data = args[1];

    ASSERT_MENU(cfg_get_pos(pos, param), "Invalid param name !")
    ASSERT_MENU(cfg_set_data(config, pos, data), "Cannot set data !")
    if (swtch)
        if (!cfg_enabled(config, pos))
            ASSERT_MENU(cfg_switch_bit(config, pos), "Could not set bit !")
}

void menu_option_g(struct h_cfg *config, size_t count, const char *args[], bool chen)
{
    h_position pos;
    ASSERT_MENU(count == 1, "Invalid argument count !")
    char *param = args[0];

    ASSERT_MENU(cfg_get_pos(pos, param), "Invalid param name !")

    if (chen)
        ASSERT_MENU(cfg_enabled(config, pos), "Param not enabled !")

    switch (config->data[pos[POS_SEG]].data.header.data.type)
    {
    case CFG_TYPE_INT:
        out("%d\n", config->data[pos[POS_SEG]].data.record.num[pos[POS_ELT]]);
        break;

    case CFG_TYPE_BYTE:
        out("%b\n", config->data[pos[POS_SEG]].data.record.raw[pos[POS_ELT]]);
        break;

    case CFG_TYPE_TEXT:
        out("%s\n", config->data[pos[POS_SEG]].data.record.text[pos[POS_ELT]]);

        break;
    }
}

void menu_option_l(struct h_cfg *config, size_t count, const char *args[], bool chen)
{
    h_position pos;
    char *param = args[0];

    if (count == 0)
    {
        for (size_t i = 0; i < config->n; i++)
        {
            for (size_t j = 0; j < 56; j++)
            {
                h_position pos = {i, j};
                if (cfg_verify_pos(config, pos))
                {

                    if (chen)
                        if (!cfg_enabled(config, pos))
                            continue;

                    switch (config->data[pos[POS_SEG]].data.header.data.type)
                    {
                    case CFG_TYPE_INT:
                        out("%d\n", config->data[pos[POS_SEG]].data.record.num[pos[POS_ELT]]);
                        break;

                    case CFG_TYPE_BYTE:
                        out("%b\n", config->data[pos[POS_SEG]].data.record.raw[pos[POS_ELT]]);
                        break;

                    case CFG_TYPE_TEXT:
                        out("%s\n", config->data[pos[POS_SEG]].data.record.text[pos[POS_ELT]]);
                        break;
                    }
                }
            }
        }
    }
    else
    {
        for (size_t i = 0; i < count; i++)
        {
            h_position pos;
            ASSERT_MENU(cfg_get_pos(pos, args[i]), "Invalid param !");
            if (cfg_verify_pos(config, pos))
            {

                if (chen)
                    if (!cfg_enabled(config, pos))
                        continue;

                switch (config->data[pos[POS_SEG]].data.header.data.type)
                {
                case CFG_TYPE_INT:
                    out("%d\n", config->data[pos[POS_SEG]].data.record.num[pos[POS_ELT]]);
                    break;

                case CFG_TYPE_BYTE:
                    out("%b\n", config->data[pos[POS_SEG]].data.record.raw[pos[POS_ELT]]);
                    break;

                case CFG_TYPE_TEXT:
                    out("%s\n", config->data[pos[POS_SEG]].data.record.text[pos[POS_ELT]]);
                    break;
                }
            }
        }
    }
}


#undef ASSERT_MENU