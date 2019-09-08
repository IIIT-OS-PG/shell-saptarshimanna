#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <string.h>
#include <map>
#include "pwd.h"

using namespace std;

map<string,string> ali;

void set_path() {
    int read_head, write_head, ret_in, ret_out;
    char buf[2048];
    read_head = open("./myrc", O_RDONLY);
    ret_in = read(read_head, buf, 2048);
    buf[ret_in] = '\0';
    string path(buf);
    putenv(buf);
    close(read_head);
    ali["$PATH"]=path;
}

string get_user() {
    char buf[30];
    getlogin_r(buf, 30);
    string usr(buf);
    ali["$USER"]=usr;
    return usr;
}

string create_terminal() {
    string PS1= get_user()+":$ ";
    return PS1;
}

void set_shell() {
    char buf[200];
    getcwd(buf, 200);
    string pwd(buf);
    ali["$PWD"]=pwd;
    string home = string(getpwuid(getuid())->pw_dir);
    ali["$HOME"]=home;
    ali["$$"]=to_string(getpid());
    set_path();
}

#endif

