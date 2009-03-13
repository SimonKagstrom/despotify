#!/usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2005-2008 (ita)

"""
Configuration system

A configuration instance is created when "waf configure" is called, it is used to:
* create data dictionaries (Environment instances)
* store the list of modules to import

The old model (copied from Scons) was to store logic (mapping file extensions to functions)
along with the data. In Waf a way was found to separate that logic by adding an indirection
layer (storing the names in the Environment instances)

In the new model, the logic is more object-oriented, and the user scripts provide the
logic. The data files (Environments) must contain configuration data only (flags, ..).

Note: the c/c++ related code is in the module config_c
"""

import os, types, imp
try: import cPickle
except ImportError: import pickle as cPickle
import Environment, Utils, Options
from Logs import warn
from Constants import *

class ConfigurationError(Utils.WscriptError):
	pass

autoconfig = False
"reconfigure the project automatically"

def find_file(filename, path_list):
	"""find a file in a list of paths
	@param filename: name of the file to search for
	@param path_list: list of directories to search
	@return: the first occurrence filename or '' if filename could not be found
"""
	if type(path_list) is types.StringType:
		lst = path_list.split()
	else:
		lst = path_list
	for directory in lst:
		if os.path.exists( os.path.join(directory, filename) ):
			return directory
	return ''

def find_program_impl(env, filename, path_list=[], var=None):
	"""find a program in folders path_lst, and sets env[var]
	@param env: environment
	@param filename: name of the program to search for
	@param path_list: list of directories to search for filename
	@param var: environment value to be checked for in env or os.environ
	@return: either the value that is referenced with [var] in env or os.environ
         or the first occurrence filename or '' if filename could not be found
"""
	try: path_list = path_list.split()
	except AttributeError: pass

	if var:
		if var in os.environ: env[var] = os.environ[var]
		if env[var]: return env[var]

	if not path_list: path_list = os.environ['PATH'].split(os.pathsep)

	ext = (Options.platform == 'win32') and '.exe,.com,.bat,.cmd' or ''
	for y in [filename+x for x in ext.split(',')]:
		for directory in path_list:
			x = os.path.join(directory, y)
			if os.path.isfile(x):
				# quote filenames with whitespaces in name or path.
				x = Utils.quote_whitespace(x)
				if var: env[var] = x
				return x
	return ''

class ConfigurationContext(object):
	tests = {}
	error_handlers = []
	def __init__(self, env=None, blddir='', srcdir=''):
		self.env       = None
		self.envname = ''

		self.line_just = 40

		self.blddir = blddir
		self.srcdir = srcdir
		self.cachedir = os.path.join(blddir, CACHE_DIR)

		self.all_envs = {}
		self.defines = {}
		self.cwd = os.getcwd()

		self.tools = [] # tools loaded in the configuration, and that will be loaded when building

		self.setenv(DEFAULT)

		self.lastprog = ''

		self.hash = 0
		self.files = []

		path = os.path.join(self.blddir, WAF_CONFIG_LOG)
		try: os.unlink(path)
		except (OSError, IOError): pass
		self.log = open(path, 'wb')

	def __del__(self):
		"""cleanup function: close config.log"""

		# may be ran by the gc, not always after initiazliation
		if hasattr(self, 'log') and self.log:
			self.log.close()

	def fatal(self, msg):
		raise ConfigurationError(msg)

	def check_tool(self, input, tooldir=None, funs=None):
		"load a waf tool"
		tools = Utils.to_list(input)
		if tooldir: tooldir = Utils.to_list(tooldir)
		for tool in tools:
			try:
				file,name,desc = imp.find_module(tool, tooldir)
			except ImportError:
				self.fatal("no tool named '%s' found." % tool)
			module = imp.load_module(tool, file, name, desc)

			func = getattr(module, 'detect', None)
			if func:
				if type(func) is types.FunctionType: func(self)
				else: self.eval_rules(funs or func)

			self.tools.append({'tool':tool, 'tooldir':tooldir, 'funs':funs})

	def sub_config(self, dir):
		"executes the configure function of a wscript module"

		current = self.cwd

		self.cwd = os.path.join(self.cwd, dir)
		cur = os.path.join(self.cwd, WSCRIPT_FILE)

		mod = Utils.load_module(cur)
		if not hasattr(mod, 'configure'):
			self.fatal('the module %s has no configure function; make sure such a function is defined' % cur)

		ret = mod.configure(self)
		global autoconfig
		if autoconfig:
			self.hash = Utils.hash_function_with_globals(self.hash, mod.configure)
			self.files.append(os.path.abspath(cur))
		self.cwd = current
		return ret

	def store(self, file=''):
		"save the config results into the cache file"
		if not os.path.isdir(self.cachedir):
			os.makedirs(self.cachedir)

		if not file:
			file = open(os.path.join(self.cachedir, 'build.config.py'), 'w')
		file.write('version = 0x%x\n' % HEXVERSION)
		file.write('tools = %r\n' % self.tools)
		file.close()

		if not self.all_envs:
			self.fatal('nothing to store in the configuration context!')
		for key in self.all_envs:
			tmpenv = self.all_envs[key]
			tmpenv.store(os.path.join(self.cachedir, key + CACHE_SUFFIX))

	def set_env_name(self, name, env):
		"add a new environment called name"
		self.all_envs[name] = env
		return env

	def retrieve(self, name, fromenv=None):
		"retrieve an environment called name"
		try:
			env = self.all_envs[name]
		except KeyError:
			env = Environment.Environment()
			self.all_envs[name] = env
		else:
			if fromenv: warn("The environment %s may have been configured already" % name)
		return env

	def setenv(self, name):
		"enable the environment called name"
		self.env = self.retrieve(name)
		self.envname = name

	def add_os_flags(self, var, dest=None):
		if not dest: dest = var
		# do not use 'get' to make certain the variable is not defined
		try: self.env[dest] = os.environ[var]
		except KeyError: pass

	def check_message_1(self, sr):
		self.line_just = max(self.line_just, len(sr))
		self.log.write(sr + '\n\n')
		Utils.pprint('NORMAL', "%s :" % sr.ljust(self.line_just), sep='')

	def check_message_2(self, sr, color='GREEN'):
		Utils.pprint(color, sr)

	def check_message(self, th, msg, state, option=''):
		"print an checking message. This function is used by other checking functions"
		sr = 'Checking for %s %s' % (th, msg)
		self.check_message_1(sr)
		p = self.check_message_2
		if state: p('ok ' + option)
		else: p('not found', 'YELLOW')

	# the parameter 'option' is not used (kept for compatibility)
	def check_message_custom(self, th, msg, custom, option='', color='PINK'):
		"""print an checking message. This function is used by other checking functions"""
		sr = 'Checking for %s %s' % (th, msg)
		self.check_message_1(sr)
		self.check_message_2(custom, color)

	def find_program(self, filename, path_list=[], var=None, mandatory=False):
		"wrapper that adds a configuration message"
		ret = find_program_impl(self.env, filename, path_list, var)
		self.check_message('program', filename, ret, ret)
		self.log.write('find program=%r paths=%r var=%r -> %r\n\n' % (filename, path_list, var, ret))
		if not ret and mandatory:
			self.fatal('The program %s could not be found' % filename)
		return ret

	def __getattr__(self, name):
		r = self.__class__.__dict__.get(name, None)
		if r: return r
		if name and name.startswith('require_'):

			for k in ['check_', 'find_']:
				n = name.replace('require_', k)
				ret = self.__class__.__dict__.get(n, None)
				if ret:
					def run(*k, **kw):
						r = ret(self, *k, **kw)
						if not r:
							self.fatal('requirement failure')
						return r
					return run
		self.fatal('No such method %r' % name)

	def eval_rules(self, rules):
		self.rules = Utils.to_list(rules)
		for x in self.rules:
			f = getattr(self, x)
			try:
				f()
			except Exception, e:
				ret = self.err_handler(x, e)
				if ret == BREAK:
					break
				elif ret == CONTINUE:
					continue
				else:
					raise

	def err_handler(self, fun, error):
		pass

def conf(f):
	"decorator: attach new configuration functions"
	setattr(ConfigurationContext, f.__name__, f)
	return f

def conftest(f):
	"decorator: attach new configuration tests (registered as strings)"
	ConfigurationContext.tests[f.__name__] = f
	return conf(f)


