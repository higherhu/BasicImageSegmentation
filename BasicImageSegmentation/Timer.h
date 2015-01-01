//
// Author: hujiagao@gmail.com
//

#pragma once

#include <iostream>
#include <time.h>
using namespace std;

class Timer{
public:
	Timer(const string & name="Timer"):_title(name) {_start = false; _startTime = 0;};
	~Timer() {};

	void Start()
	{
		if (! _start)
			_start = true;
		_startTime = clock();
	};

	float Stop()
	{
		if (! _start)
		{
			return 0;
		}
		float timecost = float(clock() - _startTime) / CLOCKS_PER_SEC;
		_start = false;
		cout<<"["<<_title<<"]"<<" Time cost: "<<timecost<<endl<<endl;
		return timecost;
	}

private:
	string _title;

	bool _start;
	clock_t _startTime;

};