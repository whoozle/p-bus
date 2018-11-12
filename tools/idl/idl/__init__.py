class Type(object):
	def __init__(self, mods, type):
		self.mods, self.type = mods, type

	def visit(self, visitor):
		visitor.mods(self.mods)
		visitor.type(self.type)

	def __repr__(self):
		return ' '.join(self.mods + [self.type])

class Argument(object):
	def __init__(self, type, name):
		self.type, self.name = type, name

	def visit(self, visitor):
		arg = visitor.begin_argument(self.name)
		rtype = arg.begin_return_type()
		self.type.visit(rtype)
		rtype.end_return_type(visitor)

	def __repr__(self):
		return 'Argument(%s, %s)' %(self.type, self.name)

class Interface(object):
	def __init__(self, name, base = None):
		self.name = name
		self.base = base
		self.methods = []

	def visit(self, visitor):
		obj = visitor.begin_interface(self.name, self.base)
		for method in self.methods:
			method.visit(obj)
		obj.end_interface(visitor)

class Method(object):
	def __init__(self, rtype, name, args):
		self.name = name
		self.rtype = rtype
		self.args = args

	def visit(self, visitor):
		method = visitor.begin_method(self.name, self.rtype)
		for arg in self.args:
			arg.visit(method)
		method.end_method(visitor)
