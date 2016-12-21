#!/usr/bin/python

try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup

config = {
    'description': 'Processing library manager',
    'author': 'Mavicona, Quenza Inc.',
    'author_email': 'ydw@x3.quenza.net',
    'version': '0.1',
    'install_requires': ['nose'],
    'packages': ['ecoli'],
    'scripts': [],
    'name': 'ecoli'
}

setup(**config)
