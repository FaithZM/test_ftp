#include "XFtpServerCMD.h"
#include<event2/event.h>
#include<event2/bufferevent.h>
#include<string.h>
#include<iostream>
#include<string>

using namespace std;

#include "XFtp_log.h"
extern Logger _logger;

void XFtpServerCMD::Reg(std::string cmd, XFtpTask*call){
	if(!call){
		cout<<"XFtpServerCMD::Reg call is null"<<endl;
		return;
	}
	if(cmd.empty()){
		cout<<"XFtpServerCMD::Reg cmd is null"<<endl;
		return;
	}
	if(calls.find(cmd) != calls.end()){
		cout<<cmd<<" is already register"<<endl;
		return;
	}
	calls[cmd] = call;
	calls_del[call] = 0;
}
//子线程XThread  event事件分发
void XFtpServerCMD::Read(struct bufferevent *bev){
	char data[1024] = { 0 };
	for (;;)
	{
		int len = bufferevent_read(bev, data, sizeof(data) - 1);
		if (len <= 0)break;
		data[len] = '\0';
		cout <<"Recv CMD: "<< data << flush;

		string type = "";
		for(int i=0; i<len; i++){
			if(data[i]==' '||data[i]=='\r'){
				break;
			}
			type += data[i];
		}
		cout<<"type is ["<<type<<"]"<<endl;
		if(calls.find(type) != calls.end()){
			XFtpTask *t = calls[type];
			t->cmdTask = this;
			t->ip = ip;
			t->port = port;
			t->base = base;

			//这里记录用户所有操作行为
			string data2 = data;
			string info_t = ip+":"+to_string(port)+" ["+type+"] ["+(data2.substr(0, data2.length()-2))+"]";
			LOG4CPLUS_INFO(_logger, info_t.c_str());

			t->Parse(type, data);
			if(type == "PORT"){
				ip = t->ip;
				port = t->port;
			}

		}else{
			string msg = "200 OK\r\n";
			bufferevent_write(bev, msg.c_str(), msg.size());
		}
	}
	//分发到处理对象

}
void XFtpServerCMD::Event(struct bufferevent *bev, short what){

	//如果对方网络断掉，或者机器死机有可能收不到BEV_EVENT_EOF数据
	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT)){
		cout << "BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT" << endl;
		// bufferevent_free(bev);
		delete this;
	}
}



//初始化任务 运行在子线程中
bool XFtpServerCMD::Init()
{
	cout << "XFtpServerCMD::Init()" << endl;
	//监听socket bufferevent
	//base  socket
	bufferevent *bev = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE);
	if(!bev){
		delete this;
		return false;
	}
	this->bev = bev;
	this->SetCallback(bev);

	//添加超时
	timeval rt = {120,0};
	bufferevent_set_timeouts(bev, &rt, 0);
	string msg = "220 Welcome to libevent XFtpServer\r\n";
	bufferevent_write(bev, msg.c_str(), msg.size());
	return true;
}

XFtpServerCMD::~XFtpServerCMD()
{
	Close();
	for(auto ptr = calls_del.begin(); ptr != calls_del.end(); ptr++){
		ptr->first->Close();
		delete ptr->first;
	}
	// delete logs;
}


