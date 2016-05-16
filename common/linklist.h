#ifndef LINK_LIST_H
#define LINK_LIST_H

#include <iostream>
#include <string>

struct Item {
	std::string key;
	Item *next;
};

class LinkList {
  private:
	Item *head;
	int length;

  public:
	LinkList() {
		head = new Item;
		head->next = nullptr;
		length = 0;
	}

	~LinkList() {
		Item *p = head;
		Item *q = head;
		while (q) {
			p = q;
			q = p->next;
			if (q)
				delete p;
		}
	}

	void insertItem(Item *newItem) {
		if (!head->next) {
			head->next = newItem;
			length++;
			return;
		}

		Item *p = head;
		Item *q = head;
		while (q) {
			p = q;
			q = p->next;
		}
		p->next = newItem;
		newItem->next = nullptr;
		length++;
	}

	bool removeItem(std::string itemKey) {
		if (!head->next)
			return false;

		Item *p = head;
		Item *q = head;
		while (q) {
			if (q->key == itemKey) {
				p->next = q -> next;
				delete q;
				length--;
				return true;
			}
			p = q;
			q = p->next;
		}

		return false;
	}

	Item *getItem(std::string itemKey) {
		Item *p = head;
		Item *q = head;
		while (q) {
			p = q;
			if ((p != head) && (p->key == itemKey))
				return p;
			q = p->next;
		}

		return NULL;
	}

	void printList() {
		if (length == 0) {
			std::cout << "\n{ }\n";
			return;
		}

		Item * p = head;
		Item * q = head;
		std::cout << "\n{ ";
		while (q) {
			p = q;
			if (p != head) {
				std::cout << p->key;
				if (p->next)
					std::cout << ", ";
				else
					std::cout << " ";
			}
			q = p->next;
		}
		std::cout << "}\n";
	}

	inline int getLength() {
		return length;
	}
};

#endif // LINK_LIST_H
