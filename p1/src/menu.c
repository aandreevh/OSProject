#include "menu.h"
#include "api.h"
void exec_menu(size_t count, const char* args[]){

    if(count <2 ){
        err("Invalid argument count !");
        exit(1);
    }
    struct h_cfg cfg;
    if(!cfg_load(args[0],&cfg)){
        err("Invalid config !");
        exit(1);
    }

    if(strlen(args[1]) != 2){

        err("Invalid  arguments !");
        exit(1);
    
    }

     if(args[1][0] != '-'){

        err("Invalid  arguments !");
        exit(1);
    
    }

    char cmd = args[1][1];
    
    count-=2;
    args+=2;

    switch(cmd){
        case 's':
            menu_option_s(&cfg,count,args);
        break;
        default:
            err("Invalid command !");
             exit(1);
        break;

    }

    cfg_save(args[0],&cfg);
}

void menu_option_s(struct h_cfg* config,size_t count,const char* args[]){
    if(count != 2){
        err("Invalid argument count !");
        exit(1);
    }

    int pos[2];

   if(!cfg_get_pos(pos,args[0])){
       err("Invalid name !");
       exit(1);
   }

    int seg = pos[0],position = pos[1];

    if(config->n-1 > seg){
        err("Invalid segment !");
        exit(1);
    }

    if(cfg_enabled(&config->data[seg],position)){
        switch(  config->data[seg].data.header.data.type){
            case CFG_TYPE_BYTE:

            break;

            case CFG_TYPE_INT:

            break;

            case CFG_TYPE_TEXT:
            if(strlen(args[1]) <16){
                    out(args[1]);
            }
                
            break;

            default:
            err("Invalid config format !");
            exit(1);
            break;
        }
          
    }else{
        err("Config not enabled !");
        exit(1);
    }
}