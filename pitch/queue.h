#ifndef QUEUE_H
#define QUEUE_H

#include <queue>
#include <leveldb/db.h>

template<class T>
class Queue {
	std::queue<T> taskList;
	std::queue<T> taskListHigh;
	std::queue<T> taskListLow;
	std::queue<T> taskListIdle;

	leveldb::DB *db = nullptr;

  public:
	Queue() {
		leveldb::Options options;
		options.create_if_missing = true;
		leveldb::Status status = leveldb::DB::Open(options, "qstore", &this->db);

		if (!status.ok())
			std::cerr << status.ToString() << std::endl;
	}

	~Queue() {
		delete this->db;
	}

	inline size_t totalTaskCount() {
		return taskList.size() + taskListHigh.size() + taskListLow.size();
	}

	inline size_t idleTaskCount() {
		return taskListIdle.size();
	}

	inline void doneTask(T task) {
		taskList.pop();
	}

	T getNextTask() {
		T task = taskList.front();
		taskList.pop();
		return task;
	}

	T getNextIdleTask() {
		T task = taskListIdle.front();
		taskList.pop();
		return task;
	}

	inline void push(T task, int prio) {
		if (prio > 50) {
			this->taskListIdle.push(task);
		} else {
			this->taskList.push(task);
		}
	}
	void sync();
};

#endif // QUEUE_H
