#ifndef QUEUE_H
#define QUEUE_H

#include <queue>
#include <leveldb/db.h>

struct CompareTaskPriority : public std::binary_function<Task, Task, bool> {
	bool operator()(const Task lhs, const Task rhs) const {
		return lhs.priority() > rhs.priority();
	}
};

template<class T>
class Queue {
	std::priority_queue<T, std::vector<T>, CompareTaskPriority> taskList;
	std::queue<T> taskListIdle;

	leveldb::DB *db = nullptr;

  public:
	Queue(const std::string& dbName = "qstore") {
		leveldb::Options options;
		options.create_if_missing = true;
		leveldb::Status status = leveldb::DB::Open(options, dbName, &this->db);

		if (!status.ok())
			std::cerr << status.ToString() << std::endl;
	}

	~Queue() {
		delete this->db;
	}

	inline size_t totalTaskCount() {
		return taskList.size();
	}

	inline size_t idleTaskCount() {
		return taskListIdle.size();
	}

	inline void doneTask(T task) {
		taskList.pop();
	}

	inline T getNextTask() {
		T task = taskList.top();
		taskList.pop();
		return task;
	}

	inline T getNextIdleTask() {
		T task = taskListIdle.top();
		taskList.pop();
		return task;
	}

	inline void push(T task, int prio) {
		// std::string value;
		// if (s.ok()) {
		// this->db->Put(leveldb::WriteOptions(), key2, value);
		// }

		if (prio > 50) {
			this->taskListIdle.push(task);
		} else {
			this->taskList.push(task);
		}
	}

	void sync();
};

#endif // QUEUE_H
