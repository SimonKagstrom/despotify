#!/usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2008

"""
Nasm processing
"""

import os
import TaskGen, Task
from TaskGen import taskgen, before, extension

nasm_str = '${NASM} ${NASM_FLAGS} ${NASM_INCLUDES} ${SRC} -o ${TGT}'

EXT_NASM = ['.s', '.S', '.asm', '.ASM', '.spp', '.SPP']

@taskgen
@before('apply_link')
def apply_nasm_vars(self):

	# flags
	if hasattr(self, 'nasm_flags'):
		for flag in self.to_list(self.nasm_flags):
			self.env.append_value('NASM_FLAGS', flag)

	# includes - well, if we suppose it works with c processing
	if hasattr(self, 'includes'):
		for inc in self.to_list(self.includes):
			node = self.path.find_dir(inc)
			if not node:
				raise ValueError("cannot find the dir" + inc)
			self.env.append_value('NASM_INCLUDES', '-I %s' % node.srcpath(self.env))
			self.env.append_value('NASM_INCLUDES', '-I %s' % node.bldpath(self.env))

@extension(EXT_NASM)
def nasm_file(self, node):
	try: obj_ext = self.obj_ext
	except AttributeError: obj_ext = '_%d.o' % self.idx

 	task = self.create_task('nasm')
	task.inputs = [node]
	task.outputs = [node.change_ext(obj_ext)]

	self.compiled_tasks.append(task)

	self.meths.append('apply_nasm_vars')

# create our action here
Task.simple_task_type('nasm', nasm_str, color='BLUE', ext_out='.o')

def detect(conf):
	nasm = conf.find_program('nasm', var='NASM')
	if not nasm: nasm = conf.find_program('yasm', var='NASM')
	if not nasm: conf.fatal('could not find nasm (or yasm), install it or set PATH env var')

