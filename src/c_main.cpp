#include "../inc/MIPS.hpp"
#include "../inc/Translator.hpp"
#include <fstream>
#include <string>

extern FILE* yyin;
extern FILE* zzin;

int main(int argc, char *argv[])
{
    std::vector<std::string> program;

    if(argc < 5){
      std::cerr << "Need 5 params: bin/c_compiler (--translate/-S) [source-file.c] -o [dest-file.s]" << '\n';
      return -1;
    }

    if(argv[1]==std::string("--translate")){
      zzin = fopen(argv[2], "r");
      const astP* PYtree;
      PYtree = parsePAST();
      ctranslate(program,PYtree,argv[4]);
      fclose(zzin);
    }

    else if(argv[1]==std::string("-S")){
      yyin = fopen(argv[2], "r");
      const astM* tree;
      tree=parseAST();
      ccompile(tree,argv[4]);
      fclose(yyin);
    }
      // MIPS FUNCTIONS
    else{
      std::cerr << "undefined rule" << '\n';
      return -1;
    }
    return 0;
}
