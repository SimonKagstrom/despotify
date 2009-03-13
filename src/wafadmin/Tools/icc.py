#!/usr/bin/env python
# encoding: utf-8
# Stian Selnes, 2008

import os, sys
import Configure, Options, Utils
import ccroot, ar
from Configure import conftest

@conftest
def find_icc(conf):
	if sys.platform == 'win32':
		conf.find_icc_win32()
	else:
		conf.find_icc_other()

@conftest
def find_icc_win32(conf):
	v = conf.env
	cc = None
	if v['CC']: cc = v['CC']
	elif 'CC' in os.environ: cc = os.environ['CC']
	if not cc: cc = conf.find_program('ICL', var='CC')
	if not cc: conf.fatal('Intel C Compiler (icl.exe) was not found')
	v['CC'] = Utils.quote_whitespace(cc)
	v['CC_NAME'] = 'icc'
	# msvc tool overrides the CC variable with CXX. workaround...
	v['CXX'] = v['CC']

@conftest
def find_icc_other(conf):
	v = conf.env
	cc = None
	if v['CC']: cc = v['CC']
	elif 'CC' in os.environ: cc = os.environ['CC']
	if not cc: cc = conf.find_program('icc', var='CC')
	if not cc: conf.fatal('Intel C Compiler (icc) was not found')
	try:
		if Utils.cmd_output('%s --version' % cc).find('icc') < 0:
			conf.fatal('icc was not found, see the result of icc --version')
	except ValueError:
		conf.fatal('icc --version could not be executed')
	v['CC'] = cc
	v['CC_NAME'] = 'icc'
	ccroot.get_cc_version(conf, cc, 'CC_VERSION')

@conftest
def icc_modifier_win32(conf):
	if sys.platform != 'win32': return
	v = conf.env
	try:
		# Clean up after msvc tool. Hackish!
		v['CC_NAME'] = 'icc'
		# Remove flags set by msvc tool that are incompatible with icl
		v['CPPFLAGS'].remove('/errorReport:prompt')
	except ValueError:
		pass

if sys.platform == 'win32':
	detect = '''
find_icc
find_msvc
msvc_common_flags
icc_modifier_win32
cc_load_tools
cc_add_flags
'''
elif sys.platform == 'cygwin':
	# Intel C Compiler and cygwin is a strange combination
	conf.fatal ("ICC module is not supported for cygwin. Try under native Win32")
else:
	detect = '''
find_icc
find_ar
gcc_common_flags
gcc_modifier_darwin
cc_load_tools
cc_add_flags
'''

