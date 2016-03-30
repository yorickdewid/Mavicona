#ifndef QUEUE_H
#define QUEUE_H

#include <queue>

#include <leveldb/db.h>

class Queue {
	std::queue<int> taskList;
	std::queue<int> taskListHigh;
	std::queue<int> taskListLow;
	std::queue<int> taskListIdle;

	leveldb::DB *db = nullptr;

  public:
	Queue();
	~Queue();

	int getNextTask();
	void pushTask(int task, int prio);

	void sync();
};

#endif // QUEUE_H
