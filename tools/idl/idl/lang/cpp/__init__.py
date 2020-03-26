import os.path
from jinja2 import Template

BASE_DIR = os.path.normpath(os.path.join(__file__, '..'))

def split_type(typename):
	comps = typename.split('.')
	package = '.'.join(comps[:-1])
	return package, package.replace('.', '::'), comps[-1]

class Type(object):
	def __init__(self):
		self.fullname = None
		self.name = None
		self.package = None
		self.ns = None
		self.ref = False
		self.ptr = False

	def type(self, mods, type):
		self.fullname = " ".join(mods + [type])
		self.package, self.ns, self.name = split_type(self.fullname)
		if self.name == "string":
			self.ns = "std"
			self.ref = True
		if self.name[0].isupper():
			self.ref = True
			self.ptr = True

	def mangle(self, ref):
		name = self.fullname
		if self.ptr:
			deps = (name,)
			name = "%s::I%sPtr" %(self.ns, self.name) if self.ns else "I%sPtr" %(self.name)
			return name, deps
		if self.ref and ref:
			name = "const %s::%s &" %(self.ns, name) if self.ns else "const %s &" %name
		return name, ()

class Argument(object):
	def __init__(self, name):
		self.name = name
		self.deps = set()
	def begin_argument_type(self):
		return Type()
	def end_argument_type(self, type):
		self.type, deps = type.mangle(True)
		self.deps.update(deps)

class Method(object):
	def __init__(self, name, rtype):
		self.name = name
		self.rtype = None
		self.args = []
		self.deps = set()

	def type(self, mods, type):
		t = Type()
		t.type(mods, type)
		self.rtype, deps = t.mangle(False)
		self.deps.update(deps)

	def begin_argument(self, name):
		return Argument(name)

	def end_argument(self, arg):
		self.args.append(arg)

class Interface(object):
	def __init__(self, name, base):
		self.name, self.base = name, base
		self.methods = []
		self.deps = set()

	def begin_method(self, name, rtype):
		return Method(name, rtype)

	def end_method(self, method):
		self.methods.append(method)
		self.deps.update(method.deps)

class Generator(object):
	def __init__(self):
		self.interfaces = []

	def begin_interface(self, name, base):
		return Interface(name, base)

	def end_interface(self, interface):
		self.interfaces.append(interface)

	def generate(self, dst_dir):
		templates = {
			"interface.h": lambda x: "I%s.h" %x,
			"interface.cpp": lambda x: "I%s.cpp" %x,
			"remote.h" : lambda x: "%s.h" %x,
			"remote.cpp": lambda x: "%s.cpp" %x
		}

		for interface in self.interfaces:
			full_base = interface.base if interface.base else "core.CoreObject"
			base_package, base_namespace, base = split_type(full_base)
			package, package_namespace, name = split_type(interface.name)
			package_path = package.split('.')
			interface.deps.add(full_base)
			ctx = {
				"name" : name,
				"base" : base,
				"base_package": base_package,
				"base_namespace": base_namespace,
				"methods" : interface.methods,
				"package" : package,
				"package_components": package_path,
				"namespace": package_namespace,
				"deps" : sorted(map(split_type, list(interface.deps)))
			}
			for template, mangle in templates.items():
				with open(os.path.join(BASE_DIR, template)) as f:
					t = Template(f.read())

				dst_package_dir = os.path.join(dst_dir, *package_path)
				os.makedirs(dst_package_dir, exist_ok = True)
				with open(os.path.join(dst_package_dir, mangle(name)), "w") as f:
					f.write(t.render(ctx))

def generate(interface, dst_dir):
	gen = Generator()
	interface.visit(gen)
	gen.generate(dst_dir)
