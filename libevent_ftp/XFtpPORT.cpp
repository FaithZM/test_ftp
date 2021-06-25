#include "XFtpPORT.h"
#include <vector>
#include <iostream>
using namespace std;

void XFtpPORT::Parse(std::string type, std::string msg){

	vector<string> vals;
	string tmp = "";
	for(int i=5; i<msg.length(); i++){
		if(msg[i]==',' || msg[i]=='\r'){
			vals.push_back(tmp);
			tmp = "";
			continue;
		}
		tmp += msg[i];
	}
	cout<<"vals.size() = "<<vals.size()<<endl;
	if(vals.size() != 6){
		ResCMD("501 Syntax error in parameters or arguments.");
		return;
	}
	// for(int i=0; i<vals.size(); i++){
	// 	cout<<vals[i]<<endl;
	// }
	ip = vals[0]+"."+vals[1]+"."+vals[2]+"."+vals[3];

	port = atoi(vals[4].c_str())*256 + atoi(vals[5].c_str());
	cout<<"PORT ip is "<<ip<<endl;
	cout<<"PORT port is "<<port<<endl;
	ResCMD("200 PORT command successful.\r\n");

}