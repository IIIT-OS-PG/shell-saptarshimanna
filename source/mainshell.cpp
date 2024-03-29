#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <boost/algorithm/string.hpp>
#include "utilities.h"
#include "functionalities.h"

#define MAXBUFSIZE (10000)
using namespace std;
using namespace boost;

struct termios curterm, saved_attributes;
void set_input_mode();
void reset_input_mode();
void write_history(char*,int);
void read_history();
void alarm_reminder(int);
void find_alias(int);
int historyno=1;

int main()
{
    set_input_mode();
    signal(SIGALRM,alarm_reminder);
    char buf[MAXBUFSIZE];
    char c;
    string cmd, prm="";
    size_t top;
    int no_pipes;
    bool flag, bg, rdr;
    top = 0;
    set_shell();
    while(true) {
		prm = create_terminal();bg=false,no_pipes=0,rdr=false;
		write(STDOUT_FILENO, prm.c_str(), prm.size());
        while (read(STDIN_FILENO, &c, sizeof c) == 1) {
			flag = false;
            switch (c) {
            case 127:
                if (top) {
                    --top;
                    const char delbuf[] = "\b \b";
                    write(STDOUT_FILENO, delbuf, strlen(delbuf));
                    if(buf[top]=='|') no_pipes--;
                    if(buf[top]=='&') bg=false;
                    if(buf[top]=='>') rdr=false;
                }
                break;
            case 10:
                write(STDOUT_FILENO, &c, sizeof c);
                buf[top]='\0';
                //write(STDOUT_FILENO, buf, top);
                cmd.assign(buf);
                write_history(buf,top);
                top = 0;
                flag = true;
                break;
            case 9:
                //call trie DS//
                //write(STDOUT_FILENO, buf, top);
                break;
            default:
                buf[top++] = c;
                write(STDOUT_FILENO, &c, sizeof c);
                if(c=='|') no_pipes++;
                if(c=='&') bg=true;
                if(c=='>') rdr=true;
                break;
            }
            if(flag) break;
        }
        if(cmd=="exit"){
			reset_input_mode();
			break;
		}
		else if (cmd=="history"){
			read_history();
			top=0;
		}
		else if (cmd.substr(0, 5) == "alias" ){
			string temp;
			temp = cmd.substr(6);
			trim(temp);
			int index = temp.find("=");
			find_alias(no_pipes);
			string key = temp.substr(0, index-1); //function
			string value = temp.substr(index+2);
			ali[key] = value;
		}
		else if (cmd.substr(0,2)=="fg"){
			int chd = stoi(cmd.substr(3));
			tcsetpgrp(0, chd);
			kill(chd, SIGCONT);
			int status;
			wait(&status);
		}
		else if(cmd.substr(0,5)=="alarm"){
			alarm(stoi(cmd.substr(6)));
		}
		else if(cmd.substr(0,2)=="cd"){
			chdir(cmd.substr(3).c_str());
		}
		else if(cmd.substr(0,4)=="echo" && (cmd.substr(5)=="$$" || cmd.substr(5)=="$HOME" || cmd.substr(5)=="$PATH" || cmd.substr(5)=="$PWD" || cmd.substr(5)=="$USER")){
			string str = ali[cmd.substr(5)];
			write(STDOUT_FILENO, str.c_str(), str.size());
			char ctr[] ={'\n'};
			write(STDOUT_FILENO, ctr, 2);
		}
		else{
			if(rdr)
				parse_command_redirect(no_pipes,cmd);
			else if(bg){
				cmd.replace(cmd.find("&"),1,"");
				parse_command_bg(no_pipes,cmd);
			}
			else{
				parse_command_pipe(no_pipes,cmd);
			}
		}
    }
    return 0;
}

void set_input_mode() {

	tcgetattr(STDIN_FILENO, &saved_attributes);
    tcgetattr(STDIN_FILENO, &curterm);
    curterm.c_lflag &= ~(ICANON|ECHO);
    curterm.c_lflag |= (TOSTOP);
    //parse_command_pipe(0,"stty tostop");
    curterm.c_cc[VTIME] = 0;
    curterm.c_cc[VMIN] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &curterm);

}
void reset_input_mode ()
{
	tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
}

void write_history(char* buf,int size){

	int read_head, write_head;
	buf[size]='\n';
    buf[size+1]='\0';
	write_head = open("./myrc_history", O_WRONLY | O_CREAT | O_APPEND, 0666);
	write (write_head, buf, size+1);
	close(write_head);
}

void read_history(){
	int read_head, size;
	char buf[1024];
	read_head = open ("./myrc_history", O_RDONLY);
	size=read (read_head, &buf, 1024);
	write(STDOUT_FILENO, buf, size);
	close(read_head);
}

void alarm_reminder(int x){
	write(STDOUT_FILENO,"Reminded\n",10);
}

void find_alias(int count){
	string cmd ="echo $HOME $PATH $USER";
	char arg[60]; char argv[30]; int j,i;
	for(i=0; i<cmd.size(); i++){
		arg[i]=cmd[i];
		j=100;
		while(j!=0){
			j--;
		}
	}
	arg[i]='\0';
}

