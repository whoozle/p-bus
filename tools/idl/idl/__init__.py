class Type(object):
	def __init__(self, decl):
		self.signed = True
		self.size = 4
		self.type = None
		for t in decl:
			if t == 'short':
				self.size = 2
			elif t == 'long':
				self.size = 8
			elif t == 'unsigned':
				self.signed = False
			elif t == 'signed':
				self.signed = True
			elif self.type is None:
				self.type = t
			else:
				raise Exception('unknown type %s' %t)
		if self.type is None:
			self.type = 'int'

	def __repr__(self):
		flags = ''
		if self.type == 'int':
			if not self.signed:
				flags = 'u'
			if self.size == 8:
				flags += 'long'
			elif self.size == 4:
				flags += 'int'
			elif self.size == 2:
				flags += 'short'
			elif self.size == 1:
				flags += 'char'
		if not flags:
			flags = self.type
		return flags

class Argument(object):
	def __init__(self, type, name):
		self.type, self.name = type, name
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
		obj.end_interface()

class Method(object):
	def __init__(self, rtype, name, args):
		self.name = name
		self.rtype = rtype
		self.args = args

	def visit(self, visitor):
		print METHOD
