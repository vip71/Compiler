//
// Created by Asia i Tomek on 30.12.2023.
//
#include "handlers.h"

void handle_heap_init(int variables,unsigned long long *line, FILE * file){
    append_to_result_code("RST h \n",line,file);
    append_to_result_code("RST g \n",line,file);
    append_to_result_code("INC g #HEAP INIT\n",line,file);
    add_to_register("g",variables,line,file);
}

void handle_procedure_head(struct Map *map,const char *s,unsigned long long line){
    //printf("PROC HEAD %s at line %d\n",s,line);
    addToMap(map, s, line);
}

void handle_procedure_call(const char *caller,const char *procedure,struct Map *result_code_func_map,struct MapHeap *heap,struct Map * given_params,unsigned long long *line, FILE * file){
    int procedure_start = getFromMap(result_code_func_map,procedure);
    if(strcmp(caller,procedure)==0){
        yyerror("Recursion is forbidden");
        return;
    }

    if(procedure_start==-1){
        yyerror("Unknown procedure");
        return;
    }

    //printf("PROC CALL %s into line %d from %s\n",procedure,procedure_start,caller);
    struct Map * caller_params = getHeapElementByNumber(heap,getKeyIndex(result_code_func_map,caller));
    struct Map * expected_variables = getHeapElementByNumber(heap,getKeyIndex(result_code_func_map,procedure));
    int expected_parameters_size = count_in_outs(getHeapElementByNumber(heap,getKeyIndex(result_code_func_map,procedure)));
    //printf("expected parameters %d\n",expected_parameters_size);
    //printf("given_params: \n");
    //printMap(given_params);
    if(hasValue(given_params,-1) == 1){
        yyerror("One of given parameters is unknown");
        return;
    }
    if(expected_parameters_size != given_params->size){
        yyerror("Invalid amount of parameters!");
        return;
    }
    for(int i=0;i<expected_parameters_size;i++){
        //printMap(expected_variables);
        //printMap(given_params);
        if(expected_variables->entries[i].type != given_params->entries[i].type){
            yyerror("Invalid types of parameters in function call");
            return;
        }
    }
    //printf("\n");


    //append_to_result_code("GET g # debug\n",line,file);
    //append_to_result_code("WRITE # debug\n",line,file);
    append_to_result_code("GET h\n",line,file);
    append_to_result_code("PUT b #zapisujemy stary koniec stosu w b\n",line,file);
    append_to_result_code("GET g\n",line,file);
    append_to_result_code("PUT h\n",line,file);
    append_to_result_code("INC h #zapisujemy nowy koniec stosu w miejscu poprzedni return line +1\n",line,file);
    append_to_result_code("GET b\n",line,file);
    append_to_result_code("STORE h #zapisany stary koniec stosu w miejscu pamieci nowego\n",line,file);
    append_to_result_code("PUT c # !!!!! uwaga to stosujemy później!!!!\n",line,file);
    append_to_result_code("GET h\n",line,file);
    append_to_result_code("PUT g\n",line,file);
    append_to_result_code("INC g #tylko paceholder na przesuniecie i nowe zmienne\n",line,file);


    int procedure_number = getKeyIndex(result_code_func_map,procedure);
    struct Map * procedure_variables_map = getHeapElementByNumber(heap,procedure_number);
    int procedure_all_variables = getFromMap(procedure_variables_map,"!all_variables!");

    //manage parameters
    for(int i=0;i<given_params->size;i++) {
        set_register("b", given_params->entries[i].value+1, line, file);
        append_to_result_code("GET c\n",line,file);
        append_to_result_code("ADD b\n",line,file);
        //append_to_result_code("WRITE \n",line,file);
        if(is_in_out(caller_params,given_params->entries[i].key)) {
            //printMap(caller_params);
            //printf("DUUUUUUUUUUUPAAAAAAAAAAAAAAAAAAA %s \n",given_params->entries[i].key);
            append_to_result_code("LOAD a\n",line,file);
            //append_to_result_code("WRITE \n",line,file);
        }
        //append_to_result_code("LOAD a\n",line,file);
        //append_to_result_code("WRITE \n",line,file);

        append_to_result_code("STORE g\n",line,file);
        append_to_result_code("INC g\n",line,file);
    }
    //alloc memory for not * variables
    //parameters always have length 1
    add_to_register("g",procedure_all_variables-expected_parameters_size,line,file);

    append_to_result_code("RST b\n",line,file);
    append_to_result_code("INC b\n",line,file);
    append_to_result_code("SHL b\n",line,file);
    append_to_result_code("SHL b #ustawiono b=4\n",line,file);
    append_to_result_code("STRK a\n",line,file);
    append_to_result_code("ADD b\n",line,file);
    append_to_result_code("STORE g\n",line,file);
    append_to_result_code(concat(append_int_to_string("JUMP ",procedure_start),"\n"),line,file);
}

void handle_procedure_end(unsigned long long *result_code_next_line, FILE * file){
    append_to_result_code("LOAD g\n",result_code_next_line,file);
    append_to_result_code("PUT b # loads return function address into b\n",result_code_next_line,file);
    append_to_result_code("GET h\n",result_code_next_line,file);
    append_to_result_code("PUT g\n",result_code_next_line,file);
    append_to_result_code("DEC g # sets previous function return address \n",result_code_next_line,file);
    append_to_result_code("LOAD h\n",result_code_next_line,file);
    append_to_result_code("PUT h # sets previous heap return address\n",result_code_next_line,file);
    append_to_result_code("JUMPR b # jumps back to call+1\n",result_code_next_line,file);
}

void handle_variable_declaration(struct MapHeap* heap, const char* variable_name, int* procedure_variables,int vars_declared,char type){
    addToTopMapUniqueWithType(heap,variable_name,*procedure_variables,type);
    *procedure_variables+=vars_declared;
}

void handle_declarations(struct MapHeap* heap, int* procedure_variables, int* procedures_declared){
    *procedures_declared += 1;
    addToTopMapWithType(heap,"!all_variables!",*procedure_variables,'!');
}

void handle_procedure_declaration_start(struct MapHeap* heap, int* procedure_variables){
    //TODO: ADD if(heap.empty) skip;
    //addToTopMapWithType(heap,"!all_variables!",*procedure_variables,'!');
    *procedure_variables = 0;
    addToHeap(heap, createMap());
}

void get_pointer(const char * result_register,const char * var_name, int table_offset,struct Map * procedure_variables,unsigned long long *line, FILE * file){
    int place_in_heap = get_parameter_location(procedure_variables,var_name);
    int in_out = is_in_out(procedure_variables,var_name);

    //printf("%s place in h: %d, is in out %d\n",var_name,place_in_heap,in_out);
    if(place_in_heap == -1){
        yyerror(concat(var_name," is undeclared variable"));
        return;
    }

    set_register("a",place_in_heap+1,line,file);
    append_to_result_code("ADD h\n",line,file);
    if(in_out == 1){
        append_to_result_code("LOAD a\n",line,file);
    }
    //append_to_result_code("WRITE #debug\n",line,file);
    append_to_result_code(concat(concat("PUT ",result_register),"\n"),line,file);
    if(table_offset>0){
        add_to_register(result_register,table_offset,line,file);
    }
}

void get_table_pointer(const char * result_register,const char * var_name, int table_offset,struct Map * procedure_variables,unsigned long long *line, FILE * file){
    char type = get_parameter_type(procedure_variables,var_name);
    if((type=='t' && (max_offset(procedure_variables,var_name) >= table_offset)) || is_in_out(procedure_variables,var_name)) {
        get_pointer(result_register, var_name, table_offset, procedure_variables, line, file);
    }
    else if(type=='t'){
        //printf("table offset %d\n",table_offset);
        //printf("max offset %d\n",max_offset(procedure_variables,var_name));
        printMap(procedure_variables);
        yyerror("Index out of range!");
    }
    else if(type =='i'){
        yyerror(concat("Variable ",concat(var_name," is not TABLE")));
    } else{
        yyerror(concat(var_name," is undeclared variable"));
    }
}

void get_number_pointer(const char * result_register,const char * var_name,struct Map * procedure_variables,unsigned long long *line, FILE * file){
    char type = get_parameter_type(procedure_variables,var_name);
    if(type=='i') {
        get_pointer(result_register, var_name, 0, procedure_variables, line, file);
    } else if(type =='t'){
        yyerror(concat("Variable ",concat(var_name," is not NUMBER")));
    } else{
        yyerror(concat(var_name," is undeclared variable"));
    }
}

void get_pointer_value(const char * result_register,unsigned long long *line, FILE * file){
    append_to_result_code(concat(concat("LOAD ",result_register),"\n"),line,file);
    append_to_result_code(concat(concat("PUT ",result_register),"\n"),line,file);
}

void get_pointer_table_variant(const char * table_name,const char * var_name, struct Map * procedure_variables,unsigned long long *line, FILE * file){
    char var_type = get_parameter_type(procedure_variables,var_name);
    char table_type = get_parameter_type(procedure_variables,table_name);
    if(var_type !='i' || table_type!='t'){
        yyerror(concat(concat(concat(concat("invalid use of ",table_name),"["),var_name),"]"));
        return;
    }
    //printf("finding in memory %s and shifting %s\n",table_name,var_name);
    get_pointer("e",var_name,0,procedure_variables,line,file);
    get_pointer_value("e",line,file);
    get_pointer("b",table_name,0,procedure_variables,line,file);
    //printf("liniia przed dodaniem %llu\n",*line);
    append_to_result_code("GET b\n",line,file);
    append_to_result_code("ADD e\n",line,file);
    append_to_result_code("PUT b\n",line,file);
}

void copy_register(const char * source_reg, const char * destination_reg,unsigned long long *line, FILE * file){
    append_to_result_code(concat("GET ",concat(source_reg,"\n")),line,file);
    append_to_result_code(concat("PUT ",concat(destination_reg,"\n")),line,file);
}

void count_expression(char expression_char,unsigned long long *line, FILE * file){
    if(expression_char == '+'){
        append_to_result_code("GET b\n",line,file);
        append_to_result_code("ADD c\n",line,file);
        append_to_result_code("STORE d\n",line,file);
    }
    else if(expression_char == '-'){
        append_to_result_code("GET c\n",line,file);
        append_to_result_code("SUB b\n",line,file);
        append_to_result_code("STORE d\n",line,file);
    }
    else if(expression_char == '*'){
        //swap
        append_to_result_code("GET b\n",line,file);
        append_to_result_code("SUB c\n",line,file);
        append_to_result_code(concat(append_int_to_string("JZERO ",*line+7),"\n"),line,file);
        append_to_result_code("GET c\n",line,file);
        append_to_result_code("PUT e\n",line,file);
        append_to_result_code("GET b\n",line,file);
        append_to_result_code("PUT c\n",line,file);
        append_to_result_code("GET e\n",line,file);
        append_to_result_code("PUT b\n",line,file);

        //multiplication
        append_to_result_code("RST f\n",line,file);
        append_to_result_code("GET c\n",line,file);
        append_to_result_code(concat(append_int_to_string("JZERO ",*line+14),"\n"),line,file);
        append_to_result_code("GET b\n",line,file);
        append_to_result_code(concat(append_int_to_string("JZERO ",*line+12),"\n"),line,file);
        append_to_result_code("PUT e\n",line,file);
        append_to_result_code("SHR e\n",line,file);
        append_to_result_code("SHL e\n",line,file);
        append_to_result_code("SUB e\n",line,file);
        append_to_result_code(concat(append_int_to_string("JZERO ",*line+4),"\n"),line,file);
        append_to_result_code("GET c\n",line,file);
        append_to_result_code("ADD f\n",line,file);
        append_to_result_code("PUT f\n",line,file);
        append_to_result_code("SHL c\n",line,file);
        append_to_result_code("SHR b\n",line,file);
        append_to_result_code(concat(append_int_to_string("JUMP ",*line-12),"\n"),line,file);
        append_to_result_code("GET f\n",line,file);
        append_to_result_code("STORE d\n",line,file);
    }
    else if(expression_char == '/' || expression_char == '%'){
        //kopia
        append_to_result_code("GET b\n",line,file);
        append_to_result_code(concat(append_int_to_string("JZERO ",*line+30),"\n"),line,file);
        append_to_result_code("PUT e\n",line,file);
        append_to_result_code("GET c\n",line,file);
        append_to_result_code("PUT f\n",line,file);

        append_to_result_code("GET e\n",line,file);
        append_to_result_code(concat(append_int_to_string("JZERO ",*line+4),"\n"),line,file);
        append_to_result_code("SHR e\n",line,file);
        append_to_result_code("SHR f\n",line,file);
        append_to_result_code(concat(append_int_to_string("JUMP ",*line-4),"\n"),line,file);

        append_to_result_code("GET b\n",line,file);
        append_to_result_code("PUT e\n",line,file);

        append_to_result_code("GET f\n",line,file);
        append_to_result_code(concat(append_int_to_string("JZERO ",*line+4),"\n"),line,file);
        append_to_result_code("SHR f\n",line,file);
        append_to_result_code("SHL b\n",line,file);
        append_to_result_code(concat(append_int_to_string("JUMP ",*line-4),"\n"),line,file);

        append_to_result_code("SHL f\n",line,file);
        append_to_result_code("GET c\n",line,file);
        append_to_result_code("INC a\n",line,file);
        append_to_result_code("SUB b\n",line,file);
        append_to_result_code(concat(append_int_to_string("JZERO ",*line+4),"\n"),line,file);
        append_to_result_code("INC f\n",line,file);
        append_to_result_code("DEC a\n",line,file);
        append_to_result_code("PUT c\n",line,file);
        append_to_result_code("SHR b\n",line,file);
        append_to_result_code("GET b\n",line,file);
        append_to_result_code("INC a\n",line,file);
        append_to_result_code("SUB e\n",line,file);
        append_to_result_code(concat(append_int_to_string("JPOS ",*line-12),"\n"),line,file);
        append_to_result_code(concat(append_int_to_string("JUMP ",*line+2),"\n"),line,file);

        if(expression_char == '/') {
            append_to_result_code("RST f\n", line, file);
        }
        else {
            append_to_result_code("RST c\n", line, file);
        }

        if(expression_char == '/') {
            append_to_result_code("GET f\n", line, file);
        }
        else {
            append_to_result_code("GET c\n", line, file);
        }

        append_to_result_code("STORE d\n",line,file);
    }
    else if(expression_char == '_'){
        append_to_result_code("GET b\n",line,file);
        append_to_result_code("STORE d\n",line,file);
    }
    else if(expression_char == '<'){
        append_to_result_code("GET b\n",line,file);
        append_to_result_code("SUB c\n",line,file);
    }
    else if(expression_char == '{'){
        append_to_result_code("GET b\n",line,file);
        append_to_result_code("INC a\n",line,file);
        append_to_result_code("SUB c\n",line,file);
    }
    else if(expression_char == '>'){
        append_to_result_code("GET c\n",line,file);
        append_to_result_code("SUB b\n",line,file);
    }
    else if(expression_char == '}'){
        append_to_result_code("GET c\n",line,file);
        append_to_result_code("INC a\n",line,file);
        append_to_result_code("SUB b\n",line,file);
    }
    else if(expression_char == '='){
        append_to_result_code("RST d # = poczatek\n",line,file);
        append_to_result_code("GET b\n",line,file);
        append_to_result_code("SUB c\n",line,file);
        append_to_result_code(concat(append_int_to_string("JPOS ",*line+5),"\n"),line,file);
        append_to_result_code("GET c\n",line,file);
        append_to_result_code("SUB b\n",line,file);
        append_to_result_code(concat(append_int_to_string("JPOS ",*line+2),"\n"),line,file);
        append_to_result_code("INC d\n",line,file);
        append_to_result_code("GET d # = koniec\n",line,file);
    }
    else if(expression_char == '!'){
        append_to_result_code("RST d\n",line,file);
        append_to_result_code("GET b\n",line,file);
        append_to_result_code("SUB c\n",line,file);
        append_to_result_code(concat(append_int_to_string("JZERO ",*line+2),"\n"),line,file);
        append_to_result_code("INC d\n",line,file);

        append_to_result_code("GET c\n",line,file);
        append_to_result_code("SUB b\n",line,file);
        append_to_result_code(concat(append_int_to_string("JZERO ",*line+2),"\n"),line,file);
        append_to_result_code("INC d\n",line,file);
        append_to_result_code("GET d\n",line,file);
    }
}

void append_to_result_code(const char *s, unsigned long long *result_code_next_line, FILE * file){
    fprintf(file, "%s", s);
    (*result_code_next_line)++;
}

void add_to_register(const char* machine_register, unsigned long long constant,unsigned long long *line, FILE * file){
    // mozna zobaczyc kiedy robic fora z ++
    // obslozyc blad gdy rejest to a
    if(constant>10){
        set_register("a",constant,line,file);
        append_to_result_code(concat("ADD ",concat(machine_register,"\n")),line,file);
        append_to_result_code(concat("PUT ",concat(machine_register,"\n")),line,file);
    }
    else{
        for(int i=0;i<constant;i++){
            append_to_result_code(concat("INC ",concat(machine_register,"\n")),line,file);
        }
    }
}

void set_register(const char* machine_register, unsigned long long constant,unsigned long long *result_code_next_line, FILE * file)//emaby make long
{
    int amount_of_digits = findLeftmostSetBit(constant);
    append_to_result_code(concat("RST ", concat(machine_register, "#set reg start\n")), result_code_next_line, file);
    for(int i=0;i<amount_of_digits;i++){
        if(getNthBinaryDigit(constant, amount_of_digits-i)){
            append_to_result_code(concat("INC ", concat(machine_register, "\n")), result_code_next_line, file);
        }
        append_to_result_code(concat("SHL ", concat(machine_register, "\n")), result_code_next_line, file);
    }
    if(getNthBinaryDigit(constant, 0)){
        append_to_result_code(concat("INC ", concat(machine_register, "\n")), result_code_next_line, file);
    }
}

int max_offset(struct Map* map,const char* parameter_name){
    if(parameter_name[0]=='*'){
        return INT_MAX;
    }
    int index = getKeyIndex(map,parameter_name);
    return map->entries[index+1].value - map->entries[index].value - 1;
}

int get_parameter_location(struct Map* map,const char* parameter_name){
    int parameter_location = getFromMap(map, concat("*",parameter_name));
    if(parameter_location == -1){
        parameter_location = getFromMap(map, parameter_name);
    }
    return parameter_location;
}

char get_parameter_type(struct Map* map,const char* parameter_name){
    char parameter_type = getTypeFromMap(map, concat("*",parameter_name));
    if(parameter_type == '?'){
        parameter_type = getTypeFromMap(map, parameter_name);
    }
    return parameter_type;
}

int count_in_outs(struct Map* map){
    int in_outs=0;
    for (int i = 0; i < map->size; ++i) {
        if (map->entries[i].key[0] == '*') {
            in_outs++;
        }
        else {
            return in_outs;
        }
    }
    return in_outs;
}

int is_in_out(struct Map* map, const char * key){
    return hasKey(map, concat("*",key));
}

int has_var_innited(char * var_name, int var_shift, struct Map* initialized, struct Map* procedureVariables){
    if(strcmp(var_name,"?")==0 || is_in_out(procedureVariables,var_name)==1 || hasPair(initialized,var_name,var_shift)){
        return 1;
    }
    yywarning("Variable might not be initialized");
    return 0;
}

void handle_bin_op(char * var_name, int var_shift, struct Map* initialized, struct Map* procedureVariables,unsigned long long *result_code_next_line, FILE * file){
    has_var_innited(var_name,var_shift,initialized,procedureVariables);
    copy_register("b","c",result_code_next_line,file);
}
