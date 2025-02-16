%{
#include <stdio.h>
#include "myInstructions.hh"
#include "map.h"
#include "heap.h"
#include "map_heap.h"
#include "handlers.h"
#include "strings.h"
int yylex( void );
extern void yyerror(const char*);
extern void yywarning(const char*);
extern int errorOccurred;

struct Map* current_innited_vars;
char * last_var_name;
char * last_assigned_name;
int last_var_shift;
int last_assigned_shift;

struct Map* result_code_func_map;
struct MapHeap variables;
FILE *file;
FILE *input;
extern FILE *yyin;
FILE *output;
unsigned long long result_code_next_line = 0;
int procedure_variables = 0;
int procedures_declared = 0;
char * current_parsed_proc;
struct Map* current_references;
struct Heap* repeat_until_jumps;
struct Heap* while_repeat_jumps;
struct Heap* while_skip_jumps;
struct Heap* true_if_jumps;
struct Heap* false_if_jumps;
char expression_char;
%}

%union {
    char* string;
    unsigned long long number;
}

%token PROCEDURE_T
%token PROGRAM_T
%token IS_T
%token IN_T
%token END_T

%token IF_T
%token THEN_T
%token ELSE_T
%token ENDIF_T

%token REPEAT_T
%token UNTIL_T

%token DO_T
%token WHILE_T
%token ENDWHILE_T

%token ASSIGN_T
%token NEQ_T
%token GT_T
%token LT_T

%token READ_T
%token WRITE_T

%token TABLE_T
%token <string> ID_T
%token <number> NUM_T

%type <number> identifier

%%
expression : program_all

program_all : procedures {prepend_to_file(file,concat(right_padding(append_int_to_string("JUMP ",result_code_next_line),20),"\n"));} main

procedures:   procedures PROCEDURE_T proc_head IS_T declarations {handle_declarations(&variables, &procedure_variables, &procedures_declared);}
              IN_T commands END_T                                {handle_procedure_end(&result_code_next_line, file);}
            | procedures PROCEDURE_T proc_head IS_T              {handle_declarations(&variables, &procedure_variables, &procedures_declared);}
              IN_T commands END_T                                {handle_procedure_end(&result_code_next_line, file);}
            |

main :        PROGRAM_T IS_T {handle_procedure_declaration_start(&variables, &procedure_variables);} declarations {addToTopMapWithType(&variables,"!all_variables!",procedure_variables,'!');current_parsed_proc="!main!";procedures_declared++;addToMap(result_code_func_map,"!main!",result_code_next_line);handle_heap_init(procedure_variables,&result_code_next_line, file);}
              IN_T commands END_T         {append_to_result_code("HALT\n",&result_code_next_line, file);}
            | PROGRAM_T IS_T {handle_procedure_declaration_start(&variables, &procedure_variables);addToTopMap(&variables,"!all_variables!",procedure_variables);current_parsed_proc="!main!";procedures_declared++;addToMap(result_code_func_map,"!main!",result_code_next_line);handle_heap_init(procedure_variables,&result_code_next_line, file);}
              IN_T commands END_T         {append_to_result_code("HALT\n",&result_code_next_line, file);}

commands:     commands command
            | command

command:      identifier
              ASSIGN_T {copy_register("b","d",&result_code_next_line,file);
                        last_assigned_name = last_var_name;
                        last_assigned_shift = last_var_shift;}
              expression ';' {count_expression(expression_char,&result_code_next_line,file);
                        addToMap(current_innited_vars,last_assigned_name,last_assigned_shift);}

            | IF_T condition THEN_T {count_expression(expression_char,&result_code_next_line,file);push(false_if_jumps,ftell(file));append_to_result_code(concat(right_padding("JZERO ?",30),"\n"),&result_code_next_line,file);} commands endif
            | WHILE_T {push(while_repeat_jumps,result_code_next_line);}
                condition DO_T
                 {count_expression(expression_char,&result_code_next_line,file);
                  push(while_skip_jumps,ftell(file));
                  append_to_result_code(concat(right_padding("JZERO ?",30),"#while skip\n"),&result_code_next_line,file);}
                  commands
             ENDWHILE_T {append_to_result_code(concat(append_int_to_string("JUMP ",pop(while_repeat_jumps)),"#while repeat\n"),&result_code_next_line,file);
                         overwrite_file(file,concat(right_padding(append_int_to_string("JZERO ",result_code_next_line),30),"#while skip\n"),pop(while_skip_jumps));}
            | REPEAT_T {push(repeat_until_jumps,result_code_next_line);} commands UNTIL_T condition ';' {count_expression(expression_char,&result_code_next_line,file);append_to_result_code(concat(append_int_to_string("JZERO ",pop(repeat_until_jumps)),"\n"),&result_code_next_line,file);}
            | proc_call ';'
            | READ_T identifier ';' {append_to_result_code("READ\n",&result_code_next_line,file);
                                     append_to_result_code("STORE b\n",&result_code_next_line,file);
                                     addToMap(current_innited_vars,last_var_name,last_var_shift);}
            | WRITE_T value ';' {append_to_result_code("GET b\n",&result_code_next_line,file);
                                 append_to_result_code("WRITE\n",&result_code_next_line,file);
                                 has_var_innited(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables));}

endif: ELSE_T {  push(true_if_jumps,ftell(file));append_to_result_code(concat(right_padding("JUMP ?",30),"\n"),&result_code_next_line,file);
                overwrite_file(file,concat(right_padding(append_int_to_string("JZERO ",result_code_next_line),30),"\n"),pop(false_if_jumps));}
                commands ENDIF_T {overwrite_file(file,concat(right_padding(append_int_to_string("JUMP ",result_code_next_line),30),"\n"),pop(true_if_jumps));}
     | ENDIF_T {overwrite_file(file,concat(right_padding(append_int_to_string("JZERO ",result_code_next_line),30),"\n"),pop(false_if_jumps));}


proc_head:  ID_T {handle_procedure_declaration_start(&variables, &procedure_variables);}
            '(' args_decl ')' {current_parsed_proc=$1;
                               handle_procedure_head(result_code_func_map,$1,result_code_next_line);
                               destroyMap(current_innited_vars);
                               current_innited_vars = createMap();}

proc_call:  ID_T '(' args ')' {handle_procedure_call(current_parsed_proc,$1,result_code_func_map,&variables,current_references,&result_code_next_line,file);destroyMap(current_references);current_references=createMap();}

declarations: declarations ',' ID_T                 {handle_variable_declaration(&variables,$3,&procedure_variables,1,'i');}
             | declarations ',' ID_T '[' NUM_T ']'  {handle_variable_declaration(&variables,$3,&procedure_variables,$5,'t');}
             | ID_T                                 {handle_variable_declaration(&variables,$1,&procedure_variables,1,'i');}
             | ID_T '[' NUM_T ']'                   {handle_variable_declaration(&variables,$1,&procedure_variables,$3,'t');}

args_decl:   args_decl ',' ID_T          {handle_variable_declaration(&variables,concat("*",$3),&procedure_variables,1,'i');}
            | args_decl ',' TABLE_T ID_T {handle_variable_declaration(&variables,concat("*",$4),&procedure_variables,1,'t');}
            | ID_T                       {handle_variable_declaration(&variables,concat("*",$1),&procedure_variables,1,'i');}
            | TABLE_T ID_T               {handle_variable_declaration(&variables,concat("*",$2),&procedure_variables,1,'t');}

args:     args ',' ID_T {addToMapWithType(current_references,$3,get_parameter_location(lookAtHeap(&variables),$3),get_parameter_type(lookAtHeap(&variables),$3));}
         | ID_T         {addToMapWithType(current_references,$1,get_parameter_location(lookAtHeap(&variables),$1),get_parameter_type(lookAtHeap(&variables),$1));}

expression:     value       {expression_char='_';has_var_innited(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables));}
                | value '+' {expression_char='+';handle_bin_op(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables),&result_code_next_line,file);} value {has_var_innited(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables));}
                | value '-' {expression_char='-';handle_bin_op(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables),&result_code_next_line,file);} value {has_var_innited(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables));}
                | value '*' {expression_char='*';handle_bin_op(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables),&result_code_next_line,file);} value {has_var_innited(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables));}
                | value '/' {expression_char='/';handle_bin_op(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables),&result_code_next_line,file);} value {has_var_innited(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables));}
                | value '%' {expression_char='%';handle_bin_op(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables),&result_code_next_line,file);} value {has_var_innited(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables));}

condition:      value '='       {expression_char='=';handle_bin_op(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables),&result_code_next_line,file);} value {has_var_innited(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables));}
                | value NEQ_T   {expression_char='!';handle_bin_op(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables),&result_code_next_line,file);} value {has_var_innited(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables));}
                | value '>'     {expression_char='>';handle_bin_op(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables),&result_code_next_line,file);} value {has_var_innited(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables));}
                | value '<'     {expression_char='<';handle_bin_op(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables),&result_code_next_line,file);} value {has_var_innited(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables));}
                | value LT_T    {expression_char='{';handle_bin_op(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables),&result_code_next_line,file);} value {has_var_innited(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables));}
                | value GT_T    {expression_char='}';handle_bin_op(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables),&result_code_next_line,file);} value {has_var_innited(last_var_name,last_var_shift,current_innited_vars,lookAtHeap(&variables));}

value: NUM_T {set_register("b",$1,&result_code_next_line,file);last_var_name="?";last_var_shift=0;}
       | identifier {get_pointer_value("b",&result_code_next_line,file);}


identifier:     ID_T                 {get_number_pointer("b",$1, lookAtHeap(&variables),&result_code_next_line,file);   last_var_name=$1;last_var_shift=0;}
                | ID_T '[' NUM_T ']' {get_table_pointer("b",$1, $3,lookAtHeap(&variables),&result_code_next_line,file); last_var_name=$1;last_var_shift=$3;}
                | ID_T '[' ID_T ']'  {get_pointer_table_variant($1, $3,lookAtHeap(&variables),&result_code_next_line,file); last_var_name=$3;last_var_shift=0;}
%%

int main(int argc, char *argv[]) {
    variables.size = 0;
    result_code_func_map = createMap();
    current_innited_vars = createMap();
    repeat_until_jumps = createHeap();
    while_repeat_jumps = createHeap();
    while_skip_jumps = createHeap();
    true_if_jumps = createHeap();
    false_if_jumps = createHeap();
    current_references=createMap();

    input = fopen(argv[1],"r");
    file = fopen("temp.txt", "w");


    if (file == NULL || input == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }
    append_to_result_code(concat(right_padding("JUMP ?",20),"\n"), &result_code_next_line, file);
    yyin = input;
    yyparse();
    fclose(input);
    fclose(file);

    if(errorOccurred==0)
    {
        char ch;
        file = fopen("temp.txt", "rb");
        output = fopen(argv[2], "wb");
        while ((ch = fgetc(file)) != EOF) {
            fputc(ch, output);
        }
        fclose(output);
        fclose(file);
    }
    else
    {
    printf("\033[31mError Occured. Compilation Failed.\n");
    }


    remove("temp.txt");

    destroyMap(result_code_func_map);
    destroyMap(current_innited_vars);
    destroyHeap(repeat_until_jumps);
    destroyHeap(while_repeat_jumps);
    destroyHeap(while_skip_jumps);
    destroyHeap(true_if_jumps);
    destroyHeap(false_if_jumps);
    destroyMap(current_references);
    return 0;
}