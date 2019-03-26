#ifndef MIPS_hpp
#define MIPS_hpp

#include "MIPS_ast_node.hpp"

#include <iomanip>
#include <fstream>

void ccompile(const astM* tree,char* name )
{

    context c = context();
    tree->compile_m(c);
    c.post_process();

    std::ofstream outfile;
    outfile.open (std::string(name));
    for(ctr i =0;i<c.program.size();i++){
      outfile<<c.program[i]<<'\n';
      //std::cout << program[i] << '\n';
    }
    //c.print();

    outfile.close();
}

#endif
