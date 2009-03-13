#! /usr/bin/env python
# encoding: utf-8
# Thomas Nagy, 2006-2008 (ita)

"GLib2 support"

import Task, Utils
from TaskGen import taskgen, before, after, feature

#
# glib-genmarshal
#

@taskgen
def add_marshal_file(self, filename, prefix):
	if not hasattr(self, 'marshal_list'):
		self.marshal_list = []
	self.meths.append('process_marshal')
	self.marshal_list.append((filename, prefix))

@taskgen
@before('apply_core')
def process_marshal(self):
	for filename, prefix in getattr(self, 'marshal_list', []):
		node = self.path.find_resource(filename)

		if not node:
			raise Utils.WafError('file not found ' + filename)

		# Generate the header
		header_env = self.env.copy()
		header_env['GLIB_GENMARSHAL_PREFIX'] = prefix
		header_env['GLIB_GENMARSHAL_MODE'] = '--header'
		task = self.create_task('glib_genmarshal', header_env)
		task.set_inputs(node)
		task.set_outputs(node.change_ext('.h'))

		# Generate the body
		body_env = self.env.copy()
		body_env['GLIB_GENMARSHAL_PREFIX'] = prefix
		body_env['GLIB_GENMARSHAL_MODE'] = '--body'
		task = self.create_task('glib_genmarshal', body_env)
		task.set_inputs(node)
		task.set_outputs(node.change_ext('.c'))
		# the c file generated will be processed too
		outnode = node.change_ext('.c')
		self.allnodes.append(outnode)


#
# glib-mkenums
#

@taskgen
def add_enums_from_template(self, source='', target='', template='', comments=''):
	if not hasattr(self, 'enums_list'):
		self.enums_list = []
	self.meths.append('process_enums')
	self.enums_list.append({'source': source,
	                        'target': target,
	                        'template': template,
	                        'file-head': '',
	                        'file-prod': '',
	                        'file-tail': '',
	                        'enum-prod': '',
	                        'value-head': '',
	                        'value-prod': '',
	                        'value-tail': '',
	                        'comments': comments})

@taskgen
def add_enums(self, source='', target='',
              file_head='', file_prod='', file_tail='', enum_prod='',
              value_head='', value_prod='', value_tail='', comments=''):
	if not hasattr(self, 'enums_list'):
		self.enums_list = []
	self.meths.append('process_enums')
	self.enums_list.append({'source': source,
	                        'template': '',
	                        'target': target,
	                        'file-head': file_head,
	                        'file-prod': file_prod,
	                        'file-tail': file_tail,
	                        'enum-prod': enum_prod,
	                        'value-head': value_head,
	                        'value-prod': value_prod,
	                        'value-tail': value_tail,
	                        'comments': comments})

@taskgen
@before('apply_core')
def process_enums(self):
	for enum in getattr(self, 'enums_list', []):
		# temporary
		env = self.env.copy()
		task = self.create_task('glib_mkenums', env)
		inputs = []

		# process the source
		source_list = self.to_list(enum['source'])
		if not source_list:
			raise Utils.WafError('missing source ' + str(enum))
		source_list = [self.path.find_resource(k) for k in source_list]
		inputs += source_list
		env['GLIB_MKENUMS_SOURCE'] = [k.abspath(env) for k in source_list]

		# find the target
		if not enum['target']:
			raise Utils.WafError('missing target ' + str(enum))
		tgt_node = self.path.find_or_declare(enum['target'])
		if tgt_node.name.endswith('.c'):
			self.allnodes.append(tgt_node)
		env['GLIB_MKENUMS_TARGET'] = tgt_node.abspath(env)


		options = []

		if enum['template']: # template, if provided
			template_node = self.path.find_resource(enum['template'])
			options.append('--template %s' % (template_node.abspath(env)))
			inputs.append(template_node)
		params = {'file-head' : '--fhead',
		           'file-prod' : '--fprod',
		           'file-tail' : '--ftail',
		           'enum-prod' : '--eprod',
		           'value-head' : '--vhead',
		           'value-prod' : '--vprod',
		           'value-tail' : '--vtail',
		           'comments': '--comments'}
		for param, option in params.iteritems():
			if enum[param]:
				options.append('%s %r' % (option, enum[param]))

		env['GLIB_MKENUMS_OPTIONS'] = ' '.join(options)

		# update the task instance
		task.set_inputs(inputs)
		task.set_outputs(tgt_node)



Task.simple_task_type('glib_genmarshal',
	'${GLIB_GENMARSHAL} ${SRC} --prefix=${GLIB_GENMARSHAL_PREFIX} ${GLIB_GENMARSHAL_MODE} > ${TGT}',
	color='BLUE', before='cc')
Task.simple_task_type('glib_mkenums',
	'${GLIB_MKENUMS} ${GLIB_MKENUMS_OPTIONS} ${GLIB_MKENUMS_SOURCE} > ${GLIB_MKENUMS_TARGET}',
	color='PINK', before='cc')


def detect(conf):
	glib_genmarshal = conf.find_program('glib-genmarshal', var='GLIB_GENMARSHAL')
	mk_enums_tool = conf.find_program('glib-mkenums', var='GLIB_MKENUMS')

