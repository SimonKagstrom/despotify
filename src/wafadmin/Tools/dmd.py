#!/usr/bin/env python
# encoding: utf-8
# Carlos Rafael Giani, 2007 (dv)
# Thomas Nagy, 2008 (ita)

import sys
import ar

def find_dmd(conf):
	v = conf.env
	d_compiler = None
	if v['D_COMPILER']:
		d_compiler = v['D_COMPILER']
	if not d_compiler: d_compiler = conf.find_program('dmd', var='D_COMPILER')
	if not d_compiler: return 0
	v['D_COMPILER'] = d_compiler

def common_flags(conf):
	v = conf.env

	# _DFLAGS _DIMPORTFLAGS _DLIBDIRFLAGS _DLIBFLAGS

	# Compiler is dmd so 'gdc' part will be ignored, just
	# ensure key is there, so wscript can append flags to it
	v['DFLAGS']            = {'gdc': [], 'dmd': ['-version=Posix']}

	v['D_SRC_F']           = ''
	v['D_TGT_F']           = '-c -of'
	v['DPATH_ST']          = '-I%s' # template for adding import paths

	# linker
	v['D_LINKER']          = v['D_COMPILER']
	v['DLNK_SRC_F']        = ''
	v['DLNK_TGT_F']        = '-of'

	v['DLIB_ST']           = '-L-l%s' # template for adding libs
	v['DLIBPATH_ST']       = '-L-L%s' # template for adding libpaths

	# linker debug levels
	v['DFLAGS_OPTIMIZED']  = ['-O']
	v['DFLAGS_DEBUG']      = ['-g', '-debug']
	v['DFLAGS_ULTRADEBUG'] = ['-g', '-debug']
	v['DLINKFLAGS']        = ['-quiet']

	v['D_shlib_DFLAGS']    = ['-fPIC']
	v['D_shlib_LINKFLAGS'] = ['-L-shared']

	v['DHEADER_ext']       = '.di'
	v['D_HDR_F']           = '-H -Hf'

	if sys.platform == "win32":
		v['D_program_PATTERN']   = '%s.exe'
		v['D_shlib_PATTERN']     = 'lib%s.dll'
		v['D_staticlib_PATTERN'] = 'lib%s.a'
	else:
		v['D_program_PATTERN']   = '%s'
		v['D_shlib_PATTERN']     = 'lib%s.so'
		v['D_staticlib_PATTERN'] = 'lib%s.a'

def detect(conf):
	v = conf.env
	find_dmd(conf)
	ar.find_ar(conf)
	conf.check_tool('d')
	common_flags(conf)

def set_options(opt):
	pass
