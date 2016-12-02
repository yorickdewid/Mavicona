import ace.job

""" Package info """
def package():
	return {
		'name'			: 'Simple',
		'description'	: 'Most basic job',
		'author' 		: 'Yorick de Wid',
		'license' 		: 'GPL',
		'url' 			: 'http://x3.mavicona.net/',
		'keywords' 		: ['example', 'basic'],
	}

class Simple(ace.job.Job):
	def run(self, data):
		""" Run the job content """
		print("Hello cluster")

""" Declare job properties """
def job_init(cfg):
	cfg.invoke = Simple
	return cfg
