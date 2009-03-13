#!/usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2006-2008 (ita)
# Ralf Habacker, 2006 (rh)

"ar and ranlib"

import os, sys
import Task
from Configure import conftest

ar_str = '${AR} ${ARFLAGS} ${TGT} ${SRC} && ${RANLIB} ${RANLIBFLAGS} ${TGT}'

# FIXME
if sys.platform == "win32":
	ar_str = '${AR} s${ARFLAGS} ${TGT} ${SRC}'
cls = Task.simple_task_type('ar_link_static', ar_str, color='YELLOW', ext_in='.o')
cls.maxjobs = 1

def detect(conf):
	comp = conf.find_program('ar', var='AR')
	if not comp: return

	ranlib = conf.find_program('ranlib', var='RANLIB')
	if not ranlib: return

	v = conf.env
	v['AR']          = comp
	v['ARFLAGS']     = 'rc'
	v['RANLIB']      = ranlib
	v['RANLIBFLAGS'] = ''

@conftest
def find_ar(conf):
	v = conf.env
	conf.check_tool('ar')
	if not v['AR']: conf.fatal('ar is required for static libraries - not found')

@conftest
def find_cpp(conf):
	v = conf.env
	cpp = None
	if v['CPP']: cpp = v['CPP']
	elif 'CPP' in os.environ: cpp = os.environ['CPP']
	if not cpp: cpp = conf.find_program('cpp', var='CPP')
	if not cpp: cpp = v['CC']
	if not cpp: cpp = v['CXX']
	v['CPP'] = cpp

@conftest
def cc_add_flags(conf):
	conf.add_os_flags('CFLAGS', 'CCFLAGS')
	conf.add_os_flags('CPPFLAGS')
	conf.add_os_flags('LINKFLAGS')

@conftest
def cxx_add_flags(conf):
	conf.add_os_flags('CXXFLAGS')
	conf.add_os_flags('CPPFLAGS')
	conf.add_os_flags('LINKFLAGS')

@conftest
def cc_load_tools(conf):
	conf.check_tool('cc')

@conftest
def cxx_load_tools(conf):
	conf.check_tool('cxx')
