#ifndef Translator_hpp
#define Translator_hpp

#include "../inc/Translator_ast_node.hpp"

#include <iomanip>
#include <fstream>

void globals(std::vector<std::string>& program);
void deletebrace(std::vector<std::string>& program);
void indentation(std::vector<std::string>& program);

void ctranslate(std::vector<std::string> program,const astP* tree,char* name){

    tree->compile_p(program);
    globals(program);
    indentation(program);
    deletebrace(program);

    std::ofstream outfile;
    outfile.open (std::string(name));
    for(ctr i =0;i<program.size();i++){
      outfile<<program[i]<<'\n';
      //std::cout << program[i] << '\n';
    }

    outfile << "# Boilerplat \n";
    outfile << "if __name__ == \"__main__\":\n";
    outfile << "\timport sys\n";
    outfile << "\tret=main()\n";
    outfile << "\tsys.exit(ret)\n";

    outfile.close();

}

bool contains(std::string str, char c){
    return (str.find(c) != std::string::npos);
}

bool is_def(std::string str){
    char word[5] = "def ";
    return (str.find(word) != std::string::npos);
}

void globals(std::vector<std::string>& program){
    std::vector<std::string> globals;
    int indent = 0;
    for(ctr i = 0; i < program.size(); i++){
        if(contains(program[i], '{')) indent++;
        if(contains(program[i], '}')) indent--;
        if((indent == 0) && contains(program[i], '=') ){
            std::string var_name = program[i].substr(0,program[i].find('=')-1);
            globals.push_back(var_name);
        }

    }
    std::vector<std::string>::iterator it;
    for(it = globals.begin(); it != globals.end(); it++)
        *it = "global " + *it;

    bool ins_glob = false;
    std::vector<std::string> temp;
    for(ctr i = 0; i < program.size(); i++){
        temp.push_back(program[i]);
        if(ins_glob)
            for(ctr j = 0; j < globals.size(); j++)
                temp.push_back(globals[j]);
        ins_glob = is_def(program[i]) && contains(program[i],':');
    }
    program = temp;
}

void indentation(std::vector<std::string>& program){
    int indent = 0;
    bool last_def = false;
    std::vector<bool> sig_brac;
    for(ctr i = 0; i < program.size(); i++){
        if(contains(program[i],'{')){
            sig_brac.push_back(last_def);
            if(last_def) indent++;
        }
        if(contains(program[i],'}')){
            if(sig_brac[sig_brac.size()-1]) indent--;
            sig_brac.pop_back();
        }
        last_def = contains(program[i],':');
        for(int j = 0; j < indent; j++)
            program[i]='\t'+program[i];
    }
}

void deletebrace(std::vector<std::string>& program){
    for (int i = program.size() - 1; i >= 0; i--)
        if(contains(program[i],'{') || contains(program[i],'}'))
            program.erase(program.begin()+i);
}

#endif
