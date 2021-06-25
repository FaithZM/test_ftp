// first_libevent.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <event2/event.h>
#include <event2/listener.h>
#include "XThreadPool.h"
#include <string.h>
#include <string>
#ifndef _WIN32
#include<signal.h> //linux适配
#endif
#include <iostream>
#include <json/json.h>
#include <fstream>
#include "XFtpFactory.h"
#include "XFtp_log.h"

using namespace std;


//全剧文件读取变量
Json::Reader reader;
Json::Value root;
// #define SPORT 5001

Logger _logger;


void listen_cb(struct evconnlistener *e, evutil_socket_t s, struct sockaddr * a, int socklen, void *arg)
{
	cout << "listen_cb" << endl;
	XTask *task = XFtpFactory::Get()->CreateTask();
	task->sock = s;
	XThreadPool::Get()->Dispatch(task);
}

void Parse_json(){
	
	ifstream in ("config.json", ios::binary);
	if (!in.is_open()) {
	    cout << "Error opening file of json\n";
	    return;
	}else cout << "success opening file of json\n";
	if (reader.parse(in ,root)) {
	    //读取根节点信息  
	    // rootDir = root["rootDir"];
	    // cout<<"root['port'] = "<<root["port"].asInt()<<endl;
	    // cout<<"root['rootDir'] = "<<root["rootDir"]<<endl;
	} else {
	    cout << "Parse_json error\n" << endl;
	}
	// cout<<"初始化配置文件成功"<<endl;
	in .close();
}

//初始化日志
void Init_log(){
	string pattern = "[%D{%Y-%m-%d %H:%M:%S}] %-5p - %m %n";
	auto_ptr < Layout > _layout(new PatternLayout(pattern));

    //RollingFileAppender滚动追加
    //FileAppender 覆盖写入
    //日志文件最小是200k,这里是200k
    SharedAppenderPtr _append(new RollingFileAppender(root["log_dir"].asString(),  200*1024, 10));
    // _append -> setName("file log test");
    _append -> setLayout(_layout);

    /* step 4: Instantiate a logger object */

    _logger = Logger::getInstance("test");
    /* step 5: Attach the appender object to the logger */
    _logger.addAppender(_append);
}

int main()
{

#ifdef _WIN32
	//初始化socket库
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,2), &wsa);
#else
	//忽略管道信号， 发送数据给已关闭的socket
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)return 1;
#endif
	//1. 初始化线程池
	XThreadPool::Get()->Init(10);

	Parse_json();
	Init_log();

    cout << "test thread pool!\n";
	event_base *base = event_base_new();
	if (base) 
	{
		cout << "event_base_new success" << endl;
	}
	//监听端口
	//socket, bind, listen, 绑定事件

	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	cout<<"root[\"port\"].asInt() = "<<root["port"].asInt()<<endl;
	sin.sin_port = htons(root["port"].asInt());

	evconnlistener *ev = evconnlistener_new_bind(base, //libevent的上下文
		listen_cb, //接收到连接的回调函数
		base, //回调函数获取参数arg
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, //地址重用，evconnlistener关闭同时关闭Socket
		10, //连接队列大小，对应listen函数
		(sockaddr*)&sin,	//绑定的地址和端口 
		sizeof(sin)
	);

	//事件分发处理
	if (base)event_base_dispatch(base);
	else cout<<"base failed!"<<endl;

	if (ev)evconnlistener_free(ev);
	else cout<<"ev failed!"<<endl; //监听绑定失败

	if(base)event_base_free(base);
	else  cout<<"event_base_free failed!"<<endl;
	
#ifdef _WIN32
	WSACleanup();
#endif
	cout<<"退出程序"<<endl;

}
