#pragma once
#include "XFtpTask.h"
class XFtpSTOR:public XFtpTask
{
public:
	virtual void Parse(std::string type, std::string msg);
	virtual void Read(struct bufferevent *bev);
	virtual void Event(struct bufferevent *bev, short what);
private:
	FILE *fp = 0;
	char buf[1024] = {0};
	// std::string file_name = "";
};