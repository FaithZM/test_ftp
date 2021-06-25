#include "XFtpLIST.h"
#include <iostream>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <string>
// #include <io.h>
#ifdef _WIN32
#include <io.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif
using namespace std;

void XFtpLIST::Write(struct bufferevent *bev){
	ResCMD("226 Transfer complete\r\n");
	Close();
}
void XFtpLIST::Event(struct bufferevent *bev, short what){
	//如果对方网络断掉，或者机器死机有可能收不到BEV_EVENT_EOF数据
	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT)){
		cout << "BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT" << endl;
		Close();
	}else if(what&BEV_EVENT_CONNECTED){
		cout<<"XFtpLIST BEV_EVENT_CONNECTED"<<endl;
	}
}

//解析协议
void XFtpLIST::Parse(std::string type, std::string msg){
	string resmsg = "";

	if(type == "PWD"){
		resmsg = "257 \"";
		resmsg += cmdTask->curDir;
		resmsg += "\" is current dir.\r\n";

		ResCMD(resmsg);
	}else if(type == "LIST"){
		ConnectPORT();
		ResCMD("150 Here comes the directory listing.\r\n");
		// string listdata = "-rwxrwxrwx 1 root group 64463 Mar 14 09:53 101.jpg\r\n";
		string listdata = GetListData(cmdTask->rootDir + cmdTask->curDir);
		Send(listdata);
	}else if(type == "CWD"){
		int pos = msg.rfind(" ")+1;
		string path = msg.substr(pos, msg.length()-pos-2);
		if(path[0]=='/'){
			cmdTask->curDir = path;
		}else{
			if(cmdTask->curDir[cmdTask->curDir.size()-1] != '/'){
				cmdTask->curDir += "/";
			}
			cmdTask->curDir += path+"/";
		}

		
		ResCMD("250 Directory success chanaged.\r\n");
	}else if(type=="CDUP"){
		string path = cmdTask->curDir;
		if(path[path.size()-1] == '/'){
			path = path.substr(0, path.length()-1);
		}
		int pos = path.rfind("/");
		path = path.substr(0, pos);
		cmdTask->curDir = path;
		ResCMD("250 Directory success chanaged.\r\n");
	}
}
std::string XFtpLIST::GetListData(std::string path){

	string data = "";
#ifdef _WIN32
	_finddata_t file;
	path += "/*.*";
	intptr_t dir =_findfirst(path.c_str(), &file);
	if(dir<0){
		return data;
	}

	do{
		string tmp = "";
		if(file.attrib & A_SUBDIR){
			if(strcmp(file.name, ".")==0 || strcmp(file.name, "..")==0){
				continue;
			}
			tmp = "drwxrwxrwx 1 root group ";
		}else{
			tmp = "-rwxrwxrwx 1 root group ";
		}

		char buf[1024];
		sprintf(buf, "%u", file.size);
		tmp += buf;

		strftime(buf, sizeof(buf)-1, "%b %d %H:%M", localtime(file.time_write));
		tmp += buf;
		tmp += file.name;
		tmp += "\r\n";
		data += tmp;

	}while(_findnext(dir, &file) == 0);
#else
	char buffer[1024];
	string cmd = "ls -l "; //有些linux系统会把月任翻译，所以需要扫为处理下
	cmd += path;
	cout<<"cmd = "<<cmd<<endl;
	FILE *f = popen(cmd.c_str(), "r");
	if(!f)return data;
	for(;;){
		int len = fread(buffer, 1, sizeof(buffer)-1, f);
		if(len<=0)break;
		buffer[len] = '\0';

		string temps = buffer;
		// cout<<temps<<endl<<endl;
		
		vector<string> month1={" 1月 "," 2月 "," 3月 "," 4月 "," 5月 "," 6月 "," 7月 "," 8月 "
		," 9月 "," 10月 "," 11月 "," 12月 "};
		vector<string> month2={" Jan "," Feb "," Mar "," Apr "," May "," Jun "," Jul "," Aug "
		," Sep "," Oct "," Nov "," Dec "};

		for(int i=0; i<month1.size(); i++){
			while(true){
	            string::size_type pos(0);
	            if((pos=temps.find(month1[i]))!=string::npos)
	                temps.replace(pos,month1[i].length(),month2[i]);
	            else break;
	        }
		}
		data += temps;
	}
	// Handle_date(data);
	cout<<"data = "<<data<<endl;
	pclose(f);
    
#endif

	return data;
}
