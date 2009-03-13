#!/usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2006-2008 (ita)

"""
Java support

Javac is one of the few compilers that behaves very badly:
* it outputs files where it wants to (-d is only for the package root)
* it recompiles files silently behind your back
* it outputs an undefined amount of files (inner classes)

Fortunately, the convention makes it possible to use th build dir without
too many problems for the moment

Inner classes must be located and cleaned when a problem arise,
for the moment waf does not track the production of inner classes.

Adding all the files to a task and executing it if any of the input files
change is only annoying for the compilation times
"""

import os, re
from Configure import conf
import TaskGen, Task, Utils
from TaskGen import feature, taskgen

class java_taskgen(TaskGen.task_gen):
	def __init__(self, *k, **kw):
		TaskGen.task_gen.__init__(self, *k, **kw)

@taskgen
@feature('java')
def apply_java(self):
	Utils.def_attrs(self, jarname='', jaropts='', classpath='',
		source_root='.', jar_mf_attributes={}, jar_mf_classpath=[])

	nodes_lst = []

	if not self.classpath:
		if not self.env['CLASSPATH']:
			self.env['CLASSPATH'] = '..' + os.pathsep + '.'
	else:
		self.env['CLASSPATH'] = self.classpath

	re_foo = re.compile(self.source)

	source_root_node = self.path.find_dir(self.source_root)

	src_nodes = []
	bld_nodes = []

	prefix_path = source_root_node.abspath()
	for (root, dirs, filenames) in os.walk(source_root_node.abspath()):
		for x in filenames:
			file = root + '/' + x
			file = file.replace(prefix_path, '')
			if file.startswith('/'):
				file = file[1:]

			if re_foo.search(file) > -1:
				node = source_root_node.find_resource(file)
				src_nodes.append(node)

				node2 = node.change_ext(".class")
				bld_nodes.append(node2)

	self.env['OUTDIR'] = source_root_node.abspath(self.env)

	tsk = self.create_task('javac')
	tsk.set_inputs(src_nodes)
	tsk.set_outputs(bld_nodes)

	if self.jarname:
		tsk = self.create_task('jar_create')
		tsk.set_inputs(bld_nodes)
		tsk.set_outputs(self.path.find_or_declare(self.jarname))

		if not self.env['JAROPTS']:
			if self.jaropts:
				self.env['JAROPTS'] = self.jaropts
			else:
				dirs = '.'
				self.env['JAROPTS'] = '-C %s %s' % (self.env['OUTDIR'], dirs)

Task.simple_task_type('jar_create', '${JAR} ${JARCREATE} ${TGT} ${JAROPTS}', color='GREEN')
cls = Task.simple_task_type('javac', '${JAVAC} -classpath ${CLASSPATH} -d ${OUTDIR} ${SRC}', before='jar_create')
cls.color = 'BLUE'
def post_run_javac(self):
	"""this is for cleaning the folder
	javac creates single files for inner classes
	but it is not possible to know which inner classes in advance"""

	par = {}
	for x in self.inputs:
		par[x.parent.id] = x.parent

	inner = {}
	for k in par.values():
		path = k.abspath(self.env)
		lst = os.listdir(path)

		for u in lst:
			if u.find('$') >= 0:
				inner_class_node = k.find_or_declare(u)
				inner[inner_class_node.id] = inner_class_node

	to_add = set(inner.keys()) - set([x.id for x in self.outputs])
	for x in to_add:
		self.outputs.append(inner[x])

	return Task.Task.post_run(self)
cls.post_run = post_run_javac

def detect(conf):
	# If JAVA_PATH is set, we prepend it to the path list
	java_path = os.environ['PATH'].split(os.pathsep)
	v = conf.env

	if 'JAVA_HOME' in os.environ:
		java_path = [os.path.join(os.environ['JAVA_HOME'], 'bin')] + java_path
		conf.env['JAVA_HOME'] = os.environ['JAVA_HOME']

	conf.find_program('javac', var='JAVAC', path_list=java_path)
	conf.find_program('java', var='JAVA', path_list=java_path)
	conf.find_program('jar', var='JAR', path_list=java_path)
	v['JAVA_EXT'] = ['.java']

	if 'CLASSPATH' in os.environ:
		v['CLASSPATH'] = os.environ['CLASSPATH']

	if not v['JAR']: conf.fatal('jar is required for making java packages')
	if not v['JAVAC']: conf.fatal('javac is required for compiling java classes')
	v['JARCREATE'] = 'cf' # can use cvf

@conf
def check_java_class(self, classname, with_classpath=None):
	"""Check if the specified java class is installed"""

	class_check_source = """
public class Test {
	public static void main(String[] argv) {
		Class lib;
		if (argv.length < 1) {
			System.err.println("Missing argument");
			System.exit(77);
		}
		try {
			lib = Class.forName(argv[0]);
		} catch (ClassNotFoundException e) {
			System.err.println("ClassNotFoundException");
			System.exit(1);
		}
		lib = null;
		System.exit(0);
	}
}
"""
	import shutil

	javatestdir = '.waf-javatest'

	classpath = javatestdir
	if self.env['CLASSPATH']:
		classpath += os.pathsep + self.env['CLASSPATH']
	if isinstance(with_classpath, str):
		classpath += os.pathsep + with_classpath

	shutil.rmtree(javatestdir, True)
	os.mkdir(javatestdir)

	java_file = open(os.path.join(javatestdir, 'Test.java'), 'w')
	java_file.write(class_check_source)
	java_file.close()

	# Compile the source
	os.popen(self.env['JAVAC'] + ' ' + os.path.join(javatestdir, 'Test.java'))

	cmd = self.env['JAVA'] + ' -cp ' + classpath + ' Test ' + classname
	self.log.write("%s\n" % cmd)
	found = Utils.exec_command(cmd, shell=True, log=self.log)

	self.check_message('Java class %s' % classname, "", not found)

	shutil.rmtree(javatestdir, True)

	return found

