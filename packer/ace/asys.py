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

import sys

out = sys.stdout
err = sys.stderr
ins = sys.stderr

class asys(object):
	def __init__(self, writer):
		self.value = ''
		self.closed = True
		self.writer = out

	def flush(self):
		self.value = ''

	def close(self):
		pass

	def isatty(self):
		return False

	def readline(self):
		return '\0'

	def write(self, txt):
		self.value += txt
		self.writer.write(txt)

sys.stdout = asys(out)
sys.stderr = asys(err)
sys.stdin = asys(ins)
