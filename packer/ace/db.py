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

class DB(object):
	def __init__(self):
		self.store = {}

	def put(self, key, value):
		self.store[key] = value
	
	def get(self, key):
		return self.store[key]

	def delete(self, key):
		del self.store[key]
