#include "../inc/MIPS_ast_node.hpp"

#include <iomanip>
#include <fstream>

int main(int argc, char *argv[])
{
    std::ifstream src(argv[1]);
    //std::vector<std::string> program;
    const astM* tree = parseAST();
    context c = context();
    tree->compile_m(c);
    std::ofstream outfile;
    //outfile.open ("test_output/"+arg[0]+".py"); output the same name as input file
    //outfile.open ("test_output/test1.s");
    c.print();
    outfile.close();
    return 0;
}

// add O3 optimisation at the end
