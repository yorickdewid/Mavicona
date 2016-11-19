##
# Copyright (C) 2015-2016 Mavicona, Quenza Inc.
# All Rights Reserved
#
# This file is part of the Mavicona project.
#
# Content can not be copied and/or distributed without the express
# permission of the author.
#

import time;
from enum import Enum

class JobModel(Enum):
	once = 0
	dispere = 1
	mapred = 2
	fan = 3

class JobStatus(Enum):
	spawn = 0
	partition = 1
	funnel = 2

class Chain(object):
	subjobs = []
	parent_quid = None

	def __init__(self, quid):
		self.parent_quid = quid

	def add(self, obj, name):
		self.subjobs.append({
			'object' : obj,
			'name' : name,
		})

class Worker(object):
	id = 456

	def name(self):
		return "worker-" + str(self.id)

class Cluster(object):
	_ipc = None

	def __init__(self, ipc):
		self._ipc = ipc

	def job_count(self):
		return self._ipc.job_count()

class Env(object):
	def current_directory(self):
		return "/"

	def cache_directory(self):
		return "cache/"

	def hostname(self):
		return "somehost"

	def login(self):
		return "eve"

class JobInterface(object):
	def setup_once(self):
		pass

	def setup(self):
		pass

	def teardown(self):
		pass

	def teardown_once(self):
		pass

	def run(self):
		raise NotImplementedError("Must overload run()")

class Job(JobInterface):
	env = Env()
	worker = Worker()

	status = JobStatus.spawn
	chains = []

	def inject(self, ipc, id, name, module, quid, partition=0, total_partitions=0, parent=None):
		print("Prepare job at", time.asctime(time.localtime(time.time())))

		self._ipc = ipc()
		self.id = id
		self.name = name
		self.module = module
		self.module = module
		self.quid = quid
		self.partition = partition
		self.total_partitions = total_partitions
		self.parent = parent

		self.cluster = Cluster(self._ipc)

	def update_status(self, status):
		self.status = JobStatus(status)

	def is_spawn(self):
		return self.status == JobStatus.spawn

	def is_partition(self):
		return self.status == JobStatus.partition

	def is_funnel(self):
		return self.status == JobStatus.funnel

	def push_chain(self, chain):
		if not isinstance(chain, Chain):
			raise TypeError("Object must be of ace.Chain")

		self.chains.append(chain)

	def update_progress(self, process):
		if process < 0:
			process = 0
	
		if process > 1000:
			process = 1000
	
		self._ipc.update_progress(process)
