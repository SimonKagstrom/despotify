#!/usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2005-2008 (ita)

"base for all c/c++ programs and libraries"

import os, sys, re
import TaskGen, Utils, preproc, Logs, Build, Options
from Logs import error, debug, warn
from Utils import md5
from TaskGen import taskgen, after, before, feature
from Constants import *

import config_c # <- necessary for the configuration, do not touch

get_version_re = re.compile('\d+\.\d+(\.?\d+)*')
def get_cc_version(conf, cc, version_var):
	v = conf.env
	output = Utils.cmd_output('%s -dumpversion' % cc)
	if output:
		match = get_version_re.search(output)
		if match:
			v[version_var] = match.group(0)
			conf.check_message('compiler', 'version', 1, v[version_var])
			return v[version_var]
	warn('could not determine the compiler version')

class DEBUG_LEVELS:
	ULTRADEBUG = "ultradebug"
	DEBUG = "debug"
	RELEASE = "release"
	OPTIMIZED = "optimized"
	CUSTOM = "custom"

	ALL = [ULTRADEBUG, DEBUG, RELEASE, OPTIMIZED, CUSTOM]

def scan(self):
	"look for .h the .cpp need"
	debug('ccroot: _scan_preprocessor(self, node, env, path_lst)')
	all_nodes = []
	all_names = []
	seen = []
	for node in self.inputs:
		gruik = preproc.c_parser(nodepaths = self.env['INC_PATHS'], defines = self.defines)
		gruik.start(node, self.env)
		if Logs.verbose:
			debug('deps: deps for %s: %s; unresolved %s' % (str(node), str(gruik.nodes), str(gruik.names)))
		for x in gruik.nodes:
			if id(x) in seen: continue
			seen.append(id(x))
			all_nodes.append(x)
		# TODO: use a set ?
		for x in gruik.names:
			if not x in all_names:
				all_names.append(x)
	return (all_nodes, all_names)

class ccroot_abstract(TaskGen.task_gen):
	"Parent class for programs and libraries in languages c, c++ and moc (Qt)"
	def __init__(self, *k, **kw):
		TaskGen.task_gen.__init__(self, *k, **kw)

		# COMPAT
		if len(k) > 1:
			self.features.append('c' + k[1])

@feature('cc', 'cxx')
@before('init_cc', 'init_cxx')
def default_cc(self):
	Utils.def_attrs(self,
		includes='',
		defines='',
		rpaths='',
		uselib='',
		uselib_local='',
		add_objects='',
		p_flag_vars=[],
		p_type_vars=[],
		scanner_defines={},
		compiled_tasks=[],
		link_task=None)

def get_target_name(self):
	tp = 'program'
	for x in self.features:
		if x in ['cshlib', 'cstaticlib']:
			tp = x.lstrip('c')

	pattern = self.env[tp + '_PATTERN']
	if not pattern: pattern = '%s'

	dir, name = os.path.split(self.target)
	return os.path.join(dir, pattern % name)

@feature('cprogram', 'dprogram', 'cstaticlib', 'dstaticlib', 'cshlib', 'dshlib')
def apply_verif(self):
	if not self.source and not self.add_objects:
		raise Utils.WafError('no source files specified for %s' % self)
	if not self.target:
		raise Utils.WafError('no target for %s' % self)

def install_shlib(self):
	nums = self.vnum.split('.')

	path = self.install_path
	if not path: return
	libname = self.outputs[0].name

	name3 = libname + '.' + self.vnum
	name2 = libname + '.' + nums[0]
	name1 = libname

	filename = self.outputs[0].abspath(self.env)
	bld = Build.bld
	bld.install_as(os.path.join(path, name3), filename, env=self.env)
	bld.symlink_as(os.path.join(path, name2), name3)
	bld.symlink_as(os.path.join(path, name1), name3)

# TODO reference the d programs, shlibs in d.py, not here

@feature('cprogram', 'dprogram')
@before('apply_core')
def vars_target_cprogram(self):
	self.default_install_path = '${PREFIX}/bin'
	self.default_chmod = O755

@feature('cstaticlib', 'dstaticlib', 'cshlib', 'dshlib')
@before('apply_core')
def vars_target_cstaticlib(self):
	self.default_install_path = '${PREFIX}/lib'
	if sys.platform in ['win32', 'cygwin']:
		# on win32, libraries need the execute bit, else we
		# get 'permission denied' when using them (issue 283)
		self.default_chmod = O755

@feature('cprogram', 'dprogram', 'cstaticlib', 'dstaticlib', 'cshlib', 'dshlib')
@after('apply_objdeps', 'apply_link')
def install_target_cstaticlib(self):
	if not Options.is_install: return
	self.link_task.install_path = self.install_path

@feature('cshlib', 'dshlib')
@after('apply_objdeps', 'apply_link')
def install_target_cshlib(self):
	if getattr(self, 'vnum', '') and sys.platform != 'win32':
		tsk = self.link_task
		tsk.vnum = self.vnum
		tsk.install = install_shlib

@feature('cc', 'cxx')
@after('apply_type_vars')
def apply_incpaths(self):
	"used by the scanner"
	lst = []
	for lib in self.to_list(self.uselib):
		for path in self.env['CPPPATH_' + lib]:
			if not path in lst:
				lst.append(path)
	if preproc.go_absolute:
		for path in preproc.standard_includes:
			if not path in lst:
				lst.append(path)

	for path in self.to_list(self.includes):
		if not path in lst:
			if preproc.go_absolute or not os.path.isabs(path):
				lst.append(path)
			else:
				self.env.prepend_value('CPPPATH', path)

	for path in lst:
		node = None
		if os.path.isabs(path):
			if preproc.go_absolute:
				node = self.bld.root.find_dir(path)
		else:
			node = self.path.find_dir(path)

		if node:
			self.env.append_value('INC_PATHS', node)

	# TODO we will need to review this
	self.env.append_value('INC_PATHS', self.bld.srcnode)

@feature('cc', 'cxx')
def apply_type_vars(self):
	for x in self.features:
		if not x in ['cprogram', 'cstaticlib', 'cshlib']:
			continue
		x = x.lstrip('c')

		# if the type defines uselib to add, add them
		st = self.env[x + '_USELIB']
		if st: self.uselib = self.uselib + ' ' + st

		# each compiler defines variables like 'shlib_CXXFLAGS', 'shlib_LINKFLAGS', etc
		# so when we make a task generator of the type shlib, CXXFLAGS are modified accordingly
		for var in self.p_type_vars:
			compvar = '%s_%s' % (x, var)
			#print compvar
			value = self.env[compvar]
			if value: self.env.append_value(var, value)

@taskgen
@feature('cprogram', 'cshlib', 'cstaticlib')
@after('apply_core')
def apply_link(self):
	# use a custom linker if specified (self.link='name-of-custom-link-task')
	link = getattr(self, 'link', None)
	if not link:
		if 'cstaticlib' in self.features: link = 'ar_link_static'
		elif 'cxx' in self.features: link = 'cxx_link'
		else: link = 'cc_link'
		# that's something quite ugly for unix platforms
		# both the .so and .so.x must be present in the build dir
		# for darwin the version number is forcibly undefined for a lack of specs
		if getattr(self, 'vnum', None):
			if sys.platform == 'darwin' or sys.platform == 'win32':
				self.vnum = ''
			else:
				link = 'vnum_' + link
	linktask = self.create_task(link)
	outputs = [t.outputs[0] for t in self.compiled_tasks]
	linktask.set_inputs(outputs)
	linktask.set_outputs(self.path.find_or_declare(get_target_name(self)))
	linktask.chmod = self.chmod

	self.link_task = linktask

@feature('cc', 'cxx')
@after('apply_vnum')
def apply_lib_vars(self):
	env = self.env

	# 1. the case of the libs defined in the project (visit ancestors first)
	# the ancestors external libraries (uselib) will be prepended
	uselib = self.to_list(self.uselib)
	seen = []
	names = self.to_list(self.uselib_local)[:] # consume a copy of the list of names
	while names:
		x = names.pop(0)
		# visit dependencies only once
		if x in seen:
			continue

		y = self.name_to_obj(x)
		if not y:
			raise Utils.WafError("object '%s' was not found in uselib_local (required by '%s')" % (x, self.name))

		# object has ancestors to process: add them to the end of the list
		if getattr(y, 'uselib_local', None):
			lst = y.to_list(y.uselib_local)
			for u in lst:
				if not u in seen:
					names.append(u)

		# safe to process the current object
		y.post()
		seen.append(x)

		# some linkers can link against programs
		if 'cshlib' in y.features or 'cprogram' in y.features:
			env.append_value('LIB', y.target)
		elif 'cstaticlib' in y.features:
			env.append_value('STATICLIB', y.target)

		# add the link path too
		tmp_path = y.path.bldpath(self.env)
		if not tmp_path in env['LIBPATH']: env.prepend_value('LIBPATH', tmp_path)

		# set the dependency over the link task
		if y.link_task is not None:
			self.link_task.set_run_after(y.link_task)
			dep_nodes = getattr(self.link_task, 'dep_nodes', [])
			self.link_task.dep_nodes = dep_nodes + y.link_task.outputs

		# add ancestors uselib too
		morelibs = y.to_list(y.uselib)
		for v in morelibs:
			if v in uselib: continue
			uselib = [v]+uselib

		# if the library task generator provides 'export_incdirs', add to the include path
		# the export_incdirs must be a list of paths relative to the other library
		if getattr(y, 'export_incdirs', None):
			cpppath_st = self.env['CPPPATH_ST']
			for x in self.to_list(y.export_incdirs):
				node = y.path.find_dir(x)
				if not node:
					raise Utils.WafError('object %s: invalid folder %s in export_incdirs' % (y.target, x))
				self.env.append_unique('INC_PATHS', node)

	# 2. the case of the libs defined outside
	for x in uselib:
		for v in self.p_flag_vars:
			val = self.env[v+'_'+x]
			if val: self.env.append_value(v, val)

@feature('cprogram', 'cstaticlib', 'cshlib')
@after('apply_obj_vars', 'apply_vnum')
def apply_objdeps(self):
	"add the .o files produced by some other object files in the same manner as uselib_local"
	if not getattr(self, 'add_objects', None): return

	seen = []
	names = self.to_list(self.add_objects)
	while names:
		x = names[0]

		# visit dependencies only once
		if x in seen:
			names = names[1:]
			continue

		# object does not exist ?
		y = self.name_to_obj(x)
		if not y:
			raise Utils.WafError("object '%s' was not found in uselib_local (required by add_objects '%s')" % (x, self.name))

		# object has ancestors to process first ? update the list of names
		if y.add_objects:
			added = 0
			lst = y.to_list(y.add_objects)
			lst.reverse()
			for u in lst:
				if u in seen: continue
				added = 1
				names = [u]+names
			if added: continue # list of names modified, loop

		# safe to process the current object
		y.post()
		seen.append(x)

		for t in y.compiled_tasks:
			self.link_task.inputs.extend(t.outputs)

@feature('cprogram', 'cshlib', 'cstaticlib')
@after('apply_lib_vars')
def apply_obj_vars(self):
	v = self.env
	lib_st           = v['LIB_ST']
	staticlib_st     = v['STATICLIB_ST']
	libpath_st       = v['LIBPATH_ST']
	staticlibpath_st = v['STATICLIBPATH_ST']
	rpath_st         = v['RPATH_ST']

	app = v.append_unique

	if v['FULLSTATIC']:
		v.append_value('LINKFLAGS', v['FULLSTATIC_MARKER'])

	for i in v['RPATH']:
		if i and rpath_st:
			app('LINKFLAGS', rpath_st % i)

	for i in v['LIBPATH']:
		app('LINKFLAGS', libpath_st % i)
		app('LINKFLAGS', staticlibpath_st % i)

	if v['STATICLIB']:
		v.append_value('LINKFLAGS', v['STATICLIB_MARKER'])
		k = [(staticlib_st % i) for i in v['STATICLIB']]
		app('LINKFLAGS', k)

	# fully static binaries ?
	if not v['FULLSTATIC']:
		if v['STATICLIB'] or v['LIB']:
			v.append_value('LINKFLAGS', v['SHLIB_MARKER'])

	app('LINKFLAGS', [lib_st % i for i in v['LIB']])

@feature('cprogram', 'cshlib', 'cstaticlib')
@after('apply_link')
def apply_vnum(self):
	"use self.vnum and self.soname to modify the command line (un*x)"
	# this is very unix-specific
	if sys.platform != 'darwin' and sys.platform != 'win32':
		try:
			nums = self.vnum.split('.')
		except AttributeError:
			pass
		else:
			try: name3 = self.soname
			except AttributeError: name3 = self.link_task.outputs[0].name + '.' + nums[0]
			self.link_task.outputs.append(self.link_task.outputs[0].parent.find_or_declare(name3))
			self.env.append_value('LINKFLAGS', '-Wl,-h,'+name3)

@taskgen
@after('apply_link')
def process_obj_files(self):
	if not hasattr(self, 'obj_files'): return
	for x in self.obj_files:
		node = self.path.find_resource(x)
		self.link_task.inputs.append(node)

@taskgen
def add_obj_file(self, file):
	"""Small example on how to link object files as if they were source
	obj = bld.create_obj('cc')
	obj.add_obj_file('foo.o')"""
	if not hasattr(self, 'obj_files'): self.obj_files = []
	if not 'process_obj_files' in self.meths: self.meths.append('process_obj_files')
	self.obj_files.append(file)

c_attrs = {
'cxxflag' : 'CXXFLAGS',
'cflag' : 'CCFLAGS',
'ccflag' : 'CCFLAGS',
'linkflag' : 'LINKFLAGS',
'ldflag' : 'LINKFLAGS',
'lib' : 'LIB',
'libpath' : 'LIBPATH',
'staticlib': 'STATICLIB',
'staticlibpath': 'STATICLIBPATH',
'rpath' : 'RPATH',
}

@taskgen
@feature('cc', 'cxx')
@before('init_cxx', 'init_cc')
def add_extra_flags(self):
	"case and plural insensitive"
	for x in self.__dict__.keys():
		y = x.lower()
		if y[-1] == 's':
			y = y[:-1]
		if c_attrs.get(y, None):
			self.env.append_unique(c_attrs[y], getattr(self, x))

