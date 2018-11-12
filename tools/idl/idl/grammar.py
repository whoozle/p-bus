from pyparsing import *
import idl

def on_type(s, l, t):
	return idl.Type(t[0])

def on_argument(s, l, t):
	return idl.Argument(t[0][0], t[0][1])

def on_interface(s, l, t):
	if len(t[0]) > 1:
		name, base = t[0]
	else:
		name, base = t[0][0], None

	interface = idl.Interface(name, base)
	for t in t[1]:
		if isinstance(t, idl.Method):
			interface.methods.append(t)
	return interface

def on_method(s, l, t):
	rtype, name, args = t
	return idl.Method(rtype, name, args.asList())

identifier = Word(alphas, alphanums)

type_modifier = Keyword("unsigned") | Keyword("signed") | Keyword("short") | Keyword("long")
type_declaration = Group(OneOrMore(type_modifier) | (ZeroOrMore(type_modifier) + identifier))
type_declaration.setParseAction(on_type)

argument_declaration = Group(type_declaration + identifier)
argument_declaration.setParseAction(on_argument)

method_declaration = type_declaration + identifier + Suppress("(") + Group(Optional(delimitedList(argument_declaration))) + Suppress(")")
method_declaration.setParseAction(on_method)

attribute_declaration = Keyword("attribute") + identifier

interface_property = method_declaration | attribute_declaration
interface_properties = ZeroOrMore(interface_property + Suppress(";"))

base_class = Suppress(":") + identifier

interface_declaration = Keyword("interface").suppress() + Group(identifier + Optional(base_class)) + Suppress("{") + Group(Optional(interface_properties)) + Suppress("}")
interface_declaration.ignore(cStyleComment)
interface_declaration.ignore(cppStyleComment)
interface_declaration.setParseAction(on_interface)

def parse(text):
	return interface_declaration.parseString(text, parseAll = True)
