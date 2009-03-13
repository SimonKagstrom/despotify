#!/usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2006 (ita)
# Ralf Habacker, 2006 (rh)

import os, optparse, sys, re
import Configure, Options, Utils
import ccroot, ar
from Configure import conftest


@conftest
def find_gxx(conf):
	v = conf.env
	cxx = None
	if v['CXX']: cxx = v['CXX']
	elif 'CXX' in os.environ: cxx = os.environ['CXX']
	if not cxx: cxx = conf.find_program('g++', var='CXX')
	if not cxx: cxx = conf.find_program('c++', var='CXX')
	if not cxx: conf.fatal('g++ was not found')
	try:
		if Utils.cmd_output('%s --version' % cxx).find('g++') < 0:
			conf.fatal('g++ was not found, see the result of g++ --version')
	except ValueError:
		conf.fatal('g++ --version could not be executed')
	v['CXX']  = cxx
	v['CXX_NAME'] = 'gcc'
	ccroot.get_cc_version(conf, cxx, 'CXX_VERSION')

@conftest
def gxx_common_flags(conf):
	v = conf.env

	# CPPFLAGS CXXDEFINES _CXXINCFLAGS _CXXDEFFLAGS _LIBDIRFLAGS _LIBFLAGS

	v['CXX_SRC_F']           = ''
	v['CXX_TGT_F']           = '-c -o '
	v['CPPPATH_ST']          = '-I%s' # template for adding include paths

	# linker
	if not v['LINK_CXX']: v['LINK_CXX'] = v['CXX']
	v['CXXLNK_SRC_F']        = ''
	v['CXXLNK_TGT_F']        = '-o '

	v['LIB_ST']              = '-l%s' # template for adding libs
	v['LIBPATH_ST']          = '-L%s' # template for adding libpaths
	v['STATICLIB_ST']        = '-l%s'
	v['STATICLIBPATH_ST']    = '-L%s'
	v['RPATH_ST']            = '-Wl,-rpath,%s'
	v['CXXDEFINES_ST']       = '-D%s'

	v['SHLIB_MARKER']        = '-Wl,-Bdynamic'
	v['STATICLIB_MARKER']    = '-Wl,-Bstatic'
	v['FULLSTATIC_MARKER']   = '-static'

	# program
	v['program_PATTERN']     = '%s'

	# shared library
	v['shlib_CXXFLAGS']      = ['-fPIC', '-DPIC']
	v['shlib_LINKFLAGS']     = ['-shared']
	v['shlib_PATTERN']       = 'lib%s.so'

	# static lib
	v['staticlib_LINKFLAGS'] = ['-Wl,-Bstatic']
	v['staticlib_PATTERN']   = 'lib%s.a'

	# osx stuff
	v['LINKFLAGS_MACBUNDLE'] = ['-bundle', '-undefined dynamic_lookup']
	v['CCFLAGS_MACBUNDLE']   = ['-fPIC']
	v['macbundle_PATTERN']   = '%s.bundle'

@conftest
def gxx_modifier_win32(conf):
	if sys.platform != 'win32': return
	v = conf.env
	v['program_PATTERN']     = '%s.exe'

	v['shlib_PATTERN']       = 'lib%s.dll'
	v['shlib_CXXFLAGS']      = ['']

	v['staticlib_LINKFLAGS'] = ['']

@conftest
def gxx_modifier_cygwin(conf):
	if sys.platform != 'cygwin': return
	v = conf.env
	v['program_PATTERN']     = '%s.exe'

	v['shlib_PATTERN']       = 'lib%s.dll'
	v['shlib_CXXFLAGS']      = ['']

	v['staticlib_LINKFLAGS'] = ['']

@conftest
def gxx_modifier_darwin(conf):
	if sys.platform != 'darwin': return
	v = conf.env
	v['shlib_CXXFLAGS']      = ['-fPIC', '-compatibility_version 1', '-current_version 1']
	v['shlib_LINKFLAGS']     = ['-dynamiclib']
	v['shlib_PATTERN']       = 'lib%s.dylib'

	v['staticlib_LINKFLAGS'] = ['']

	v['SHLIB_MARKER']        = ''
	v['STATICLIB_MARKER']    = ''

@conftest
def gxx_modifier_aix5(conf):
	if sys.platform != 'aix5': return
	v = conf.env
	v['program_LINKFLAGS']   = ['-Wl,-brtl']

	v['shlib_LINKFLAGS']     = ['-shared','-Wl,-brtl,-bexpfull']

	v['SHLIB_MARKER']        = ''

detect = '''
find_gxx
find_cpp
find_ar
gxx_common_flags
gxx_modifier_win32
gxx_modifier_cygwin
gxx_modifier_darwin
gxx_modifier_aix5
cxx_load_tools
cxx_add_flags
'''
