#!/usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2007 (ita)
# Gustavo Carneiro (gjc), 2007

"Python support"

import os, sys
import TaskGen, Utils, Utils, Runner, Options, Build
from Logs import debug, warn
from TaskGen import extension, taskgen, before, after, feature
from Configure import conf
import pproc

EXT_PY = ['.py']

@taskgen
@before('apply_incpaths')
@feature('pyext')
@before('apply_bundle')
def init_pyext(self):
	self.default_install_path = '${PYTHONDIR}'
	self.uselib = self.to_list(getattr(self, 'uselib', ''))
	if not 'PYEXT' in self.uselib:
		self.uselib.append('PYEXT')
	self.env['MACBUNDLE'] = True

@taskgen
@before('apply_link')
@before('apply_lib_vars')
@after('apply_bundle')
@feature('pyext')
def pyext_shlib_ext(self):
	# override shlib_PATTERN set by the osx module
	self.env['shlib_PATTERN'] = self.env['pyext_PATTERN']


@taskgen
@before('apply_incpaths')
@feature('pyembed')
def init_pyembed(self):
	self.uselib = self.to_list(getattr(self, 'uselib', ''))
	if not 'PYEMBED' in self.uselib:
		self.uselib.append('PYEMBED')

TaskGen.bind_feature('py', ['apply_core'])

@extension(EXT_PY)
def process_py(self, node):
	if Options.is_install and self.install_path:
		if not hasattr(self, '_py_installed_files'):
			self._py_installed_files = []
		installed_files = Build.bld.install_files(
					self.install_path,
					node.abspath(self.env),
					self.env,
					self.chmod)
		self._py_installed_files.extend(installed_files)

@feature('py')
@after('install')
def byte_compile_py(self):
	if Options.is_install and self.install_path:
		installed_files = self._py_installed_files
		if not installed_files:
			return
		if Options.commands['uninstall']:
			print "* removing byte compiled python files"
			for fname in installed_files:
				try:
					os.remove(fname + 'c')
				except OSError:
					pass
				try:
					os.remove(fname + 'o')
				except OSError:
					pass

		if Options.commands['install']:
			if self.env['PYC'] or self.env['PYO']:
				print "* byte compiling python files"

			if self.env['PYC']:
				program = ("""
import sys, py_compile
for pyfile in sys.argv[1:]:
	py_compile.compile(pyfile, pyfile + 'c')
""")
				argv = [self.env['PYTHON'], "-c", program ]
				argv.extend(installed_files)
				retval = pproc.Popen(argv).wait()
				if retval:
					raise Utils.WafError("bytecode compilation failed")


			if self.env['PYO']:
				program = ("""
import sys, py_compile
for pyfile in sys.argv[1:]:
	py_compile.compile(pyfile, pyfile + 'o')
""")
				argv = [self.env['PYTHON'], self.env['PYFLAGS_OPT'], "-c", program ]
				argv.extend(installed_files)
				retval = pproc.Popen(argv).wait()
				if retval:
					raise Utils.WafError("bytecode compilation failed")

# COMPAT
class py_taskgen(TaskGen.task_gen):
	def __init__(self, *k, **kw):
		TaskGen.task_gen.__init__(self, *k, **kw)

@taskgen
@before('apply_core')
@after('vars_target_cprogram')
@after('vars_target_cstaticlib')
@feature('py')
def init_py(self):
	self.default_install_path = '${PYTHONDIR}'

def _get_python_variables(python_exe, variables, imports=['import sys']):
	"""Run a python interpreter and print some variables"""
	program = list(imports)
	program.append('')
	for v in variables:
		program.append("print repr(%s)" % v)
	proc = pproc.Popen([python_exe, "-c", '\n'.join(program)], stdout=pproc.PIPE)
	output = proc.communicate()[0].split("\n")
	if proc.returncode:
		if Logs.verbose:
			warn("Python program to extract python configuration variables failed:\n%s"
				       % '\n'.join(["line %03i: %s" % (lineno+1, line) for lineno, line in enumerate(program)]))
		raise ValueError
	return_values = []
	for s in output:
		s = s.strip()
		if not s:
			continue
		if s == 'None':
			return_values.append(None)
		elif s[0] == "'" and s[-1] == "'":
			return_values.append(s[1:-1])
		elif s[0].isdigit():
			return_values.append(int(s))
		else: break
	return return_values

@conf
def check_python_headers(conf):
	"""Check for headers and libraries necessary to extend or embed python.

	If successful, xxx_PYEXT and xxx_PYEMBED variables are defined in the
	environment (for uselib). PYEXT should be used for compiling
	python extensions, while PYEMBED should be used by programs that
	need to embed a python interpreter.

	Note: this test requires that check_python_version was previously
	executed and successful."""

	env = conf.env
	python = env['PYTHON']
	assert python, ("python is %r !" % (python,))

	## On Mac OSX we need to use mac bundles for python plugins
	if Options.platform == 'darwin':
		conf.check_tool('osx')

	try:
		# Get some python configuration variables using distutils
		v = 'prefix SO SYSLIBS LDFLAGS SHLIBS LIBDIR LIBPL INCLUDEPY Py_ENABLE_SHARED'.split()
		(python_prefix, python_SO, python_SYSLIBS, python_LDFLAGS, python_SHLIBS,
		 python_LIBDIR, python_LIBPL, INCLUDEPY, Py_ENABLE_SHARED) = \
			_get_python_variables(python, ["get_config_var('%s')" % x for x in v],
					      ['from distutils.sysconfig import get_config_var'])
	except ValueError:
		conf.fatal("Python development headers not found (-v for details).")

	conf.log.write("""Configuration returned from %r:
python_prefix = %r
python_SO = %r
python_SYSLIBS = %r
python_LDFLAGS = %r
python_SHLIBS = %r
python_LIBDIR = %r
python_LIBPL = %r
INCLUDEPY = %r
Py_ENABLE_SHARED = %r
""" % (python, python_prefix, python_SO, python_SYSLIBS, python_LDFLAGS, python_SHLIBS,
	python_LIBDIR, python_LIBPL, INCLUDEPY, Py_ENABLE_SHARED))

	env['pyext_PATTERN'] = '%s'+python_SO

	# Check for python libraries for embedding
	if python_SYSLIBS is not None:
		for lib in python_SYSLIBS.split():
			if lib.startswith('-l'):
				lib = lib[2:] # strip '-l'
			env.append_value('LIB_PYEMBED', lib)
	if python_SHLIBS is not None:
		for lib in python_SHLIBS.split():
			if lib.startswith('-l'):
				lib = lib[2:] # strip '-l'
			env.append_value('LIB_PYEMBED', lib)

	env.append_value('LINKFLAGS_PYEMBED', python_LDFLAGS)

	code = '''
#ifdef __cplusplus
extern "C" {
#endif
 void Py_Initialize(void);
 void Py_Finalize(void);
#ifdef __cplusplus
}
#endif
int main(int argc, char *argv[]) { Py_Initialize(); Py_Finalize(); return 0; }
'''

	result = False
	name = 'python' + env['PYTHON_VERSION']

	if python_LIBDIR is not None:
		path = [python_LIBDIR]
		result = conf.check(lib=name, uselib='PYEMBED', libpath=path)

	# try again with -L$python_LIBPL (some systems don't install the python library in $prefix/lib)
	if not result and python_LIBPL is not None:
		path = [python_LIBPL]
		result = conf.check(lib=name, uselib='PYEMBED', libpath=path)

	# try again with -L$prefix/libs, and pythonXY name rather than pythonX.Y (win32)
	if not result:
		path = [os.path.join(python_prefix, "libs")]
		name = 'python' + env['PYTHON_VERSION'].replace('.', '')
		result = conf.check(lib=name, uselib='PYEMBED', libpath=path)

	if result:
		env['LIBPATH_PYEMBED'] = path
		env.append_value('LIB_PYEMBED', name)

	# under certain conditions, python extensions must link to
	# python libraries, not just python embedding programs.
	if (sys.platform == 'win32' or sys.platform.startswith('os2')
		or sys.platform == 'darwin' or Py_ENABLE_SHARED):
		env['LIBPATH_PYEXT'] = env['LIBPATH_PYEMBED']
		env['LIB_PYEXT'] = env['LIB_PYEMBED']

	# We check that pythonX.Y-config exists, and if it exists we
	# use it to get only the includes, else fall back to distutils.
	python_config = conf.find_program(
		'python%s-config' % ('.'.join(env['PYTHON_VERSION'].split('.')[:2])),
		var='PYTHON_CONFIG')
	if not python_config:
		python_config = conf.find_program(
			'python-config-%s' % ('.'.join(env['PYTHON_VERSION'].split('.')[:2])),
			var='PYTHON_CONFIG')

	includes = []
	if python_config:
		for incstr in os.popen("%s %s --includes" % (python, python_config)).readline().strip().split():
			# strip the -I or /I
			if (incstr.startswith('-I')
			    or incstr.startswith('/I')):
				incstr = incstr[2:]
			# append include path, unless already given
			if incstr not in includes:
				includes.append(incstr)
		conf.log.write("Include path for Python extensions "
			       "(found via python-config --includes): %r\n" % (includes,))
		env['CPPPATH_PYEXT'] = includes
		env['CPPPATH_PYEMBED'] = includes
	else:
		conf.log.write("Include path for Python extensions "
			       "(found via distutils module): %r\n" % (INCLUDEPY,))
		env['CPPPATH_PYEXT'] = [INCLUDEPY]
		env['CPPPATH_PYEMBED'] = [INCLUDEPY]

	# Code using the Python API needs to be compiled with -fno-strict-aliasing
	if env['CC']:
		version = os.popen("%s --version" % env['CC']).readline()
		if '(GCC)' in version or 'gcc' in version:
			env.append_value('CCFLAGS_PYEMBED', '-fno-strict-aliasing')
			env.append_value('CCFLAGS_PYEXT', '-fno-strict-aliasing')
	if env['CXX']:
		version = os.popen("%s --version" % env['CXX']).readline()
		if '(GCC)' in version or 'g++' in version:
			env.append_value('CXXFLAGS_PYEMBED', '-fno-strict-aliasing')
			env.append_value('CXXFLAGS_PYEXT', '-fno-strict-aliasing')

	# See if it compiles
	test_env = env.copy()
	test_env.append_value('CPPPATH', env['CPPPATH_PYEMBED'])
	test_env.append_value('LIBPATH', env['LIBPATH_PYEMBED'])
	test_env.append_value('LIB', env['LIB_PYEMBED'])
	test_env.append_value('LINKFLAGS', env['LINKFLAGS_PYEMBED'])
	test_env.append_value('CXXFLAGS', env['CXXFLAGS_PYEMBED'])
	test_env.append_value('CCFLAGS', env['CCFLAGS_PYEMBED'])
	conf.check(header_name='Python.h', define_name='HAVE_PYTHON_H', env=test_env,
		fragment='''#include <Python.h>\nint main(int argc, char *argv[]) { Py_Initialize(); Py_Finalize(); return 0; }\n''', errmsg='Could not find the python development headers', mandatory=1)

@conf
def check_python_version(conf, minver=None):
	"""
	Check if the python interpreter is found matching a given minimum version.
	minver should be a tuple, eg. to check for python >= 2.4.2 pass (2,4,2) as minver.

	If successful, PYTHON_VERSION is defined as 'MAJOR.MINOR'
	(eg. '2.4') of the actual python version found, and PYTHONDIR is
	defined, pointing to the site-packages directory appropriate for
	this python version, where modules/packages/extensions should be
	installed.
	"""
	assert minver is None or isinstance(minver, tuple)
	python = conf.env['PYTHON']
	assert python, ("python is %r !" % (python,))

	# Get python version string
	cmd = [python, "-c", "import sys\nfor x in sys.version_info: print str(x)"]
	debug('python: Running python command %r' % cmd)
	proc = pproc.Popen(cmd, stdout=pproc.PIPE)
	lines = proc.communicate()[0].split()
	assert len(lines) == 5, "found %i lines, expected 5: %r" % (len(lines), lines)
	pyver_tuple = (int(lines[0]), int(lines[1]), int(lines[2]), lines[3], int(lines[4]))

	# compare python version with the minimum required
	result = (minver is None) or (pyver_tuple >= minver)

	if result:
		# define useful environment variables
		pyver = '.'.join([str(x) for x in pyver_tuple[:2]])
		conf.env['PYTHON_VERSION'] = pyver

		if 'PYTHONDIR' in os.environ:
			pydir = os.environ['PYTHONDIR']
		else:
			if sys.platform == 'win32':
				(python_LIBDEST,) = \
						_get_python_variables(python, ["get_config_var('LIBDEST')"],
						['from distutils.sysconfig import get_config_var'])
			else:
				python_LIBDEST = None
			if python_LIBDEST is None:
				if conf.env['LIBDIR']:
					python_LIBDEST = os.path.join(conf.env['LIBDIR'], "python" + pyver)
				else:
					python_LIBDEST = os.path.join(conf.env['PREFIX'], "lib", "python" + pyver)
			pydir = os.path.join(python_LIBDEST, "site-packages")

		if hasattr(conf, 'define'): # conf.define is added by the C tool, so may not exist
			conf.define('PYTHONDIR', pydir)
		conf.env['PYTHONDIR'] = pydir

	# Feedback
	pyver_full = '.'.join(map(str, pyver_tuple[:3]))
	if minver is None:
		conf.check_message_custom('Python version', '', pyver_full)
	else:
		minver_str = '.'.join(map(str, minver))
		conf.check_message('Python version', ">= %s" % (minver_str,), result, option=pyver_full)

	if not result:
		conf.fatal("Python too old.")

@conf
def check_python_module(conf, module_name):
	"""
	Check if the selected python interpreter can import the given python module.
	"""
	result = not pproc.Popen([conf.env['PYTHON'], "-c", "import %s" % module_name],
			   stderr=pproc.PIPE, stdout=pproc.PIPE).wait()
	conf.check_message('Python module', module_name, result)
	if not result:
		conf.fatal("Python module not found.")

def detect(conf):
	python = conf.find_program('python', var='PYTHON')
	if not python: return

	v = conf.env

	v['PYCMD'] = '"import sys, py_compile;py_compile.compile(sys.argv[1], sys.argv[2])"'
	v['PYFLAGS'] = ''
	v['PYFLAGS_OPT'] = '-O'

	v['PYC'] = getattr(Options.options, 'pyc', 1)
	v['PYO'] = getattr(Options.options, 'pyo', 1)

def set_options(opt):
	opt.add_option('--nopyc',
			action='store_false',
			default=1,
			help = 'Do not install bytecode compiled .pyc files (configuration) [Default:install]',
			dest = 'pyc')
	opt.add_option('--nopyo',
			action='store_false',
			default=1,
			help='Do not install optimised compiled .pyo files (configuration) [Default:install]',
			dest='pyo')

