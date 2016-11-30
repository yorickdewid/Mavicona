#!/usr/bin/python3
##
# Copyright (C) 2015-2016 Mavicona, Quenza Inc.
# All Rights Reserved
#
# This file is part of the Mavicona project.
#
# Content can not be copied and/or distributed without the express
# permission of the author.
#
# Mock the worker environment. Jobs can be run in dry mode to see
# how they behave in the cluster.
#

import os
import sys
import random
import uuid
import json
import time
import importlib
import socket
import pathlib
import ace.config
import ace.job
import ace.ipc
import ace.db
import ace.sha1

jobnames = ['Random', 'SuperTest', 'Generator', 'WordCount', 'Coverage', 'Unit']
jobdata = ('Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo'
			'ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis '
			'dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies'
			'nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. '
			'Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. '
			'In enim justo, rhoncus ut, imperdiet a, venenatis vitae, justo. Nullam '
			'dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. '
			'Vivamus elementum semper nisi. Aenean vulputate eleifend tellus. Aenean '
			'leo ligula, porttitor eu, consequat vitae, eleifend ac, enim. Aliquam lorem '
			'ante, dapibus in, viverra quis, feugiat a, tellus. Phasellus viverra nulla '
			'ut metus varius laoreet. Quisque rutrum. Aenean imperdiet. Etiam ultricies '
			'nisi vel augue. Curabitur ullamcorper ultricies nisi. Nam eget dui.')

def usage(name):
	print(name, '[Python file]')

def jobrunner(name, status=ace.job.JobStatus.spawn, partition=0, partition_count=0, parent=None):
	quid = uuid.uuid4()
	ins = job_mod.job_init(ace.config.Config())
	obj = ins.invoke()
	obj.cfg = ins
	obj.__ipc__ = ace.ipc.Callback()
	obj.__db__ = ace.db.DB()
	obj.inject(
		random.randint(1, 10000),
		name,
		ace.sha1.sha1(bytearray(os.urandom(10))),
		quid,
		partition,
		partition_count,
		parent)

	obj.update_status(status)

	if status is ace.job.JobStatus.spawn:
		obj.setup_once()
	
	obj.setup()
	obj.run(jobdata)
	obj.teardown()
	
	#TODO: find out when the latest job runs
	if status is ace.job.JobStatus.partition:
		obj.teardown_once()

	print('Chains', len(obj.chains))
	for chain in obj.chains:
		print('Subjobs', len(chain.subjobs))

		partition = 0
		for job in chain.subjobs:
			print('> Subjob', job['name'])
			newstatus = ace.job.JobStatus.funnel
			if status is ace.job.JobStatus.spawn:
				newstatus = ace.job.JobStatus.partition
			if status is ace.job.JobStatus.partition:
				newstatus = ace.job.JobStatus.funnel
			jobrunner(job['name'], newstatus, partition, len(chain.subjobs), quid)
			partition += 1

if __name__ == '__main__':
	if len(sys.argv) < 2:
		usage(sys.argv[0])
		exit(0)
	
	os.environ["WORKERID"] = str(random.randint(0, 150))

	if sys.argv[1] == '--help' or sys.argv[1] == '-h':
		usage(sys.argv[0])
		exit(0)

	file = pathlib.Path(sys.argv[1])
	if not file.is_file():
		print('Cannot open python file:',file.name)
		exit(0)

	import ace.asys
	job_mod = importlib.import_module(file.name[:-3])
	if 'package' in dir(job_mod):
		meta = job_mod.package()
		with open('package.json', 'w') as fp:
			meta['meta'] = {}
			meta['meta']['main'] = file.name
			meta['meta']['invoke'] = 'job_init'
			meta['meta']['host'] = socket.gethostname()
			meta['meta']['timestamp'] = time.time()
			json.dump(meta, fp)

	if os.path.isfile('.env'):
		with open('.env') as envdata:
			for line in envdata.readlines():
				kv = line[:-1].split('=', 1)
				if (len(kv) != 2):
					continue
				os.environ[kv[0]] = str(kv[1])

	jobrunner(random.choice(jobnames))
