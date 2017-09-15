//
// Created by jayendra on 4/17/17.
//

#ifndef PROJECT_FINAL_BST_FUNCTIONS_H
#define PROJECT_FINAL_BST_FUNCTIONS_H

#include "other_functions.h"

bool search(struct args* t, size_t key)
{
    struct node* node;
    size_t nKey;

    seek(t,key,&t->targetRecord);
    node = t->targetRecord.lastEdge.child;
    nKey = andKey(node->markedKey);
    if(nKey == key)
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

bool insert(struct args* t, size_t key)
{
    struct node* node;
    struct node* newNode;
    struct node* address;
    size_t nKey;
    bool result,which;
    struct node* temp;
    while(true)
    {
        seek(t,key,&t->targetRecord);
        node = t->targetRecord.lastEdge.child;
        nKey = andKey(node->markedKey);
        if(nKey == key)
        {
            return(false);
        }
        if(!t->isNewNodeAvailable)
        {
            t->newNode = createNode(key);
            t->isNewNodeAvailable = true;
        }
        newNode = t->newNode;
        newNode->markedKey = key;
        which = t->targetRecord.injectionEdge.which;
        address = t->targetRecord.injectionEdge.child;
        result = CAS(node,which,setNull(address),newNode);
        if(result)
        {
            t->isNewNodeAvailable = false;
            return(true);
        }
        temp = node->child[which];
        if(DFlagSet(temp))
        {
            helpTargetNode(t,&t->targetRecord.lastEdge,1);
        }
        else if(PFlagSet(temp))
        {
            helpSuccessorNode(t,&t->targetRecord.lastEdge,1);
        }
    }
}

bool remove(struct args* t, unsigned long key)
{
    struct stateRecord* myState;
    struct seekRecord* targetRecord;
    struct edge targetEdge,pTargetEdge;
    size_t nKey;
    bool result;

    myState = &t->myState;
    myState->depth = 0;
    myState->targetKey = key;
    myState->currentKey = key;
    myState->mode = INJECTION;
    targetRecord = &t->targetRecord;

    while(true)
    {
        seek(t,myState->currentKey,targetRecord);
        targetEdge = targetRecord->lastEdge;
        pTargetEdge = targetRecord->pLastEdge;
        nKey = andKey(targetEdge.child->markedKey);
        if(myState->currentKey != nKey)
        {
            if(myState->mode == INJECTION)
            {
                return(false);
            }
            else
            {
                return(true);
            }
        }
        if(myState->mode == INJECTION)
        {
            myState->targetEdge = targetEdge;
            myState->pTargetEdge = pTargetEdge;
            inject(t,myState);
        }
        if(myState->mode != INJECTION)
        {
            if(myState->targetEdge.child != targetEdge.child)
            {
                return(true);
            }
            myState->targetEdge = targetEdge;
        }
        if(myState->mode == DISCOVERY)
        {
            findAndMarkSuccessor(t,myState);
        }
        if(myState->mode == DISCOVERY)
        {
            removeSuccessor(t,myState);
        }
        if(myState->mode == CLEANUP)
        {
            result = cleanup(t,myState);
            if(result)
            {
                return(true);
            }
            else
            {
                nKey = andKey(targetEdge.child->markedKey);
                myState->currentKey = nKey;
            }
        }
    }
}

#endif //PROJECT_FINAL_BST_FUNCTIONS_H
