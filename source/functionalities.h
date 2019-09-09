#ifndef FUNCTIONALITIES_H_
#define FUNCTIONALITIES_H_

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <boost/algorithm/string.hpp>
#include "utilities.h"
#include <vector>
#include <sys/wait.h>

using namespace std;
using namespace boost;

bool value_exist_alias(string);

void parse_command_pipe(int count, string cmd){
	//cmd="ls -al | grep rc | wc -l";
	vector<string> parsed_cmd;int i=0,j=0,k=0,pid,status;
	split(parsed_cmd,cmd,is_any_of(" "));

	int pipe_fd[2*count];
	while(i<count){
		pipe(pipe_fd+2*i);
		i++;
	}

	if(value_exist_alias(parsed_cmd[0])){
		string val = ali[parsed_cmd[0]];
		vector<string> templst;
		split(templst,val,is_any_of(" "));
		parsed_cmd.erase(parsed_cmd.begin());
		parsed_cmd.insert(parsed_cmd.begin(),templst.begin(),templst.end());
	}

	char* arg[30];
	for(i=0,j=0; i<parsed_cmd.size();i++){
		if(parsed_cmd[i]=="|"){
			arg[j]=NULL;
			j=0;
			pid=fork();
			if(pid==0){
				if(k!=0)
					dup2(pipe_fd[(k-1)*2], STDIN_FILENO);
				dup2(pipe_fd[k*2+1], STDOUT_FILENO);
				for(i=0;i<2*count;i++){
					close(pipe_fd[i]);
				}
				execvp(arg[0],arg);
			}
			k++;
		}
		else{
			arg[j]=new char[parsed_cmd.size()];
			parsed_cmd[i].copy(arg[j++],parsed_cmd[i].size());
		}

	}
	arg[j]=NULL;
	pid=fork();
	if(pid==0){
		dup2(pipe_fd[k*2-2], STDIN_FILENO);
		for(i=0;i<2*count;i++){
			close(pipe_fd[i]);
		}
		execvp(arg[0],arg);
	}
	for(i=0;i<2*count;i++){
		close(pipe_fd[i]);
	}
	for(i=0;i<=count;i++){
		wait(&status);
	}
}
void parse_command_redirect(int count, string cmd){
	//cmd="ls -al | grep project >> output.txt";
	vector<string> parsed_cmd;int i=0,j=0,k=0,pid,mode,temp_fd,status;
	split(parsed_cmd,cmd,is_any_of(" "));
	int pipe_fd[2*count];
	while(i<count){
		pipe(pipe_fd+2*i);
		i++;
	}

	char* arg[30];
	for(i=0,j=0; i<parsed_cmd.size();i++){
		if(parsed_cmd[i]=="|"){
			arg[j]=NULL;
			j=0;
			pid=fork();
			if(pid==0){
				if(k!=0)
					dup2(pipe_fd[(k-1)*2], STDIN_FILENO);
				dup2(pipe_fd[k*2+1], STDOUT_FILENO);
				for(i=0;i<2*count;i++){
					close(pipe_fd[i]);
				}
				//close(temp_fd);
				execvp(arg[0],arg);
			}
			k++;
		}
		else if(parsed_cmd[i]==">" || parsed_cmd[i]==">>"){
			if(parsed_cmd[i]==">")
				mode=1;
			else
				mode=2;
			i++;
			break;
		}
		else{
			arg[j]=new char[parsed_cmd.size()];
			parsed_cmd[i].copy(arg[j++],parsed_cmd[i].size());
		}

	}
	arg[j]=NULL;
	if(mode==1){
		temp_fd=open(parsed_cmd[i].c_str(),O_WRONLY|O_CREAT,0777);
	}
	else if(mode==2){
		temp_fd=open(parsed_cmd[i].c_str(),O_WRONLY|O_CREAT|O_APPEND,0777);
	}
	pid=fork();
	if(pid==0){
		dup2(pipe_fd[k*2-2], STDIN_FILENO);
		dup2(temp_fd,STDOUT_FILENO);
		for(i=0;i<2*count;i++){
			close(pipe_fd[i]);
		}
		close(temp_fd);
		execvp(arg[0],arg);
	}
	for(i=0;i<2*count;i++){
		close(pipe_fd[i]);
	}
	close(temp_fd);
	for(i=0;i<=count;i++){
		wait(&status);
	}
}
void parse_command_bg(int count, string cmd){
	//cmd="ls -al | grep rc | wc -l";
	vector<string> parsed_cmd;int i=0,pid,status;
	split(parsed_cmd,cmd,is_any_of(" "));

	char* arg[30];
	for(i=0; i<parsed_cmd.size();i++){
		arg[i]=new char[parsed_cmd.size()];
		parsed_cmd[i].copy(arg[i],parsed_cmd[i].size());
	}
	arg[i]=NULL;
	pid=fork();
	if(pid==0){
		setpgid(0, 0);
		execvp(arg[0],arg);
	}
}

bool value_exist_alias(string key){
	if (ali.find(key) == ali.end()){
		return false;
	}
	else
		return true;
}

#endif
