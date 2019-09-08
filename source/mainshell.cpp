#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include "utilities.h"

#define MAXBUFSIZE (10000)
using namespace std;

struct termios curterm, saved_attributes;
void set_input_mode();
void reset_input_mode();
void write_history(char*,int);
void read_history();
int historyno=1;

int main()
{
    set_input_mode();
    char buf[MAXBUFSIZE];
    char c;
    string command, prm="";
    size_t top;
    bool flag;
    top = 0;
    while(true) {
		prm = create_terminal();
		write(STDOUT_FILENO, prm.c_str(), prm.size());
        while (read(STDIN_FILENO, &c, sizeof c) == 1) {
			flag = false;
            switch (c) {
            case 127:
                if (top) {
                    --top;
                    const char delbuf[] = "\b \b";
                    write(STDOUT_FILENO, delbuf, strlen(delbuf));
                }
                break;
            case 10:
                write(STDOUT_FILENO, &c, sizeof c);
                buf[top]='\0';
                //write(STDOUT_FILENO, buf, top);
                command.assign(buf);
                write_history(buf,top);
                top = 0;
                flag = true;
                break;
            case 9:
                //call trie DS//
                write(STDOUT_FILENO, buf, top);
                break;
            default:
                buf[top++] = c;
                write(STDOUT_FILENO, &c, sizeof c);
                break;
            }
            if(flag) break;
        }
        if(command=="exit"){
			reset_input_mode();
			break;
		}
		else if (command=="history"){
			read_history();
			top=0;
		}
    }
    return 0;
}

void set_input_mode() {

	tcgetattr(STDIN_FILENO, &saved_attributes);
    tcgetattr(STDIN_FILENO, &curterm);
    curterm.c_lflag &= ~(ICANON| ECHO);
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
	if(write_head == -1) throw "write history failed";
	write (write_head, buf, size+1);
	close(write_head);
}

void read_history(){
	int read_head, size;
	char buf[1024];
	read_head = open ("./myrc_history", O_RDONLY);
	if(read_head == -1) throw "reading history failed";
	size=read (read_head, &buf, 1024);
	write(STDOUT_FILENO, buf, size);
	close(read_head);
}

