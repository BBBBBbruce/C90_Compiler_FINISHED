/*
1. globals      (name, types)
2. functions    (return type, parameters)
3. types        (name, size)
4. structs      (name, variable names)
5. scopes + variables (name, scope, +if_temp)

one context that will hold all of that + char

f - function definition
fr - function return type
fn - function name
fp - function params

b - body of function
v - variable
g - global var
s - struct definition
e - enum definition

bj - jump to function
bv - variable
*/

#ifndef context_hpp
#define context_hpp

#include <string>
#include <iostream>
#include <vector>
#include <deque>
#include <stdint.h>
#include <limits.h>

#define GLOBAL_MEM 1000000 // addresses milion+ - means it's global

typedef unsigned long ctr;
typedef long address;

struct type{
    std::string name;
    address size;
    bool sign;
    type( std::string n, address s, bool is_signed)
    : name(n), size(s), sign(is_signed) { }
    type() {}
};
struct variable{
    std::string v_name;
    std::string t_name; // just the name
    address adr;
    bool temp;
    bool ptr;
    std::deque<int> dim;

    variable( std::string vn, std::string tn, address a, bool t = false)
    : v_name(vn), t_name(tn), adr(a), temp(t), ptr(false) { }

    variable(){
        temp = false;
        ptr = false;
    }

    void print(){
        std::cout<<"name: "<<v_name;
        std::cout<<"\ttype: "<<t_name;
        std::cout<<"\taddr: \t"<<adr;
        if(temp) std::cout<<" is temp";
        if(ptr) std::cout<<"\t is ptr";
        std::cout<<'\n';
    }
};
struct scope{
    std::vector<variable> vars;
    address next_addr;
};
struct function{
    std::string r_type;
    std::string name;
    std::deque<variable> params;
    address instruction_entry_addr; //redundant
    int size_of_stack;
    int size_of_params;
};

class context{
private:
    std::vector<function> fnc;
    std::vector<type> types;
    scope globals;

    std::vector<scope> scopes;
    std::vector<std::vector<address>> arg_lists;

    long Nlabels;
    long Ntemp_vars;

public:
    std::vector<std::string> program;
    char state[3];
    context(){
        globals.next_addr = GLOBAL_MEM;
        Nlabels = 0;
        Ntemp_vars = 0;

        state[0] = 'x';
        state[1] = 'x';
        state[2] = 'x';

        //initialize all types
        (*this).add_type("char",  4);
        (*this).add_type("unsigned char",  4);
        (*this).add_type("short",  4);
        (*this).add_type("int",   4);
        (*this).add_type("float", 4);
        (*this).add_type("bool",  4);
        (*this).add_type("double",8);
    };

    // function
    void add_function(){
        function e;
        e.instruction_entry_addr = 0;
        fnc.push_back(e);
    }
    void set_return(std::string return_type){
        if(fnc.size()==0) std::cerr<<"e: set_retrun - no functions\n";
        else{
            fnc.back().r_type = return_type;
        }
    }
    address get_return_size(){
        if(fnc.size()==0) std::cerr<<"e: get_return_size - no functions\n";
        else{
            std::string t = fnc.back().r_type;
            return get_type_size(t);
        }
        return 0;
    }
    void set_fnc_name(std::string function_name){
        if(fnc.size()==0) std::cerr<<"e: set_f_name - no functions\n";
        else fnc.back().name = function_name;}
    std::string get_fnc_name(){
        if(fnc.size()==0) std::cerr<<"e: get_fnc_name - no functions\n";
        else
            return fnc.back().name;
        return NULL;
    }
    void add_parameter(){
        if(fnc.size()==0) std::cerr<<"e: add_parameter - no functions\n";
        else{
            variable e;
            fnc.back().params.push_front(e);
        }
    }
    void set_par_type(std::string par_type){
        if(fnc.size()==0) std::cerr<<"e: set_par_type - no functions\n";
        else if(fnc.back().params.size()==0)
            std::cerr<<"e: set_par_type - no parameters\n";
        else{
            fnc.back().params[0].t_name = par_type;
        }
    }
    void set_par_name(std::string par_name){
        if(fnc.size()==0) std::cerr<<"e: set_par_type - no functions\n";
        else if(fnc.back().params.size()==0)
            std::cerr<<"e: set_par_type - no parameters\n";
        else
            fnc.back().params[0].v_name = par_name;
    }

    // types
    void add_type(std::string s, address n){
        type new_type;
        new_type.name = s;
        new_type.size = n;
        types.push_back(new_type);
    }
    address get_type_size(std::string type_name) {
        for(ctr i=0;i<types.size();i++)
            if(types[i].name==type_name)
                return types[i].size;
        std::cerr<<"e: get_type_size - no type: "<<type_name<<'\n';
        return 0;
    }

    // scope
    void add_scope(){
        //std::cerr << "adding scope\n";
        scope e;
        if(scopes.size()==0) e.next_addr = 0;
        else e.next_addr = scopes.back().next_addr;
        scopes.push_back(e);
    }
    void pop_scope(){
        //std::cerr << "poping scope\n";
        //print_scope();
        std::cerr << '\n';
        if(scopes.size()==0)std::cerr<<"e: pop_scope - no scopes\n";
        else scopes.pop_back();
    }
    void add_var(){
        variable e;
        if(scopes.size()==0)
            globals.vars.push_back(e);
        else
            scopes.back().vars.push_back(e);
    }
    void add_tvar(){
        variable e;
        e.temp = true;
        if(scopes.size()==0)
            globals.vars.push_back(e);
        else
            scopes.back().vars.push_back(e);
    }
    void set_var_name(std::string name){
        if(scopes.size()==0) {
            if(globals.vars.size()==0)
                std::cerr << "e: set_var_name - no g variable\n";
            else globals.vars.back().v_name = name;
        } else if(scopes.back().vars.size()==0)
            std::cerr<<"e: set_var_name - no l variables\n";
        else scopes.back().vars.back().v_name = name;
    }
    address set_var_type(std::string type_name){
        if(scopes.size()==0){
            if(globals.vars.size()==0)
                std::cerr << "e: set_var_type - no g variable\n";
            else{
                globals.vars.back().t_name = type_name;
                address temp = globals.next_addr;
                globals.vars.back().adr = temp;
                globals.next_addr += get_type_size(type_name);
                return temp;
            }
        }
        else if(scopes.back().vars.size()==0)
            std::cerr<<"e: set_var_name - no l variables\n";
        else{
            scopes.back().vars.back().t_name = type_name;
            address temp = scopes.back().next_addr;
            scopes.back().vars.back().adr = temp;
            scopes.back().next_addr += get_type_size(type_name);
            return temp;
        }
        return 0;
    }
    std::string get_var_type(address var_a){
        for(ctr i=scopes.size()-1;i!=ULONG_MAX;i--)
            for(ctr j=0;j<scopes[i].vars.size();j++)
                if(scopes[i].vars[j].adr == var_a){
                    std::string typ = scopes[i].vars[j].t_name;
                    return typ;
                }

        for(ctr i=0;i<globals.vars.size();i++){
            if(globals.vars[i].adr == var_a){
                std::string typ = globals.vars[i].t_name;
                return typ;
            }
        }
        std::cerr<<"e: get_var_t - no variable at address: "<<var_a<<'\n';
        return 0;
    }
    address get_var(std::string name){
        for(ctr i=scopes.size()-1;i!=ULONG_MAX;i--)
            for(ctr j=0;j<scopes[i].vars.size();j++)
                if(scopes[i].vars[j].v_name == name &&
                  !scopes[i].vars[j].temp)
                    return scopes[i].vars[j].adr;

        for(ctr i=0;i<globals.vars.size();i++)
            if(globals.vars[i].v_name == name &&
              !globals.vars[i].temp)
              return globals.vars[i].adr;

        std::cerr<<"e: get_var - no variable named: "<<name<<'\n';
        return 0;
    }
    int get_var_size(address var_a){
        for(ctr i=scopes.size()-1;i!=ULONG_MAX;i--)
            for(ctr j=0;j<scopes[i].vars.size();j++){
              if(scopes[i].vars[j].adr == var_a){
                  std::string typ = scopes[i].vars[j].t_name;
                  return get_type_size(typ);
                }
            }

        for(ctr i=0;i<globals.vars.size();i++){
            if(globals.vars[i].adr == var_a){
                std::string typ = globals.vars[i].t_name;
                return get_type_size(typ);
            }
        }
        std::cerr<<"e: get_var_s - no variable at address: "<<var_a<<'\n';
        return 0;
    }
    std::string get_glbl_name(address var_a){
      for(ctr i=0;i<globals.vars.size();i++){
          if(globals.vars[i].adr == var_a){
              std::string name = globals.vars[i].v_name;
              return name;
          }
      }
      std::cerr << "e: get_glbl_name - no variable" << '\n';
      return "";
    }
    void save_3_reg(){
        scopes.back().next_addr += 16;

    }
    void add_param_to_scope(std::string n, int& off){
        variable e;
        scopes.back().vars.push_back(e);
        if(scopes.back().vars.size()==0)
            std::cerr<<"e: set_var_name - no l variables [apts]\n";
        else{
            scopes.back().vars.back().t_name = "int";
            scopes.back().vars.back().v_name = n;
            scopes.back().vars.back().adr = off;
            off -= 4;
        }
    }
    address get_end_stack(){
        if(fnc.size()==0)std::cerr<<"e: get_end_stack - no scopes\n";
        else return scopes.back().next_addr - 4;
        return 0;
    }
    bool is_ptr(address var_a){
        for(ctr i=scopes.size()-1;i!=ULONG_MAX;i--)
            for(ctr j=0;j<scopes[i].vars.size();j++)
                if(scopes[i].vars[j].adr == var_a){
                    return scopes[i].vars[j].ptr;
                }

        for(ctr i=0;i<globals.vars.size();i++){
            if(globals.vars[i].adr == var_a){
                return globals.vars[i].ptr;
            }
        }
        std::cerr<<"e: get_var - no variable at address: "<<var_a<<'\n';
        return 0;
    }

    //arrays
    variable& get_var(address var_a){
        for(ctr i=scopes.size()-1;i!=ULONG_MAX;i--)
            for(ctr j=0;j<scopes[i].vars.size();j++)
                if(scopes[i].vars[j].adr == var_a){
                    return scopes[i].vars[j];
                }

        for(ctr i=0;i<globals.vars.size();i++){
            if(globals.vars[i].adr == var_a){
                return globals.vars[i];
            }
        }
        std::cerr<<"e: get_var - no variable at address: "<<var_a<<'\n';
        variable e;
        return e;
    }
    address add_child_array(variable& var){
        std::string i = "int";
        address temp = add_temp_var(i);
        set_ptr();
        for(ctr i=1;i<var.dim.size();i++)
            add_arr_dim(var.dim[i]);
        return temp;
    }
    void add_arr_dim(address n){
        if(scopes.size()==0) {
            if(globals.vars.size()==0)
                std::cerr << "e: add_arr_dim - no g variable\n";
            else {
                int s = (n-1)*4;
                for(ctr i=0;i<globals.vars.back().dim.size();i++)
                    s *= globals.vars.back().dim[i];
                globals.next_addr += s;
                globals.vars.back().dim.push_front(n);
            }
        } else if(scopes.back().vars.size()==0)
            std::cerr<<"e: add_arr_dim - no l variables\n";
        else{
            int s = (n-1)*4;
            for(ctr i=0;i<scopes.back().vars.back().dim.size();i++)
                s *= scopes.back().vars.back().dim[i];
            scopes.back().next_addr += s;
            scopes.back().vars.back().dim.push_back(n);
        }
    }
    void set_ptr(){
        if(scopes.size()==0){
            if(globals.vars.size()==0)
                std::cerr << "e: set_ptr - no g variable\n";
            else
                globals.vars.back().ptr = true;
        } else if(scopes.back().vars.size()==0)
            std::cerr<<"e: set_ptr - no l variables\n";
        else
            scopes.back().vars.back().ptr = true;
    }
    address get_el_size(variable& var){
        address s = 4; // size of element
        for(ctr i=1;i<var.dim.size();i++)
            s*=var.dim[i];
        return s;
    }

    //arg_lists
    void add_arg_list(){
        std::vector<address> e;
        arg_lists.push_back(e);
    };
    void pop_arg_list(){
        if(arg_lists.size()==0) std::cerr<<"e: pop_arg_list - no lists\n";
        else arg_lists.pop_back();
    };
    void add_arg(address a){
        if(arg_lists.size()==0) std::cerr<<"e: add_arg - no lists\n";
        else{
            arg_lists.back().push_back(a);
        }
    }

    // program
    void w(std::string line){
        if(program.size()==0) std::cerr << "e: w(l) - no lines\n";
        else{
            std::string current = program.back();
            current.append(line);
            program.back() = current;
        }
    }
    void w(address a){
        if(program.size()==0) std::cerr << "e: w(a) - no lines\n";
        else{
            std::string current = program.back();
            current.append(std::to_string(a));
            program.back() = current;
        }
    }
    void w(int r){
        if(program.size()==0) std::cerr << "e: w(a) - no lines\n";
        else{
            std::string current = program.back();
            current.append(std::to_string(r));
            program.back() = current;
        }
    }
    void new_line(){
        program.push_back("");
    }
    void new_line(std::string line){
        new_line();
        w(line);
    }
    void ret(address a){
      if(!is_ptr(a)){
        if(a < GLOBAL_MEM) {    // local variables
            new_line("lw $2, ");
            w(-a);
            w("($fp)");
        } else {                // global variables
            new_line("lui $t7,\%hi(");
            w(get_glbl_name(a));
            w(")");
            new_line("lw $2, \%lo(");
            w(get_glbl_name(a));
            w(")($t7)");
        }
      }else{
        if(a < GLOBAL_MEM) {    // local pointers
            new_line("lw $t7, ");
            w(-a);
            w("($fp)");
            new_line("nop");
            new_line("lw $2, 0($t7)");
        } else {
          std::cerr<<"e: load - global ptrs not implemented.\n";
        }
      }

    }
    void load(address a, int r){
      if(r<0||7<r){
          std::cerr<<"e: load wrong register: "<<r<<'\n';
      } else {
        if(!is_ptr(a)){
          if(a < GLOBAL_MEM) {    // local variables
              new_line("lw $t");
              w(r);
              w(", ");
              w(-a);
              w("($fp)");
          } else {                // global variables
              new_line("lui $t7,\%hi(");
              w(get_glbl_name(a));
              w(")");
              new_line("lw $t");
              w(r);
              w(", \%lo(");
              w(get_glbl_name(a));
              w(")($t7)");
          }
        }else{
          if(a < GLOBAL_MEM) {    // local pointers
              new_line("lw $t7, ");
              w(-a);
              w("($fp)");
              new_line("nop");
              new_line("lw $t");
              w(r);
              w(", 0($t7)");
          } else {
            std::cerr<<"e: load - global ptrs not implemented.\n";
          }
        }
      }
    }
    void save(address a, int r){
        if(r<0||7<r){
            std::cerr<<"e: load wrong register: "<<r<<'\n';
        } else {
          if(!is_ptr(a)){
            if(a < GLOBAL_MEM) {    // local variables
              new_line("sw $t");
              w(r);
              w(", -");
              w(a);
              w("($fp)");
            } else {                // global variables
              new_line("lui $t7,\%hi(");
              w(get_glbl_name(a));
              w(")");
              new_line("sw $t");
              w(r);
              w(", \%lo(");
              w(get_glbl_name(a));
              w(")($t7)");
            }
          } else {
            if(a < GLOBAL_MEM) {    // local pointers
                new_line("lw $t7, ");
                w(-a);
                w("($fp)");
                new_line("nop");
                new_line("sw $t");
                w(r);
                w(", 0($t7)");
            } else {
              std::cerr<<"e: save - global ptrs not implemented.\n";
            }
          }
        }
    }
    void abs_save(address a, int r){
        if(r<0||7<r){
            std::cerr<<"e: load wrong register: "<<r<<'\n';
        } else {
            if(a < GLOBAL_MEM) {    // local variables
              new_line("sw $t");
              w(r);
              w(", -");
              w(a);
              w("($fp)");
            } else {                // global variables
              new_line("lui $t7,\%hi(");
              w(get_glbl_name(a));
              w(")");
              new_line("sw $t");
              w(r);
              w(", \%lo(");
              w(get_glbl_name(a));
              w(")($t7)");
            }
        }
    }
    void abs_load(address a, int r){
      if(r<0||7<r){
          std::cerr<<"e: load wrong register: "<<r<<'\n';
      } else {
          if(a < GLOBAL_MEM) {    // local variables
              new_line();
              w("lw $t");
              w(r);
              w(", ");
              w(-a);
              w("($fp)");
          } else {                // global variables
              new_line("lui $t7,\%hi(");
              w(get_glbl_name(a));
              w(")");
              new_line("lw $t");
              w(r);
              w(", \%lo(");
              w(get_glbl_name(a));
              w(")($t7)");
          }
      }
    }
    void li(address a, int r){
        if(r<0||7<r)
            std::cerr<<"e: load wrong register: "<<r<<'\n';
        else{
            new_line("lui $t");
            w(r);
            w(", ");
            w(a>>16);
            new_line("ori $t");
            w(r);
            w(", $t");
            w(r);
            w(", ");
            w(a&0xFFFF);
        }
    }

    //MIPS
    std::string get_label(){
        std::string l = "L";
        return l.append(std::to_string(Nlabels++));
    }

    std::string get_end(){
      std::string l = "E";
      return l.append(std::to_string(Nlabels++));
    }
    std::string get_ini(){
      std::string l = "ini";
      return l.append(std::to_string(Nlabels++));
    }

    address add_temp_var(std::string t){
        add_tvar();
        return set_var_type(t);
    }
    void initailize_function(){
        new_line(".text");
        new_line(".align 2");
        new_line(".globl ");
        w(get_fnc_name());
        new_line(".ent ");
        w(get_fnc_name());
        new_line(".type ");
        w(get_fnc_name());
        w(", @function");

        new_line(get_fnc_name());
        w(": ");
        if(get_fnc_name()=="main")
          new_line("add $fp, $sp, $zero");
        // space for sp, fp and $31
        save_3_reg();
        new_line("sw $sp, -4($fp)");
        new_line("sw $fp, -8($fp)");
        new_line("sw $31, -12($fp)");
        new_line("nop");

        int neg_offset = 0;
        for(ctr i=0;i<fnc.back().params.size();i++){
            add_param_to_scope(fnc.back().params[i].v_name, neg_offset);
        }

        int no_reg = fnc.back().params.size();
        if(no_reg >= 1) new_line("sw $4, 0($fp)");
        if(no_reg >= 2) new_line("sw $5, 4($fp)");
        if(no_reg >= 3) new_line("sw $6, 8($fp)");
        if(no_reg >= 4) new_line("sw $7, 12($fp)");
        new_line("nop");
    }
    void function_end(){
        new_line("lw $sp, -4($fp)");
        new_line("lw $31, -12($fp)");
        new_line("lw $fp, -8($fp)");
        new_line("jr $31");
        new_line("nop");
    }
    void put_arg_list(){
        if(arg_lists.size()==0) std::cerr<<"e: pop_arg_list - no lists\n";
        else{
            ctr n = arg_lists.back().size();
            for(ctr i = 0; i<n; i++){
                load(arg_lists.back()[i], 0);
                std::string i_t = "int";
                address t = add_temp_var(i_t);
                save(t, 0);
            }
            // put args to regs
            if(n>=1){
              load(arg_lists.back()[n-1],0);
              new_line("nop");
              new_line("add $4, $t0, $zero");
            }
            if(n>=2){
              load(arg_lists.back()[n-2],0);
              new_line("nop");
              new_line("add $5, $t0, $zero");
            }
            if(n>=3){
              load(arg_lists.back()[n-3],0);
              new_line("nop");
              new_line("add $6, $t0, $zero");
            }
            if(n>=4){
              load(arg_lists.back()[n-4],0);
              new_line("nop");
              new_line("add $7, $t0, $zero");
            }
        }
    }
    void add_global(address val){
      std::string name = "x";
      for(ctr i=globals.vars.size()-1;i!=ULONG_MAX;i--)
          if(!globals.vars[i].temp){
              name = globals.vars[i].v_name;
              break;
            }
      new_line(".globl ");
      w(name);
      new_line(".data");
      new_line(name);
      w(":");
      new_line(".word ");
      w(val);
    }

    //POST
    void post_process(){
          // breaks
          bool change = true;
          while(change){
            change = false;
            std::string endlabel;
            std::string beglabel;

            // breaks
            for(ctr i = program.size()-1;i!=ULONG_MAX;i--){
              if(program[i].find('E') != std::string::npos)
                endlabel = program[i].substr(0, program[i].size()-1);
              if(program[i].find("break") != std::string::npos){
                program[i] = "beq $zero, $zero, "+endlabel;
                change = true;
                break;
              }
            }
            // =continues
            for(ctr i = 0;i<program.size();i++){
              if(program[i].find('L') != std::string::npos && program[i].find(':') != std::string::npos)
                beglabel = program[i].substr(0, program[i].size()-1);
              if(program[i].find("continue") != std::string::npos){
                program[i] = "beq $zero, $zero, "+beglabel;
                change = true;
                break;
              }
            }

          }
        }

    // print
    void print_type(){
        std::cout<<"type:\n";
        for(ctr i=0;i<types.size();i++){
            std::cout<<"name: "<<types[i].name;
            std::cout<<"\tsize: "<<types[i].size<<'\n';
        }
    }
    void print_function(){
        std::cout<<"functions:\n";
        for(ctr i=0;i<fnc.size();i++){
            std::cout<<i<<". return:\t"<<fnc[i].r_type<<'\n';
            std::cout<<i<<". name:\t"<<fnc[i].name<<'\n';
            std::cout<<"params:"<<'\n';
            for(ctr j=0;j<fnc[i].params.size();j++)
                fnc[i].params[j].print();
        }
    }
    void print_scope(){
        std::cout<<"scopes:\n";
        for (ctr i=0;i<scopes.size();i++) {
            std::cout<<"scope "<<i<<":\n";
            for(ctr j = 0; j < scopes[i].vars.size(); j++)
                scopes[i].vars[j].print();
        }
            std::cout<<"\nglobals:\n";
            for(ctr i = 0; i < globals.vars.size(); i++)
                globals.vars[i].print();
    }
    void print_program() {
        std::cout<<"program:\n";
        for(ctr i=0;i<program.size();i++){
          std::cout << program[i] << '\n';
        }
    }
    void print_globals() {}
    void print(){
        print_type();
        std::cout<<'\n';
        print_function();
        std::cout<<'\n';
        print_scope();
        std::cout << '\n';
        std::cout << state[0] << '\n';
        std::cout << state[1] << '\n';
        std::cout << state[2] << '\n';
        std::cout << '\n';
        std::cout << '\n';
        print_program();
    }
};

#endif
