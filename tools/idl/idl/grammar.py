from __future__ import unicode_literals, print_function
import idl
import ply.lex as lex
import ply.yacc as yacc

reserved = {
	'interface': 'INTERFACE',
	'string': 'STRING',
	'unsigned': 'UNSIGNED',
	'signed': 'SIGNED',
	'void': 'VOID',
	'int': 'INT',
	'short': 'SHORT',
	'long': 'LONG',
	'byte': 'BYTE',
	'char': 'CHAR',
}

tokens = [
	'IDENTIFIER'
] + list(reserved.values())

literals = [ '{', '}' , '(', ')', ',', ';', ':' ]

t_ignore = ' \f\r\t'

def t_IDENTIFIER(t):
	r'[a-zA-Z$_][a-zA-Z$_0-9]*'
	t.type = reserved.get(t.value, 'IDENTIFIER')
	return t

# C or C++ comment (ignore)
def t_ccode_comment(t):
	r'(/\*(.|\n)*?\*/)|(//.*)'
	t.lexer.lineno += t.value.count('\n')

def t_newline(t):
    r'\n+'
    t.lexer.lineno += t.value.count("\n")

def t_error(t):
    raise(Exception("Illegal character '%s'" % t.value[0]))

lex.lex()

def p_error(p):
    raise(Exception("Syntax error at '%s', line %d" % (p.value, p.lexer.lineno)))

def p_type_atom(p):
	""" type_atom : STRING
		| SIGNED
		| UNSIGNED
		| INT
		| LONG
		| BYTE
		| CHAR
		| SHORT
		| VOID
	"""

def p_primitive(p):
	"""
		primitive 	: primitive type_atom
					| type_atom
	"""

def p_type(p):
	"""
		type 	: primitive
				| IDENTIFIER
	"""

def p_argument(p):
	"""
		argument : type IDENTIFIER
	"""


def p_argument_list(p):
	"""
		argument_list 	: argument_list ',' argument
						| argument
	"""

def p_method(p):
	"""
	method : type IDENTIFIER '(' argument_list ')'
	"""

def p_declaration(p):
	"""
		declaration : method
	"""

def p_declaration_list(p):
	"""
		declaration_list : declaration_list declaration ';'
			| declaration ';'
	"""

def p_scope(p):
	"""
		scope : '{' declaration_list '}'
	"""

def p_interface_declaration(p):
	"""
		interface_declaration 	: INTERFACE IDENTIFIER scope
								| INTERFACE IDENTIFIER ':' IDENTIFIER scope
	"""
	print(p)

start = 'interface_declaration'
parser = yacc.yacc()

def parse(text):
	return parser.parse(text)
