#ifndef IO_H
#define IO_H

namespace Ace {

class StoreIO {
	Callback *cb = NULL;

  public:
	StoreIO() {}

	void querySelect() {

	}

	void queryInsert() {

	}

	void queryUpdate() {

	}

	void queryDelete() {

	}

	void querySearch() {

	}

	void setCallback(Callback *_cb) {
		cb = _cb;
	}
};

}

#endif // IO_H
