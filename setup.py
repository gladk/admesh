from distutils.core import Extension, setup

f = open('version.h')
version_define = f.readline()
f.close()
version = version_define.split()[-1]

admesh_ext = Extension(
    '_admesh',
    sources=['admesh.i', 'connect.c', 'stl_io.c', 'stlinit.c', 'util.c', 'normals.c', 'shared.c'])

setup(
    name='admesh',
    ext_modules=[admesh_ext],
    py_modules=['admesh'],
    version=version,
    platforms=['x86_64'],
    description='Wrapper for admesh C library',
    long_description='This library is just a SWIG wrapper around admesh C library.',
    license='GPLv2',
    author='Anthony D. Martin',
    author_email='amartin@engr.csulb.edu',
    maintainer='Vladimir Sapronov',
    maintainer_email='vladimir.sapronov@gmail.com',
    url='https://github.com/vsapronov/admesh')
