#include "XFtpUSER.h"
#include <iostream>
#include <string>
#include <json/json.h>
using namespace std;
extern Json::Reader reader;
extern Json::Value root;

void XFtpUSER::Parse(std::string type, std::string msg){
	cout<<"XFtpUSER::Parse "<<type<<" "<<msg<<endl;
	
	int pos = msg.find(" ")+1;
	
	if(type=="USER"){
		username = msg.substr(pos, msg.length()-pos-2);
		if(username == "anonymous" && root["allow_anonymous"].asBool()){
			//允许匿名登陆
			ResCMD("200 OK.\r\n");
		}else{
			//账户登陆
			ResCMD("331 User name okay, need password.\r\n");
		}
		
	}else{
		string pwd = msg.substr(pos, msg.length()-pos-2);//密码

		for (Json::Value::ArrayIndex i = 0; i != root["users"].size(); i++){
            cout<<root["users"][i]["name"].asString()<<","<<root["users"][i]["password"].asString()<<endl;
        	cout<< (username == root["users"][i]["name"].asString()) <<" && ";
        	cout<< (root["users"][i]["password"].asString() == pwd) <<endl;

        	if(username == root["users"][i]["name"].asString() && root["users"][i]["password"].asString() == pwd){
            	cout<<"XFtpUSER  cmdTask->rootDir = "<<cmdTask->rootDir<<endl;
            	cmdTask->rootDir = root["users"][i]["rootDir"].asString();
            	cout<<"XFtpUSER  cmdTask->rootDir = "<<cmdTask->rootDir<<endl;
            	ResCMD("200 OK.\r\n");
            	return;
            }
        }
		ResCMD("530 Not logged in.\r\n");
	}
	
}

XFtpUSER::XFtpUSER(){}
XFtpUSER::~XFtpUSER(){}