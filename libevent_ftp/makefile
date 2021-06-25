SRC= ev_ftp_server.cpp XThreadPool.cpp XFtpServerCMD.cpp XThread.cpp\
	XFtpFactory.cpp XFtpTask.cpp XFtpUSER.cpp XFtpLIST.cpp XFtpPORT.cpp\
	XFtpRETR.cpp XFtpSTOR.cpp XFtpRNFR.cpp XFtpMKD.cpp

	
ev_ftp_server:$(SRC)
	g++ $^ -o $@ -levent -pthread -ljson -llog4cplus -lrt -lpthread
	./$@
clean:
	rm -rf ev_ftp_server
	rm -rf *.o