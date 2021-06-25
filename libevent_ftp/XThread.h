#pragma once
#include <event2/event.h>
#include <list>
#include <mutex>
class XTask;
class XThread
{
public:
	
	void Start();

	
	void Main();

	
	bool Setup();


	void Notify(evutil_socket_t fd, short which);


	void Activate();


	void AddTask(XTask *t);
	XThread();
	~XThread();
	

	int id = 0;
private:
	int notify_send_fd = 0;
	struct event_base *base = 0;

	std::list<XTask*> tasks;

	std::mutex tasks_mutex;
};

