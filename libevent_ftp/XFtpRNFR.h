#pragma once
#include "XFtpTask.h"

class XFtpRNFR:public XFtpTask
{
public:
	//解析协议
	virtual void Parse(std::string type, std::string msg);
	virtual void Read(struct bufferevent *bev);
	virtual void Event(struct bufferevent *bev, short what);
private:
	std::string old_name = "";//旧的文件名
};