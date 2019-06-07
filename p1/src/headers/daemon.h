#ifndef H_DAEMON
#define H_DAEMON

#define INPUT 0
#define OUTPUT 1

typedef int _fd;
typedef int _pid;


struct h_daemon_impl{
    _fd in,out;
    _pid pid;
};
typedef struct h_daemon_impl h_daemon;

typedef int hf_callback(int,char**);

extern int wait_daemon(h_daemon* daemon);
extern void close_daemon_out(h_daemon* daemon );

extern void close_daemon_in(h_daemon* daemon);

extern void close_daemon(h_daemon* daemon);

extern h_daemon init_inner_daemon(_fd in,_fd out,hf_callback* callback,int argc,char** argv);
extern h_daemon init_out_daemon(_fd out,hf_callback* callback,int argc,char** argv);
extern h_daemon init_in_daemon(_fd in,hf_callback* callback,int argc,char** argv);
extern h_daemon init_daemon(hf_callback* callback,int argc,char** argv);

#endif