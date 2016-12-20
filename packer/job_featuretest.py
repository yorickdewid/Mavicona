##
# Copyright (C) 2015-2016 Mavicona, Quenza Inc.
# All Rights Reserved
#
# This file is part of the Mavicona project.
#
# Content can not be copied and/or distributed without the express
# permission of the author.
#
# This is an example job covering most of the options available
# for use.
#

import os
import time
import pprint
import pathlib
import ace.job

""" Package info """
def package():
	return {
		'name'			: 'FeatureTest',
		'description'	: 'Utilize all features in the Ace framework',
		'author' 		: 'Yorick de Wid',
		'license' 		: 'MIT',
		'url' 			: 'http://x3.mavicona.net/',
		'keywords' 		: ['feature', 'test', 'introduction'],
		'checksum' 		: 'abaf2abe1e8185f3b17c4d0c34dbeed2b4b9d2ad9232b195378d8ced6c519270',
		'settings'		: {
			'no-cache'	: False,
			'sticky'	: True,
		}
	}

class Example(ace.job.Job):
	""" Example job class """

	def __init__(self):
		pass

	def setup_once(self):
		""" Do some early initialization once """
		print("First setup")
		time.sleep(2)

		if ace.job.PackageHelper().has_meta():
			pprint.pprint(ace.job.PackageHelper().meta)

		print('Operation mode', self.cfg.mode)
		print('Max job runtime', self.cfg.max_runtime)
		print('Retry count', self.cfg.retry)

	def setup(self):
		""" This routine is run for every node """
		print('')

		""" Print job info """
		print("Job id", self.id);
		print("Job name", self.name);
		print("Job quid", self.quid);
		print("Job module", self.module);
		print("Job partition", self.partition);
		print("Job total partitions", self.total_partitions);
		print("Job status", self.status);
		print("Job parent", self.parent);

		""" Print environment """
		print("Working dir", self.env.current_directory())
		print("Hostname", self.env.hostname())
		print("Login", self.env.login())

		""" Print cluster info """
		print("Worker", self.worker.id())
		print("Worker", self.worker.name())
		print("Jobs in cluster", self.cluster.job_count())

		print('')

		""" Print environment injected variables """
		print('Database host', os.environ.get('PG_HOST'))
		print('Database user', os.environ.get('PG_USER'))
		print('Database pasword', os.environ.get('PG_PASSWD'))

		print('')

		self.db.put('start_time', time.time())

		if self.is_spawn():
			print("Spawn child jobs")

			chain = ace.job.Chain(self.quid)
			chain.add(Example, self.name + '_1')
			chain.add(Example, self.name + '_2')

			self.push_chain(chain)

		time.sleep(2)

	def run(self, data):
		""" Run the job content """
		time.sleep(1)
		self.update_progress(150)
		time.sleep(1)
		self.update_progress(450)
		time.sleep(1)
		print("Running, data size", len(data), "bytes")
		time.sleep(1)
		self.update_progress(750)
		time.sleep(1)
		self.update_progress(1000)
		time.sleep(2)

		if self.is_partition():
			mydata = pathlib.Path("mydata.txt")
			if mydata.is_file():
				print("Parse my data file")

		if self.is_spawn():
			self.chains[0].subjobs[0]['data'] = data
			self.chains[0].subjobs[1]['data'] = 'additional data'

	def teardown(self):
		""" This routine is run for every node """
		print("Teardown")

		print('Start time was', self.db.get('start_time'))

		time.sleep(2)

	def teardown_once(self):
		""" Close handles, release resources """
		print("Teardown last")

		time.sleep(2)

""" Declare job properties """
def job_init(cfg):
	cfg.invoke = Example
	cfg.mode = ace.job.JobModel.fan
	cfg.max_runtime = 60
	cfg.retry = 1
	return cfg
