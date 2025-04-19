%{
/***********************************************************************/
/*                                                                     */
/*                                OCaml                                */
/*                                                                     */
/*            Xavier Leroy, projet Cristal, INRIA Rocquencourt         */
/*                                                                     */
/*  Copyright 1996 Institut National de Recherche en Informatique et   */
/*  en Automatique.  All rights reserved.  This file is distributed    */
/*  under the terms of the Q Public License version 1.0.               */
/*                                                                     */
/***********************************************************************/
%}

%token AMPERAMPER
%token AMPERSAND
%token AND
%token AS
%token ASSERT
%token BACKQUOTE
%token BANG
%token BAR
%token BARBAR
%token BARRBRACKET
%token BEGIN
%token CHAR
%token CLASS
%token COLON
%token COLONCOLON
%token COLONEQUAL
%token COLONGREATER
%token COMMA
%token CONSTRAINT
%token DO
%token DONE
%token DOT
%token DOTDOT
%token DOWNTO
%token ELSE
%token END
%token EOF
%token EQUAL
%token EXCEPTION
%token EXTERNAL
%token FALSE
%token FLOAT
%token FOR
%token FUN
%token FUNCTION
%token FUNCTOR
%token GREATER
%token GREATERRBRACE
%token GREATERRBRACKET
%token IF
%token IN
%token INCLUDE
%token INFIXOP0
%token INFIXOP1
%token INFIXOP2
%token INFIXOP3
%token INFIXOP4
%token INHERIT
%token INITIALIZER
%token INT
%token INT32
%token INT64
%token LABEL
%token LAZY
%token LBRACE
%token LBRACELESS
%token LBRACKET
%token LBRACKETBAR
%token LBRACKETLESS
%token LBRACKETGREATER
%token LBRACKETPERCENT
%token LBRACKETPERCENTPERCENT
%token LESS
%token LESSMINUS
%token LET
%token LIDENT
%token LPAREN
%token LBRACKETAT
%token LBRACKETATAT
%token LBRACKETATATAT
%token MATCH
%token METHOD
%token MINUS
%token MINUSDOT
%token MINUSGREATER
%token MODULE
%token MUTABLE
%token NATIVEINT
%token NEW
%token OBJECT
%token OF
%token OPEN
%token OPTLABEL
%token OR
%token PERCENT
%token PLUS
%token PLUSDOT
%token PLUSEQ
%token PREFIXOP
%token PRIVATE
%token QUESTION
%token QUOTE
%token RBRACE
%token RBRACKET
%token REC
%token RPAREN
%token SEMI
%token SEMISEMI
%token SHARP
%token SIG
%token STAR
%token STRING
%token STRUCT
%token THEN
%token TILDE
%token TO
%token TRUE
%token TRY
%token TYPE
%token UIDENT
%token UNDERSCORE
%token VAL
%token VIRTUAL
%token WHEN
%token WHILE
%token WITH
%token COMMENT

%token error

%token EOL

%nonassoc IN
%nonassoc below_SEMI
%nonassoc SEMI
%nonassoc LET
%nonassoc below_WITH
%nonassoc FUNCTION WITH
%nonassoc AND
%nonassoc THEN
%nonassoc ELSE
%nonassoc LESSMINUS
%right    COLONEQUAL
%nonassoc AS
%left     BAR
%nonassoc below_COMMA
%left     COMMA
%right    MINUSGREATER
%right    OR BARBAR
%right    AMPERSAND AMPERAMPER
%nonassoc below_EQUAL
%left     INFIXOP0 EQUAL LESS GREATER
%right    INFIXOP1
%nonassoc below_LBRACKETAT
%nonassoc LBRACKETAT
%nonassoc LBRACKETATAT
%right    COLONCOLON
%left     INFIXOP2 PLUS PLUSDOT MINUS MINUSDOT PLUSEQ
%left     PERCENT INFIXOP3 STAR
%right    INFIXOP4
%nonassoc prec_unary_minus prec_unary_plus
%nonassoc prec_constant_constructor
%nonassoc prec_constr_appl
%nonassoc below_SHARP
%nonassoc SHARP
%nonassoc below_DOT
%nonassoc DOT
%nonassoc BACKQUOTE BANG BEGIN CHAR FALSE FLOAT INT INT32 INT64
          LBRACE LBRACELESS LBRACKET LBRACKETBAR LIDENT LPAREN
          NEW NATIVEINT PREFIXOP STRING TRUE UIDENT
          LBRACKETPERCENT LBRACKETPERCENTPERCENT

%start implementation

%%

implementation:
    structure EOF
;

functor_arg:
    LPAREN RPAREN
  | LPAREN functor_arg_name COLON module_type RPAREN
;

functor_arg_name:
    UIDENT
  | UNDERSCORE
;

functor_args:
    functor_args functor_arg
  | functor_arg
;

module_expr:
    mod_longident
  | STRUCT structure END
  | STRUCT structure error
  | FUNCTOR functor_args MINUSGREATER module_expr
  | module_expr LPAREN module_expr RPAREN
  | module_expr LPAREN RPAREN
  | module_expr LPAREN module_expr error
  | LPAREN module_expr COLON module_type RPAREN
  | LPAREN module_expr COLON module_type error
  | LPAREN module_expr RPAREN
  | LPAREN module_expr error
  | LPAREN VAL expr RPAREN
  | LPAREN VAL expr COLON package_type RPAREN
  | LPAREN VAL expr COLON package_type COLONGREATER package_type RPAREN
  | LPAREN VAL expr COLONGREATER package_type RPAREN
  | LPAREN VAL expr COLON error
  | LPAREN VAL expr COLONGREATER error
  | LPAREN VAL expr error
  | module_expr attribute
  | extension
;

structure:
    seq_expr post_item_attributes structure_tail
  | structure_tail
;
structure_tail:
  | SEMISEMI structure
  | structure_item structure_tail
;
structure_item:
    LET rec_flag let_bindings
  | EXTERNAL val_ident COLON core_type EQUAL primitive_declaration
    post_item_attributes
  | TYPE type_declarations
  | TYPE str_type_extension
  | EXCEPTION str_exception_declaration
  | MODULE module_binding
  | MODULE REC module_bindings
  | MODULE TYPE ident post_item_attributes
  | MODULE TYPE ident EQUAL module_type post_item_attributes
  | open_statement
  | CLASS class_declarations
  | CLASS TYPE class_type_declarations
  | INCLUDE module_expr post_item_attributes
  | item_extension post_item_attributes
  | floating_attribute

;
module_binding_body:
    EQUAL module_expr
  | COLON module_type EQUAL module_expr
  | functor_arg module_binding_body
;
module_bindings:
    module_binding
  | module_bindings AND module_binding
;
module_binding:
    UIDENT module_binding_body post_item_attributes
;

module_type:
    mty_longident
  | SIG signature END
  | SIG signature error
  | FUNCTOR functor_args MINUSGREATER module_type
      %prec below_WITH
  | module_type WITH with_constraints
  | MODULE TYPE OF module_expr %prec below_LBRACKETAT
  | LPAREN MODULE mod_longident RPAREN
  | LPAREN module_type RPAREN
  | LPAREN module_type error
  | extension
  | module_type attribute
;
signature:
  | SEMISEMI signature
  | signature_item signature
;
signature_item:
    VAL val_ident COLON core_type post_item_attributes
  | EXTERNAL val_ident COLON core_type EQUAL primitive_declaration
    post_item_attributes
  | TYPE type_declarations
  | TYPE sig_type_extension
  | EXCEPTION sig_exception_declaration
  | MODULE UIDENT module_declaration post_item_attributes
  | MODULE UIDENT EQUAL mod_longident post_item_attributes
  | MODULE REC module_rec_declarations
  | MODULE TYPE ident post_item_attributes
  | MODULE TYPE ident EQUAL module_type post_item_attributes
  | open_statement
  | INCLUDE module_type post_item_attributes %prec below_WITH
  | CLASS class_descriptions
  | CLASS TYPE class_type_declarations
  | item_extension post_item_attributes
  | floating_attribute
;
open_statement:
    OPEN override_flag mod_longident post_item_attributes
;
module_declaration:
    COLON module_type
  | LPAREN UIDENT COLON module_type RPAREN module_declaration
  | LPAREN RPAREN module_declaration
;
module_rec_declarations:
    module_rec_declaration
  | module_rec_declarations AND module_rec_declaration
;
module_rec_declaration:
    UIDENT COLON module_type post_item_attributes
;

class_declarations:
    class_declarations AND class_declaration
  | class_declaration
;
class_declaration:
    virtual_flag class_type_parameters LIDENT class_fun_binding
    post_item_attributes
;
class_fun_binding:
    EQUAL class_expr
  | COLON class_type EQUAL class_expr
  | labeled_simple_pattern class_fun_binding
;
class_type_parameters:
  | LBRACKET type_parameter_list RBRACKET
;
class_fun_def:
    labeled_simple_pattern MINUSGREATER class_expr
  | labeled_simple_pattern class_fun_def
;
class_expr:
    class_simple_expr
  | FUN class_fun_def
  | class_simple_expr simple_labeled_expr_list
  | LET rec_flag let_bindings_no_attrs IN class_expr
  | class_expr attribute
  | extension
;
class_simple_expr:
    LBRACKET core_type_comma_list RBRACKET class_longident
  | class_longident
  | OBJECT class_structure END
  | OBJECT class_structure error
  | LPAREN class_expr COLON class_type RPAREN
  | LPAREN class_expr COLON class_type error
  | LPAREN class_expr RPAREN
  | LPAREN class_expr error
;
class_structure:
    class_self_pattern class_fields
;
class_self_pattern:
    LPAREN pattern RPAREN
  | LPAREN pattern COLON core_type RPAREN
  |
;
class_fields:
  | class_fields class_field
;
class_field:
    INHERIT override_flag class_expr parent_binder post_item_attributes
  | VAL value post_item_attributes
  | METHOD method_ post_item_attributes
  | CONSTRAINT constrain_field post_item_attributes
  | INITIALIZER seq_expr post_item_attributes
  | item_extension post_item_attributes
  | floating_attribute
;
parent_binder:
    AS LIDENT
  |
;
value:
    override_flag MUTABLE VIRTUAL label COLON core_type
  | VIRTUAL mutable_flag label COLON core_type
  | override_flag mutable_flag label EQUAL seq_expr
  | override_flag mutable_flag label type_constraint EQUAL seq_expr
;
method_:
    override_flag PRIVATE VIRTUAL label COLON poly_type
  | override_flag VIRTUAL private_flag label COLON poly_type
  | override_flag private_flag label strict_binding
  | override_flag private_flag label COLON poly_type EQUAL seq_expr
  | override_flag private_flag label COLON TYPE lident_list
;

class_type:
    class_signature
  | QUESTION LIDENT COLON simple_core_type_or_tuple_no_attr MINUSGREATER class_type
  | OPTLABEL simple_core_type_or_tuple_no_attr MINUSGREATER class_type
  | LIDENT COLON simple_core_type_or_tuple_no_attr MINUSGREATER class_type
  | simple_core_type_or_tuple_no_attr MINUSGREATER class_type
 ;
class_signature:
    LBRACKET core_type_comma_list RBRACKET clty_longident
  | clty_longident
  | OBJECT class_sig_body END
  | OBJECT class_sig_body error
  | class_signature attribute
  | extension
;
class_sig_body:
    class_self_type class_sig_fields
;
class_self_type:
    LPAREN core_type RPAREN
  |
;
class_sig_fields:
  | class_sig_fields class_sig_field
;
class_sig_field:
    INHERIT class_signature post_item_attributes
  | VAL value_type post_item_attributes
  | METHOD private_virtual_flags label COLON poly_type post_item_attributes
  | CONSTRAINT constrain_field post_item_attributes
  | item_extension post_item_attributes
  | floating_attribute
;
value_type:
    VIRTUAL mutable_flag label COLON core_type
  | MUTABLE virtual_flag label COLON core_type
  | label COLON core_type
;
constrain:
        core_type EQUAL core_type
;
constrain_field:
        core_type EQUAL core_type
;
class_descriptions:
    class_descriptions AND class_description
  | class_description
;
class_description:
    virtual_flag class_type_parameters LIDENT COLON class_type post_item_attributes
;
class_type_declarations:
    class_type_declarations AND class_type_declaration
  | class_type_declaration
;
class_type_declaration:
    virtual_flag class_type_parameters LIDENT EQUAL class_signature post_item_attributes
;

seq_expr:
    expr        %prec below_SEMI
  | expr SEMI
  | expr SEMI seq_expr
;
labeled_simple_pattern:
    QUESTION LPAREN label_let_pattern opt_default RPAREN
  | QUESTION label_var
  | OPTLABEL LPAREN let_pattern opt_default RPAREN
  | OPTLABEL pattern_var
  | TILDE LPAREN label_let_pattern RPAREN
  | TILDE label_var
  | LABEL simple_pattern
  | simple_pattern
;
pattern_var:
    LIDENT
  | UNDERSCORE
;
opt_default:
  | EQUAL seq_expr
;
label_let_pattern:
    label_var
  | label_var COLON core_type
;
label_var:
    LIDENT
;
let_pattern:
    pattern
  | pattern COLON core_type
;
expr:
    simple_expr %prec below_SHARP
  | simple_expr simple_labeled_expr_list
  | LET rec_flag let_bindings_no_attrs IN seq_expr
  | LET MODULE UIDENT module_binding_body IN seq_expr
  | LET OPEN override_flag mod_longident IN seq_expr
  | FUNCTION opt_bar match_cases
  | FUN labeled_simple_pattern fun_def
  | FUN LPAREN TYPE LIDENT RPAREN fun_def
  | MATCH seq_expr WITH opt_bar match_cases
  | TRY seq_expr WITH opt_bar match_cases
  | TRY seq_expr WITH error
  | expr_comma_list %prec below_COMMA
  | constr_longident simple_expr %prec below_SHARP
  | name_tag simple_expr %prec below_SHARP
  | IF seq_expr THEN expr ELSE expr
  | IF seq_expr THEN expr
  | WHILE seq_expr DO seq_expr DONE
  | FOR pattern EQUAL seq_expr direction_flag seq_expr DO seq_expr DONE
  | expr COLONCOLON expr
  | LPAREN COLONCOLON RPAREN LPAREN expr COMMA expr RPAREN
  | expr INFIXOP0 expr
  | expr INFIXOP1 expr
  | expr INFIXOP2 expr
  | expr INFIXOP3 expr
  | expr INFIXOP4 expr
  | expr PLUS expr
  | expr PLUSDOT expr
  | expr PLUSEQ expr
  | expr MINUS expr
  | expr MINUSDOT expr
  | expr STAR expr
  | expr PERCENT expr
  | expr EQUAL expr
  | expr LESS expr
  | expr GREATER expr
  | expr OR expr
  | expr BARBAR expr
  | expr AMPERSAND expr
  | expr AMPERAMPER expr
  | expr COLONEQUAL expr
  | subtractive expr %prec prec_unary_minus
  | additive expr %prec prec_unary_plus
  | simple_expr DOT label_longident LESSMINUS expr
  | simple_expr DOT LPAREN seq_expr RPAREN LESSMINUS expr
  | simple_expr DOT LBRACKET seq_expr RBRACKET LESSMINUS expr
  | simple_expr DOT LBRACE expr RBRACE LESSMINUS expr
  | label LESSMINUS expr
  | ASSERT simple_expr %prec below_SHARP
  | LAZY simple_expr %prec below_SHARP
  | OBJECT class_structure END
  | OBJECT class_structure error
  | expr attribute
;
simple_expr:
    val_longident
  | constant
  | constr_longident %prec prec_constant_constructor
  | name_tag %prec prec_constant_constructor
  | LPAREN seq_expr RPAREN
  | LPAREN seq_expr error
  | BEGIN seq_expr END
  | BEGIN END
  | BEGIN seq_expr error
  | LPAREN seq_expr type_constraint RPAREN
  | simple_expr DOT label_longident
  | mod_longident DOT LPAREN seq_expr RPAREN
  | mod_longident DOT LPAREN seq_expr error
  | simple_expr DOT LPAREN seq_expr RPAREN
  | simple_expr DOT LPAREN seq_expr error
  | simple_expr DOT LBRACKET seq_expr RBRACKET
  | simple_expr DOT LBRACKET seq_expr error
  | simple_expr DOT LBRACE expr RBRACE
  | simple_expr DOT LBRACE expr_comma_list error
  | LBRACE record_expr RBRACE
  | LBRACE record_expr error
  | mod_longident DOT LBRACE record_expr RBRACE
  | mod_longident DOT LBRACE record_expr error
  | LBRACKETBAR expr_semi_list opt_semi BARRBRACKET
  | LBRACKETBAR expr_semi_list opt_semi error
  | LBRACKETBAR BARRBRACKET
  | mod_longident DOT LBRACKETBAR expr_semi_list opt_semi BARRBRACKET
  | mod_longident DOT LBRACKETBAR expr_semi_list opt_semi error
  | LBRACKET expr_semi_list opt_semi RBRACKET
  | LBRACKET expr_semi_list opt_semi error
  | mod_longident DOT LBRACKET expr_semi_list opt_semi RBRACKET
  | mod_longident DOT LBRACKET expr_semi_list opt_semi error
  | PREFIXOP simple_expr
  | BANG simple_expr
  | NEW class_longident
  | LBRACELESS field_expr_list opt_semi GREATERRBRACE
  | LBRACELESS field_expr_list opt_semi error
  | LBRACELESS GREATERRBRACE
  | mod_longident DOT LBRACELESS field_expr_list opt_semi GREATERRBRACE
  | mod_longident DOT LBRACELESS field_expr_list opt_semi error
  | simple_expr SHARP label
  | LPAREN MODULE module_expr RPAREN
  | LPAREN MODULE module_expr COLON package_type RPAREN
  | LPAREN MODULE module_expr COLON error
  | mod_longident DOT LPAREN MODULE module_expr COLON package_type RPAREN
  | mod_longident DOT LPAREN MODULE module_expr COLON error
  | extension
;
simple_labeled_expr_list:
    labeled_simple_expr
  | simple_labeled_expr_list labeled_simple_expr
;
labeled_simple_expr:
    simple_expr %prec below_SHARP
  | label_expr
;
label_expr:
    LABEL simple_expr %prec below_SHARP
  | TILDE label_ident
  | QUESTION label_ident
  | OPTLABEL simple_expr %prec below_SHARP
;
label_ident:
    LIDENT
;
let_bindings:
    let_binding
  | let_bindings AND let_binding
;
let_bindings_no_attrs:
   let_bindings
;

lident_list:
    LIDENT
  | LIDENT lident_list
;
let_binding:
    let_binding_ post_item_attributes
;
let_binding_:
    val_ident fun_binding
  | val_ident COLON typevar_list DOT core_type EQUAL seq_expr
  | val_ident COLON TYPE lident_list DOT core_type EQUAL seq_expr
  | pattern EQUAL seq_expr
  | simple_pattern_not_ident COLON core_type EQUAL seq_expr
;
fun_binding:
    strict_binding
  | type_constraint EQUAL seq_expr
;
strict_binding:
    EQUAL seq_expr
  | labeled_simple_pattern fun_binding
  | LPAREN TYPE LIDENT RPAREN fun_binding
;
match_cases:
    match_case
  | match_cases BAR match_case
;
match_case:
    pattern MINUSGREATER seq_expr
  | pattern WHEN seq_expr MINUSGREATER seq_expr
;
fun_def:
    MINUSGREATER seq_expr
  | labeled_simple_pattern fun_def
  | LPAREN TYPE LIDENT RPAREN fun_def
;
expr_comma_list:
    expr_comma_list COMMA expr
  | expr COMMA expr
;
record_expr:
    simple_expr WITH lbl_expr_list
  | lbl_expr_list
;
lbl_expr_list:
    lbl_expr
  | lbl_expr SEMI lbl_expr_list
  | lbl_expr SEMI
;
lbl_expr:
    label_longident EQUAL expr
  | label_longident

;
field_expr_list:
    label EQUAL expr
  | field_expr_list SEMI label EQUAL expr
;
expr_semi_list:
    expr
  | expr_semi_list SEMI expr
;
type_constraint:
    COLON core_type
  | COLON core_type COLONGREATER core_type
  | COLONGREATER core_type
  | COLON error
  | COLONGREATER error
;

pattern:
    simple_pattern
  | pattern AS val_ident
  | pattern AS error
  | pattern_comma_list  %prec below_COMMA
  | constr_longident pattern %prec prec_constr_appl
  | name_tag pattern %prec prec_constr_appl
  | pattern COLONCOLON pattern
  | pattern COLONCOLON error
  | LPAREN COLONCOLON RPAREN LPAREN pattern COMMA pattern RPAREN
  | LPAREN COLONCOLON RPAREN LPAREN pattern COMMA pattern error
  | pattern BAR pattern
  | pattern BAR error
  | LAZY simple_pattern
  | EXCEPTION pattern %prec prec_constr_appl
  | pattern attribute
;
simple_pattern:
    val_ident %prec below_EQUAL
  | simple_pattern_not_ident
;
simple_pattern_not_ident:
    UNDERSCORE
  | signed_constant
  | signed_constant DOTDOT signed_constant
  | constr_longident
  | name_tag
  | SHARP type_longident
  | LBRACE lbl_pattern_list RBRACE
  | LBRACE lbl_pattern_list error
  | LBRACKET pattern_semi_list opt_semi RBRACKET
  | LBRACKET pattern_semi_list opt_semi error
  | LBRACKETBAR pattern_semi_list opt_semi BARRBRACKET
  | LBRACKETBAR BARRBRACKET
  | LBRACKETBAR pattern_semi_list opt_semi error
  | LPAREN pattern RPAREN
  | LPAREN pattern error
  | LPAREN pattern COLON core_type RPAREN
  | LPAREN pattern COLON core_type error
  | LPAREN pattern COLON error
  | LPAREN MODULE UIDENT RPAREN
  | LPAREN MODULE UIDENT COLON package_type RPAREN
  | LPAREN MODULE UIDENT COLON package_type error
  | extension
;

pattern_comma_list:
    pattern_comma_list COMMA pattern
  | pattern COMMA pattern
  | pattern COMMA error
;
pattern_semi_list:
    pattern
  | pattern_semi_list SEMI pattern
;
lbl_pattern_list:
    lbl_pattern
  | lbl_pattern SEMI
  | lbl_pattern SEMI UNDERSCORE opt_semi
  | lbl_pattern SEMI lbl_pattern_list

;
lbl_pattern:
    label_longident EQUAL pattern
  | label_longident
;

primitive_declaration:
    STRING
  | STRING primitive_declaration
;

type_declarations:
    type_declaration
  | type_declarations AND type_declaration
;

type_declaration:
    optional_type_parameters LIDENT type_kind constraints post_item_attributes
;
constraints:
        constraints CONSTRAINT constrain
      |
;
type_kind:
  | EQUAL core_type
  | EQUAL PRIVATE core_type
  | EQUAL constructor_declarations
  | EQUAL PRIVATE constructor_declarations
  | EQUAL private_flag BAR constructor_declarations
  | EQUAL DOTDOT
  | EQUAL private_flag LBRACE label_declarations opt_semi RBRACE
  | EQUAL core_type EQUAL private_flag opt_bar constructor_declarations
  | EQUAL core_type EQUAL DOTDOT
  | EQUAL core_type EQUAL private_flag LBRACE label_declarations opt_semi RBRACE
;
optional_type_parameters:
  | optional_type_parameter
  | LPAREN optional_type_parameter_list RPAREN
;
optional_type_parameter:
    type_variance optional_type_variable
;
optional_type_parameter_list:
    optional_type_parameter
  | optional_type_parameter_list COMMA optional_type_parameter
;
optional_type_variable:
    QUOTE ident
  | UNDERSCORE
;

type_parameters:
  | type_parameter
  | LPAREN type_parameter_list RPAREN
;
type_parameter:
    type_variance type_variable
;
type_variance:
  | PLUS
  | MINUS
;
type_variable:
    QUOTE ident
;
type_parameter_list:
    type_parameter
  | type_parameter_list COMMA type_parameter
;
constructor_declarations:
    constructor_declaration
  | constructor_declarations BAR constructor_declaration
;
constructor_declaration:
    constr_ident attributes generalized_constructor_arguments
;
str_exception_declaration:
    extension_constructor_declaration post_item_attributes
  | extension_constructor_rebind post_item_attributes
;
sig_exception_declaration:
  | extension_constructor_declaration post_item_attributes
;
generalized_constructor_arguments:
  | OF core_type_list
  | COLON core_type_list MINUSGREATER simple_core_type
  | COLON simple_core_type
;



label_declarations:
    label_declaration
  | label_declarations SEMI label_declaration
;
label_declaration:
    mutable_flag label attributes COLON poly_type
;

str_type_extension:
  optional_type_parameters type_longident PLUSEQ private_flag opt_bar str_extension_constructors post_item_attributes
;
sig_type_extension:
  optional_type_parameters type_longident PLUSEQ private_flag opt_bar sig_extension_constructors post_item_attributes
;
str_extension_constructors:
    extension_constructor_declaration
  | extension_constructor_rebind
  | str_extension_constructors BAR extension_constructor_declaration
  | str_extension_constructors BAR extension_constructor_rebind
;
sig_extension_constructors:
    extension_constructor_declaration
  | sig_extension_constructors BAR extension_constructor_declaration
;
extension_constructor_declaration:
    constr_ident attributes generalized_constructor_arguments
;
extension_constructor_rebind:
    constr_ident attributes EQUAL constr_longident
;

with_constraints:
    with_constraint
  | with_constraints AND with_constraint
;
with_constraint:
    TYPE type_parameters label_longident with_type_binder core_type constraints
  | TYPE type_parameters label COLONEQUAL core_type
  | MODULE mod_longident EQUAL mod_ext_longident
  | MODULE UIDENT COLONEQUAL mod_ext_longident
;
with_type_binder:
    EQUAL
  | EQUAL PRIVATE
;

typevar_list:
        QUOTE ident
      | typevar_list QUOTE ident
;
poly_type:
        core_type
      | typevar_list DOT core_type
;

core_type:
    core_type2
  | core_type2 AS QUOTE ident
;
core_type2:
    simple_core_type_or_tuple
  | QUESTION LIDENT COLON core_type2 MINUSGREATER core_type2
  | OPTLABEL core_type2 MINUSGREATER core_type2
  | LIDENT COLON core_type2 MINUSGREATER core_type2
  | core_type2 MINUSGREATER core_type2
;

simple_core_type:
    simple_core_type2  %prec below_SHARP
  | LPAREN core_type_comma_list RPAREN %prec below_SHARP
  | simple_core_type attribute
;

simple_core_type_no_attr:
    simple_core_type2  %prec below_SHARP
  | LPAREN core_type_comma_list RPAREN %prec below_SHARP
;

simple_core_type2:
    QUOTE ident
  | UNDERSCORE
  | type_longident
  | simple_core_type2 type_longident
  | LPAREN core_type_comma_list RPAREN type_longident
  | LESS meth_list GREATER
  | LESS GREATER
  | SHARP class_longident
  | simple_core_type2 SHARP class_longident
  | LPAREN core_type_comma_list RPAREN SHARP class_longident
  | LBRACKET tag_field RBRACKET
  | LBRACKET BAR row_field_list RBRACKET
  | LBRACKET row_field BAR row_field_list RBRACKET
  | LBRACKETGREATER opt_bar row_field_list RBRACKET
  | LBRACKETGREATER RBRACKET
  | LBRACKETLESS opt_bar row_field_list RBRACKET
  | LBRACKETLESS opt_bar row_field_list GREATER name_tag_list RBRACKET
  | LPAREN MODULE package_type RPAREN
  | extension
;
package_type:
    mty_longident
  | mty_longident WITH package_type_cstrs
;
package_type_cstr:
    TYPE label_longident EQUAL core_type
;
package_type_cstrs:
    package_type_cstr
  | package_type_cstr AND package_type_cstrs
;
row_field_list:
    row_field
  | row_field_list BAR row_field
;
row_field:
    tag_field
  | simple_core_type
;
tag_field:
    name_tag attributes OF opt_ampersand amper_type_list
  | name_tag attributes
;
opt_ampersand:
    AMPERSAND
  |
;
amper_type_list:
    core_type
  | amper_type_list AMPERSAND core_type
;
name_tag_list:
    name_tag
  | name_tag_list name_tag
;
simple_core_type_or_tuple:
    simple_core_type %prec below_LBRACKETAT
  | simple_core_type STAR core_type_list

;
simple_core_type_or_tuple_no_attr:
    simple_core_type_no_attr
  | simple_core_type_no_attr STAR core_type_list_no_attr
;
core_type_comma_list:
    core_type
  | core_type_comma_list COMMA core_type
;
core_type_list:
    simple_core_type %prec below_LBRACKETAT
  | core_type_list STAR simple_core_type
;
core_type_list_no_attr:
    simple_core_type_no_attr
  | core_type_list STAR simple_core_type_no_attr
;
meth_list:
    field SEMI meth_list
  | field opt_semi
  | DOTDOT
;
field:
    label attributes COLON poly_type
;
label:
    LIDENT
;

constant:
    INT
  | CHAR
  | STRING
  | FLOAT
  | INT32
  | INT64
  | NATIVEINT
;
signed_constant:
    constant
  | MINUS INT
  | MINUS FLOAT
  | MINUS INT32
  | MINUS INT64
  | MINUS NATIVEINT
  | PLUS INT
  | PLUS FLOAT
  | PLUS INT32
  | PLUS INT64
  | PLUS NATIVEINT
;

ident:
    UIDENT
  | LIDENT
;
val_ident:
    LIDENT
  | LPAREN operator RPAREN
  | LPAREN operator error
  | LPAREN error
  | LPAREN MODULE error
;
operator:
    PREFIXOP
  | INFIXOP0
  | INFIXOP1
  | INFIXOP2
  | INFIXOP3
  | INFIXOP4
  | BANG
  | PLUS
  | PLUSDOT
  | MINUS
  | MINUSDOT
  | STAR
  | EQUAL
  | LESS
  | GREATER
  | OR
  | BARBAR
  | AMPERSAND
  | AMPERAMPER
  | COLONEQUAL
  | PLUSEQ
  | PERCENT
;
constr_ident:
    UIDENT
  | LPAREN RPAREN
  | COLONCOLON
  | FALSE
  | TRUE
;

val_longident:
    val_ident
  | mod_longident DOT val_ident
;
constr_longident:
    mod_longident       %prec below_DOT
  | LBRACKET RBRACKET
  | LPAREN RPAREN
  | FALSE
  | TRUE
;
label_longident:
    LIDENT
  | mod_longident DOT LIDENT
;
type_longident:
    LIDENT
  | mod_ext_longident DOT LIDENT
;
mod_longident:
    UIDENT
  | mod_longident DOT UIDENT
;
mod_ext_longident:
    UIDENT
  | mod_ext_longident DOT UIDENT
  | mod_ext_longident LPAREN mod_ext_longident RPAREN
;
mty_longident:
    ident
  | mod_ext_longident DOT ident
;
clty_longident:
    LIDENT
  | mod_ext_longident DOT LIDENT
;
class_longident:
    LIDENT
  | mod_longident DOT LIDENT
;

name_tag:
    BACKQUOTE ident
;
rec_flag:
  | REC
;
direction_flag:
    TO
  | DOWNTO
;
private_flag:
  | PRIVATE
;
mutable_flag:
  | MUTABLE
;
virtual_flag:
  | VIRTUAL
;
private_virtual_flags:
  | PRIVATE
  | VIRTUAL
  | PRIVATE VIRTUAL
  | VIRTUAL PRIVATE
;
override_flag:
  | BANG
;
opt_bar:
  | BAR
;
opt_semi:
  | SEMI
;
subtractive:
    MINUS
  | MINUSDOT
;
additive:
    PLUS
  | PLUSDOT
;

single_attr_id:
    LIDENT
  | UIDENT
  | AND
  | AS
  | ASSERT
  | BEGIN
  | CLASS
  | CONSTRAINT
  | DO
  | DONE
  | DOWNTO
  | ELSE
  | END
  | EXCEPTION
  | EXTERNAL
  | FALSE
  | FOR
  | FUN
  | FUNCTION
  | FUNCTOR
  | IF
  | IN
  | INCLUDE
  | INHERIT
  | INITIALIZER
  | LAZY
  | LET
  | MATCH
  | METHOD
  | MODULE
  | MUTABLE
  | NEW
  | OBJECT
  | OF
  | OPEN
  | OR
  | PRIVATE
  | REC
  | SIG
  | STRUCT
  | THEN
  | TO
  | TRUE
  | TRY
  | TYPE
  | VAL
  | VIRTUAL
  | WHEN
  | WHILE
  | WITH
;

attr_id:
    single_attr_id
  | single_attr_id DOT attr_id
;
attribute:
  LBRACKETAT attr_id payload RBRACKET
;
post_item_attribute:
  LBRACKETATAT attr_id payload RBRACKET
;
floating_attribute:
  LBRACKETATATAT attr_id payload RBRACKET
;
post_item_attributes:
  | post_item_attribute post_item_attributes
;
attributes:
  | attribute attributes
;
extension:
  LBRACKETPERCENT attr_id payload RBRACKET
;
item_extension:
  LBRACKETPERCENTPERCENT attr_id payload RBRACKET
;
payload:
    structure
  | COLON core_type
  | QUESTION pattern
  | QUESTION pattern WHEN seq_expr
;
