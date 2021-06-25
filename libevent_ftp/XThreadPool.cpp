#include "XThreadPool.h"
#include "XThread.h"
#include "XTask.h"
#include <thread>
#include <iostream>
using namespace std;

void XThreadPool::Dispatch(XTask *task)
{
	
	if (!task)return;
	int tid = (lastThread + 1) % threadCount;
	lastThread = tid;
	XThread *t = threads[tid];

	t->AddTask(task);
	
	t->Activate();


}

void XThreadPool::Init(int threadCount)
{
	this->threadCount = threadCount;
	this->lastThread = -1;
	for (int i = 0; i < threadCount; i++)
	{
		XThread *t = new XThread();
		t->id = i + 1;
		cout << "Create thread " << i << endl;
		
		t->Start();
		threads.push_back(t);
		this_thread::sleep_for(20ms);

	}
}
