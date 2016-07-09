#ifndef STOREIO_H
#define STOREIO_H

namespace Ace {

class StoreIO {
	Callback *cb = NULL;

  public:
	StoreIO() {}

	void QuerySelect() {

	}

	void QueryInsert() {

	}

	void QueryUpdate() {

	}

	void QueryDelete() {

	}

	void QuerySearch() {

	}

	void setCallback(Callback *_cb) {
		cb = _cb;
	}
};

}

#endif // STOREIO_H
