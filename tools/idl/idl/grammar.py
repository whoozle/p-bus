from pyparsing import *


identifier = Word(alphas, alphanums)

type_modifier = Keyword("unsigned") | Keyword("signed") | Keyword("short") | Keyword("long")
type_declaration = Group(OneOrMore(type_modifier)) | Group(ZeroOrMore(type_modifier) + identifier)

argument_declaration = type_declaration + identifier

method_declaration = type_declaration + identifier + Suppress("(") + Group(Optional(delimitedList(argument_declaration))) + Suppress(")")
attribute_declaration = Keyword("attribute") + identifier

interface_property = method_declaration | attribute_declaration
interface_properties = ZeroOrMore(interface_property + Suppress(";"))

base_class = Suppress(":") + identifier

interface_declaration = Keyword("interface").suppress() + Group(identifier + Optional(base_class)) + Suppress("{") + Group(Optional(interface_properties)) + Suppress("}")
interface_declaration.ignore(cStyleComment)
interface_declaration.ignore(cppStyleComment)

def parse(text):
	return interface_declaration.parseString(text, parseAll = True)
