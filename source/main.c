#include <stdio.h>
#include "ast.h"
#include "generate_ir_sym.h"
#include "data_hash.h"
#include "generate_ir.h"
#include "ir_code.h"
#include "generate_mips.h"

extern int yyparse();
extern int yyrestart();
extern int yylineno;
extern int yydebug;
extern struct ast* root;
extern InterCodes start;
int error_flag = 0;
FILE *out_file;
 
int main(int argc, char *argv[]) {
    //yydebug = 1;
    /*
        TODO: Arg Parsing
    */
    // for (int i = 1; i < argc; i++) {
    //     if (strcmp(argv[i], "--print-ast") == 0) {
    //         _print_tree = 1;
    //     } 
    //     else if (strcmp(argv[i], "-emit-ir") == 0) {
    //         _print_ir = 1;
    //     }
    //     else if (strcmp(argv[i], "-S") == 0 || 
    //              strcmp(argv[i], "-s") == 0) {
    //         _print_asm = 1;
    //     }
    //     else if (strcmp(argv[i], "-o") == 0) {
    //         _fout = 1;
    //         fout.open(argv[++i]);
    //         std::cout.rdbuf(fout.rdbuf());
    //     } 
    //     else {
    //         _fin = 1;
    //         _fin_name = argv[i];
    //     }
    // }
    // if (_fin) {
    //     yyin = fopen(_fin_name, "r");
    // }
    // yyparse();
    // if (_print_tree) root->print(0);
    // // if (_do_semantic) {
    // //     // if (root->precheck()) std::cout << "Semantic error." << std::endl;  // check error in CompUnit
    // //     if (root->check(NoneDecl)) std::cout << "Semantic error." << std::endl;  // error!
    // // }
    // Module *m = get_module(root);
    // auto ssa = new mem2reg(m);
    // // auto clean = new clean_control_flow(m);
    // std::vector<Pass *> passes;
    // passes.push_back(ssa);
    // // passes.push_back(clean);
    // Pass::run_passes(passes);
    // m->set_reg_num();
    // if (_print_ir) std::cerr << m->print() << std::endl;

    // if (_print_asm) std::cout << CodeGen::module_gen(m) << std::endl;

    // if (_fout) fout.close();
    if (argc <= 1) return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        perror(argv[1]);
        return 1;
    }
    out_file = fopen(argv[2], "w");
    yyrestart(f);
    yyparse();
    generate_ir_sym(root, 0);
    generate_ir(root, 0);
    // generate_module();
    print_ir(start);
    final_check();
    return 0;
}

void yyerror(const char *str){
    printf("Error type B at Line %d: Mysterious characters \'%s'\n", yylineno, str);
    error_flag = 1;
}