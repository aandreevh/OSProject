#include <daemon.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

typedef int hf_callback(int,char**);

int wait_daemon(h_daemon* daemon){
    
    int status;
    waitpid(daemon->pid,&status,NULL);
    daemon->pid = -1;
    return WEXITSTATUS(status);
}

void close_daemon_out(h_daemon* daemon ){
    close(daemon->out);
    daemon->out = -1;
}

void close_daemon_in(h_daemon* daemon){
    close(daemon->in);
    daemon->in = -1;
}

void close_daemon(h_daemon* daemon){
    close_daemon_in(daemon);
    close_daemon_out(daemon);
}

h_daemon init_inner_daemon(_fd in,_fd out,hf_callback* callback,int argc,char** argv){
    h_daemon d;

    d.pid = fork();

    if(d.pid == 0){
        dup2(in,INPUT);
        dup2(out,OUTPUT);

        exit(callback(argc,argv));
    }

    d.in = -1;
    d.out = -1;

    return d;
}
h_daemon init_out_daemon(_fd out,hf_callback* callback,int argc,char** argv){
    h_daemon d;

    _fd io_in[2];

    pipe(io_in);

    d.pid = fork();

    if(d.pid == 0){
        close(io_in[OUTPUT]);

        dup2(out,OUTPUT);
        dup2(io_in[INPUT],INPUT);
        close(io_in[INPUT]);

        exit(callback(argc,argv));
    }
    close(io_in[INPUT]);
    d.out = io_in[OUTPUT];
    d.in = -1;
    return d;
}

h_daemon init_in_daemon(_fd in,hf_callback* callback,int argc,char** argv){
    h_daemon d;
    
    _fd io_out[2];

    pipe(io_out);

    d.pid = fork();

    if(d.pid == 0){
        close(io_out[INPUT]);
       
        dup2(in,INPUT);
        dup2(io_out[OUTPUT],OUTPUT);
        close(io_out[OUTPUT]);

        exit(callback(argc,argv));
    }
    close(io_out[OUTPUT]);

    d.out = -1;
    d.in = io_out[INPUT];

    return d;
}
h_daemon init_daemon(hf_callback* callback,int argc,char** argv){

h_daemon d;
_fd io_in[2],io_out[2];

pipe(io_in);
pipe(io_out);

d.pid = fork();

if(d.pid == 0){
    close(io_in[OUTPUT]);
    close(io_out[INPUT]);

    dup2(io_in[INPUT],INPUT);
    dup2(io_out[OUTPUT],OUTPUT);

    close(io_in[INPUT]);
    close(io_out[OUTPUT]);

    exit(callback(argc,argv));
}
close(io_in[INPUT]);
close(io_out[OUTPUT]);

d.in = io_out[INPUT];
d.out = io_in[OUTPUT];

return d;
}
