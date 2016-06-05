#!/usr/bin/python

try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup

config = {
    'description': 'Processing library manager',
    'author': 'Yorick de Wid',
    #'url': 'URL to get it at.',
    #'download_url': 'Where to download it.',
    'author_email': 'yorick17@outlook.com',
    'version': '0.1',
    'install_requires': ['nose'],
    'packages': ['ecoli'],
    'scripts': [],
    'name': 'ecoli'
}

setup(**config)