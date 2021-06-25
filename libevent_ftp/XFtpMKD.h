#pragma once
#include "XFtpTask.h"
class XFtpMKD:public XFtpTask
{
public:
	//解析协议
	virtual void Parse(std::string type, std::string msg);
	virtual void Read(struct bufferevent *bev);
	virtual void Event(struct bufferevent *bev, short what);

	int rm_dir(std::string dir_full_path);
	int rm(std::string file_name); //rm()函数，判断文件类型，如果是目录文件则rm_dir，普通文件则unlink.
	
};