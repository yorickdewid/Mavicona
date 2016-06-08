from __future__ import print_function

import os
import sys
import argparse
import urllib2
import pickledb
import simplejson
import zipfile

url = 'http://ecoli.mavicona.net/index.json'
file_name = 'index.json'

def query_yes_no(question, default="yes"):
	"""Ask a yes/no question via raw_input() and return their answer.

	"question" is a string that is presented to the user.
	"default" is the presumed answer if the user just hits <Enter>.
		It must be "yes" (the default), "no" or None (meaning
		an answer is required of the user).

	The "answer" return value is True for "yes" or False for "no".
	"""
	valid = {"yes": True, "y": True, "ye": True, "no": False, "n": False}
	if default is None:
		prompt = " [y/n] "
	elif default == "yes":
		prompt = " [Y/n] "
	elif default == "no":
		prompt = " [y/N] "
	else:
		raise ValueError("invalid default answer: '%s'" % default)

	while True:
		sys.stdout.write(question + prompt)
		choice = raw_input().lower()
		if default is not None and choice == '':
			return valid[default]
		elif choice in valid:
			return valid[choice]
		else:
			sys.stdout.write("Please respond with 'yes' or 'no' (or 'y' or 'n')")

def package_get_location(search):
	if not os.path.isfile('.ecoli/' + file_name):
		update_repo()

	with open('.ecoli/' + file_name) as repo:
		data = simplejson.load(repo)
		for package in data['packages']:
			if search not in package['name'].lower():
					continue

			return package

	return None

def install(install_list, db):
	print('Installing')

	downloader = []
	for package in install_list:
		loc = package_get_location(package)
		print('\t- ', package, '[found]' if loc else '[not found]')
		if loc:
			downloader.append(loc)

	if not downloader:
		print('\nNo packages to install')
		return

	if query_yes_no('\nInstalling %d package(s)?' % len(downloader)):
		print('Downloading...')
		for download in downloader:
			reponse = urllib2.urlopen(download['location'])
			file = open('.ecoli/tmp/' + download['name'].lower() + '.zip', 'wb')
			print('Fetching %s...' % download['name'], end="")

			block_sz = 8192
			while True:
				buffer = reponse.read(block_sz)
				if not buffer:
					break

				file.write(buffer)
				print('.', end="")

			file.close()

			with zipfile.ZipFile('.ecoli/tmp/' + download['name'].lower() + '.zip', 'r') as czip:
				czip.extractall('.ecoli/packages/' + download['name'])

			# push to localdb

			print('[done]')

def remove(package):
	print('Removing ', package)

def update_repo():
	reponse = urllib2.urlopen(url)
	file = open('.ecoli/' + file_name, 'wb')
	print('Updating catalog...', end="")

	block_sz = 8192
	while True:
		buffer = reponse.read(block_sz)
		if not buffer:
			break

		file.write(buffer)
		print('.', end="")

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
	if not os.path.isfile('.ecoli/' + file_name):
		update_repo()

	with open('.ecoli/' + file_name) as repo:
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
	group.add_argument('-i', action='append', metavar='package', dest='install_package', default=[], help='install package(s)')
	group.add_argument('-d', action='append', metavar='package', dest='remove_package', default=[], help='remove package(s)')
	group.add_argument('-u', action='store_true', dest='run_update', help='update local packages')
	group.add_argument('-s', action='store', metavar='pattern', dest='pattern', help='search for package')

	parser.add_argument('--show-repos', action='store_true', dest='run_show_repos', help='list repositories')
	parser.add_argument('--list', action='store_true', dest='run_list', help='show all packages')
	parser.add_argument('--version', action='version', version='%(prog)s 1.0')
	parser.parse_args()

	take_action = False
	results = parser.parse_args()

	if results.install_package:
		take_action = True
		install(results.install_package, localdb)
	
	if results.remove_package:
		take_action = True
		remove(results.remove_package)

	if results.run_update:
		take_action = True
		update_repo()
		update_packages()

	#if results.run_show_repos:
	#	take_action = True
	#	update_repo()

	if results.pattern:
		take_action = True
		list_repo(results.pattern.lower())

	if results.run_list:
		take_action = True
		list_repo()

	if not take_action:
		print('No actions requested, see --help')

	localdb.dump()

	return 0
