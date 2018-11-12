class Type(object):
	def __init__(self):
		pass
	def mods(self, mods):
		pass
	def type(self, name):
		pass
	def end_return_type(self, parent):
		pass

class Argument(object):
	def __init__(self, name):
		pass
	def begin_return_type(self):
		return Type()
	def end_argument(self, method):
		pass

class Method(object):
	def __init__(self, name, rtype):
		pass

	def begin_argument(self, name):
		return Argument(name)

	def end_method(self, interface):
		pass

class Interface(object):
	def __init__(self, gen):
		self.gen = gen

	def begin_method(self, name, rtype):
		return Method(name, rtype)

	def end_interface(self, gen):
		pass

class Generator(object):
	def __init__(self):
		pass

	def begin_interface(self, name, base):
		return Interface(self)

def generate(interface):
	interface.visit(Generator())
