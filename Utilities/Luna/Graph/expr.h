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

#ifndef __EXPR_H__
#define __EXPR_H__

/* The maximum length of an identifier. */
#define MAX_ID	64

/* Error codes. */
#define EXPR_OK				0
#define EXPR_INVALID_CHAR		1
#define EXPR_LEXEME_TOO_BIG	2
#define EXPR_UNEXPECTED		3
#define EXPR_STACK_OVERFLOW	4
#define EXPR_UNKNOWN_ID		5
#define EXPR_WRONG_PARAM	6
#define EXPR_TYPE_MISMATCH	7
#define EXPR_DIV_BY_ZERO		8
#define EXPR_READ_ONLY		9
#define EXPR_INVALID_DIGIT		10
#define EXPR_TOO_MANY_PARAMS	11
#define EXPR_EXTRA_INPUT		12

typedef struct {
	const char	*name;
	int		num_parameters;
	double	(*execute)(void *, double *);
} expr_function_t;

typedef struct {
	const char	*name;
	double	(*get)(void *);
	void		(*set)(void *, double);
} expr_variable_t;

typedef struct {
	void			*user_data;
	int			(*get_char)(void *);
	void			(*unget_char)(void *);
	expr_function_t	*(*get_function)(void *, const char *);
	expr_variable_t	*(*get_variable)(void *, const char *);
	char			id[MAX_ID];
} expr_config_t;

int expr_evaluate(expr_config_t *, double *);
int expr_evaluate_string(expr_config_t *, const char *, double *);

#endif
