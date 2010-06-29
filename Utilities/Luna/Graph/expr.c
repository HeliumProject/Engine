/*
Copyright (c) 2007 Andre de Leiradella

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "expr.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define inline __inline

/* Tokens. */
#define EOE	0
#define NUM	1
#define ID		2
#define MUL	3
#define DIV	4
#define MOD	5
#define ADD	6
#define SUB	7
#define LPAR	8
#define RPAR	9
#define COMMA	10
#define SHL	11
#define SHR	12
#define LT		13
#define GT		14
#define LE		15
#define GE		16
#define NEG	17
#define NOT	18
#define EQ		19
#define NE		20
#define AND	21
#define XOR	22
#define OR		23
#define INC	24
#define DEC	25
#define LAND	26
#define LOR	27
#define LET	28
#define ADDLET	29
#define SUBLET	30
#define MULLET	31
#define DIVLET	32
#define MODLET	33
#define ANDLET	34
#define XORLET	35
#define ORLET	36
#define SHLLET	37
#define SHRLET	38
#define QUEST	39
#define COLON	40
#define OCTAL	41
#define HEX	42

/* Maximum stack size. */
#define MAX_STACK	32

/* Maximum number of parameters. */
#define MAX_PARAMS	8

/* Values on the operand stack. */
typedef struct {
	int	type;
	union {
		double		d;
		expr_variable_t	*v;
	};
} value_t;

/* The parser structure. */
typedef struct {
	/* Configuration. */
	expr_config_t	*config;
	/* The lookahead. */
	char			lexeme[MAX_ID], *lexeme_max;
	int			token;
	/* The operand stack. */
	value_t		stack[MAX_STACK], *stack_pointer, *stack_max;
	/* The error code. */
	int			error;
} parser_t;

static double str2u(const char *str, int base) {
	double num = 0;
	while (*str != '\0') {
		int digit = tolower(*str++) - '0';
		if (digit > 9)
			digit -= 'a' - '0';
		num = num * base + digit;
	}
	return num;
}

/* return true if k is a space character. */
static inline int is_space(char k) {
	return isspace(k);
}

/* Return true if k is a digit. */
static inline int is_digit(char k) {
	return isdigit(k);
}

/* Return true if k if a hexadecimal digit. */
static inline int is_hexdigit(char k) {
	return isxdigit(k);
}

/* Return true if k is a letter of '_'. */
static inline int is_alpha(char k) {
	return isalpha(k) || k == '_';
}

/* Match any token in the expression and retrieves the next one. */
static int match_any(parser_t *p) {
	int	k;
	char	*lexeme;

	/* Skip spaces. */
	do
		k = p->config->get_char(p->config->user_data);
	while (k != -1 && is_space(k));
	/* Return EOE if we've reached the end of the input. */
	if (k == -1) {
		p->token = EOE;
		strcpy_s(p->lexeme, sizeof(p->lexeme), "EOE");
		return 1;
	}
	lexeme = p->lexeme;
	/* If the character is a digit or a dot, the token is a number. */
	if (is_digit(k) || k == '.') {
		/* Assume a NUM for now... */
		p->token = NUM;
		/* If the character is a dot, add it to the lexeme and skip it. */
		if (k == '.') {
			*lexeme++ = k;
			k = p->config->get_char(p->config->user_data);
		}
		/* Check for octal and hexadecimal numbers. */
		else if (k == '0') {
			/* Skip the leading zero... */
			*lexeme++ = k;
			k = p->config->get_char(p->config->user_data);
			if (k == 'x') {
				/* Grab the hexadecimal number. */
				*lexeme++ = 'x';
				k = p->config->get_char(p->config->user_data);
				while (is_hexdigit(k)) {
					if (lexeme == p->lexeme_max) goto toobig;
					*lexeme++ = k;
					k = p->config->get_char(p->config->user_data);
				}
				p->config->unget_char(p->config->user_data);
				p->token = HEX;
				goto ok;
			} else if (is_digit(k)) {
				/* Grab the octal number. */
				while (is_digit(k) && k != '8' && k != '9') {
					if (lexeme == p->lexeme_max) goto toobig;
					*lexeme++ = k;
					k = p->config->get_char(p->config->user_data);
				}
				/* Check for an invalid digit. */
				if (k == '8' || k == '9') {
					*lexeme = '\0';
					sprintf_s(p->config->id, sizeof(p->config->id), "%c", k);
					p->error = EXPR_INVALID_DIGIT;
					return 0;
				}
				p->config->unget_char(p->config->user_data);
				p->token = OCTAL;
				goto ok;
			}
			/* Else fall through and read a regular decimal number. */
		}
		/* Get all digits. */
		while (is_digit(k)) {
			if (lexeme == p->lexeme_max) goto toobig;
			*lexeme++ = k;
			k = p->config->get_char(p->config->user_data);
		}
		/* Check for decimals. */
		if (k == '.') {
			if (lexeme == p->lexeme_max) goto toobig;
			*lexeme++ = k;
			k = p->config->get_char(p->config->user_data);
			/* Get any digits after the decimal digit. */
			while (is_digit(k)) {
				if (lexeme == p->lexeme_max) goto toobig;
				*lexeme++ = k;
				k = p->config->get_char(p->config->user_data);
			}
		}
		/* Check for scientific notation. */
		if (k == 'e' || k == 'E') {
			if (lexeme == p->lexeme_max) goto toobig;
			*lexeme++ = k;
			k = p->config->get_char(p->config->user_data);
			/* Check optional exponent sign. */
			if (k == '+' || k == '-') {
				if (lexeme == p->lexeme_max) goto toobig;
				*lexeme++ = k;
				k = p->config->get_char(p->config->user_data);
			}
			/* Get exponent. */
			while (is_digit(k)) {
				if (lexeme == p->lexeme_max) goto toobig;
				*lexeme++ = k;
				k = p->config->get_char(p->config->user_data);
			}
		}
		p->config->unget_char(p->config->user_data);
		p->token = NUM;
		goto ok;
	}
	/* If the character is alphabetic or '_', the token is an identifier. */
	if (is_alpha(k)) {
		/* Get all alphanumeric and '_' characters. */
		while (is_alpha(k) || is_digit(k)) {
			if (lexeme == p->lexeme_max) goto toobig;
			*lexeme++ = k;
			k = p->config->get_char(p->config->user_data);
		}
		p->config->unget_char(p->config->user_data);
		p->token = ID;
		*lexeme = '\0';
		switch (lexeme[0]) {
			case 'a':
				if (!strcmp(lexeme, "and"))
					p->token = LAND;
				break;
			case 'o':
				if (!strcmp(lexeme, "or"))
					p->token = LOR;
				break;
			case 'n':
				if (!strcmp(lexeme, "not"))
					p->token = NOT;
				break;
			case 's':
				if (!strcmp(lexeme, "shl"))
					p->token = SHL;
				else if (!strcmp(lexeme, "shr"))
					p->token = SHR;
				break;
		}
		return 1;
	}
	/* Otherwise the token is one of the bellow. */
	*lexeme++ = k;
	switch (k) {
		case '*':
			k = p->config->get_char(p->config->user_data);
			if (k == '=') {
				*lexeme++ = k;
				p->token = MULLET;
			} else {
				p->config->unget_char(p->config->user_data);
				p->token = MUL;
			}
			goto ok;
		case '/':
			k = p->config->get_char(p->config->user_data);
			if (k == '=') {
				*lexeme++ = k;
				p->token = DIVLET;
			} else {
				p->config->unget_char(p->config->user_data);
				p->token = DIV;
			}
			goto ok;
		case '%':
			k = p->config->get_char(p->config->user_data);
			if (k == '=') {
				*lexeme++ = k;
				p->token = MODLET;
			} else {
				p->config->unget_char(p->config->user_data);
				p->token = MOD;
			}
			goto ok;
		case '+':
			k = p->config->get_char(p->config->user_data);
			if (k == '+') {
				*lexeme++ = k;
				p->token = INC;
			} else if (k == '=') {
				*lexeme++ = k;
				p->token = ADDLET;
			} else {
				p->config->unget_char(p->config->user_data);
				p->token = ADD;
			}
			goto ok;
		case '-':
			k = p->config->get_char(p->config->user_data);
			if (k == '-') {
				*lexeme++ = k;
				p->token = DEC;
			} else if (k == '=') {
				*lexeme++ = k;
				p->token = SUBLET;
			} else {
				p->config->unget_char(p->config->user_data);
				p->token = SUB;
			}
			goto ok;
		case '(':
			p->token = LPAR;
			goto ok;
		case ')':
			p->token = RPAR;
			goto ok;
		case ',':
			p->token = COMMA;
			goto ok;
		case '<':
			k = p->config->get_char(p->config->user_data);
			if (k == '<') {
				*lexeme++ = k;
				k = p->config->get_char(p->config->user_data);
				if (k == '=') {
					*lexeme++ = k;
					p->token = SHLLET;
				} else {
					p->config->unget_char(p->config->user_data);
					p->token = SHL;
				}
			} else if (k == '=') {
				*lexeme++ = k;
				p->token = LE;
			} else if (k == '>') {
				*lexeme++ = k;
				p->token = NE;
			} else {
				p->config->unget_char(p->config->user_data);
				p->token = LT;
			}
			goto ok;
		case '>':
			k = p->config->get_char(p->config->user_data);
			if (k == '>') {
				*lexeme++ = k;
				k = p->config->get_char(p->config->user_data);
				if (k == '=') {
					*lexeme++ = k;
					p->token = SHRLET;
				} else {
					p->config->unget_char(p->config->user_data);
					p->token = SHR;
				}
			} else if (k == '=') {
				*lexeme++ = k;
				p->token = GE;
			} else {
				p->config->unget_char(p->config->user_data);
				p->token = GT;
			}
			goto ok;
		case '~':
			p->token = NEG;
			goto ok;
		case '!':
			k = p->config->get_char(p->config->user_data);
			if (k == '=') {
				*lexeme++ = k;
				p->token = NE;
			} else {
				p->config->unget_char(p->config->user_data);
				p->token = NOT;
			}
			goto ok;
		case '=':
			k = p->config->get_char(p->config->user_data);
			if (k == '=') {
				*lexeme++ = k;
				p->token = EQ;
			} else {
				p->config->unget_char(p->config->user_data);
				p->token = LET;
			}
			goto ok;
		case '&':
			k = p->config->get_char(p->config->user_data);
			if (k == '&') {
				*lexeme++ = k;
				p->token = LAND;
			} else if (k == '=') {
				*lexeme++ = k;
				p->token = ANDLET;
			} else {
				p->config->unget_char(p->config->user_data);
				p->token = AND;
			}
			goto ok;
		case '^':
			k = p->config->get_char(p->config->user_data);
			if (k == '=') {
				*lexeme++ = k;
				p->token = XORLET;
			} else {
				p->config->unget_char(p->config->user_data);
				p->token = XOR;
			}
			goto ok;
		case '|':
			k = p->config->get_char(p->config->user_data);
			if (k == '|') {
				*lexeme++ = k;
				p->token = LOR;
			} else if (k == '=') {
				*lexeme++ = k;
				p->token = ORLET;
			} else {
				p->config->unget_char(p->config->user_data);
				p->token = OR;
			}
			goto ok;
		case '?':
			p->token = QUEST;
			goto ok;
		case ':':
			p->token = COLON;
			goto ok;
	}
	/* Invalid character found. */
	if (k >= 0 && k < 32)
		sprintf_s(p->config->id, sizeof(p->config->id), "0x%.2x", k);
	else
		sprintf_s(p->config->id, sizeof(p->config->id), "%c", k);
	p->error = EXPR_INVALID_CHAR;
	return 0;
	/* Lexeme too big. */
	toobig:
	*lexeme = '\0';
	strcpy_s(p->config->id, sizeof(p->config->id), lexeme);
	p->error = EXPR_LEXEME_TOO_BIG;
	return 0;
	/* Ok. */
	ok:
	*lexeme = '\0';
	return 1;
}

/* Match a specific token. */
static int match(parser_t *p, int token) {
	if (p->token != token) {
		p->error = EXPR_UNEXPECTED;
		strcpy_s(p->config->id, sizeof(p->config->id), p->lexeme);
		return 0;
	}
	return match_any(p);
}

/* Push a number on the stack. */
static int push_number(parser_t *p, double d) {
	if (p->stack_pointer == p->stack_max) {
		p->error = EXPR_STACK_OVERFLOW;
		return 0;
	}
	p->stack_pointer++;
	p->stack_pointer->type = NUM;
	p->stack_pointer->d = d;
	return 1;
}

/* Pushe a variable on the stack. */
static int push_variable(parser_t *p, expr_variable_t *v) {
	if (p->stack_pointer == p->stack_max) {
		p->error = EXPR_STACK_OVERFLOW;
		return 0;
	}
	p->stack_pointer++;
	p->stack_pointer->type = ID;
	p->stack_pointer->v = v;
	return 1;
}

/* Gets the value of an variable. */
static void get_value(parser_t *p, value_t *v) {
	if (v->type == ID) {
		v->d = v->v->get(p->config->user_data);
		v->type = NUM;
	}
}

/* Forward declarations. */
static int expression(parser_t *p);
static int assignment(parser_t *p);

/* Parses a terminal (number, identifier, function call or sub-expression). */
static int terminal(parser_t *p) {
	expr_function_t	*f;
	double		parameters[MAX_PARAMS];
	int			num_parameters;
	expr_variable_t	*v;

	switch (p->token) {
		case OCTAL:
			/* Push the operand on the stack. */
			return push_number(p, str2u(p->lexeme, 8)) && match_any(p);
		case HEX:
			/* Push the operand on the stack. */
			return push_number(p, str2u(p->lexeme, 16)) && match_any(p);
		case NUM:
			/* Push the operand on the stack. */
			return push_number(p, strtod(p->lexeme, NULL)) && match_any(p);
		case ID:
			/* Get the identifier. */
			strcpy_s(p->config->id, sizeof(p->config->id), p->lexeme);
			if (!match_any(p)) return 0;
			/* If there is an '(' following the id, it's a method call. */
			if (p->token == LPAR) {
				if (!match_any(p)) return 0;
				/* Get the function. */
				f = p->config->get_function(p->config->user_data, p->config->id);
				if (f == NULL) {
					p->error = EXPR_UNKNOWN_ID;
					return 0;
				}
				// Push all parameters. */
				num_parameters = 0;
				if (p->token != RPAR) {
					if (!assignment(p)) return 0;
					get_value(p, p->stack_pointer);
					parameters[num_parameters++] = p->stack_pointer->d;
					/* Repeat until there are no more parameters. */
					while (p->token == COMMA) {
						if (!match_any(p) || !assignment(p)) return 0;
						if (num_parameters == MAX_PARAMS) {
							p->error = EXPR_TOO_MANY_PARAMS;
							return 0;
						}
						get_value(p, p->stack_pointer);
						parameters[num_parameters++] = p->stack_pointer->d;
					}
				}
				if (!match(p, RPAR)) return 0;
				if (num_parameters != f->num_parameters) {
					p->error = EXPR_WRONG_PARAM;
					return 0;
				}
				p->stack_pointer -= num_parameters;
				return push_number(p, f->execute(p->config->user_data, parameters));
			}
			/* Otherwise, it's an identifier. */
			v = p->config->get_variable(p->config->user_data, p->config->id);
			if (v == NULL) {
				p->error = EXPR_UNKNOWN_ID;
				return 0;
			}
			return push_variable(p, v);
		case LPAR:
			// Push the sub-expression
			return match_any(p) && expression(p) && match(p, RPAR);
	}
	return match(p, -1);
}

/* Parses an unary operator. */
static int unary(parser_t *p) {
	int		op;
	value_t	*arg;
	double	d;

	if (p->token == ADD || p->token == SUB || p->token == NEG || p->token == NOT || p->token == INC || p->token == DEC) {
		op = p->token;
		if (!match_any(p) || !unary(p)) return 0;
	} else {
		op = 0;
		if (!terminal(p)) return 0;
	}
	arg = p->stack_pointer;
	switch (op) {
		case ADD:
			break;
		case SUB:
			get_value(p, arg);
			arg->d = -arg->d;
			break;
		case NEG:
			get_value(p, arg);
			arg->d = ~(long)arg->d;
			break;
		case NOT:
			get_value(p, arg);
			arg->d = !arg->d;
			break;
		case INC:
			if (arg->type != ID) {
				p->error = EXPR_TYPE_MISMATCH;
				return 0;
			}
			if (arg->v->set == NULL) {
				p->error = EXPR_READ_ONLY;
				return 0;
			}
			d = arg->v->get(p->config->user_data) + 1;
			arg->v->set(p->config->user_data, d);
			arg->d = d;
			arg->type = NUM;
			break;
		case DEC:
			if (arg->type != ID) {
				p->error = EXPR_TYPE_MISMATCH;
				return 0;
			}
			if (arg->v->set == NULL) {
				p->error = EXPR_READ_ONLY;
				return 0;
			}
			d = arg->v->get(p->config->user_data) - 1;
			arg->v->set(p->config->user_data, d);
			arg->d = d;
			arg->type = NUM;
			break;
	}
	if (p->token == INC) {
		if (!match_any(p)) return 0;
		if (arg->type != ID) {
			p->error = EXPR_TYPE_MISMATCH;
			return 0;
		}
		if (arg->v->set == NULL) {
			p->error = EXPR_READ_ONLY;
			return 0;
		}
		d = arg->v->get(p->config->user_data);
		arg->v->set(p->config->user_data, d + 1);
		arg->d = d;
		arg->type = NUM;
	} else if (p->token == DEC) {
		if (!match_any(p)) return 0;
		if (arg->type != ID) {
			p->error = EXPR_TYPE_MISMATCH;
			return 0;
		}
		if (arg->v->set == NULL) {
			p->error = EXPR_READ_ONLY;
			return 0;
		}
		d = arg->v->get(p->config->user_data);
		arg->v->set(p->config->user_data, d - 1);
		arg->d = d;
		arg->type = NUM;
	}
	return 1;
}

/* Parses a multiplication or division. */
static int fact(parser_t *p) {
	int		op;
	value_t	*larg, *rarg;

	/* Get the left operand. */
	if (!unary(p)) return 0;
	/* Parse multiplications and divisions. */
	while (p->token == MUL || p->token == DIV || p->token == MOD) {
		/* Save the operator. */
		op = p->token;
		if (!match_any(p)) return 0;
		/* Get the right operand. */
		if (!unary(p)) return 0;
		/* Perform the operation. */
		get_value(p, rarg = p->stack_pointer--);
		get_value(p, larg = p->stack_pointer);
		switch (op) {
			case MUL:
				larg->d *= rarg->d;
				break;
			case DIV:
				if (rarg->d == 0) goto divbyzero;
				larg->d /= rarg->d;
				break;
			case MOD:
				if (rarg->d == 0) goto divbyzero;
				larg->d = fmod(larg->d, rarg->d);
				break;
                }
        }
	return 1;
	divbyzero:
	p->error = EXPR_DIV_BY_ZERO;
	return 0;
}

/* Parses a addition or subtraction. */
static int term(parser_t *p) {
	int		op;
	value_t	*larg, *rarg;

	/* Get the left operand. */
	if (!fact(p)) return 0;
	/* Parse additions and subtractions. */
	while (p->token == ADD || p->token == SUB) {
		/* Save the operator. */
		op = p->token;
		if (!match_any(p)) return 0;
		/* Get the right operand. */
		if (!fact(p)) return 0;
		/* Perform the operation. */
		get_value(p, rarg = p->stack_pointer--);
		get_value(p, larg = p->stack_pointer);
		switch (op) {
			case ADD:
				larg->d += rarg->d;
				break;
			case SUB:
				larg->d -= rarg->d;
				break;
                }
        }
	return 1;
}

/* Parses shifts. */
static int shift(parser_t *p) {
	int		op;
	value_t	*larg, *rarg;

	/* Get the left operand. */
	if (!term(p)) return 0;
	/* Parse shifts. */
	while (p->token == SHL || p->token == SHR) {
		/* Save the operator. */
		op = p->token;
		if (!match_any(p)) return 0;
		/* Get right operand. */
		if (!term(p)) return 0;
		/* Perform the operation. */
		get_value(p, rarg = p->stack_pointer--);
		get_value(p, larg = p->stack_pointer);
		switch (op) {
			case SHL:
				larg->d = (long)larg->d << (long)rarg->d;
				break;
			case SHR:
				larg->d = (long)larg->d >> (long)rarg->d;
				break;
		}
	}
	return 1;
}

/* Parses conditionals. */
static int conditional(parser_t *p) {
	int		op;
	value_t	*larg, *rarg;

	/* Get the left operand. */
	if (!shift(p)) return 0;
	/* Parse conditionals. */
	while (p->token == LT || p->token == LE || p->token == GT || p->token == GE) {
		/* Save the operator. */
		op = p->token;
		if (!match_any(p)) return 0;
		/* Get right operand. */
		if (!shift(p)) return 0;
		/* Perform the operation. */
		get_value(p, rarg = p->stack_pointer--);
		get_value(p, larg = p->stack_pointer);
		switch (op) {
			case LT:
				larg->d = larg->d < rarg->d;
				break;
			case LE:
				larg->d = larg->d <= rarg->d;
				break;
			case GT:
				larg->d = larg->d > rarg->d;
				break;
			case GE:
				larg->d = larg->d >= rarg->d;
				break;
		}
	}
	return 1;
}

/* Parses (in)equalities. */
static int equal(parser_t *p) {
	int		op;
	value_t	*larg, *rarg;

	/* Get the left operand. */
	if (!conditional(p)) return 0;
	/* Parse (in)equalities. */
	while (p->token == EQ || p->token == NE) {
		/* Save the operator. */
		op = p->token;
		if (!match_any(p)) return 0;
		/* Get the right operand. */
		if (!conditional(p)) return 0;
		/* Perform the operation. */
		get_value(p, rarg = p->stack_pointer--);
		get_value(p, larg = p->stack_pointer);
		switch (op) {
			case EQ:
				larg->d = larg->d == rarg->d;
				break;
			case NE:
				larg->d = larg->d != rarg->d;
				break;
		}
	}
	return 1;
}

/* Parses bitwise and operations. */
static int bitwise_and(parser_t *p) {
	value_t	*larg, *rarg;

	/* Get the left operand. */
	if (!equal(p)) return 0;
	/* Parse bitwise ands. */
	while (p->token == AND) {
		if (!match_any(p)) return 0;
		/* Get the right operand. */
		if (!equal(p)) return 0;
		/* Perform the operation. */
		get_value(p, rarg = p->stack_pointer--);
		get_value(p, larg = p->stack_pointer);
		larg->d = (long)larg->d & (long)rarg->d;
	}
	return 1;
}

/* Parses bitwise xor operations. */
static int bitwise_xor(parser_t *p) {
	value_t	*larg, *rarg;

	/* Get the left operand. */
	if (!bitwise_and(p)) return 0;
	/* Parse bitwise xors. */
	while (p->token == XOR) {
		if (!match_any(p)) return 0;
		/* Get the right operand. */
		if (!bitwise_and(p)) return 0;
		/* Perform the operation. */
		get_value(p, rarg = p->stack_pointer--);
		get_value(p, larg = p->stack_pointer);
		larg->d = (long)larg->d ^ (long)rarg->d;
	}
	return 1;
}

/* Parses bitwise or operations. */
static int bitwise_or(parser_t *p) {
	value_t	*larg, *rarg;

	/* Get the left operand. */
	if (!bitwise_xor(p)) return 0;
	/* Parse bitwise ors. */
	while (p->token == OR) {
		if (!match_any(p)) return 0;
		/* Get the right operand. */
		if (!bitwise_xor(p)) return 0;
		/* Perform the operation. */
		get_value(p, rarg = p->stack_pointer--);
		get_value(p, larg = p->stack_pointer);
		larg->d = (long)larg->d | (long)rarg->d;
	}
	return 1;
}

/* Parses logical and operations. */
static int logical_and(parser_t *p) {
	value_t	*larg, *rarg;

	/* Get the left operand. */
	if (!bitwise_or(p)) return 0;
	/* Parse logical ands. */
	while (p->token == LAND) {
		if (!match_any(p)) return 0;
		/* Get the right operand. */
		if (!bitwise_or(p)) return 0;
		/* Perform the operation. */
		get_value(p, rarg = p->stack_pointer--);
		get_value(p, larg = p->stack_pointer);
		larg->d = larg->d && rarg->d;
	}
	return 1;
}

/* Parses logical or operations. */
static int logical_or(parser_t *p) {
	value_t	*larg, *rarg;

	/* Get the left operand. */
	if (!logical_and(p)) return 0;
	/* Parse logical ors. */
	while (p->token == LOR) {
		if (!match_any(p)) return 0;
		/* Get the right operand. */
		if (!logical_and(p)) return 0;
		/* Perform the operation. */
		get_value(p, rarg = p->stack_pointer--);
		get_value(p, larg = p->stack_pointer);
		larg->d = larg->d || rarg->d;
	}
	return 1;
}

/* Parses the ternary operator. */
static int ternary(parser_t *p) {
	int	first;

	/* Evaluate the logical expression. */
	if (!logical_or(p)) return 0;
	if (p->token == QUEST) {
		get_value(p, p->stack_pointer);
		first = p->stack_pointer->d != 0;
		p->stack_pointer--;
		if (!match_any(p)) return 0;
		/* Evaluate the first expression. */
		if (!expression(p)) return 0;
		/* Drop the result if the logical expression is false. */
		if (!first)
			p->stack_pointer--;
		if (!match(p, COLON)) return 0;
		/* Evaluate the second expression. */
		if (!expression(p)) return 0;
		/* Drop the result if the logical expression is true. */
		if (first)
			p->stack_pointer--;
	}
	return 1;
}

/* Parses the assignment operators. */
static int assignment(parser_t *p) {
	int		op;
	value_t	*lvalue, *expr;

	/* Evaluate the lvalue. */
	if (!ternary(p)) return 0;
	if (p->token == LET || p->token == ADDLET || p->token == SUBLET || p->token == MULLET || p->token == DIVLET || p->token == MODLET || p->token == ANDLET || p->token == XORLET || p->token == ORLET || p->token == SHLLET || p->token == SHRLET) {
		op = p->token;
		if (!match_any(p)) return 0;
		if (!assignment(p)) return 0;
		get_value(p, expr = p->stack_pointer--);
		lvalue = p->stack_pointer;
		if (lvalue->type != ID) {
			p->error = EXPR_TYPE_MISMATCH;
			return 0;
		}
		if (lvalue->v->set == NULL) {
			p->error = EXPR_READ_ONLY;
			return 0;
		}
		switch (op) {
			case LET:
				lvalue->v->set(p, expr->d);
				break;
			case ADDLET:
				lvalue->v->set(p, lvalue->v->get(p) + expr->d);
				break;
			case SUBLET:
				lvalue->v->set(p, lvalue->v->get(p) - expr->d);
				break;
			case MULLET:
				lvalue->v->set(p, lvalue->v->get(p) * expr->d);
				break;
			case DIVLET:
				if (expr->d == 0) goto divbyzero;
				lvalue->v->set(p, lvalue->v->get(p) / expr->d);
				break;
			case MODLET:
				if (expr->d == 0) goto divbyzero;
				lvalue->v->set(p, fmod(lvalue->v->get(p), expr->d));
				break;
			case ANDLET:
				lvalue->v->set(p, (long)lvalue->v->get(p) & (long)expr->d);
				break;
			case XORLET:
				lvalue->v->set(p, (long)lvalue->v->get(p) ^ (long)expr->d);
				break;
			case ORLET:
				lvalue->v->set(p, (long)lvalue->v->get(p) | (long)expr->d);
				break;
			case SHLLET:
				lvalue->v->set(p, (long)lvalue->v->get(p) << (long)expr->d);
				break;
			case SHRLET:
				lvalue->v->set(p, (long)lvalue->v->get(p) >> (long)expr->d);
				break;
		}
	}
	return 1;
	divbyzero:
	p->error = EXPR_DIV_BY_ZERO;
	return 0;
}

/* Parses an expression. */
static int expression(parser_t *p) {
	if (!assignment(p)) return 0;
	while (p->token == COMMA) {
		if (!match_any(p)) return 0;
		p->stack_pointer--;
		if (!assignment(p)) return 0;
	}
	return 1;
}

/* Evaluate an expression. */
int expr_evaluate(expr_config_t *config, double *result) {
	parser_t p;

	/* Initialize the parser. */
	p.config = config;
	config->id[0] = '\0';
	p.lexeme_max = p.lexeme + MAX_ID - 1;
	p.token = 0;
	p.stack_pointer = p.stack - 1;
	p.stack_max = p.stack + MAX_STACK - 1;
	p.error = EXPR_OK;
	/* Parse the expression. */
	match_any(&p) && expression(&p);
	/* Set the result if needed. */
	if (result != NULL) {
		get_value(&p, p.stack);
		*result = p.stack[0].d;
	}
	/* See if there is extra input. */
	if (p.token != EOE)
		return EXPR_EXTRA_INPUT;
	return p.error;
}

/* Evaluate a string. */

typedef struct {
	expr_config_t	*config;
	const char		*string;
} strud_t;

static int string_get(void *user_data) {
	strud_t *ud = (strud_t *)user_data;
	char k = *ud->string++;
	return k == '\0' ? -1 : k;
}

static void string_unget(void *user_data) {
	strud_t *ud = (strud_t *)user_data;
	ud->string--;
}

static expr_function_t *string_function(void *user_data, const char *name) {
	strud_t *ud = (strud_t *)user_data;
	return ud->config->get_function(ud->config->user_data, name);
}

static expr_variable_t *string_variable(void *user_data, const char *name) {
	strud_t *ud = (strud_t *)user_data;
	return ud->config->get_variable(ud->config->user_data, name);
}

int expr_evaluate_string(expr_config_t *config, const char *string, double *result) {
	expr_config_t	config2;
	strud_t		ud;
	int			error;

	ud.config = config;
	ud.string = string;
	config2.user_data = (void *)&ud;
	config2.get_char = string_get;
	config2.unget_char = string_unget;
	config2.get_function = string_function;
	config2.get_variable = string_variable;
	error = expr_evaluate(&config2, result);
	strcpy_s(config->id, sizeof(config->id), config2.id);
	return error;
}
