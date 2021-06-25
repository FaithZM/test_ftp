#include "XFtpSTOR.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <event2/event.h>
#include <event2/bufferevent.h>
using namespace std;

void XFtpSTOR::Parse(std::string type, std::string msg){
	int pos = msg.rfind(" ")+1;
	string filename = msg.substr(pos, msg.length()-pos-2);
	string path = cmdTask->rootDir;
	path += cmdTask->curDir;
	path += filename;
	cout<<"XFtpSTOR::Parse  path = "<<path<<endl;
	if(type == "STOR"){
		// file_name = path;
		fp = fopen(path.c_str(), "wb+");
		if(fp){
			ConnectPORT();

			ResCMD("125 File OK.\r\n");
			bufferevent_trigger(bev, EV_READ, 0);
		}else{
			ResCMD("450 file open failed!!!\r\n");
		}
	}else if(type == "DELE"){
		if (remove(path.c_str()) == 0)ResCMD("125 File Remove OK.\r\n");
		else ResCMD("450 file remove failed!\r\n");
	}
	// ResCMD("450 file open failed!\r\n");
	
}
void XFtpSTOR::Read(struct bufferevent *bev){
	cout<<"============XFtpSTOR::Read"<<endl;
	if(!fp){
		cout<<"fp 为空"<<endl;
		return;
	}else cout<<"fp 存在"<<endl;

	for(;;){
		int len = bufferevent_read(bev, buf, sizeof(buf));
		if(len<=0){
			// file_name = "";
			break;
		}
		if(fwrite(buf,strlen(buf),1,fp) != 1){
			ResCMD("450 file save failed!\r\n");
			return;
		}
		// cout<<"XFtpSTOR::Read   buf = "<<buf<<endl;//打印上传内容
		// cout<<"  <"<<len<<":"<<size<<">"<<flush;
	}
	//读取以下看是否上传成功
	char buf2[10]={0};
	fread(buf2, sizeof(buf2),1,fp);
	// cout<<"buf2 = "<<buf2<<endl;
}
void XFtpSTOR::Event(struct bufferevent *bev, short what){
	//如果对方网络断掉，或者机器死机有可能收不到BEV_EVENT_EOF数据
	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT)){
		cout << "XFtpSTOR BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT" << endl;
		Close();
		
		ResCMD("226 Transfer complete\r\n");
	}else if(what&BEV_EVENT_CONNECTED){
		cout<<"XFtpSTOR BEV_EVENT_CONNECTED"<<endl;
	}
}
