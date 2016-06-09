

from distutils.core import setup, Extension

module1=Extension('upscaledb', 
      libraries=['upscaledb'],
      include_dirs=['../include'],
      library_dirs=['../src/.libs'],
      sources=['src/python.cc'])

setup(name='upscaledb-python', 
      version='2.1.11',
      author='Christoph Rupp',
      author_email='chris@crupp.de',
      url='http://upscaledb.com',
      description='The Python wrapper for upscaledb',
      license='GNU Public License 3.0',
      ext_modules=[module1])
