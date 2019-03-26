#ifndef ast_node_hpp
#define ast_node_hpp

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

typedef unsigned long ctr;

class astP {
public:
    std::string name;
    astP* l;
    astP* r;
    int n;

    astP(const std::string& _name, astP* left = NULL, astP* right = NULL): name(_name){
        n = 0;
        if(left!=NULL){  l = left;  n++; }
        if(right!=NULL){ r = right; n++; }
    }

    astP(astP* left = NULL, astP* right = NULL){
        n = 0;
        if(left!=NULL){  l = left;  n++; }
        if(right!=NULL){ r = right; n++; }
    }
    virtual ~astP() {
        if(n>=1) delete l;
        if(n>=2) delete r;
    }

    virtual void compile_p( std::vector<std::string>& program) const {
        if(n>=1) l->compile_p(program);
        if(n>=2) r->compile_p(program);
     }

     void addToLine(std::vector<std::string> &program, std::string a) const {
         std::string current = program[program.size()-1];
         current.append(a);
         program[program.size()-1] = current;
     }
};

class translation_unit_p : public astP {
public:
    translation_unit_p(astP* t_u, astP* e_d): astP(t_u , e_d){}
    translation_unit_p(astP * e_d): astP(e_d) {}
};

class declaration_p : public astP {
public:
    declaration_p(astP* i_d_l): astP(i_d_l){}
};

class init_declarator_list_p : public astP {
public:
    init_declarator_list_p(astP* i_d_l, astP* i_d): astP(i_d_l, i_d){}
    init_declarator_list_p(astP* i_d): astP(i_d){}

    void compile_p(std::vector<std::string> &program) const{
        program.push_back("");
        if(n==2) r->compile_p(program);
        l->compile_p(program);
    }
};

class init_declarator_p : public astP {
public:
    init_declarator_p(astP* d_d, astP* a_e): astP(d_d, a_e) { }
    init_declarator_p(astP* d_d): astP(d_d) { }

    void compile_p(std::vector<std::string> &program) const{
        l->compile_p(program);
        if(n==1)
            addToLine(program, " = 0");
        if(n==2){
            addToLine(program, " = ");
            r->compile_p(program);
        }
    }
};

class function_p : public astP{
public:
    function_p(astP* d_d, astP* c_s): astP(d_d, c_s){}

    void compile_p(std::vector<std::string> &program) const{
        program.push_back("");
        program.push_back("def ");
        l->compile_p(program);
        r->compile_p(program);
    }
};

class direct_declarator_p : public astP{
public:
    bool function;

    direct_declarator_p(astP* id, bool func, astP* p_l)
                    : astP(id, p_l), function(func) {}

    direct_declarator_p(astP* id, bool func)
                    : astP(id), function(func) {}

    void compile_p(std::vector<std::string> &program) const{
        l->compile_p(program);
        if(function){
            addToLine(program,"(");
            if(n==2) r->compile_p(program);
            addToLine(program,"):");
        }
    }
};

class parameter_list_p : public astP{
public:
    parameter_list_p(astP* p_l, astP* p_d): astP(p_l , p_d){}
    parameter_list_p(astP * p_d): astP(p_d) {}

    virtual void compile_p( std::vector<std::string>& program) const {
        l->compile_p(program);
        if(n==2){
            addToLine(program, ", ");
            r->compile_p(program);
        }
     }
};

class parameter_declaration_p : public astP {
public:
    parameter_declaration_p(astP * p_d): astP(p_d) {}

};

class compound_statement_p : public astP {
public:
    compound_statement_p(astP* p_d): astP(p_d) {}
    compound_statement_p(): astP() {}

    virtual void compile_p( std::vector<std::string>& program) const {
        program.push_back("{");
        if(n==1) l->compile_p(program);
        program.push_back("}");
     }
};

class block_item_list_p : public astP {
public:
    block_item_list_p(astP* b_i_l, astP* b_i): astP(b_i_l , b_i){}
    block_item_list_p(astP * b_i): astP(b_i) {}
};

class block_item_p : public astP {
public:
    block_item_p(astP * ds): astP(ds) {}
};

class statement_p : public astP {
public:
    statement_p(astP * ss): astP(ss) {}
};

class expression_statement_p : public astP {
public:
    expression_statement_p(astP* e): astP(e) {}
    expression_statement_p(): astP() {}

    virtual void compile_p( std::vector<std::string>& program) const {
        program.push_back("");
        if(n==1) l->compile_p(program);
     }
};

class selection_statement_p : public astP {
public:
    astP* eelsse;

    selection_statement_p(astP* e, astP* s1, astP* s2)
    : astP(e, s1) {
        eelsse = s2;
        n++;
    }
    selection_statement_p(astP* e, astP* s1) : astP(e, s1) { }

    //think about the indentation
    virtual void compile_p( std::vector<std::string>& program) const {
        program.push_back("if(");
        l->compile_p(program);
        addToLine(program, "):");
        program.push_back("{");
        r->compile_p(program);
        program.push_back("}");
        if(n==3){
            program.push_back("else:");
            program.push_back("{");
            eelsse->compile_p(program);
            program.push_back("}");
        }
     }
};

class iteration_statement_p : public astP {
public:
    iteration_statement_p(astP* e, astP* s) : astP(e, s) { }

    virtual void compile_p( std::vector<std::string>& program) const {
        program.push_back("while(");
        l->compile_p(program);
        addToLine(program, "):");
        //program.push_back("{");
        r->compile_p(program);
        //program.push_back("}");
    }
};

class jump_statement_p : public astP {
public:
    jump_statement_p(astP* e) : astP(e) { }
    jump_statement_p() : astP() { }

    virtual void compile_p( std::vector<std::string>& program) const {
        program.push_back("return ");
        if(n==1) l->compile_p(program);
    }
};

class expression_p : public astP {
public:
    expression_p(astP* e, astP* a_e) : astP(e, a_e) { }
    expression_p(astP* a_e) : astP(a_e) { }

    virtual void compile_p( std::vector<std::string>& program) const {
        l->compile_p(program);
        if(n==2){
            addToLine(program, ", ");
            r->compile_p(program);
        }
    }
};

class assignment_expression_p : public astP {
public:
    assignment_expression_p(astP* p_e, astP* a_e): astP(p_e, a_e){ }

    virtual void compile_p( std::vector<std::string>& program) const {
        l->compile_p(program);
        addToLine(program, " = ");
        r->compile_p(program);
    }
};

class binary_expression_p : public astP {
public:
    binary_expression_p(std::string name, astP* left, astP* right) : astP(name, left, right){};

    virtual void compile_p( std::vector<std::string>& program) const {
        //addToLine(program, "( ");
        l->compile_p(program);
        addToLine(program, " "+name+" ");
        r->compile_p(program);
        //addToLine(program, " )");
    }
};

class postfix_expression_p : public astP {
public:
    postfix_expression_p(astP* p_e): astP(p_e){}
    postfix_expression_p(astP* p_e, astP* a_e_l): astP(p_e, a_e_l){}

    virtual void compile_p( std::vector<std::string>& program) const {
        l->compile_p(program);
        addToLine(program, "(");
        if(n==2) r->compile_p(program);
        addToLine(program, ")");
    }
};

class argument_expression_list_p : public astP {
public:
    argument_expression_list_p(astP * p_d): astP(p_d) {}
    argument_expression_list_p(astP * p_l, astP* p_d): astP(p_l , p_d){}


    virtual void compile_p( std::vector<std::string>& program) const {
        l->compile_p(program);
        if(n==2){
            addToLine(program, ", ");
            r->compile_p(program);
        }
     }
};

class id_p : public astP {
public:
    id_p(const std::string& n): astP(n) {}

    virtual void compile_p( std::vector<std::string>& program) const {
        addToLine(program, name);
     }
};


extern const astP *parsePAST();

// TODO Compile as it is -> find all globals and add at the begining of all functons

// TODO indentation the same
#endif
