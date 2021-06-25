#pragma once
#include "XFtpTask.h"
#include <map>
class XFtpServerCMD :public XFtpTask
{
public:
	//初始化任务
	virtual bool Init();

	virtual void Read(struct bufferevent *bev);
	virtual void Event(struct bufferevent *bev, short what);

	//注册命令处理对象
	void Reg(std::string, XFtpTask*call);

	~XFtpServerCMD();
private:
	std::map<std::string, XFtpTask*> calls;
	std::map<XFtpTask*, int> calls_del;
	std::string new_root_dir = "";
	// XFtp_log *logs = new XFtp_log();
};

