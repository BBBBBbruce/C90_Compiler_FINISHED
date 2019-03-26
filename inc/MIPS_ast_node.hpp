#ifndef MIPS_AST_node_hpp
#define MIPS_AST_node_hpp

#include "MIPS_context.hpp"

class astM {
public:
    astM* l;
    astM* r;
    int n;

    astM(astM* left = NULL, astM* right = NULL){
        n = 0;
        if(left!=NULL){  l = left;  n++; }
        if(right!=NULL){ r = right; n++; }
    }
    virtual ~astM(){
        if(n>=1) delete l;
        if(n>=2) delete r;
    }

    virtual address compile_m( context& c ) const {
        address a = 0;
        if(n>=1) a = l->compile_m(c);
        if(n>=2) r->compile_m(c);
        return a;
    }

    virtual void e_print() const {
        std::cerr<< "l:\t" << l << '\n';
        std::cerr<< "r:\t" << r << '\n';
        std::cerr<< "n:\t" << n << '\n';
    }
};
class translation_unit : public astM {
public:
    translation_unit(astM* t, astM* e) : astM(t,e) {}
    translation_unit(astM* e) : astM(e) {}
    // deafult compile
};
class external_declaration : public astM {
public:
    external_declaration(astM* d): astM(d) {}

    virtual address compile_m( context& c ) const {
        c.state[0] = 'g';
        if(n>=1) l->compile_m(c);
        return 0;
    }
};
class function_definition : public astM {
public:
    astM* body;
    function_definition(astM* d_s, astM* d, astM* c_s): astM(d_s, d) {
        if(c_s!=NULL){
            body = c_s;
            n++;
        }
    }
    virtual ~function_definition(){
        delete l;
        delete r;
        delete body;
    }

    virtual address compile_m( context& c ) const {
        c.add_function();
        c.state[0] = 'f';
        c.state[1] = 'r';
        if(n>=1) l->compile_m(c);
        c.state[1] = 'n';
        if(n>=2) r->compile_m(c);
        c.add_scope();
        c.initailize_function();
        c.state[0] = 'b';
        if(n>=3) body->compile_m(c);
        c.function_end();
        c.new_line(".end ");
        c.w(c.get_fnc_name());
        if(c.get_fnc_name()=="main")
          c.new_line(".size   main, .-main");
        c.pop_scope();
        return 0;
    }
};
class declaration_specifiers : public astM {
public:
    declaration_specifiers(astM* s, astM* d): astM(s,d){}
    declaration_specifiers(astM* s): astM(s){}

    virtual address compile_m( context& c ) const {
        if(c.state[0]=='f'&&c.state[1]=='r'){
            if(n>=1) l->compile_m(c);
            if(n>=2){
                std::cerr<< "e: declaration_specifiers - only one type pls: fr" << '\n';
                //r->compile_m(c);
            }
        }
        else if(c.state[0]=='f'&&c.state[1]=='p'){
            if(n>=1) l->compile_m(c);
            if(n>=2){
                std::cerr<< "e: declaration_specifiers - only one type pls: fp" << '\n';
                //r->compile_m(c);
            }
        }
        else if(c.state[0]=='v'&&c.state[1]=='t'){
            if(n>=1) l->compile_m(c);
            if(n>=2){
                std::cerr<< "e: declaration_specifiers - only one type pls: vt" << '\n';
                //r->compile_m(c);
            }
        }
        else
        std::cerr<< "e :declaration_specifiers - not implemented" << '\n';
        return 0;
    }
};
class type_specifier : public astM {
public:
    bool leaf;
    std::string name;
    type_specifier(astM* s): astM(s) {}
    type_specifier(const std::string& type_name)
    :  astM(), name(type_name) {}

    virtual address compile_m( context& c ) const {
        if(n == 0){
            //add return type to the last function
            if(c.state[0]=='f'&&c.state[1]=='r'){
                c.set_return(name);
                return 0;
            }
            //add type to the first parameter
            if(c.state[0]=='f'&&c.state[1]=='p'){
                c.set_par_type(name);
                return 0;
            }
            //add type to the last variable
            if(c.state[0]=='v'&&c.state[1]=='t'){
                return c.set_var_type(name);
            }

        } else
        std::cerr<< "e :type_specifier - not implemented\n";
        return 0;
    }
};
class declarator : public astM {
public:
    declarator(astM* p, astM* d_d): astM(p,d_d){}
    declarator(astM* d_d): astM(d_d){}

    virtual address compile_m( context& c ) const {
        if(n==1) return l->compile_m(c);
        if(n==2){
            address a = r->compile_m(c);
            c.set_ptr();
            return a;
        }
        return 0;
    }
};
class direct_declarator : public astM {
public:
    char kind; // for now is useless
    std::string name;

    direct_declarator(const std::string& id, char k)
    : astM(), kind(k), name(id){ }
    direct_declarator(astM* d_d, char k)
    : astM(d_d), kind(k) { }
    direct_declarator(astM* d_d, astM* p_l, char k)
    : astM(d_d, p_l), kind(k) { }

    virtual address compile_m( context& c ) const {
        if(c.state[0]=='f' && c.state[1]=='n'){
            if(n==0){
                    c.set_fnc_name(name);  // name(id)
            }
            if(n>=1) l->compile_m(c);      // name
            if(n>=2){
                c.state[1] = 'p';          // params
                r->compile_m(c);
            }
        }else if(c.state[0]=='f' && c.state[1]=='p'){
            if(kind=='f') std::cerr<<"e: direct_declarator - bad c 1.\n";
            if(n==0) c.set_par_name(name);  //name of param
            else std::cerr<< "e :direct_declarator - 1.\n";
        }else if(c.state[0]=='v' && c.state[1]=='n'){
            if(n==0){
                c.set_var_name(name);
                return c.get_var(name); // TODO : delet this?
            }else if(n==2){
                if(kind!='a') std::cerr<<"e: direct_declarator - bad c 2.\n";
                address a = l->compile_m(c);
                address v = r->compile_m(c); //size of the array
                c.add_arr_dim(v); // adds_dim and alocates space
                return a;
            }
            else std::cerr<< "e :direct_declarator - 2.\n";
        } else
            std::cerr<< "e :direct_declarator - not implemented.\n";
    return 0;
    }
};
class parameter_list : public astM {
public:
    parameter_list(astM* p_d): astM(p_d) {}
    parameter_list(astM* p_l, astM* p_d): astM(p_l, p_d) {}

    virtual address compile_m( context& c ) const {
        c.add_parameter();
        if(n>=2) r->compile_m(c); // last parm
        if(n>=1) l->compile_m(c); // only param / rest params
        return 0;
    }
};
class parameter_declaration : public astM {
public:
    parameter_declaration(astM* d_s, astM* d): astM(d_s, d) { }
    //deafult compile_m
};
class declaration : public astM {
public:
    declaration(astM* d_s, astM* i_d_l): astM(d_s, i_d_l) { }

    virtual address compile_m( context& c ) const {
        bool glbl = (c.state[0] == 'g');
        // add new variable - gets offset addres - added to scope
        c.add_var();
        c.state[0] = 'v';
        c.state[1] = 't';
        if(n>=1) l->compile_m(c); // get type - set the size inside the scope
        if(glbl) c.state[0] = 'g';
        c.state[1] = 'n';
        address v = 0;
        if(n>=2) v= r->compile_m(c); // get name - set name
        if(glbl){
          c.add_global(v);
        }
        return 0;
    }
};
class init_declarator_list : public astM {
public:
    init_declarator_list(astM* i_d): astM(i_d) { }
    // deafult compile_m
};
class init_declarator : public astM {
public:
    init_declarator(astM* d): astM(d) { }
    init_declarator(astM* d, astM* i): astM(d, i) { }

    virtual address compile_m( context& c ) const {
        bool glbl = (c.state[0] == 'g');
        c.state[0] = 'v';
        if(n==1)
          l->compile_m(c);
        if(n==2){
          if(!glbl){
            address lhs = l->compile_m(c);
            address rhs = r->compile_m(c);
            address size = c.get_var_size(lhs);
            for(ctr i=0;i<size/4;i++){
                c.load(rhs+i*size, 0);
                c.new_line("nop");
                c.save(lhs+i*size, 0);
            }
            c.new_line("nop");
            return lhs;
          }else{
            address lhs = l->compile_m(c);
            char prev = c.state[2];
            c.state[2] = 'c';
            address rhs = r->compile_m(c);
            c.state[2] = prev;
            return rhs;
          }
        }
        return 0;
    }
};
class initializer : public astM {
public:
    initializer(astM* a_e) : astM(a_e){}
    //deafult compile_m
};
class compound_statement : public astM {
public:
    compound_statement(): astM() { }
    compound_statement(astM* b_i_l): astM(b_i_l) { }

    virtual address compile_m( context& c ) const {
        c.add_scope(); // add scope
        if(n==1) l->compile_m(c);
        c.pop_scope(); // delete scope
        return 0;
    }
};
class block_item_list : public astM {
public:
    block_item_list(astM* b_i, astM* b_i_l): astM(b_i, b_i_l) { }
    block_item_list(astM* b_i): astM(b_i) { }
    // deafult compile_m
};
class block_item : public astM {
public:
    block_item(astM* ds): astM(ds) { }
    // deafult compile_m
};
class statement : public astM {
public:
    statement(astM* _s) : astM(_s) { }
};
// if pointer gets state = fn - musi dodac do return type ???
class selection_statement : public astM {
public:
    char t;
    astM* else_statement;
    selection_statement(astM* e, astM* s, char c) : astM(e, s), t(c) {}
    selection_statement(astM* e, astM* s, astM* e_s, char c) : astM(e, s), t(c){
        if(e_s != NULL){
            else_statement = e_s;
            n++;
        }
    }

    virtual address compile_m( context& c ) const {
        if(t == 'i'){
            address e = 0;
            if(n>=1) e = l->compile_m(c);
            c.load(e, 0);
            c.new_line("nop");
            c.new_line("beq $t0, $zero, ");
            std::string l_e = c.get_label();
            c.w(l_e);
            c.new_line("nop");
            if(n>=2) r->compile_m(c);
            c.new_line(l_e);
            c.w(": ");
        }
        if(t == 'e'){
            address e = 0;
            if(n>=1) e = l->compile_m(c);
            c.load(e, 0);
            c.new_line("nop");
            c.new_line("beq $t0, $zero, ");
            std::string l_else = c.get_label();
            c.w(l_else);
            c.new_line("nop");
            if(n>=2) r->compile_m(c);
            std::string l_end = c.get_label();
            c.new_line("beq $zero, $zero, ");
            c.w(l_end);
            c.new_line("nop");
            c.new_line(l_else);
            c.w(": ");
            if(n>=3) else_statement->compile_m(c);
            c.new_line(l_end);
            c.w(": ");
        }
        if(t == 's'){
            std::cerr<<"e: selection_statement - switch not inplemented.\n";
        }
        return 0;
    }
};
class iteration_statement : public astM{
public:
    astM* body;
    astM* inc;
    char t;
    iteration_statement(astM* e,astM* s): astM(e,s) { }
    iteration_statement(astM* es1,astM* es2,astM* s,astM* e=NULL)
    : astM(es1,es2) {
        if(s!=NULL) n++;
        body = s;
        if(e!=NULL) n++;
        inc = e;
    }

    virtual address compile_m( context& c ) const {
        if(n==2){
            c.add_scope();
            std::string beg = c.get_label();
            std::string end = c.get_end();
            c.new_line(beg);
            c.w(":");
            address a = l->compile_m(c);
            c.load(a, 0);
            c.new_line("nop");
            c.new_line("beq $t0, $zero, ");
            c.w(end);
            c.new_line("nop");
            r->compile_m(c);
            c.new_line("beq $zero, $zero, ");
            c.w(beg);
            c.new_line("nop");
            c.new_line(end);
            c.w(":");
            c.pop_scope();
        }
        else if(n>=3){
          c.add_scope();
          std::string beg = c.get_label();
          std::string end = c.get_end();
          std::string ini = c.get_ini();
          if(n>=1) l->compile_m(c);    // 1 initialize
          c.new_line("beq $zero, $zero, ");
          c.w(ini);                   // jump initialization
          c.new_line(beg);
          c.w(":");
          address a = 0;
          if(n>=4) inc->compile_m(c); // 4 increment
          c.new_line(ini+" :");            // initialization
          if(n>=2) a = r->compile_m(c); // 2 check condition
          c.load(a, 0);
          c.new_line("nop");
          c.new_line("beq $t0, $zero, ");
          c.w(end);
          c.new_line("nop");
          if(n>=3) body->compile_m(c); // 3 body
          c.new_line("beq $zero, $zero, ");
          c.w(beg);
          c.new_line("nop");
          c.new_line(end);
          c.w(":");
          c.pop_scope();
        }
        return 0;
    }
};
class jump_statement : public astM{
public:
    char t;
    jump_statement(astM* e, char k): astM(e) { t=k; }
    jump_statement(char k): astM() { t=k; }

    virtual address compile_m( context& c ) const {
        if(t=='r'){
            if(n>=1){
                address a = l->compile_m(c);
                c.ret(a);
            }
            c.function_end();
        }
        else if(t=='c')
            c.new_line("continue");
        else if(t=='b')
            c.new_line("break");
        return 0;
    }
};
class expression_statement : public astM{
public:
    expression_statement(astM* e) : astM(e) { }
    expression_statement() : astM() { }
    // deafult compile_m
};
class expression : public astM{
public:
    expression(astM* e, astM* a_e) : astM(e, a_e) {}
    expression(astM* a_e) : astM(a_e) {}
    // deafult compile_m
};
class assignment_expression : public astM {
public:
    astM* ast_expr;
    assignment_expression(astM* c_e) : astM(c_e) { }
    assignment_expression(astM* c_e, astM* a_o, astM* a_e) : astM(c_e, a_o) {
        if(a_e!=NULL){
            ast_expr = a_e;
            n++;
        }
    }

    virtual address compile_m( context& c ) const {
        if(n==1) return l->compile_m(c);
        if(n==3){
            address eq_code = r->compile_m(c);
            address lhs = l->compile_m(c);
            address rhs = ast_expr->compile_m(c);

            c.load(rhs, 1);
            if(eq_code == 1){ // '='
                c.save(lhs, 1);
                return lhs;
            }

            c.load(lhs, 0);
            c.new_line("nop");
            if(eq_code == 2){
              c.new_line("mult $t0, $t1");
              c.new_line("nop");
              c.new_line("nop");
              c.new_line("mflo $t0");
            }
            else if(eq_code == 3){
              c.new_line("div $t0, $t1");
              c.new_line("nop");
              c.new_line("nop");
              c.new_line("mflo $t0");
            }
            else if(eq_code == 4){
              c.new_line("div $t0, $t1");
              c.new_line("nop");
              c.new_line("nop");
              c.new_line("mfhi $t0");
            }
            else if(eq_code == 5){
              c.new_line("add $t0, $t0, $t1");
            }
            else if(eq_code == 6){
              c.new_line("sub $t0, $t0, $t1");
            }
            else if(eq_code == 7){
              //left shift?
              c.new_line("sll $t0, $t0, $t1");
            }
            else if(eq_code == 8){
              //right shift?
              c.new_line("sra $t0, $t0, $t1");
            }
            else if(eq_code == 9){
              c.new_line("and $t0, $t0, $t1");
            }
            else if(eq_code ==10){
              c.new_line("xor $t0, $t0, $t1");
            }
            else if(eq_code ==11){
              c.new_line("or $t0, $t0, $t1");
            }
            else {std::cerr<<"e: undefined assignment operator\n";}

            c.save(lhs, 0);
            c.new_line("nop");
            return lhs;
        }
        return 0;
    }
};
class assignment_operator : public astM {
public:
    char op;
    assignment_operator(char c): astM() { op = c; }

    virtual address compile_m( context& c ) const {
        switch (op) {
            case '=': return 1; break;
            case '*': return 2; break;
            case '/': return 3; break;
            case '%': return 4; break;
            case '+': return 5; break;
            case '-': return 6; break;
            case 'l': return 7; break;
            case 'r': return 8; break;
            case '&': return 9; break;
            case '^': return 10;break;
            case '|': return 11;break;
        }
        std::cerr<<"e: assignment_operator no operator.\n";
        return 0;
    }
};
class conditional_expression : public astM {
public:
    astM* con_expr;
    conditional_expression(astM* l_e) : astM(l_e) {}
    conditional_expression(astM* l_e, astM* e, astM* c_e): astM(l_e, e) {
        if(c_e!=NULL) {
            con_expr = c_e;
            n++;
        }
    }

    virtual address compile_m( context& c ) const {
        if(n==1) return l->compile_m(c);
        if(n==3){
            std::cerr<<"e: conditional_expression ? : not implemented.\n";
        }
        return 0;
    }
};
class bin_expression : public astM {
public:
    char t;
    bin_expression(astM* e) : astM(e) {}
    bin_expression(astM* s, astM*e, char c)
    : astM(s,e) { t=c; }

    virtual address compile_m( context& c ) const {
        if(n==1) return l->compile_m(c);
        if(n==2){
        if(c.state[2]=='c'){
                std::cerr << "t: const bin " << '\n';
                e_print();
                address left = l->compile_m(c);
                address righ = r->compile_m(c);
                switch(t){
                    case 'o': return left || righ;
                    case 'a': return left && righ;
                    case '|': return left |  righ;
                    case '^': return left ^  righ;
                    case '&': return left &  righ;
                    case 'e': return left == righ;
                    case 'n': return left != righ;
                    case 'g': return left >= righ;
                    case 's': return left <= righ;
                    case '<': return left <  righ;
                    case '>': return left >  righ;
                    case 'l': return left >> righ;
                    case 'r': return left << righ;
                    case '+': return left +  righ;
                    case '-': return left -  righ;
                    case '*': return left *  righ;
                    case '/': return left /  righ;
                    case '%': return left %  righ;
                }
            }
            c.state[0] = 'b';
            c.state[1] = 'v';
            address lhs = l->compile_m(c);
            address rhs = r->compile_m(c);

            // deafult for int (or anything not catched)
            std::string i = "int";
            if(c.get_var_type(lhs)!=i || c.get_var_type(rhs)!=i){
                std::cerr<<"w: using deafult bin_opperations on lhs - \n";
                std::cerr<<c.get_var_type(lhs)<<" , rhs - "<<c.get_var_type(rhs);
            }
            address temp = c.add_temp_var(i);
            c.load(lhs, 0);
            c.load(rhs, 1);
            c.new_line("nop");
            switch(t){
                case 'o': {
                    c.new_line("or  $t3, $t1, $t0");
                    c.new_line("add $t2, $zero, $zero");
                    c.new_line("beq $zero, $t3, ");
                    std::string l_end = c.get_label();
                    c.w(l_end);
                    c.new_line("nop");
                    c.new_line("addi $t2, $zero, 1");
                    c.new_line();
                    c.w(l_end);
                    c.w(":");
                    break;
                }
                case 'a': {
                    c.new_line("add $t2, $zero, $zero");
                    c.new_line("beq $zero, $t0, ");
                    std::string l_end = c.get_label();
                    c.w(l_end);
                    c.new_line("nop");
                    c.new_line("beq $zero, $t1, ");
                    c.w(l_end);
                    c.new_line("nop");
                    c.new_line("addi $t2, $zero, 1");
                    c.new_line();
                    c.w(l_end);
                    c.w(":");
                    break;
                }
                case '|': { c.new_line("or $t2, $t0, $t1"); break;}
                case '^': { c.new_line("xor $t2, $t0, $t1"); break;}
                case '&': { c.new_line("and $t2, $t0, $t1"); break;}
                case 'e': {
                    c.new_line("add $t2, $zero, $zero");
                    c.new_line("bne $t0, $t1, ");
                    std::string l_end = c.get_label();
                    c.w(l_end);
                    c.new_line("nop");
                    c.new_line("addi $t2, $t2, 1");
                    c.new_line();
                    c.w(l_end);
                    c.w(":");
                    break;
                    }
                case 'n': {
                    std::string l_end = c.get_label();
                    c.new_line("add $t2, $zero, $zero");
                    c.new_line("beq $t0, $t1, ");
                    c.w(l_end);
                    c.new_line("nop");
                    c.new_line("addi $t2, $t2, 1");
                    c.new_line();
                    c.w(l_end);
                    c.w(":");
                    break;
                    }
                case 'g': {
                    c.new_line("slt $t2, $t0, $t1");
                    c.new_line("xori $t2, $t2, 1");
                    break;
                }
                case 's': {
                    c.new_line("slt $t2, $t1, $t0");
                    c.new_line("xori $t2, $t2, 1");
                    break;
                }
                case '<': {c.new_line("slt $t2, $t0, $t1"); break;}
                case '>': {c.new_line("slt $t2, $t1, $t0"); break;}
                case 'l': {c.new_line("sll $t2, $t0, $t1"); break;}
                case 'r': {c.new_line("sra $t2, $t0, $t1"); break;}
                case '+': {c.new_line("add $t2, $t0, $t1"); break;}
                case '-': {c.new_line("sub $t2, $t0, $t1"); break;}
                case '*': {
                    c.new_line("mult $t0, $t1");
                    c.new_line("nop");
                    c.new_line("nop");
                    c.new_line("mflo $t2");
                    break;}
                case '/': {
                    c.new_line("div $t0, $t1");
                    c.new_line("nop");
                    c.new_line("nop");
                    c.new_line("mflo $t2");
                    break;}
                case '%': {
                    c.new_line("div $t0, $t1");
                    c.new_line("nop");
                    c.new_line("nop");
                    c.new_line("mfhi $t2");
                    break;}
            }
            c.save(temp, 2);
            c.new_line("nop");
            return temp;
        }
        std::cerr<<"e: bin_expression - no retrun.\n";
        return 0;
    }
};
class cast_expression : public astM {
public:
    cast_expression(astM* u_e): astM(u_e) { }
    //deafult compile_m
    // TODO : FINISH
};
class unary_expression : public astM {
public:
    char t;
    //unary_expression(astM* p_e): astM(p_e) { }
    unary_expression(astM* u_o, astM* c_e): astM(u_o, c_e) { }
    unary_expression(astM* p_e, char k): astM(p_e) { t = k;}

      virtual address compile_m( context& c ) const {
        if(n==1) {
            if(t == 'p'){
              return l->compile_m(c);
            } else if(t == 'i'){
              address a = l->compile_m(c);
              c.load(a,0);
              c.new_line("addi $t1, $t0, 1");
              c.save(a,1);
              return a;
            } else if(t == 'd'){
              address a = l->compile_m(c);
              c.load(a,0);
              c.new_line("li $t2, 1");
              c.new_line("sub $t1, $t0, $t2");
              c.save(a,1);
              return a;
          } else if(t == 's'){
              address a = l->compile_m(c);
              variable var = c.get_var(a);
              int s = 4;
              for(ctr i=0;i<var.dim.size();i++)
                  s*=var.dim[i];
              std::string i = "int";
              address r = c.add_temp_var(i);
              c.li(r, s);
              c.new_line("nop");
              return r;
          }}
        else if(n==2){
          address a = r->compile_m(c);
          address unary_code = l->compile_m(c);
          std::string i = "int";
          address tmp = c.add_temp_var(i);

          switch (unary_code) {
            case 1:{
                    c.li(a,1);
                    c.new_line("sub $t1, $fp, $t1");
                    break;      // &
            }
            case 2: c.abs_load(a,1);c.new_line("nop");  break; // *
            case 3: c.load(a,0);c.new_line("move $t1, $t0")      ; break;
            case 4: c.load(a,0);c.new_line("sub $t1, $zero, $t0"); break;
            case 5: c.load(a,0);c.new_line("nor $t1, $t0, $zero"); break;
            case 6: {
                    c.load(a,0);
                    std::string ini = c.get_ini();
                    c.new_line("li $t1, 1");
                    c.new_line("beq $t0, $zero, ");
                    c.w(ini);
                    c.new_line("li $t1, 0");
                    c.new_line(ini+":");}
                    break;
            default: std::cerr<<"e: undefined unary operator\n"; break;
          }
          c.save(tmp,1);
          c.new_line("nop");
          return tmp;
        }
        else{std::cerr << "e: undefined unary_expression.\n";}
      }
    
};
class unary_operator : public astM {
public:
    char op;
    unary_operator(char c): astM() { op = c; }

    virtual address compile_m( context& c ) const {
        switch (op) {
            case '&': return 1;break;
            case '*': return 2;break;
            case '+': return 3;break;
            case '-': return 4;break;
            case '~': return 5;break;
            case '!': return 6;break;
        }
        std::cerr<<"e: assignment_operator no operator.\n";
        return 0;
    }
};
class postfix_expression : public astM {
public:
    char t;
    postfix_expression(astM* p_e, char k): astM(p_e) { t=k; }
    postfix_expression(astM* p_e, astM* a_l, char k)
    : astM(p_e, a_l) { t=k; }
    //deafult compile_m
    virtual address compile_m( context& c ) const {
        if(t == 'n'){
            if(n>=1) return l->compile_m(c);
        }else if (t=='(') {
            // put params on stack and regs - in a-l
            c.add_arg_list();
            if(n>=2) r->compile_m(c);
            c.put_arg_list();
            c.pop_arg_list();
            // change sp and fp
            address o = c.get_end_stack();
            c.new_line("addiu $fp, $fp ,-");
            c.w(o);
            c.new_line("addiu $sp, $fp, 0");
            // jump and link
            char s0 = c.state[0];
            char s1 = c.state[1];
            c.state[0] = 'b';
            c.state[1] = 'j';
            if(n>=1) l->compile_m(c);
            c.state[0] = s0;
            c.state[1] = s1;
            //restore the fp and sp
            c.new_line("addiu $fp, $fp ,");
            c.w(o);
            c.new_line("addiu $sp, $fp, 0");
            std::string i = "int";
            address ret = c.add_temp_var(i);
            c.new_line("sw $2, ");
            c.w(-ret);
            c.w("($fp)");
            c.new_line("nop");
            return ret;
        }else if (t=='[') {
            address ra = 0;
            address la = 0;
            if(n>=1) ra = l->compile_m(c);
            if(n>=2) la = r->compile_m(c);
             // address of array
            if(c.is_ptr(ra))
                c.abs_load(ra, 0);  //get_val
            else{
              c.li(ra, 0);    //get_addr
              c.new_line("sub $t0, $fp, $t0");
            }


            c.load(la, 1); // index of array
            // new address = ra + la * size of element
            variable v = c.get_var(ra);
            address s = c.get_el_size(v);
            address temp = c.add_child_array(v);
            c.li(s, 3);
            c.new_line("mult $t1, $t3");
            c.new_line("nop");
            c.new_line("nop");
            c.new_line("mflo $t2"); //la * s
            c.new_line("sub $t2, $t0, $t2");
            c.abs_save(temp,2);
            c.new_line("nop");
            return temp;
        }
        return 0;
    }
};
class primary_expression : public astM {
public:
    char t;
    std::string name;
    primary_expression(const std::string& id, char k)
    : astM(), t(k), name(id) { }
    primary_expression(astM* e): astM(e) { }

    virtual address compile_m( context& c ) const {
        if(n==1) return l->compile_m(c);
        else if(n==0){
            if(t=='i') {
                if(c.state[0]=='b' && c.state[1]=='j'){
                    c.new_line("jal ");
                    c.w(name);
                    c.new_line("nop");
                    return 0;
                }
                return c.get_var(name);
            }
            else if(t=='c'){
                //ONLY INTS SO FAR
                int v = std::stoi(name);
                //if(c.state[0]=='g') return v;
                if(c.state[2]=='c'){
                  return v;
                }
                c.new_line("lui $t0, ");
                c.w(v>>16);
                c.new_line("ori $t0, $t0, ");
                c.w(v&0xFFFF);
                std::string i = "int";
                address temp = c.add_temp_var(i);
                c.save(temp, 0);
                c.new_line("nop");
                return temp;
            }
            else if(t=='s') std::cerr<<"e: prim_expr - no string literal\n";
        }
        return 0;
    }
};
class argument_expression_list : public astM {
public:
    argument_expression_list(astM* a_e): astM(a_e){ }
    argument_expression_list(astM* a_e_l, astM* a_e)
    : astM(a_e_l, a_e){ }

    virtual address compile_m( context& c ) const {
        if(n==1){
            address a = l->compile_m(c);
            c.add_arg(a);
        }
        if(n==2){
            address a = r->compile_m(c);
            c.add_arg(a);
            l->compile_m(c);
        }
        return 0;
    }
};
class constant_expression : public astM {
public:
    constant_expression(astM* c_e) : astM(c_e) { }

    virtual address compile_m( context& c ) const {
        if(n==1){
            char prev = c.state[2];
            c.state[2] = 'c';
            address v = l->compile_m(c);
            c.state[2] = prev;
            return v;
        }
        return 0;
    }
};

extern const astM *parseAST();

#endif

// params addressdoesnt work
