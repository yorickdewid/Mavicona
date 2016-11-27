import os
import ace.job

""" Package info """
def package():
	return {
		'name'			: 'Random',
		'description'	: 'Generate random data',
		'author' 		: 'Yorick de Wid',
		'license' 		: 'MIT',
		'url' 			: 'http://x3.quenza.net/',
		'keywords' 		: ['random', 'test'],
		'checksum' 		: 'abaf2abe1e8185f3b17c4d0c34dbeed2b4b9d2ad9232b195378d8ced6c519270',
		'settings'		: {
			'no-cache': False
		}
	}

class Example(ace.job.Job):
	""" Example job class """

	def __init__(self):
		pass

	def setup_once(self):
		""" Do some early initialization once """
		print("First setup")

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

		# for key in os.environ.keys():
			# print("%30s = %s" % (key, os.environ[key]))

		if self.is_spawn():
			print("Spawn child jobs")

			chain = ace.job.Chain(self.quid)
			chain.add(Example, self.name + '_1')
			chain.add(Example, self.name + '_2')

			self.push_chain(chain)

	def run(self, data):
		""" Run the job content """
		self.update_progress(50)
		print("Running, data size", len(data), "bytes")
		self.update_progress(500)

	def teardown(self):
		""" This routine is run for every node """
		print("Teardown")

	def teardown_once(self):
		""" Close handles, release resources """
		print("Teardown last")

""" Declare job properties """
def job_init(cfg):
	cfg.invoke = Example
	cfg.mode = ace.job.JobModel.fan
	cfg.max_runtime = 60
	cfg.retry = 1
	return cfg
