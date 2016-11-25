##
# Copyright (C) 2015-2016 Mavicona, Quenza Inc.
# All Rights Reserved
#
# This file is part of the Mavicona project.
#
# Content can not be copied and/or distributed without the express
# permission of the author.
#
# This module is injected by the worker and does not need to be
# included in the package
#

import random

class Callback(object):
	def update_progress(self, progress):
		pass
	
	def job_count(self):
		return random.randint(0,10000)
