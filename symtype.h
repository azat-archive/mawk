
/********************************************
symtype.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/*$Log:	symtype.h,v $
 * Revision 2.1  91/04/08  08:24:14  brennan
 * VERSION 0.97
 * 
*/

/* types related to symbols are defined here */

#ifndef  SYMTYPE_H
#define  SYMTYPE_H


/* struct to hold info about builtins */
typedef struct {
char *name ;
PF_CP  fp ;  /* ptr to function that does the builtin */
unsigned char min_args, max_args ; 
/* info for parser to check correct number of arguments */
} BI_REC ;

/*---------------------------
   structures and types for arrays
 *--------------------------*/

/* array hash nodes */

typedef  struct anode {
struct anode *link ;
STRING *sval ;
CELL   *cp ;
}  ANODE, **ARRAY ;

/* note ARRAY is a ptr to a hash table */

CELL *PROTO(array_find, (ARRAY,void *, int) ) ;
int PROTO(array_test, (ARRAY, STRING *) ) ;
INST *PROTO(array_loop, (INST *, CELL *, CELL *) ) ;
void PROTO(array_delete, (ARRAY, STRING *) ) ;
CELL *PROTO(array_cat, (CELL *, int) ) ;
void PROTO(array_free, (ARRAY) ) ;

#define new_ARRAY() (ARRAY)memset(zmalloc(A_HASH_PRIME *\
                        sizeof(ANODE*)), 0, A_HASH_PRIME*sizeof(ANODE*))

extern  ARRAY  Argv ;

/* for parsing  (i,j) in A  */
typedef  struct {
INST *start ;
int cnt ;
} ARG2_REC ;

/*------------------------
  user defined functions
  ------------------------*/

typedef  struct fblock {
char *name ;
INST *code  ;
unsigned short nargs ;
char *typev ;  /* array of size nargs holding types */
} FBLOCK ;   /* function block */

void  PROTO(add_to_fdump_list, (FBLOCK *) ) ;
void  PROTO( fdump, (void) ) ;

/*-------------------------
  elements of the symbol table
  -----------------------*/

#define  ST_NONE 0
#define  ST_VAR   1
#define  ST_KEYWORD   2
#define  ST_BUILTIN 3 /* a pointer to a builtin record */
#define  ST_ARRAY   4 /* a void * ptr to a hash table */
#define  ST_FIELD   5  /* a cell ptr to a field */
#define  ST_FUNCT   6
#define  ST_LENGTH  7  /* length is special */
#define  ST_LOCAL_NONE  8
#define  ST_LOCAL_VAR   9
#define  ST_LOCAL_ARRAY 10

#define  is_local(stp)   ((stp)->type>=ST_LOCAL_NONE)

typedef  struct {
char *name ;
char type ;
unsigned char offset ;  /* offset in stack frame for local vars */
union {
CELL *cp ;
int  kw ;
PF_CP fp ;
BI_REC *bip ;
ARRAY  array ; 
FBLOCK  *fbp ;
} stval ;
}  SYMTAB ;


/*****************************
 structures for type checking function calls
 ******************************/

typedef  struct ca_rec {
struct ca_rec  *link ;
short type ;
short arg_num ;  /* position in callee's stack */
/*---------  this data only set if we'll  need to patch -------*/
/* happens if argument is an ID or type ST_NONE or ST_LOCAL_NONE */

int call_offset ;
/* where the type is stored */
SYMTAB  *sym_p ;  /* if type is ST_NONE  */
char *type_p ;  /* if type  is ST_LOCAL_NONE */
}  CA_REC  ; /* call argument record */

/* type field of CA_REC matches with ST_ types */
#define   CA_EXPR       ST_LOCAL_VAR
#define   CA_ARRAY      ST_LOCAL_ARRAY

typedef  struct fcall {
struct fcall *link ;
FBLOCK  *callee ;
short   call_scope ;
FBLOCK  *call ;  /* only used if call_scope == SCOPE_FUNCT  */
INST    *call_start ; /* computed later as code may be moved */
CA_REC  *arg_list ;
short   arg_cnt_checked ;
unsigned line_no ; /* for error messages */
} FCALL_REC ;

extern  FCALL_REC  *resolve_list ;

void PROTO(resolve_fcalls, (void) ) ;
void PROTO(check_fcall, (FBLOCK*,int,FBLOCK*,CA_REC*,unsigned) ) ;

/* hash.c */
unsigned  PROTO( hash, (char *) ) ;
SYMTAB *PROTO( insert, (char *) ) ;
SYMTAB *PROTO( find, (char *) ) ;
SYMTAB *PROTO( save_id, (char *) ) ;
void    PROTO( restore_ids, (void) ) ;

/* error.c */
void  PROTO(type_error, (SYMTAB *) ) ;

#endif  /* SYMTYPE_H */