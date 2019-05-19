import os.path
from jinja2 import Template

BASE_DIR = os.path.normpath(os.path.join(__file__, '..'))

class Type(object):
	def __init__(self):
		self.name = None
		self.ref = False
		self.ptr = False

	def type(self, mods, type):
		self.name = " ".join(mods + [type])
		if self.name == "string":
			self.name = "std::string"
			self.ref = True
		if self.name[0].isupper():
			self.ref = True
			self.ptr = True

	def mangle(self, ref):
		name = self.name
		if self.ptr:
			deps = (name,)
			name = "I%sPtr" %name
			return name, deps
		if self.ref and ref:
			name = "const %s &" %name
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
			"remote.h" : lambda x: "%sRemote.h" %x,
			"remote.cpp": lambda x: "%sRemote.cpp" %x
		}

		for interface in self.interfaces:
			base = interface.base if interface.base else "CoreObject"
			interface.deps.add(base)
			ctx = { "name" : interface.name, "base" : base, "methods" : interface.methods, "deps" : sorted(list(interface.deps)) }
			for template, mangle in templates.items():
				with open(os.path.join(BASE_DIR, template)) as f:
					t = Template(f.read())

				with open(os.path.join(dst_dir, mangle(interface.name)), "w") as f:
					f.write(t.render(ctx))

def generate(interface, dst_dir):
	gen = Generator()
	interface.visit(gen)
	gen.generate(dst_dir)
