#include "XFtpRNFR.h"
#include <event2/bufferevent.h>
#include <string>
#include <cstdlib>
#include <iostream>
using namespace std;

void XFtpRNFR::Parse(std::string type, std::string msg){
	// cout<<"XFtpRNFR====, type = "<<type<<", msg = "<<msg<<endl;
	int pos = msg.rfind(" ")+1;
	string filename = msg.substr(pos, msg.length()-pos-2);
	string path = cmdTask->rootDir;
	path += cmdTask->curDir;
	path += filename;
	// cout<<"XFtpRNFR::Parse  path = "<<path<<endl;

	if(type == "RNFR"){
		old_name = path;
		// ResCMD("200 recv OK.\r\n");
	}else if(type == "RNTO"){
		if(old_name != ""){
			if(!rename(old_name.c_str(), path.c_str())){
				ResCMD("200 filename has changed\r\n");
			}else{
				ResCMD("450 filename changed failed!\r\n");
			}
		}else ResCMD("450 file open failed!(no exsist)\r\n");
	}
	ResCMD("200 recv OK.\r\n");
	// ResCMD("450 file open failed!\r\n");
	
}
void XFtpRNFR::Read(struct bufferevent *bev){
	cout<<"XFtpRNFR::Read"<<endl;
	ResCMD("200 OK.\r\n");
}
void XFtpRNFR::Event(struct bufferevent *bev, short what){
	//如果对方网络断掉，或者机器死机有可能收不到BEV_EVENT_EOF数据
	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT)){
		cout << "BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT" << endl;
		Close();
		
	}else if(what&BEV_EVENT_CONNECTED){
		cout<<"XFtpRETR BEV_EVENT_CONNECTED"<<endl;
	}
}