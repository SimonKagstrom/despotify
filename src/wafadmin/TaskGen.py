#!/usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2005-2008 (ita)

"""
The class task_gen encapsulates the creation of task objects (low-level code)
The instances can have various parameters, but the creation of task nodes (Task.py)
is delayed. To achieve this, various methods are called from the method "apply"

The class task_gen contains lots of methods, and a configuration table:
* the methods to call (self.meths) can be specified dynamically (removing, adding, ..)
* the order of the methods (self.prec or by default task_gen.prec) is configurable
* new methods can be inserted dynamically without pasting old code

Additionally, task_gen provides the method apply_core
* file extensions are mapped to methods: def meth(self, name_or_node)
* if a mapping is not found in self.mappings, it is searched in task_gen.mappings
* when called, the functions may modify self.allnodes to re-add source to process
* the mappings can map an extension or a filename (see the code below)

WARNING: subclasses must reimplement the clone method
"""

import os, types, traceback, copy
import Build, Task, Utils, Logs, Options
from Logs import debug, error, warn
from Constants import *

typos = {
'sources':'source',
'targets':'target',
'include':'includes',
'define':'defines',
'importpath':'importpaths',
'install_var':'install_path',
'install_subdir':'install_path',
'inst_var':'install_path',
'inst_dir':'install_path',
}

class register_obj(type):
	"""no decorators for classes, so we use a metaclass
	we store into task_gen.classes the classes that inherit task_gen
	and whose names end in '_taskgen'
	"""
	def __init__(cls, name, bases, dict):
		super(register_obj, cls).__init__(name, bases, dict)
		name = cls.__name__
		suffix = '_taskgen'
		if name.endswith(suffix):
			task_gen.classes[name.replace(suffix, '')] = cls

class task_gen(object):
	"""
	Most methods are of the form 'def meth(self):' without any parameters
	there are many of them, and they do many different things:
	* task creation
	* task results installation
	* environment modification
	* attribute addition/removal

	The inheritance approach is complicated
	* mixing several languages at once
	* subclassing is needed even for small changes
	* inserting new methods is complicated

	This new class uses a configuration table:
	* adding new methods easily
	* obtaining the order in which to call the methods
	* postponing the method calls (post() -> apply)

	Additionally, a 'traits' static attribute is provided:
	* this list contains methods
	* the methods can remove or add methods from self.meths
	Example1: the attribute 'staticlib' is set on an instance
	a method set in the list of traits is executed when the
	instance is posted, it finds that flag and adds another method for execution
	Example2: a method set in the list of traits finds the msvc
	compiler (from self.env['MSVC']==1); more methods are added to self.meths
	"""

	__metaclass__ = register_obj
	mappings = {}
	mapped = {}
	prec = {}
	traits = {}
	classes = {}
	idx = {}

	def __init__(self, *kw, **kwargs):
		self.prec = {}
		"map precedence of function names to call"
		# so we will have to play with directed acyclic graphs
		# detect cycles, etc

		self.source = ''
		self.target = ''

		# list of methods to execute - does not touch it by hand unless you know
		self.meths = []

		# list of mappings extension -> function
		self.mappings = {}

		# list of features (see the documentation on traits)
		self.features = list(kw)

		# not always a good idea
		self.tasks = []

		self.default_chmod = O644
		self.default_install_path = None

		if Options.is_install:
			self.inst_files = [] # lazy list of tuples representing the files to install

		# kind of private, beware of what you put in it, also, the contents are consumed
		self.allnodes = []

		self.bld = kwargs.get('bld', Build.bld)
		self.env = self.bld.env.copy()

		self.path = self.bld.path # emulate chdir when reading scripts
		self.name = '' # give a name to the target (static+shlib with the same targetname ambiguity)
		self.bld.all_task_gen.append(self)

		# provide a unique id
		self.idx = task_gen.idx[self.path.id] = task_gen.idx.get(self.path.id, 0) + 1

		for key, val in kwargs.iteritems():
			setattr(self, key, val)

	def __str__(self):
		return ("<task_gen '%s' of type %s defined in %s>"
			% (self.name or self.target, self.__class__.__name__, str(self.path)))

	def __setattr__(self, name, attr):
		real = typos.get(name, name)
		if real != name:
			warn('typo %s -> %s' % (name, real))
			if Logs.verbose > 0:
				traceback.print_stack()
		object.__setattr__(self, real, attr)

	def to_list(self, value):
		"helper: returns a list"
		if type(value) is types.StringType: return value.split()
		else: return value

	def apply(self):
		"order the methods to execute using self.prec or task_gen.prec"
		keys = set(self.meths)

		# add the methods listed in the features
		self.features = Utils.to_list(self.features)
		for x in self.features + ['*']:
			keys.update(task_gen.traits.get(x, ()))

		# copy the precedence table
		prec = {}
		prec_tbl = self.prec or task_gen.prec
		for x in prec_tbl:
			if x in keys:
				prec[x] = prec_tbl[x]

		# elements disconnected
		tmp = []
		for a in keys:
			for x in prec.values():
				if a in x: break
			else:
				tmp.append(a)

		# topological sort
		out = []
		while tmp:
			e = tmp.pop()
			if e in keys: out.append(e)
			try:
				nlst = prec[e]
			except KeyError:
				pass
			else:
				del prec[e]
				for x in nlst:
					for y in prec:
						if x in prec[y]:
							break
					else:
						tmp.append(x)

		if prec: raise Utils.WafError("graph has a cycle %s" % str(prec))
		out.reverse()
		self.meths = out

		# then we run the methods in order
		debug('task_gen: posting %s %d' % (self, id(self)))
		for x in out:
			try:
				v = getattr(self, x)
			except AttributeError:
				raise Utils.WafError("tried to retrieve %s which is not a valid method" % x)
			debug('task_gen: -> %s (%d)' % (x, id(self)))
			v()

	def post(self):
		"runs the code to create the tasks, do not subclass"
		if not self.name: self.name = self.target

		if getattr(self, 'posted', None):
			#error("OBJECT ALREADY POSTED" + str( self))
			return
		self.apply()
		debug('task_gen: posted %s' % self.name)
		self.posted = True

	def get_hook(self, ext):
		try: return self.mappings[ext]
		except KeyError:
			try: return task_gen.mappings[ext]
			except KeyError: return None

	def create_task(self, name, env=None):
		task = Task.TaskBase.classes[name](env or self.env, generator=self)
		self.tasks.append(task)
		return task

	def name_to_obj(self, name):
		return self.bld.name_to_obj(name, self.env)

	def find_sources_in_dirs(self, dirnames, excludes=[], exts=[]):
		"subclass if necessary"
		lst = []

		# validation: excludes and exts must be lists.
		# the purpose: make sure a confused user didn't wrote
		#  find_sources_in_dirs('a', 'b', 'c')
		# instead of find_sources_in_dirs('a b c')
		err_msg = "'%s' attribute must be a list.\n" \
		"Directories should be given either as a string separated by spaces, or as a list."
		not_a_list = lambda x: x and type(x) is not types.ListType
		if not_a_list(excludes):
			raise Utils.WscriptError(err_msg % 'excludes')
		if not_a_list(exts):
			raise Utils.WscriptError(err_msg % 'exts')

		#make sure dirnames is a list helps with dirnames with spaces
		dirnames = self.to_list(dirnames)

		ext_lst = exts or self.mappings.keys() + task_gen.mappings.keys()

		for name in dirnames:
			anode = self.path.find_dir(name)

			# validation:
			# * don't use absolute path.
			# * don't use paths outside the source tree.
			if not anode or not anode.is_child_of(self.bld.srcnode):
				raise Utils.WscriptError("Unable to use '%s' - either because it's not a relative path" \
					 ", or it's not child of '%s'." % (name, self.bld.srcnode))

			self.bld.rescan(anode)

			for name in self.bld.cache_dir_contents[anode.id]:
				(base, ext) = os.path.splitext(name)
				if ext in ext_lst and not name in lst and not name in excludes:
					lst.append((anode.relpath_gen(self.path) or '.') + os.path.sep + name)

		lst.sort()
		self.source = self.to_list(self.source)
		if not self.source: self.source = lst
		else: self.source += lst

	def clone(self, env):
		""
		newobj = task_gen(bld=self.bld)
		for x in self.__dict__:
			if x in ['env', 'bld']:
				continue
			elif x in ["path", "features"]:
				setattr(newobj, x, getattr(self, x))
			else:
				setattr(newobj, x, copy.copy(getattr(self, x)))

		newobj.__class__ = self.__class__
		if type(env) is types.StringType:
			newobj.env = self.bld.all_envs[env].copy()
		else:
			newobj.env = env.copy()

		self.bld.all_task_gen.append(newobj)

		return newobj

	def get_inst_path(self):
		return getattr(self, '_install_path', getattr(self, 'default_install_path', ''))

	def set_inst_path(self, val):
		self._install_path = val

	install_path = property(get_inst_path, set_inst_path)


	def get_chmod(self):
		return getattr(self, '_chmod', getattr(self, 'default_chmod', O644))

	def set_chmod(self, val):
		self._chmod = val

	chmod = property(get_chmod, set_chmod)

def declare_extension(var, func):
	try:
		for x in var:
			task_gen.mappings[x] = func
	except:
		raise Utils.WscriptError('declare extension takes either a list or a string %s' % str(var))
	task_gen.mapped[func.__name__] = func

def declare_order(*k):
	assert(len(k) > 1)
	n = len(k) - 1
	for i in xrange(n):
		f1 = k[i]
		f2 = k[i+1]
		try:
			if not f1 in task_gen.prec[f2]: task_gen.prec[f2].append(f1)
		except:
			task_gen.prec[f2] = [f1]

def declare_chain(name='', action='', ext_in='', ext_out='', reentrant=1, color='BLUE', install=0, before=[], after=[], decider=None, rule=None):
	"""
	see Tools/flex.py for an example
	while i do not like such wrappers, some people really do
	"""

	action = action or rule
	if type(action) is types.StringType:
		act = Task.simple_task_type(name, action, color=color)
	else:
		act = Task.task_type_from_func(name, action, color=color)
	act.ext_in = tuple(Utils.to_list(ext_in))
	act.ext_out = tuple(Utils.to_list(ext_out))
	act.before = Utils.to_list(before)
	act.after = Utils.to_list(after)

	def x_file(self, node):
		if decider:
			ext = decider(self, node)
		elif type(ext_out) is types.StringType:
			ext = ext_out

		if type(ext) is types.StringType:
			out_source = node.change_ext(ext)
			if reentrant:
				self.allnodes.append(out_source)
		elif type(ext) == types.ListType:
			out_source = [node.change_ext(x) for x in ext]
			if reentrant:
				for i in xrange(reentrant):
					self.allnodes.append(out_source[i])
		else:
			# XXX: useless: it will fail on Utils.to_list above...
			raise Utils.WafError("do not know how to process %s" % str(ext))

		tsk = self.create_task(name)
		tsk.set_inputs(node)
		tsk.set_outputs(out_source)

		if Options.is_install and install:
			tsk.install = install

	declare_extension(act.ext_in, x_file)

def bind_feature(name, methods):
	lst = Utils.to_list(methods)
	try:
		l = task_gen.traits[name]
	except KeyError:
		l = set()
		task_gen.traits[name] = l
	l.update(lst)

"""
All the following decorators are registration decorators, i.e add an attribute to current class
 (task_gen and its derivatives), with same name as func, which points to func itself.
For example:
   @taskgen
   def sayHi(self):
        print("hi")
Now taskgen.sayHi() may be called
"""
def taskgen(func):
	setattr(task_gen, func.__name__, func)

def feature(*k):
	def deco(func):
		setattr(task_gen, func.__name__, func)
		for name in k:
			try:
				l = task_gen.traits[name]
			except KeyError:
				l = set()
				task_gen.traits[name] = l
			l.update([func.__name__])
		return func
	return deco

def before(*k):
	def deco(func):
		for fun_name in k:
			try:
				if not func.__name__ in task_gen.prec[fun_name]: task_gen.prec[fun_name].append(func.__name__)
			except KeyError:
				task_gen.prec[fun_name] = [func.__name__]
		return func
	return deco

def after(*k):
	def deco(func):
		for fun_name in k:
			try:
				if not fun_name in task_gen.prec[func.__name__]: task_gen.prec[func.__name__].append(fun_name)
			except KeyError:
				task_gen.prec[func.__name__] = [fun_name]
		return func
	return deco

def extension(var):
	if type(var) is types.ListType:
		pass
	elif type(var) is types.StringType:
		var = [var]
	else:
		raise Utils.WafError('declare extension takes either a list or a string %s' % str(var))

	def deco(func):
		setattr(task_gen, func.__name__, func)
		for x in var:
			task_gen.mappings[x] = func
		task_gen.mapped[func.__name__] = func
		return func
	return deco

# TODO make certain the decorators may be used here

def apply_core(self):
	"""Process the attribute source
	transform the names into file nodes
	try to process the files by name first, later by extension"""
	# get the list of folders to use by the scanners
	# all our objects share the same include paths anyway
	find_resource = self.path.find_resource

	for filename in self.to_list(self.source):
		# if self.mappings or task_gen.mappings contains a file of the same name
		x = self.get_hook(filename)
		if x:
			x(self, filename)
		else:
			node = find_resource(filename)
			if not node: raise Utils.WafError("source not found: '%s' in '%s'" % (filename, str(self.path)))
			self.allnodes.append(node)

	for node in self.allnodes:
		# self.mappings or task_gen.mappings map the file extension to a function
		x = self.get_hook(node.suffix())

		if not x:
			raise Utils.WafError("Do not know how to process %s in %s, mappings are %s" % \
				(str(node), str(self.__class__), str(self.__class__.mappings)))
		x(self, node)
feature('*')(apply_core)

def exec_rule(self):
	"""Process the attribute rule, when provided the method apply_core will be disabled
	"""
	if not getattr(self, 'rule', None):
		return

	# someone may have removed it already
	try:
		self.meths.remove('apply_core')
	except ValueError:
		pass

	# get the function and the variables
	func = self.rule
	vars2 = []
	if isinstance(func, str):
		(func, vars2) = Task.compile_fun('', self.rule)
	vars = getattr(self, 'vars', vars2)

	# create the task class
	name = getattr(self, 'name', None) or self.target or self.rule
	cls = Task.task_type_from_func(name, func, vars)

	# now create one instance
	tsk = self.create_task(name)

	# we assume that the user knows that without inputs or outputs
	#if not getattr(self, 'target', None) and not getattr(self, 'source', None):
	#	cls.quiet = True

	if getattr(self, 'target', None):
		cls.quiet = True
		tsk.outputs=[self.path.find_or_declare(x) for x in self.to_list(self.target)]

	if getattr(self, 'source', None):
		cls.quiet = True
		tsk.inputs=[self.path.find_resource(x) for x in self.to_list(self.source)]

	if getattr(self, 'always', None):
		Task.always_run(cls)

	if getattr(self, 'cwd', None):
		tsk.cwd = self.cwd

	if getattr(self, 'on_results', None):
		Task.update_outputs(cls)

	for x in ['after', 'before']:
		setattr(cls, x, getattr(self, x, []))
feature('*')(exec_rule)
before('apply_core')(exec_rule)

