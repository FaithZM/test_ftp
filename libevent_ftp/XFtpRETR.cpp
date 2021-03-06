#include "XFtpRETR.h"
#include <iostream>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <string>
using namespace std;


void XFtpRETR::Write(struct bufferevent *bev){
	if(!fp)return;
	int len = fread(buf, 1, sizeof(buf), fp);
	if(len<=0){
		ResCMD("226 Transfer complete\r\n");
		Close();
		return;
	}
	cout<<"["<<len<<"]"<<flush;

	Send(buf, len);

}
void XFtpRETR::Event(struct bufferevent *bev, short what){
	//如果对方网络断掉，或者机器死机有可能收不到BEV_EVENT_EOF数据
	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT)){
		cout << "BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT" << endl;
		Close();
		
	}else if(what&BEV_EVENT_CONNECTED){
		cout<<"XFtpRETR BEV_EVENT_CONNECTED"<<endl;
	}
}
void XFtpRETR::Parse(std::string type, std::string msg){
	int pos = msg.rfind(" ")+1;
	string filename = msg.substr(pos, msg.length()-pos-2);
	string path = cmdTask->rootDir;
	path += cmdTask->curDir;
	path += filename;
	cout<<"XFtpRETR::Parse  path = "<<path<<endl;
	fp = fopen(path.c_str(), "rb");
	if(fp){
		ConnectPORT();

		ResCMD("150 File OK.\r\n");
		bufferevent_trigger(bev, EV_WRITE, 0);
	}else{
		ResCMD("450 file open failed!\r\n");
	}
}