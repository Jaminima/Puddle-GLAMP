#pragma once
class Lock {
private:
	int _lock = 0;

public:

	Lock() restrict(amp,cpu) {

	}

	bool Grant() restrict(amp,cpu) {
		if (_lock == 0) {
			_lock = 1;
			return true;
		}
		return false;
	}

	void Free() restrict(amp, cpu) {
		_lock = 0;
	}
};