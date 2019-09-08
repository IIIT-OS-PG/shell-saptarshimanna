#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <string.h>
#include "pwd.h"

using namespace std;

string get_user(){
	char buf[30];
	getlogin_r(buf, 30);
	string usr(buf);
	return usr;
}

string create_terminal(){
	string PS1;
	PS1= get_user()+"$ ";
	return PS1;
}


#endif

