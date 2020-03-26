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
	r'[a-zA-Z$_][a-zA-Z$_0-9\.]*'
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
	p[0] = p[1]

def p_primitive(p):
	"""
		primitive 	: primitive type_atom
					| type_atom
	"""
	n = len(p)
	if n == 2:
		p[0] = idl.Type()
		p[0].add(p[1])
	elif n == 3:
		p[0] = p[1]
		p[1].add(p[2])

def p_type(p):
	"""
		type 	: primitive
				| IDENTIFIER
	"""
	if isinstance(p[1], idl.Type):
		p[0] = p[1]
	else:
		p[0] = idl.Type()
		p[0].add(p[1])
	#print('type', p[1])

def p_argument(p):
	"""
		argument : type IDENTIFIER
	"""
	p[0] = idl.Argument(p[1], p[2])


def p_argument_list(p):
	"""
		argument_list 	: argument_list ',' argument
						| argument
						| empty
	"""
	n = len(p)
	if n == 4:
		p[1].append(p[3])
		p[0] = p[1]
	elif n == 2:
		p[0] = [p[1]] if p[1] is not None else []

def p_method(p):
	"""
	method : type IDENTIFIER '(' argument_list ')'
	"""
	p[0] = idl.Method(p[1], p[2], p[4])

def p_declaration(p):
	"""
		declaration : method
	"""
	p[0] = p[1]

def p_empty(p):
	""" empty : """

def p_declaration_list(p):
	"""
		declaration_list : declaration_list declaration ';'
			| declaration ';'
			| empty
	"""
	n = len(p)
	if n == 4:
		p[0] = p[1]
		p[0].append(p[2])
	elif n == 3:
		p[0] = [p[1]]
	elif n == 2:
		p[0] = []
	else:
		raise Exception("unhandled declaration_list")

def p_scope(p):
	"""
		scope : '{' declaration_list '}'
	"""
	p[0] = p[2]

def p_interface_declaration(p):
	"""
		interface_declaration 	: INTERFACE IDENTIFIER scope
								| INTERFACE IDENTIFIER ':' IDENTIFIER scope
	"""
	n = len(p)
	if n == 4:
		p[0] = idl.Interface(p[2])
		p[0].declarations = p[3]
	elif n == 6:
		p[0] = idl.Interface(p[2], p[4])
		p[0].declarations = p[5]
	else:
		raise Exception("invalid argument count")

start = 'interface_declaration'
parser = yacc.yacc()

def parse(text):
	return parser.parse(text)
