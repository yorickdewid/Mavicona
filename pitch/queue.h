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

	inline size_t totalTaskCount() {
		return taskList.size() + taskListHigh.size() + taskListLow.size();
	}

	inline size_t idleTaskCount() {
		return taskListIdle.size();
	}

	inline void doneTask(int task) {
		taskList.pop();
	}

	int getNextTask();
	int getNextIdleTask();


	void push(int task, int prio);
	void sync();
};

#endif // QUEUE_H
