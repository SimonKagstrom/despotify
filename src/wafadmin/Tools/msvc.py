#!/usr/bin/env python
# encoding: utf-8
# Carlos Rafael Giani, 2006 (dv)
# Tamas Pal, 2007 (folti)
# Visual C support - beta, needs more testing

import os, sys, re, string, optparse
import Utils, TaskGen, Runner, Configure, Task, Options
from Logs import debug, error, warn
from Utils import quote_whitespace
from TaskGen import taskgen, after, before, feature

from Configure import conftest
import ccroot, cc, cxx, ar
from libtool import read_la_file
from os.path import exists

def msvc_linker(task):
	"""Special linker for MSVC with support for embedding manifests into DLL's
	and executables compiled by Visual Studio 2005 or probably later. Without
	the manifest file, the binaries are unusable.
	See: http://msdn2.microsoft.com/en-us/library/ms235542(VS.80).aspx
	Problems with this tool: it is always called whether MSVC creates manifests or not."""
	e = task.env
	linker = e['LINK']
	srcf = e['LINK_SRC_F']
	trgtf = e['LINK_TGT_F']
	linkflags = e.get_flat('LINKFLAGS')
	libdirs = e.get_flat('_LIBDIRFLAGS')
	libs = e.get_flat('_LIBFLAGS')

	subsystem = getattr(task.generator, 'subsystem', '')
	if subsystem:
		subsystem = '/subsystem:%s' % subsystem
	outfile=task.outputs[0].bldpath(e)
	manifest=outfile+'.manifest'
	# pdb file containing the debug symbols (if compiled with /Zi or /ZI and linked with /debug
	pdbnode=task.outputs[0].change_ext('.pdb')
	pdbfile=pdbnode.bldpath(e)

	objs=" ".join(['"%s"' % a.abspath(e) for a in task.inputs])

	cmd = "%s %s %s%s %s%s %s %s %s" % (linker, subsystem, srcf, objs, trgtf, outfile, linkflags, libdirs, libs)

	# workaround: when run with shell=True, got the following error:
	# 'C:\Program' is not recognized as an internal or external command, operable program or batch file.
	ret = task.generator.bld.exec_command(cmd, shell=False)
	if ret: return ret

	# check for the pdb file. if exists, add to the list of outputs
	if os.path.exists(pdbfile):
		task.outputs.append(pdbnode)

	if os.path.exists(manifest):
		debug('msvc: manifesttool')
		mtool = e['MT']
		if not mtool:
			return 0

		mode = ''
		# embedding mode. Different for EXE's and DLL's.
		# see: http://msdn2.microsoft.com/en-us/library/ms235591(VS.80).aspx
		if 'cprogram' in task.generator.features:
			mode = '1'
		elif 'cshlib' in task.generator.features:
			mode = '2'

		debug('msvc: embedding manifest')
		flags = ' '.join(e['MTFLAGS'] or [])

		cmd='%s %s -manifest "%s" -outputresource:"%s";#%s' % (mtool, flags,
			manifest, outfile, mode)
		ret = task.generator.bld.exec_command(cmd, shell=False)
	return ret

# importlibs provided by MSVC/Platform SDK. Do NOT search them....
g_msvc_systemlibs = """
aclui activeds ad1 adptif adsiid advapi32 asycfilt authz bhsupp bits bufferoverflowu cabinet
cap certadm certidl ciuuid clusapi comctl32 comdlg32 comsupp comsuppd comsuppw comsuppwd comsvcs
credui crypt32 cryptnet cryptui d3d8thk daouuid dbgeng dbghelp dciman32 ddao35 ddao35d
ddao35u ddao35ud delayimp dhcpcsvc dhcpsapi dlcapi dnsapi dsprop dsuiext dtchelp
faultrep fcachdll fci fdi framedyd framedyn gdi32 gdiplus glauxglu32 gpedit gpmuuid
gtrts32w gtrtst32hlink htmlhelp httpapi icm32 icmui imagehlp imm32 iphlpapi iprop
kernel32 ksguid ksproxy ksuser libcmt libcmtd libcpmt libcpmtd loadperf lz32 mapi
mapi32 mgmtapi minidump mmc mobsync mpr mprapi mqoa mqrt msacm32 mscms mscoree
msdasc msimg32 msrating mstask msvcmrt msvcurt msvcurtd mswsock msxml2 mtx mtxdm
netapi32 nmapinmsupp npptools ntdsapi ntdsbcli ntmsapi ntquery odbc32 odbcbcp
odbccp32 oldnames ole32 oleacc oleaut32 oledb oledlgolepro32 opends60 opengl32
osptk parser pdh penter pgobootrun pgort powrprof psapi ptrustm ptrustmd ptrustu
ptrustud qosname rasapi32 rasdlg rassapi resutils riched20 rpcndr rpcns4 rpcrt4 rtm
rtutils runtmchk scarddlg scrnsave scrnsavw secur32 sensapi setupapi sfc shell32
shfolder shlwapi sisbkup snmpapi sporder srclient sti strsafe svcguid tapi32 thunk32
traffic unicows url urlmon user32 userenv usp10 uuid uxtheme vcomp vcompd vdmdbg
version vfw32 wbemuuid  webpost wiaguid wininet winmm winscard winspool winstrm
wintrust wldap32 wmiutils wow32 ws2_32 wsnmp32 wsock32 wst wtsapi32 xaswitch xolehlp
""".split()


def find_lt_names_msvc(self, libname, is_static=False):
	"""
	Win32/MSVC specific code to glean out information from libtool la files.
	this function is not attached to the task_gen class
	"""
	lt_names=[
		'lib%s.la' % libname,
		'%s.la' % libname,
	]

	for path in self.libpaths:
		for la in lt_names:
			laf=os.path.join(path,la)
			dll=None
			if exists(laf):
				ltdict=read_la_file(laf)
				lt_libdir=None
				if ltdict.get('libdir', ''):
					lt_libdir = ltdict['libdir']
				if not is_static and ltdict.get('library_names', ''):
					dllnames=ltdict['library_names'].split()
					dll=dllnames[0].lower()
					dll=re.sub('\.dll$', '', dll)
					return (lt_libdir, dll, False)
				elif ltdict.get('old_library', ''):
					olib=ltdict['old_library']
					if exists(os.path.join(path,olib)):
						return (path, olib, True)
					elif lt_libdir != '' and exists(os.path.join(lt_libdir,olib)):
						return (lt_libdir, olib, True)
					else:
						return (None, olib, True)
				else:
					raise Utils.WafError('invalid libtool object file: %s' % laf)
	return (None, None, None)

def libname_msvc(self, libname, is_static=False):
	lib=libname.lower()
	lib=re.sub('\.lib$','',lib)

	if lib in g_msvc_systemlibs:
		return lib+'.lib'

	lib=re.sub('^lib','',lib)

	if lib == 'm':
		return None

	(lt_path, lt_libname, lt_static) = find_lt_names_msvc(self, lib, is_static)

	if lt_path != None and lt_libname != None:
		if lt_static == True:
			# file existance check has been made by find_lt_names
			return os.path.join(lt_path,lt_libname)

	if lt_path != None:
		_libpaths=[lt_path] + self.libpaths
	else:
		_libpaths=self.libpaths

	static_libs=[
		'%ss.lib' % lib,
		'lib%ss.lib' % lib,
		'%s.lib' %lib,
		'lib%s.lib' % lib,
		]

	dynamic_libs=[
		'lib%s.dll.lib' % lib,
		'lib%s.dll.a' % lib,
		'%s.dll.lib' % lib,
		'%s.dll.a' % lib,
		'lib%s_d.lib' % lib,
		'%s_d.lib' % lib,
		'%s.lib' %lib,
		]

	libnames=static_libs
	if not is_static:
		libnames=dynamic_libs + static_libs

	for path in _libpaths:
		for libn in libnames:
			if os.path.exists(os.path.join(path,libn)):
				debug('msvc: lib found: %s' % os.path.join(path,libn))
				return libn

	return None

@feature('cprogram', 'cshlib', 'cstaticlib')
@after('apply_lib_vars')
@before('apply_obj_vars')
def apply_obj_vars_msvc(self):
	if self.env['CC_NAME'] != 'msvc':
		return

	try:
		self.meths.remove('apply_obj_vars')
	except ValueError:
		pass

	env = self.env
	app = env.append_unique

	cpppath_st       = env['CPPPATH_ST']
	lib_st           = env['LIB_ST']
	staticlib_st     = env['STATICLIB_ST']
	libpath_st       = env['LIBPATH_ST']
	staticlibpath_st = env['STATICLIBPATH_ST']

	for i in env['LIBPATH']:
		app('LINKFLAGS', libpath_st % i)
		if not self.libpaths.count(i):
			self.libpaths.append(i)

	for i in env['LIBPATH']:
		app('LINKFLAGS', staticlibpath_st % i)
		if not self.libpaths.count(i):
			self.libpaths.append(i)

	# i doubt that anyone will make a fully static binary anyway
	if not env['FULLSTATIC']:
		if env['STATICLIB'] or env['LIB']:
			app('LINKFLAGS', env['SHLIB_MARKER'])

	if env['STATICLIB']:
		app('LINKFLAGS', env['STATICLIB_MARKER'])
		for i in env['STATICLIB']:
			debug('msvc: libname: %s' % i)
			libname = libname_msvc(self, i, True)
			debug('msvc: libnamefixed: %s' % libname)
			if libname != None:
				app('LINKFLAGS', libname)

	if self.env['LIB']:
		for i in env['LIB']:
			debug('msvc: libname: %s' % i)
			libname = libname_msvc(self, i)
			debug('msvc: libnamefixed: %s' % libname)
			if libname != None:
				app('LINKFLAGS', libname)

@feature('cprogram', 'cshlib', 'cstaticlib')
@before('apply_link')
def apply_link_msvc(self):
	if self.env['CC_NAME'] != 'msvc':
		return
	link = getattr(self, 'link', None)
	if not link:
		if 'cstaticlib' in self.features: link = 'msvc_ar_link_static'
		elif 'cxx' in self.features: link = 'msvc_cxx_link'
		else: link = 'msvc_cc_link'
		self.vnum = ''
	self.link = link

@feature('cc', 'cxx')
@after('init_cc')
@after('init_cxx')
@before('apply_type_vars')
@before('apply_core')
def init_msvc(self):
	# msvc specific init. must be called after init_cc/init_cxx but before
	# any of their @before declarations.
	try: getattr(self, 'libpaths')
	except AttributeError: self.libpaths = []

static_link_str = '${STLIBLINK} ${STLINKFLAGS} ${LINK_SRC_F}${SRC} ${LINK_TGT_F}${TGT}'
Task.simple_task_type('msvc_ar_link_static', static_link_str, color='YELLOW', ext_in='.o')
Task.task_type_from_func('msvc_cc_link', vars=['LINK', 'LINK_SRC_F', 'LINK_TGT_F', 'LINKFLAGS', '_LIBDIRFLAGS', '_LIBFLAGS', 'MT', 'MTFLAGS'] , color='YELLOW', func=msvc_linker, ext_in='.o')
Task.task_type_from_func('msvc_cxx_link', vars=['LINK', 'LINK_SRC_F', 'LINK_TGT_F', 'LINKFLAGS', '_LIBDIRFLAGS', '_LIBFLAGS', 'MT', 'MTFLAGS'] , color='YELLOW', func=msvc_linker, ext_in='.o')

rc_str='${RC} ${RCFLAGS} /fo ${TGT} ${SRC}'
Task.simple_task_type('rc', rc_str, color='GREEN', before='cc cxx')

import winres

detect = '''
find_msvc
msvc_common_flags
cc_load_tools
cxx_load_tools
cc_add_flags
cxx_add_flags
'''

@conftest
def find_msvc(conf):
	# due to path format limitations, limit operation only to native Win32. Yeah it sucks.
	if sys.platform != 'win32':
		conf.fatal('MSVC module only works under native Win32 Python! cygwin is not supported yet')

	v = conf.env

	# compiler
	cxx = None
	if v['CXX']: cxx = v['CXX']
	elif 'CXX' in os.environ: cxx = os.environ['CXX']
	if not cxx: cxx = conf.find_program('CL', var='CXX')
	if not cxx: conf.fatal('CL was not found (compiler)')

	# c/c++ compiler - check for whitespace, and if so, add quotes
	v['CXX']  = cxx
	v['CC'] = v['CXX']
	v['CXX_NAME'] = 'msvc'
	v['CC_NAME'] = 'msvc'

	# linker
	if not v['LINK_CXX']:
		link = conf.find_program('LINK')
		if link: v['LINK_CXX'] = link
		else: conf.fatal('LINK was not found (linker)')
	v['LINK']            = link

	if not v['LINK_CC']: v['LINK_CC'] = v['LINK_CXX']

	# staticlib linker
	if not v['STLIBLINK']:
		stliblink = conf.find_program('LIB')
		if not stliblink: return
		v['STLIBLINK']       = stliblink
		v["STLINKFLAGS"]  = ['/NOLOGO']

	# manifest tool. Not required for VS 2003 and below. Must have for VS 2005 and later
	manifesttool = conf.find_program('MT')
	if manifesttool:
		v['MT'] = manifesttool
		v['MTFLAGS']=['/NOLOGO']

	conf.check_tool('winres')

	if not conf.env['WINRC']:
		warn('Resource compiler not found. Compiling resource file is disabled')

@conftest
def msvc_common_flags(conf):
	v = conf.env

	v['CPPFLAGS']     = ['/W3', '/nologo', '/EHsc', '/errorReport:prompt']
	v['CCDEFINES']    = ['WIN32'] # command-line defines
	v['CXXDEFINES']   = ['WIN32'] # command-line defines

	v['_CCINCFLAGS']  = []
	v['_CCDEFFLAGS']  = []
	v['_CXXINCFLAGS'] = []
	v['_CXXDEFFLAGS'] = []

	v['CC_SRC_F']     = ''
	v['CC_TGT_F']     = '/c /Fo'
	v['CXX_SRC_F']    = ''
	v['CXX_TGT_F']    = '/c /Fo'

	v['CPPPATH_ST']   = '/I%s' # template for adding include paths

	# Subsystem specific flags
	v['CPPFLAGS_CONSOLE']   = ['/SUBSYSTEM:CONSOLE']
	v['CPPFLAGS_NATIVE']    = ['/SUBSYSTEM:NATIVE']
	v['CPPFLAGS_POSIX']     = ['/SUBSYSTEM:POSIX']
	v['CPPFLAGS_WINDOWS']   = ['/SUBSYSTEM:WINDOWS']
	v['CPPFLAGS_WINDOWSCE']	= ['/SUBSYSTEM:WINDOWSCE']

	# CRT specific flags
	v['CPPFLAGS_CRT_MULTITHREADED'] =						['/MT']
	v['CPPFLAGS_CRT_MULTITHREADED_DLL'] =				['/MD']
	v['CPPDEFINES_CRT_MULTITHREADED'] =					['_MT']
	v['CPPDEFINES_CRT_MULTITHREADED_DLL'] =			['_MT', '_DLL']

	v['CPPFLAGS_CRT_MULTITHREADED_DBG'] =				['/MTd']
	v['CPPFLAGS_CRT_MULTITHREADED_DLL_DBG'] =		['/MDd']
	v['CPPDEFINES_CRT_MULTITHREADED_DBG'] =					['_DEBUG', '_MT']
	v['CPPDEFINES_CRT_MULTITHREADED_DLL_DBG'] =			['_DEBUG', '_MT', '_DLL']

	# compiler debug levels
	v['CCFLAGS']            = ['/TC']
	v['CCFLAGS_OPTIMIZED']  = ['/O2', '/DNDEBUG']
	v['CCFLAGS_RELEASE']    = ['/O2', '/DNDEBUG']
	v['CCFLAGS_DEBUG']      = ['/Od', '/RTC1', '/D_DEBUG', '/ZI']
	v['CCFLAGS_ULTRADEBUG'] = ['/Od', '/RTC1', '/D_DEBUG', '/ZI']

	v['CXXFLAGS']            = ['/TP']
	v['CXXFLAGS_OPTIMIZED']  = ['/O2', '/DNDEBUG']
	v['CXXFLAGS_RELEASE']    = ['/O2', '/DNDEBUG']
	v['CXXFLAGS_DEBUG']      = ['/Od', '/RTC1', '/D_DEBUG', '/ZI']
	v['CXXFLAGS_ULTRADEBUG'] = ['/Od', '/RTC1', '/D_DEBUG', '/ZI']

	# linker
	v['LIB']                 = []

	v['LINK_TGT_F']        = '/OUT:'
	v['LINK_SRC_F']        = ' '

	v['LIB_ST']              = '%s.lib' # template for adding libs
	v['LIBPATH_ST']          = '/LIBPATH:%s' # template for adding libpaths
	v['STATICLIB_ST']        = '%s.lib'
	v['STATICLIBPATH_ST']    = '/LIBPATH:%s'
	v['CCDEFINES_ST']       = '/D%s'
	v['CXXDEFINES_ST']       = '/D%s'
	v['_LIBDIRFLAGS']        = ''
	v['_LIBFLAGS']           = ''

	v['SHLIB_MARKER']        = ''
	v['STATICLIB_MARKER']    = ''

	v['LINKFLAGS']           = ['/NOLOGO', '/MACHINE:X86', '/ERRORREPORT:PROMPT']

	# shared library
	v['shlib_CCFLAGS']  = ['']
	v['shlib_CXXFLAGS'] = ['']
	v['shlib_LINKFLAGS']= ['/DLL']
	v['shlib_PATTERN']  = '%s.dll'

	# static library
	v['staticlib_LINKFLAGS'] = ['']
	v['staticlib_PATTERN']   = '%s.lib'

	# program
	v['program_PATTERN']     = '%s.exe'

