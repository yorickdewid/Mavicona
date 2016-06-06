from __future__ import print_function

import os
import sys
import argparse
import urllib2
import pickledb
import simplejson

url = 'http://ecoli.mavicona.net/index.json'
file_name = 'index.json'

def install(package):
	print('Installing ', package)

def remove(package):
	print('Removing ', package)

def update_repo():
	reponse = urllib2.urlopen(url)
	file = open('.ecoli/' + file_name, 'wb')
	meta = reponse.info()
	print('Updating catalog... ', end="")

	block_sz = 8192
	while True:
		buffer = reponse.read(block_sz)
		if not buffer:
			break

		file.write(buffer)

	print('[done]')

	file.close()

def verify_datadir():
	if not os.path.exists('.ecoli'):
		os.makedirs('.ecoli')
		os.makedirs('.ecoli/data')
		os.makedirs('.ecoli/packages')
		os.makedirs('.ecoli/tmp')	

def update_packages():
	return

def list_repo(search=""):
	with open('.ecoli/index.json') as repo:
		data = simplejson.load(repo)
		for package in data['packages']:
			if search:
				if search not in package['name'].lower():
					continue
			
			print("{}\t\t{}".format(package['name'], package['description']))

def main(args=sys.argv[1:]):
	"""
	The main function.
	"""

	verify_datadir()
	localdb = pickledb.load('.ecoli/data/local.db', True)

	parser = argparse.ArgumentParser(description='Processing library manager')

	group = parser.add_argument_group('package actions')
	group.add_argument('-i', action='append', metavar='package', dest='install_package', default=[], help='Install package(s)')
	group.add_argument('-d', action='append', metavar='package', dest='remove_package', default=[], help='Remove package(s)')
	group.add_argument('-u', action='store_true', dest='run_update', help='Update local packages')
	group.add_argument('-s', action='store', metavar='pattern', dest='pattern', help='Search for package')

	parser.add_argument('--self-update', action='store_true', dest='run_self_update', help='Update local repository')
	parser.add_argument('--list', action='store_true', dest='run_list', help='Show all packages')
	parser.add_argument('--version', action='version', version='%(prog)s 1.0')
	parser.parse_args()

	results = parser.parse_args()

	if results.install_package:
		install(results.install_package)
	
	if results.remove_package:
		remove(results.remove_package)

	if results.run_update:
		update_repo()
		update_packages()

	if results.run_self_update:
		update_repo()

	if results.pattern:
		list_repo(results.pattern.lower())

	if results.run_list:
		list_repo()

	localdb.dump()

	return 0
