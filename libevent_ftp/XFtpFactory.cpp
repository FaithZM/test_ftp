#include "XFtpFactory.h"
#include "XFtpServerCMD.h"
#include "XFtpUSER.h"
#include "XFtpLIST.h"
#include "XFtpPORT.h"
#include "XFtpRETR.h"
#include "XFtpSTOR.h"
#include "XFtpRNFR.h"
#include "XFtpMKD.h"

XTask *XFtpFactory::CreateTask(){
	XFtpServerCMD *x = new XFtpServerCMD();

	//注册ftp消息处理对象
	XFtpUSER *user = new XFtpUSER();
	x->Reg("USER", user);
	x->Reg("PASS", user);
	
	XFtpLIST *list = new XFtpLIST();
	x->Reg("PWD", list);
	x->Reg("LIST", list);
	x->Reg("CWD", list);
	x->Reg("CDWP", list);
	x->Reg("PORT", new XFtpPORT());
	x->Reg("RETR", new XFtpRETR());
	
	XFtpSTOR *stor = new XFtpSTOR();
	x->Reg("STOR", stor);//创建文件
	x->Reg("DELE", stor);//删除文件

	XFtpRNFR *rnfr = new XFtpRNFR();//重命名文件和文件夹
	x->Reg("RNFR", rnfr);
	x->Reg("RNTO", rnfr);

	XFtpMKD *mkd = new XFtpMKD();
	x->Reg("MKD", mkd); //创建  目录
	x->Reg("RMD", mkd); //删除  目录


	return x;
}

XFtpFactory::XFtpFactory(){}