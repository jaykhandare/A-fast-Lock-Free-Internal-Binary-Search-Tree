//
// Created by jayendra on 4/17/17.
//

#ifndef PROJECT_FINAL_FUNCTION_DECLARATIONS_H
#define PROJECT_FINAL_FUNCTION_DECLARATIONS_H

#include "ds_libraries.h"

/*  main BST Functions*/

void seek(struct args*, size_t key, struct seekRecord*);
bool search(struct args* t, size_t key);
bool insert(struct args* t, size_t key);
bool remove(struct args* t, unsigned long key);

/*  additional functions for the algorithm  - defined by author in paper*/

void inject(struct args*, struct stateRecord*);
void findAndMarkSuccessor(struct args*, struct stateRecord*);
void removeSuccessor(struct args*, struct stateRecord*);
bool cleanup(struct args*, struct stateRecord*);
bool markChildEdge(struct args*, struct stateRecord*, bool);
void findSmallest(struct args*, struct node*, struct seekRecord*);
void initializeTypeAndUpdateMode(struct args*, struct stateRecord*);
void updateMode(struct args*, struct stateRecord*);
void helpTargetNode(struct args*, struct edge*, size_t);
void helpSuccessorNode(struct args*, struct edge*, size_t);

/*  additional functions required - not defined specifically by author  */

static bool CAS(struct node* parent, size_t which, struct node* oldChild, struct node* newChild);
static struct node* createNode(size_t key);
struct node* createSENTINodes();
bool isBSTValid(struct node* node, unsigned long min, unsigned long max);
void fillTheEdge(struct edge* e, struct node* parent, struct node* child, bool which);
void copyEdge(struct edge* d, struct edge* s);
void copySeekRecord(struct seekRecord* d, struct seekRecord* s);


#endif //PROJECT_FINAL_FUNCTION_DECLARATIONS_H
