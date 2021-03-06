##
# Copyright (C) 2015-2016 Mavicona, Quenza Inc.
# All Rights Reserved
#
# This file is part of the Mavicona project.
#
# Content can not be copied and/or distributed without the express
# permission of the author.
#
# TODO: Download job files, not zip
#

import os
import sys
import time
import string
import random
import requests
import datetime
import argparse
import shutil
import simplejson
import signal
import zipfile
import urllib.request
from . import pickledb

default_url = 'http://ecoli.mavicona.net/api/v1/repository'
file_name = 'index.json'

def query_yes_no(question, default="yes"):
	"""
	Ask a yes/no question via raw_input() and return their answer.

	"question" is a string that is presented to the user.
	"default" is the presumed answer if the user just hits <Enter>.
		It must be "yes" (the default), "no" or None (meaning
		an answer is required of the user).

	The "answer" return value is True for "yes" or False for "no".
	"""
	valid = {"yes": True, "y": True, "ye": True, "no": False, "n": False, "ney": False}
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

def signal_handler(signal, frame):
	print()
	sys.exit(0)

def get_local_dir():
	return os.path.expanduser("~") + '/.ecoli'

def id_generator(size=12, chars=string.ascii_uppercase + string.digits):
	return ''.join(random.choice(chars) for _ in range(size))

def package_get_location(search):
	if not os.path.isfile(get_local_dir() + '/' + file_name):
		update_repo()

	with open(get_local_dir() + '/' + file_name) as repo:
		data = simplejson.load(repo)
		for package in data['packages']:
			if search not in package['name'].lower():
					continue

			return package

	return None

def install(install_list, db):
	""" Install package """
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
			reponse = urllib.request.urlopen(download['location'])
			file = open(get_local_dir() + '/tmp/' + download['name'].lower() + '.zip', 'wb')
			print('Fetching %s...' % download['name'], end="")

			block_sz = 8192
			while True:
				buffer = reponse.read(block_sz)
				if not buffer:
					break

				file.write(buffer)
				print('.', end="")

			file.close()

			with zipfile.ZipFile(get_local_dir() + '/tmp/' + download['name'].lower() + '.zip', 'r') as czip:
				czip.extractall(get_local_dir() + '/packages/' + download['name'])

			db.ladd('packages_installed', download)

			print('[done]')

def remove(remove_list, db):
	""" Remove local package """
	for package in remove_list:
		for index,install in enumerate(db.lgetall('packages_installed')):
			if package.lower() == install['name'].lower():
				print('Removing ' + install['name'])

				shutil.rmtree(get_local_dir() + '/packages/' + install['name'])
				db.lpop('packages_installed', index)

def update_repo(db):
	""" Fetch remote package catalog """
	if db.get('last_updated'):
		print('Last update:',datetime.datetime.fromtimestamp(db.get('last_updated'))
			.strftime('%Y-%m-%d %H:%M:%S'))

	pkgdb = pickledb.load(get_local_dir() + '/index.db', True)
	pkgdb.lcreate('package')
	for source in db.lgetall('sources'):
		print('Updating catalog...', end="")
		try:
			data = requests.get(source).json()
		except simplejson.scanner.JSONDecodeError as q:
			print('[invalid]')
			print(q)
			continue

		except requests.exceptions.ConnectionError:
			print('[error]')
			continue

		if data['meta']['repository'] != 'main':
			print('[skip]')
			continue

		for package in data['packages']:
			pkgdb.ladd('package', package)

		for repo in data['repositories']:
			db.ladd('sources', repo)

		print('[done]')

	db.set('last_updated', time.time())

def verify_datadir():
	""" Check all directories exist """

	if not os.path.exists(get_local_dir()):
		os.makedirs(get_local_dir())
		os.makedirs(get_local_dir() + '/data')
		os.makedirs(get_local_dir() + '/packages')
		os.makedirs(get_local_dir() + '/tmp')

def update_packages(db):
	""" Update all local packages """
	if not os.path.isfile(get_local_dir() + '/index.db'):
		update_repo(db)

	for index,local in enumerate(db.lgetall('packages_installed')):
		package = package_get_location(local['name'].lower())
		if not package:
			print('Package not in remote repository')
			return

		if package['name'].lower() == local['name'].lower():
			if (package['build'] > local['build']):
				print('Installing new version')

				db.lpop('packages_installed', index)

				install([package['name'].lower()], db)

def list_repo(db, search=""):
	""" List repositories """
	if not os.path.isfile(get_local_dir() + '/index.db'):
		update_repo(db)

	pkgdb = pickledb.load(get_local_dir() + '/index.db', True)
	for package in pkgdb.lgetall('package'):
		if search:
			if search not in package['name'].lower() and search not in package['description'].lower():
				continue
		
		print("{}\t\t{}".format(package['name'], package['description']))

def list_sources(db):
	for source in db.lgetall('sources'):
		print(source)

def add_sources(sources, db):
	for source in sources:
		db.ladd('sources', source)

def cleanup():
	shutil.rmtree(get_local_dir() + '/tmp')
	os.makedirs(get_local_dir() + '/tmp')
	print('Cleanup...[done]')

def init_config():
	verify_datadir()
	localdb = pickledb.load(get_local_dir() + '/data/local.db', True)

	try:
		localdb.llen('packages_installed')
	except KeyError:
		localdb.lcreate('packages_installed')
		localdb.lcreate('sources')
		localdb.ladd('sources', default_url)

	return localdb

def main(args=sys.argv[1:]):
	""" The main function """
	localdb = init_config()

	parser = argparse.ArgumentParser(description='Processing library manager')

	group = parser.add_argument_group('package actions')
	group.add_argument('-i', action='append', metavar='package', dest='install_package', default=[], help='install package(s)')
	group.add_argument('-d', action='append', metavar='package', dest='remove_package', default=[], help='remove package(s)')
	group.add_argument('-u', action='store_true', dest='run_update', help='update local packages')
	group.add_argument('-s', action='store', metavar='pattern', dest='pattern', help='search for package')

	parser.add_argument('--show-repos', action='store_true', dest='run_show_repos', help='list repositories')
	parser.add_argument('--add-repo', action='append', metavar='source', dest='add_repo', help='add repositories')
	parser.add_argument('--list', action='store_true', dest='run_list', help='show all packages')
	parser.add_argument('--sync', action='store_true', dest='sync_repos', help='update repositories')
	parser.add_argument('--cleanup', action='store_true', dest='run_cleanup', help='remove cached files and packages')
	parser.add_argument('--version', action='version', version='%(prog)s 1.0')
	parser.parse_args()

	signal.signal(signal.SIGINT, signal_handler)

	take_action = False
	results = parser.parse_args()

	# Package actions
	if results.install_package:
		take_action = True
		install(results.install_package, localdb)

	if results.remove_package:
		take_action = True
		remove(results.remove_package, localdb)

	if results.run_update:
		take_action = True
		update_packages(localdb)

	if results.pattern:
		take_action = True
		list_repo(localdb, results.pattern.lower())

	# Optional actions
	if results.run_show_repos:
		take_action = True
		list_sources(localdb)

	if results.add_repo:
		take_action = True
		add_sources(results.add_repo,localdb)

	if results.run_list:
		take_action = True
		list_repo(localdb)

	if results.sync_repos:
		take_action = True
		update_repo(localdb)

	if results.run_cleanup:
		take_action = True
		cleanup()

	if not take_action:
		parser.print_help()

	localdb.dump()

	return 0
