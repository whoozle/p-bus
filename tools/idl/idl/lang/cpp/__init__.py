import os.path
from jinja2 import Template

BASE_DIR = os.path.normpath(os.path.join(__file__, '..'))

class Type(object):
	def __init__(self):
		self.name = None
	def type(self, mods, type):
		self.name = " ".join(mods + [type])
	def __str__(self):
		return self.name

class Argument(object):
	def __init__(self, name):
		self.name = name
	def begin_argument_type(self):
		return Type()
	def end_argument_type(self, type):
		self.type = type.name
	def __str__(self):
		return "%s %s" %(self.type, self.name)

class Method(object):
	def __init__(self, name, rtype):
		self.name = name
		self.rtype = rtype
		self.args = []

	def set_type(self, type):
		print(type)
		self.rtype = type

	def begin_argument(self, name):
		return Argument(name)

	def end_argument(self, arg):
		self.args.append(arg)

	def declaration(self, interface):
		return "%s %s (%s)" %(str(self.return_type), self.name, ", ".join(map(str, self.args)))

	def definition(self, interface):
		return "%s %s::%s (%s) { }" %(str(self.return_type), interface.name, self.name, ", ".join(map(str, self.args)))

class Interface(object):
	def __init__(self, name, base):
		self.name, self.base = name, base
		self.methods = []

	def begin_method(self, name, rtype):
		return Method(name, rtype)

	def end_method(self, method):
		self.methods.append(method)


class Generator(object):
	def __init__(self):
		self.interfaces = []

	def begin_interface(self, name, base):
		return Interface(name, base)

	def end_interface(self, interface):
		self.interfaces.append(interface)

	def generate(self):
		with open(os.path.join(BASE_DIR, "template.h")) as f:
			template_h = Template(f.read())
		with open(os.path.join(BASE_DIR, "template.cpp")) as f:
			template_cpp = Template(f.read())

		for interface in self.interfaces:
			ctx = { "name" : interface.name, "base" : interface.base, "methods" : interface.methods }
			print(template_h.render(ctx))

		for interface in self.interfaces:
			ctx = { "name" : interface.name, "base" : interface.base, "methods" : interface.methods }
			print(template_cpp.render(ctx))

def generate(interface):
	gen = Generator()
	interface.visit(gen)
	gen.generate()
