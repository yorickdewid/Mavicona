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
import random
import uuid
import ace.config
import ace.ipc
import ace.sha1
import job_example

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

ins = job_example.job_init(ace.config.Config())
obj = ins.invoke()
obj.inject(ace.ipc.Callback,
	random.randint(1, 10000),
	random.choice(jobnames),
	ace.sha1.sha1(bytearray(os.urandom(10))),
	uuid.uuid4())

# obj.update_status(1) # 0,1,2

obj.setup_once()
obj.setup()
obj.run(jobdata)
obj.teardown()
obj.teardown_once()

print('Chains', len(obj.chains))
for chain in obj.chains:
	for job in chain.subjobs:
		print('> Subjob', job['name'])
