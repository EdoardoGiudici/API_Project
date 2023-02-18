#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OFFSET 45
#define DIM 78
#define MAXINPUT 20
#define BLOCK 16000

typedef struct node{
    struct node* next;
    char* data;
    char inlist;
}node_l;

typedef struct node_s{
    struct node_s* father;
    struct node_s* left_son;
    struct node_s* right_son;
    node_l* node;
    char* data;
}node_t;


int word_length = 0;
int DB_length = 0;
int list_length = 0;
int letter_frequency[DIM];
int notAppear[DIM];
int exactly[DIM];
int atLeast[DIM];
int frq_par[DIM];


void compare(const char*, const char* , char*);
int admissible(const char []);
void createCostraints(const char*, const char*);
int stringComp(const char*, const char*);
void reset();

node_t* searchDB(const char []);
node_t* max(node_t*);
node_t* predecessor(node_t*);
node_t* insertDB(node_t*, node_t*);
void populateList(node_t*);
node_l* insertList(node_l*, node_l*, node_t*);
node_l* eliminate(node_l* );
void printList();
node_t* blockAllocation(char*);
char* stringAllocation(const char* );
node_l* blockListAllocation(char* );

node_l* head = NULL;
node_t* root = NULL;
node_t* block = NULL;
node_l* blockList = NULL;
char* stringBlock = NULL;

int nOfComparison = 0;
char* rightThere;
int notThere[DIM][DIM];
int phase = 0;
int command = -1;
int blockCounter = BLOCK;
int stringCounter  = BLOCK;
int blockListCounter = BLOCK;
char preFirstGame = 't';

int main() {
    int maxInput;
    int i,err;
    char* err2;
    char start = 't';
    err = scanf(" %d ", &word_length);
    if ( err == -1){
        return 0;
    }
    if (word_length < MAXINPUT){
        maxInput = MAXINPUT;
    }else{
        maxInput = word_length + 3;
    }
    rightThere = malloc(sizeof (char) * (word_length + 1));
    char input[maxInput];
    char cmp_result[word_length + 1];
    char reference[word_length + 1];
    node_t* wanted = NULL;
    do {
        err2 = fgets(input, maxInput, stdin);
        if(err2 != NULL){
            if(input[0] == '+'){
                if(input[1]== 'n'){
                    preFirstGame = 'f';
                    command = 0;
                    phase = 0;
                    reset();
                    if(start == 'f') {
                        printf("\n");
                        populateList(root);
                        list_length = DB_length;
                    }
                    start = 'f';
                }else if(input[1]=='s'){
                    printList();
                }else if(input[11] == 'i'){
                    command = 1;
                } else if(input[11] == 'f'){
                    command = 0;
                    if(preFirstGame == 't'){
                        command = -1;
                    }
                }
            }else{
                if(command == -1){ //prime parole che arrivano
                    char* newWord = stringAllocation(input);
                    node_t* newNode_t = blockAllocation(newWord);
                    root = insertDB(root, newNode_t);
                    node_l* newNode_l = blockListAllocation(newWord);
                    newNode_t->node = newNode_l;
                    newNode_l->inlist = 't';
                    head = insertList(head, newNode_l, newNode_t);
                }else if(command == 0) {
                    if (phase == 0) {
                        for (i = 0; i < word_length; i++) {
                            reference[i] = input[i];
                            letter_frequency[(int) reference[i] - OFFSET]++;
                        }
                        reference[i] = '\0';
                    } else if (phase == 1) {
                        nOfComparison = atoi(input);
                    } else {
                        wanted = searchDB(input);
                        if (wanted == NULL) {
                            printf("not_exists\n");
                        } else if (stringComp(wanted->data, reference) == 0) {
                            printf("ok");
                        } else {
                            compare(reference, input, cmp_result);
                            createCostraints(cmp_result, input);
                            head = eliminate(head);
                            printf("%s\n", cmp_result);
                            printf("%d\n", list_length);
                            nOfComparison--;
                        }
                        if(nOfComparison == 0){
                            printf("ko");
                        }
                    }
                    if (phase < 3) {
                        phase++;
                    }
                }else{
                    char* newWord = stringAllocation(input);
                    node_t* newNode_t = blockAllocation(newWord);

                    root = insertDB(root, newNode_t);
                    node_l* newNode_l = blockListAllocation(newWord);
                    newNode_l->inlist = 'f';

                    newNode_t->node = newNode_l;
                    if(admissible(input) == 1) {
                        newNode_l->inlist = 't';
                        head = insertList(head, newNode_l, newNode_t);
                    }
                }
            }
        }
    } while (err2 != NULL);
    return 0;
}
/** The method allocates a block of pointers*/
node_t* blockAllocation(char* word){
    if(blockCounter >= BLOCK) {
        block = ( node_t* ) malloc(sizeof(node_t) * BLOCK);
        blockCounter = 0;
    }
    block[blockCounter].right_son = NULL;
    block[blockCounter].left_son = NULL;
    block[blockCounter].father = NULL;
    block[blockCounter].data = word;
    blockCounter++;

    return &block[blockCounter-1];
}

char* stringAllocation(const char* word){
    if(stringCounter >= BLOCK) {
        stringBlock =  malloc(sizeof(char) * BLOCK * (word_length + 1));
        stringCounter = 0;
    }
    int i;
    for(i = 0 ; i < word_length; i++){
        stringBlock[stringCounter * (word_length + 1) + i] = word[i];
    }
    stringBlock[stringCounter * (word_length + 1) + i] = '\0';
    stringCounter++;
    return &stringBlock[(stringCounter - 1) * (word_length + 1)];
}

node_l* blockListAllocation(char* parola){
    if(blockListCounter >= BLOCK) {
        blockList = ( node_l* ) malloc(sizeof(node_l) * BLOCK);
        blockListCounter = 0;
    }
    blockList[blockListCounter].next = NULL;
    blockList[blockListCounter].inlist = 'f';
    blockList[blockListCounter].data = parola;
    blockListCounter++;
    return &blockList[blockListCounter-1];
}

/** The method resets all the global variable needed for a new game*/
void reset(){
    int i,j;
    for( i = 0; i < DIM; i++){
        letter_frequency[i] = 0;
        frq_par[i] = 0;
        exactly[i] = 0;
        atLeast[i] = 0;
        notAppear[i] = 0;
        for(j = 0; j < DIM; j++){
            notThere[i][j] = 0;
        }
    }
    for (i = 0; i < word_length; i++) {
        rightThere[i] = '+';
    }
    rightThere[i] = '\0';
}

/** The method inserts a new element in the BST
 *  @param r is the root of the BST
 *  @param toInsert is the node to insert, containing the word
 *  @return r is the new possible root of the BST*/
node_t* insertDB(node_t* r, node_t* toInsert){
    node_t* curr = r;
    node_t* pre = NULL;
    toInsert->father = NULL;
    toInsert->left_son = NULL;
    toInsert->right_son = NULL;
    while(curr != NULL){
        pre = curr;
        if (stringComp(toInsert->data,curr->data) == -1){
            curr = curr->left_son;
        }else{
            curr = curr->right_son;
        }
    }
    toInsert->father = pre;
    if(pre == NULL){
        r = toInsert;
    }else if(stringComp(toInsert->data, pre->data) == -1){
        pre->left_son = toInsert;
    }else{
        pre->right_son = toInsert;
    }
    DB_length++;
    return r;
}

/** The method inserts in the list an admissible element
 *  @param h is the head of the list
 *  @param newNodeList is a newNode type list
 *  @param newNodeTree is the newNode type tree to calculate the predecessor of
 *  @return h the new possible head of the list*/
node_l* insertList(node_l* h, node_l* newNodeList, node_t* newNodeTree){
    node_t* pred = predecessor(newNodeTree);
    if(pred == NULL){
        newNodeList->next = h;
        h = newNodeList;
    } else{
        newNodeList->next = pred->node->next;
        pred->node->next = newNodeList;
    }
    list_length++;

    return h;
}

/** The method eliminates all the word not admissible in the list
 *  @param h is the head of the list
 *  @return h the possible new head of the list
 *  */
node_l* eliminate(node_l* h){
    node_l* curr = h;
    node_l* prev = NULL;
    while (curr != NULL){
        if(admissible(curr->data) == -1){
            curr->inlist = 'f';
            if(prev == NULL ){
                h = curr->next;
            }else {
                prev->next = curr->next;
            }
            list_length--;
        }else {
            prev = curr;
        }
        curr = curr->next;

    }
    return h;
}

/** The method populates the list with all the words in the Database
 *  @param radice is the root of the Database
 *  */
void populateList(node_t* r){
    if (r == NULL){
        return;
    }
    populateList(r->left_son);
    r->node->inlist = 't';
    node_t *pred = predecessor(r);
    if(pred == NULL) {
        head = r->node;
    }else{
        pred->node->next = r->node;
    }
    populateList(r->right_son);
}

/** The method prints all the element in the lists*/
void printList(){
    node_l* curr = head;
    for(; curr!=NULL; curr = curr->next){
        puts(curr->data);
    }
}

/** The method search a word in the BST
 *  @param word is the word we want to search
 *  @return the pointer to the node containing the word
 *  */
node_t* searchDB(const char word[]){
    node_t* curr = root;
    while(curr!=NULL){
        if(stringComp(word, curr->data) == 1){
            curr = curr->right_son;
        }else if (stringComp(word, curr->data) == -1){
            curr = curr->left_son;
        }else{
            return curr;
        }
    }
    return curr;
}
/** The method return the maximum element in the BST
 *  @param x is the root of the BST in which we want to find the maximum
 *  @return curr is the pointer to the node containing the maximum, @NULL if there's no maximum
 *  */
node_t* max(node_t* x){
    node_t* curr = x;
    if(curr == NULL){
        return curr;
    }
    while (curr->right_son != NULL){
        curr = curr->right_son;
    }
    return curr;
}

/** The method finds the compatible predecessor of a given node
 *  @param x is the node of which we want to find the predecessor
 *  @return pred is the pointer containing the compatible predecessor, @NULL if there's no pointer*/
node_t* predecessor(node_t* x){
    node_t* pred = NULL;
    node_t* y = NULL;
    do {
        pred = max(x->left_son);
        if(pred == NULL){
            y = x->father;
            while (y != NULL && y->left_son == x){
                x = y;
                y = y->father;
            }
            pred = y;
        }
        x = pred;
    } while ( pred != NULL && pred->node->inlist == 'f');
    return pred;
}

/** The method creates the word of special character
 *  @param reference is the first word given after a +nuova_partita command is given
 *  @param word is the word to compare reference with
 *  @param cmp_res is where we will store the word of special character
*/
void compare(const char reference[], const char word[], char* cmp_res) {
    int i;
    int wrong_occurrence[DIM];
    int right_occurrence[DIM];
    for (i = 0; i < DIM; i++) {
        wrong_occurrence[i] = 0;
        right_occurrence[i] = 0;
    }

    for(i = 0 ; i < word_length; i++){
        if(letter_frequency[(int)word[i] - OFFSET] == 0){
            cmp_res[i] = '/';
        }else if(reference[i] == word[i]){
            cmp_res[i] = '+';
            rightThere[i]=word[i];
            right_occurrence[(int) word[i] - OFFSET]++;
        }
    }

    for(i = 0 ; i < word_length; i++){
        if(reference[i] != word[i] && letter_frequency[(int)word[i] - OFFSET] != 0){
            if(wrong_occurrence[(int) word[i] - OFFSET] >= (letter_frequency[(int) word[i] - OFFSET] - right_occurrence[(int) word[i] - OFFSET])){
                cmp_res[i] = '/';
            }else{
                cmp_res[i] = '|';
            }
            wrong_occurrence[(int) word[i] - OFFSET]++;
            notThere[i][(int) word[i] - OFFSET] = 1;
        }
    }
    cmp_res[i]='\0';
}

/** The method that creates the different constraints
 *  @param cmp_res is the word created by the method compare
 *  @param word is the word we compared riferimento with to create @param cmp_res.
*/
void createCostraints(const char cmp_res[], const char word[]) {
    int i;
    int pipe[DIM];
    int slash[DIM];
    int plus[DIM];
    for (i = 0; i < DIM; i++) {
        pipe[i] = 0;
        slash[i] = 0;
        plus[i] = 0;
    }
    for (i = 0; i < word_length ; i++) {
        if (cmp_res[i] == '/') {
            slash[(int) word[i] - OFFSET] = 1;
            if (pipe[(int) word[i] - OFFSET] == 0 && plus[(int) word[i] - OFFSET] == 0 && letter_frequency[(int)word[i] - OFFSET] == 0 ) {
                notAppear[(int) word[i] - OFFSET] = 1;
            }
        } else if (cmp_res[i] == '|') {
            pipe[(int) word[i] - OFFSET]++;
        } else {
            plus[(int) word[i] - OFFSET]++;
        }
        frq_par[(int) word[i] - OFFSET]++;
    }
    for (i = 0; i < DIM; i++) {
        if (slash[i] == 0 && atLeast[i] <= pipe[i] && pipe[i] != 0) {
            atLeast[i] = pipe[i] + plus[i];
        }
        if (pipe[i] != 0 && slash[i] == 1 && exactly[i] < pipe[i]) {
            exactly[i] = pipe[i] + plus[i];
        }
        if (plus[i]!=0 && slash[i] == 1 && exactly[i] < plus[i]){
            exactly[i] = plus[i] + pipe[i];
        }
    }
}

/** The method checks if a word is compatible or not
 *  @param word is the word to check
 *  @return 1 if the word is compatible with the constraints, -1 otherwise.
*/
int admissible(const char word[]){
    int i;
    unsigned short int frq[DIM] = {0};
    if(preFirstGame == 't'){
        return 1;
    }
    for (i = 0; i < word_length; i++) {
        if (notAppear[(int) word[i] - OFFSET] == 1) {
            return -1;
        }
        if (notThere[i][(int) word[i] - OFFSET] == 1) {
            return -1;
        }
        if (rightThere[i] != '+' && rightThere[i] != word[i]) {
            return -1;
        }
        frq[(int) word[i] - OFFSET]++;
    }
    for (i = 0; i < DIM; i++) {
        if (frq[i] < atLeast[i] && atLeast[i] != 0) {
            return -1;
        }
        if (frq[i] != exactly[i] && exactly[i] != 0) {
            return -1;
        }
    }
    return 1;
}

/** The method compare two strings
 *  @param str1 first string to compare
 *  @param str2 second string to compare
 *  @return -1 if the first string is less than the second, 1 otherwise. 0 if equals.
*/
int stringComp(const char str1[], const char str2[]){
    int i=0;
    while(str1[i]!='\0' && str2[i]!='\0'){
        if(str1[i]<str2[i]){
            return -1;
        }else if(str1[i]>str2[i]){
            return 1;
        }
        i++;
    }
    return 0;
}