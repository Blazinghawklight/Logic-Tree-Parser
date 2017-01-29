#include <stdio.h>
#include <string.h>   /* for all the new-fangled string functions */
#include <stdlib.h>     /* malloc, free, rand */

int Fsize=50;
int cases=10;

int isalpha(int c);
int privParse(char *g, int leftP, int parens);
struct tableau completepriv(struct tableau *t, int con);
int i;
int j;

/*typedef struct tableau tableau;*/

struct tableau {
    char *root;
    struct  tableau *left;
    struct tableau *right;
    struct tableau *parent;
}*tab, *node, *node1, *kid, *pa;

/*checks if a proper connective*/
int connective(char *g, int leftP, int parens){
    char *prev;
    prev = g-1;
    char *next;
    next = g+1;
    char previous;
    previous = *prev;
    char nextone;
    nextone = *next;
    if(previous == ')' || isalpha(previous) || nextone == '-'|| nextone == '(' || isalpha(nextone)){
        return privParse(next, leftP, parens);
    }
    return 0;
}
/*helper method for parse*/
int privParse(char *g, int leftP, int parens){
    char test;
    test = *g;
    if(test == '('){
        return privParse(g + 1, leftP + 1, parens + 1);
    }
    else if(test == ')'){
        return privParse(g + 1, leftP - 1, parens);
    }
    else if(test == 'v' || test == '^' || test == '>'){
        return connective(g, leftP, parens - 1);
    }
    else if(test == '-'){
        return privParse(g + 1, leftP, parens);
    }else if(isalpha(test)){
        return privParse(g + 1, leftP, parens);
    }
    else if(test == 0 && leftP == 0 && parens == 0){
        return 3;
    }
    else{
        return 0;
    }
    
}

/*returns 1 if a proposition, 2 if neg, 3 if binary, ow 0*/
int parse(char *g){
    int neg = 0;
    while(*g=='-'){
        neg++;
        g++;
    }
    if((neg%2)==1){
        return 2;
    }
    char *cur;
    cur = g;
    cur++;
    if(cur == '\0'){
        return 1;
    }
    return privParse(g, 0, 0);
}

/*attaches a node to the right child*/
void addRight(struct tableau *t,char *g){
    struct tableau *right = malloc(sizeof(struct tableau));
    (*right).root = g;
    (*right).parent = t;
    (*right).left = NULL;
    (*right).right = NULL;
    (*t).right = right;
}

/*attaches a node to the left child*/
void addLeft(struct tableau *t,char *g){
    struct tableau *left = malloc(sizeof(struct tableau));
    (*left).root = g;
    (*left).parent = t;
    (*left).left = NULL;
    (*left).right = NULL;
    (*t).left = left;
}

/*attaches the node at all the open left child spots below*/
void stacked(struct tableau *t,char *g){
    if((*t).left == NULL) {
        addLeft(t, g);
    }
    else {
        stacked((*t).left, g);
    }
    if((*t).right != NULL) {
        stacked((*t).right, g);
    }
}

/*attaches the nodes in the same branch*/
void stack(struct tableau *t,char *g, char *h){
    stacked(t, g);
    stacked(t, h);
}

/*attaches the nodes, one to the left, one to the right*/
void split(struct tableau *t,char *g, char *h){
    if((*t).left == NULL) {
        addLeft(t, g);
        addRight(t, h);
    } else {
        split((*t).left, g, h);
        if((*t).right != NULL) {
            split((*t).right, g, h);
        }
    }
}

/*returns 2 if double negation, 3 if negated formula, 4 if normal*/
int removeDoubleNegs(struct tableau *t){
    char *beg;
    beg = (*t).root;
    if(*beg == '-'){
        beg++;
        if(*beg == '-'){
            return 2;// Double Neg
        }
        return 3;//negated formula
    }
    return 4;//regular
}

/*returns a substring for the first half*/
char *normSubOne(char *g){
    char *cur;
    cur = g;
    char *seg = (char*) malloc(*g);
    seg[0]='-';
    int i;
    i = 0;
    int parens;
    parens = 0;
    while(*cur != 0){
        if(*cur == '('){
            parens++;
        }
        else if(*cur == ')'){
            parens--;
        }else if(parens == 1 && (*cur == '^' || *cur == 'v' || *cur == '>')){
            printf("%c", seg[i]);
            break;
        }
        if(i != 0){
            char n = *cur;
            seg[i-1]= n;
        }
        cur++;
        i++;
    }
    seg[i] = '\0';
    return seg;
}

/*returns a negated substring for the first half*/
char *negSubOne(char *g){
    char *cur;
    cur = g;
    char *seg = (char*) malloc(*g);
    int i;
    i = 0;
    int parens;
    parens = 0;
    while(*cur != 0){
        if(i==0){
            seg[i] = '-';
        }else{
            char n = *cur;
            seg[i]= n;
        }
        if(*cur == '('){
            parens++;
        }
        else if(*cur == ')'){
            parens--;
        }if(parens == 1 && (*cur == '^' || *cur == 'v' || *cur == '>')){
            break;
        }
        cur++;
        i++;
    }
    seg[i] = '\0';
    return seg;
}

/*returns a substring for the second half*/
char *normSubTwo(char *g){
    char *cur;
    cur = g;
    char *seg = (char*) malloc(*g);
    seg[0]='-';
    int i;
    i = 0;
    int parens;
    parens = 0;
    while(*cur != 0){
        if(*cur == '('){
            parens++;
        }
        else if(*cur == ')'){
            parens--;
        }else if(parens == 1 && (*cur == '^' || *cur == 'v' || *cur == '>')){
            break;
        }
        cur++;
    }
    cur++;
    while(*cur != 0){
        char n = *cur;
        seg[i]= n;
        cur++;
        i++;
    }
    seg[i-1] = '\0';
    return seg;
}

/*returns a negated substring for the second half*/
char *negSubTwo(char *g){
    char *cur;
    cur = g;
    char *seg = (char*) malloc(*g);
    int i;
    i = 1;
    int parens;
    parens = 0;
    while(*cur != 0){
        if(*cur == '('){
            parens++;
        }
        else if(*cur == ')'){
            parens--;
        }else if(parens == 1 && (*cur == '^' || *cur == 'v' || *cur == '>')){
            break;
        }
        cur++;
    }
    cur++;
    seg[0] = '-';
    while(*cur != 0){
        char n = *cur;
        seg[i]= n;
        cur++;
        i++;
    }
    seg[i-1] = '\0';
    return seg;
}

/*returns a string without a double negation*/
char *removeNeg(char *g){
    char *cur;
    cur = g+2;
    char *seg = (char*) malloc(*g);
    int i;
    i = 0;
    while(*cur != 0){
        char n = *cur;
        seg[i]= n;
        
        cur++;
        i++;
    }
    seg[i] = '\0';
    return seg;
}

/*helper method for complete*/
void expand(struct tableau *t){
    int parens;
    parens = 0;
    char *cur;
    cur = (*t).root;
    char *beg;
    beg = (*t).root;
    while(*cur != 0){
        if(*cur == '('){
            parens++;
        }
        else if(*cur == ')'){
            parens--;
        }else if(parens == 1 && (*cur == '^' || *cur == 'v' || *cur == '>')){
            break;
        }
        cur++;
    }//cur is now the connective
    if(*cur != 0){
        int initialneg;
        initialneg = removeDoubleNegs(t);
        beg += 4 - initialneg;
        if(initialneg==2){
            addLeft(t, beg);
        }else if(initialneg==3){
            if(*cur == '^'){
                split(t,negSubOne(beg),negSubTwo(beg));
            }else if(*cur == 'v'){
                stack(t,negSubOne(beg),negSubTwo(beg));
            }else if(*cur == '>'){
                stack(t,normSubOne(beg),negSubTwo(beg));
            }else{
                printf(" %s not a connective ", cur);
            }
        }else{
            if(*cur == '^'){
                stack(t,normSubOne(beg),normSubTwo(beg));
            }else if(*cur == 'v'){
                split(t,normSubOne(beg),normSubTwo(beg));
            }else if(*cur == '>'){
                split(t,negSubOne(beg),normSubTwo(beg));
            }else{
                printf(" %s not a connective ", cur);
            }
        }
    }
}

/*creates a tableau*/
void complete(struct tableau *t){
    if(t!=NULL){
        expand(t);
        complete((*t).left);
        complete((*t).right);
    }
    
}

/*helper method for closed, counts closed branches*/
int privClosed(struct tableau *t, char *g){
    char *cur;
    cur = (*t).root;
    char *temp;
    temp = ((*t).root);
    char *temp2;
    temp2 = g;
    int comp;
    comp = 0;
    int diff;
    diff = 0;
    while(*temp != 0 && *temp2 != 0){
        if(*temp != *temp2){
            diff++;
            break;
        }
        temp++;
        temp2++;
    }
    if(diff == 0){
        return 1;
    }
    
    char *seg = (char*) malloc(*cur);
    int i;
    i = 1;
    seg[0] = '-';
    while(*cur != 0){
        char n = *cur;
        seg[i]= n;
        cur++;
        i++;
    }
    seg[i] = '\0';
    if(seg[0] == '-'){
        if(seg[1] != 0){
            if(seg[1] == '-'){
                seg += 2;
            }
        }
        
    }
    if((*t).left != NULL){
        int a;
        a = comp;
        comp += privClosed((*t).left, seg);
        if(comp == a){
            comp += privClosed((*t).left,g);
        }
    }if((*t).right!=NULL){
        int a;
        a = comp;
        comp += privClosed((*t).right, seg);
        if(comp == a){
            comp += privClosed((*t).right,g);
        }
    }
    
    return comp;
    
}

/*helper method for closed, counts all branches*/
int privClosed2(struct tableau *t){
    if((*t).left == NULL && (*t).right == NULL){
        return 1;
    }
    int comp;
    comp = 0;
    if((*t).left != NULL){
        comp += (privClosed2((*t).left));
    }if((*t).right!=NULL){
        comp += (privClosed2((*t).right));
    }
    return comp;
    
}

/*checks if closed*/
int closed(struct tableau *t){
    char *cur;
    cur = (*t).root;
    char *seg = (char*) malloc(*cur);
    int i;
    i = 1;
    if(removeDoubleNegs(t)==2){
        t = (*t).left;
    }
    seg[0] = '-';
    while(*cur != 0){
        char n = *cur;
        seg[i]= n;
        cur++;
        i++;
    }
    seg[i] = '\0';
    if(seg[0] == '-'){
        if(seg[1] != 0){
            if(seg[1] == '-'){
                seg += 2;
            }
        }
        
    }
    int comp;
    comp = 0;
    if((*t).left != NULL){
        comp += privClosed2((*t).left);
        printf("%d L ", comp);
    }if((*t).right!=NULL){
        comp += privClosed2((*t).right);
        printf("%d R ", comp);
        
    }
    int closed = 0;
    if((*t).left != NULL){
        closed += privClosed((*t).left,seg);
        printf("%d L ", closed);
        
    }if((*t).right!=NULL){
        closed += privClosed((*t).right, seg);
        printf("%d R ", closed);
        
    }
    if((comp - closed) != 0){
        return 0;
    }
    return 1;
}













int main()

{ /*input a string and check if its a propositional formula */
    
    
    char *name = malloc(Fsize);
    FILE *fp, *fpout;
    
    /* reads from input.txt, writes to output.txt*/
    if ((  fp=fopen("input.txt","r"))==NULL){printf("Error opening file");exit(1);}
    if ((  fpout=fopen("output.txt","w"))==NULL){printf("Error opening file");exit(1);}
    
    int j;
    for(j=0;j<cases;j++)
    {
        fscanf(fp, "%s",name);/*read formula*/
        switch (parse(name))
        {case(0): fprintf(fpout, "%s is not a formula.  ", name);break;
            case(1): fprintf(fpout, "%s is a proposition.  ", name);break;
            case(2): fprintf(fpout, "%s is a negation.  ", name);break;
            case(3):fprintf(fpout, "%s is a binary.  ", name);break;
            default:fprintf(fpout, "What the f***!  ");
        }
        
        //make new tableau with name at root, no children, no parent
        
        struct tableau t={name, NULL, NULL, NULL};
        
        /*expand the root, recursively complete the children*/
        if (parse(name)!=0)
        { complete(&t);
            if (closed(&t)) fprintf(fpout, "%s is not satisfiable.\n", name);
            else fprintf(fpout, "%s is satisfiable.\n", name);
        }
        else fprintf(fpout, "I told you, %s is not a formula.\n", name);
    }
    
    fclose(fp);
    fclose(fpout);
    free(name);
    
    return(0);
}








