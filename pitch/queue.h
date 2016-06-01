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

	void queuCache() {
		T task;

		leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
		for (it->SeekToFirst(); it->Valid(); it->Next()) {
			task.ParseFromArray(it->value().data(), it->value().size());

			switch (task.priority()) {
				case Task::REALTIME:
					// runRealtimeTask()
					//std::cout << "Realtime task; ignores all queues" << std::endl;
					break;
				case Task::HIGH:
					push(task, 1, false);
					break;
				case Task::NORMAL:
					push(task, 2, false);
					break;
				case Task::LOW:
					push(task, 3, false);
					break;
				case Task::IDLE:
					push(task, 100, false);
					break;
			}
		}

		assert(it->status().ok());  // Check for any errors found during the scan
		delete it;
	}

  public:
	Queue(const std::string& dbName = "qstore") {
		leveldb::Options options;
		options.create_if_missing = true;
		leveldb::Status status = leveldb::DB::Open(options, dbName, &this->db);

		if (!status.ok())
			std::cerr << status.ToString() << std::endl;

		/* Retrieve cached tasks an put them back on the queue */
		queuCache();
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
		db->Delete(leveldb::WriteOptions(), std::to_string(task.id()));
	}

	inline T getNextTask() {
		T task = taskList.top();
		taskList.pop();
		return task;
	}

	inline T getNextIdleTask() {
		T task = taskListIdle.front();
		taskListIdle.pop();
		return task;
	}

	inline void push(T task, int prio, bool cache = true) {
		if (cache) {
			std::string serialized;
			task.SerializeToString(&serialized);

			db->Put(leveldb::WriteOptions(), std::to_string(task.id()), serialized);
		}

		if (prio > 50) {
			this->taskListIdle.push(task);
		} else {
			this->taskList.push(task);
		}
	}

	void sync();
};

#endif // QUEUE_H
