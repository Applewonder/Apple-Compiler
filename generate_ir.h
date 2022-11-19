#ifndef GIR_H
#define GIR_H

void translate_Cond(struct ast* exp, char* label_true, char* label_false);
void translate_Stmt(struct ast* stmt);
void translate_Exp(struct ast* exp, char* place);





#endif