from __future__ import unicode_literals, print_function
from pypeg2 import *

class Type(object):
	grammar = name()

class Argument(object):
	grammar = attr('type', Type), name()

class Arguments(object):
	grammar = optional(csl(Argument))

class Method(object):
	grammar = Type, name(), "(", Arguments, ")"

class BaseInterface(object):
	grammar = ":", name()

class Declaration(object):
	grammar = [Method], ";"

class Interface(Namespace):
	grammar = K("interface"), name(), optional(attr('base', BaseInterface)), "{", maybe_some(Declaration), "}"
