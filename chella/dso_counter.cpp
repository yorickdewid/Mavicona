/**
 * Copyright (C) 2015-2016 Mavicona, Quenza Inc.
 * All Rights Reserved
 *
 * This file is part of the Mavicona project.
 *
 * Content can not be copied and/or distributed without the express
 * permission of the author.
 */

#include <iostream>
#include <ace/interface.h>
#include <cstdlib>
#include <vector>
#include <math.h>
#include <sys/time.h>

#define DAYS45_70 (25 * 365 + 6)
#define DAYS1601_1970 134774
#define NTIMENAME (31 * 36 * 36 * 36)

namespace Timename {

void baggerid(int days, int secs, int nanosecs, char *timename) {
	double ntime;
	ldiv_t d;
	int i;

	ntime = (double)secs * 1E9 + (double)nanosecs;
	ntime = (ntime * (36.0 * 36.0 * 36.0 * 36.0 * 36.0 * 36.0 * 36.0 * 36.0)) / 86400E9;
	ntime = floor(ntime + 0.5);
	for (i = 11; i >= 4; i--) {
		double d = floor(ntime / 36.0);
		int rem = (int)(floor(ntime - (d * 36.0)));
		timename[i] = rem < 10 ? '0' + rem : 'a' + rem - 10;
		ntime = d;
	}

	d.quot = days + DAYS45_70 + NTIMENAME;
	for (i = 4; i--; ) {
		d = ldiv(d.quot, 36);
		timename[i] = d.rem < 10 ? '0' + (char)d.rem : 'a' + (char)d.rem - 10;
	}
}

static void curr_time(int *days, int *secs, int *nanosecs) {
#ifdef __MINGW32__
	FILETIME ft;
	ULARGE_INTEGER ul;
	GetSystemTimeAsFileTime(&ft);
	ul.LowPart = ft.dwLowDateTime;
	ul.HighPart = ft.dwHighDateTime;
	*days = (int)(ul.QuadPart / 864000000000) - DAYS1601_1970;
	*secs = (int)((ul.QuadPart % 864000000000) / 10000000);
	*nanosecs = (int)(ul.QuadPart % 10000000) * 100;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	*days = tv.tv_sec / 86400;
	*secs = tv.tv_sec % 86400;
	*nanosecs = tv.tv_usec * 1000;
#endif // __MINGW32__
}

char *bagger_now() {
	int days, secs, nanosecs;
	char *str = (char *)malloc(13);
	memset(str, '\0', 13);

	curr_time(&days, &secs, &nanosecs);
	baggerid(days, secs, nanosecs, str);

	return str;
}

std::string timename() {
	char *timename = Timename::bagger_now();
	std::string strtimename(timename);
	free(timename);

	return strtimename;
}

}

using namespace Ace;

class Counter : public Job {
	std::vector<Subjob *> subjobs;

  public:
	Counter() {
		subjobs.reserve(10);

		/* Create some example jobs */
		subjobs.push_back(new Subjob("counter_1"));
		subjobs.push_back(new Subjob("counter_2"));
		subjobs.push_back(new Subjob("counter_3"));
		subjobs.push_back(new Subjob("counter_4"));
		subjobs.push_back(new Subjob("counter_5"));
		subjobs.push_back(new Subjob("counter_6"));
		subjobs.push_back(new Subjob("counter_7"));
		subjobs.push_back(new Subjob("counter_8"));
		subjobs.push_back(new Subjob("counter_9"));
		subjobs.push_back(new Subjob("counter_10"));
	}

	~Counter() {
		for (std::vector<Subjob *>::iterator it = subjobs.begin(); it != subjobs.end(); ++it) {
			delete *it;
		}
	}

	void Setup() {
		std::cout << std::endl;

		/* Print job info */
		std::cout << "Job id: " << Id() << std::endl;
		std::cout << "Job name: " << Name() << std::endl;
		std::cout << "Job quid: " << Quid() << std::endl;
		std::cout << "Job partition: " << Partition() << "/" << TotalPartitions() << std::endl;
		std::cout << "Job status: " << Status() << std::endl;
		std::cout << "Job parent: " << Parent() << std::endl;
	}

	/* The actual execution of the job */
	void Run(const std::string& data) {
		if (isPartition() && !data.empty()) {
			std::cout << "Receiverd timename: " << data << std::endl;
		}

		/* Send data to subjobs */
		if (isSpawn()) {
			for (std::vector<Subjob *>::iterator it = subjobs.begin(); it != subjobs.end(); ++it) {
				(*it)->setData(Timename::timename());
			}
		}
	}

	void Teardown() {
		if (isSpawn()) {
			/* Create jobchain when job is spawned for the first time */
			Chain *chain = new Chain(Quid());
			chain->setParentName(Name());

			for (std::vector<Subjob *>::iterator it = subjobs.begin(); it != subjobs.end(); ++it) {
				chain->add(**it);
			}

			BindChain(chain);
		}
	}

};

ACE_OBJECT(Counter);
