class Type(object):
	def __init__(self):
		self.mods, self.type = [], None

	def add(self, type):
		if type in set(['unsigned', 'signed', 'short', 'long']):
			self.mods.append(type)
		else:
			if self.type is not None:
				raise Exception("duplicate type %s in %s" %(type, self.type))
			self.type = type

	def visit(self, visitor):
		visitor.type(self.mods, self.type)

	def __repr__(self):
		return 'Type(%s)' % ' '.join(self.mods + [self.type if self.type is not None else '<no-type>'])

class Argument(object):
	def __init__(self, type, name):
		self.type, self.name = type, name

	def visit(self, visitor):
		arg = visitor.begin_argument(self.name)
		rtype = arg.begin_argument_type()
		self.type.visit(rtype)
		arg.end_argument_type(rtype)
		visitor.end_argument(arg)

	def __repr__(self):
		return 'Argument(%s, %s)' %(self.type, self.name)

class Interface(object):
	def __init__(self, name, base = None):
		self.name = name
		self.base = base
		self.declarations = []

	def visit(self, visitor):
		obj = visitor.begin_interface(self.name, self.base)
		for method in self.declarations:
			method.visit(obj)
		visitor.end_interface(obj)

class Method(object):
	def __init__(self, rtype, name, args, static = False):
		self.name = name
		self.rtype = rtype
		self.args = args
		self.static = static

	def visit(self, visitor):
		method = visitor.begin_method(self.name, self.rtype)
		self.rtype.visit(method)
		for arg in self.args:
			arg.visit(method)
		visitor.end_method(method)
