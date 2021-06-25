#include "XFtpMKD.h"
#include <event2/bufferevent.h>
#include <string>
#include <sys/types.h>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

using namespace std;

//解析协议
void XFtpMKD::Parse(std::string type, std::string msg){
	cout<<"XFtpMKD====, type = "<<type<<", msg = "<<msg<<endl;
	int pos = msg.rfind(" ")+1;
	string filename = msg.substr(pos, msg.length()-pos-2);
	string path = cmdTask->rootDir;
	path += cmdTask->curDir;
	
	cout<<"XFtpMKD::Parse  path = "<<path<<endl;

	if(type == "MKD"){
		path += ("/"+filename);
		if(!mkdir(path.c_str(),S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO)){
			ResCMD("200 Dir has Created\r\n");
		}else{
			ResCMD("450 file open failed!(no exsist)\r\n");
		}
		// ResCMD("200 recv OK.\r\n");
	}else if(type =="RMD"){
		path += ("/"+filename);
		if(!rm(path.c_str())){
			ResCMD("200 Dir delete OK.\r\n");
		}else{
			ResCMD("450 Dir delte failed!\r\n");
		}
	}
	ResCMD("200 recv OK.\r\n");
	// ResCMD("450 file open failed!\r\n");
}
void XFtpMKD::Read(struct bufferevent *bev){
	cout<<"XFtpMKD::Read"<<endl;
	ResCMD("200 OK.\r\n");
}
void XFtpMKD::Event(struct bufferevent *bev, short what){
	//如果对方网络断掉，或者机器死机有可能收不到BEV_EVENT_EOF数据
	if (what & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT)){
		cout << "BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT" << endl;
		Close();
		
	}else if(what&BEV_EVENT_CONNECTED){
		cout<<"XFtpRETR BEV_EVENT_CONNECTED"<<endl;
	}
}

int XFtpMKD::rm(std::string file_name) {
    std::string file_path = file_name;
    struct stat st;
    if (lstat(file_path.c_str(), & st) == -1) {
        return -1;
    }
    if (S_ISREG(st.st_mode)) {
        if (unlink(file_path.c_str()) == -1) {
            return -1;
        }
    } else if (S_ISDIR(st.st_mode)) {
        if (file_name == "." || file_name == "..") {
            return -1;
        }
        if (rm_dir(file_path) == -1) //delete all the files in dir.
        {
            return -1;
        }
    }
    return 0;
}
int XFtpMKD::rm_dir(std::string dir_full_path) {
    DIR * dirp = opendir(dir_full_path.c_str());
    if (!dirp) {
        return -1;
    }
    struct dirent * dir;
    struct stat st;
    while ((dir = readdir(dirp)) != NULL) {
        if (strcmp(dir -> d_name, ".") == 0 || strcmp(dir -> d_name, "..") == 0) {
            continue;
        }
        std::string sub_path = dir_full_path + '/' + dir -> d_name;
        if (lstat(sub_path.c_str(), & st) == -1) {
        	cout<<"rm_dir:lstat "<<sub_path<<" error"<<endl;
            // Log("rm_dir:lstat ", sub_path, " error");
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            if (rm_dir(sub_path) == -1) // 如果是目录文件，递归删除
            {
                closedir(dirp);
                return -1;
            }
            rmdir(sub_path.c_str());
        } else if (S_ISREG(st.st_mode)) {
            unlink(sub_path.c_str()); // 如果是普通文件，则unlink
        } else {
        	cout<<"rm_dir:st_mode "<<sub_path<<" error"<<endl;
            // Log("rm_dir:st_mode ", sub_path, " error");
            continue;
        }
    }
    if (rmdir(dir_full_path.c_str()) == -1){//delete dir itself.
        closedir(dirp);
        return -1;
    }
    closedir(dirp);
    return 0;
}