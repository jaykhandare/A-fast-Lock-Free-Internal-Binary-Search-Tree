//
// Created by jayendra on 4/17/17.
//

#ifndef PROJECT_FINAL_HELPER_3_H
#define PROJECT_FINAL_HELPER_3_H

#include "function_declarations.h"
#include "flag_functions.h"

void seek(struct args* t,size_t key, struct seekRecord* s)
{
    struct anchorRecord* pAnchorRecord;
    struct anchorRecord* anchorRecord;
    struct edge pLastEdge,lastEdge;
    struct node* curr;
    struct node* next;
    struct node* temp;
    int which;

    bool n,d,p;
    size_t cKey,aKey;

    pAnchorRecord = &t->pAnchorRecord;
    anchorRecord = &t->anchorRecord;

    pAnchorRecord->node = S;
    pAnchorRecord->key = SENTI_S;

    while(true)
    {
        fillTheEdge(&pLastEdge,R,S,RC);
        fillTheEdge(&lastEdge,S,T,RC);
        curr = T;
        anchorRecord->node = S;
        anchorRecord->key = SENTI_S;
        while(true)
        {
            cKey = andKey(curr->markedKey);
            which = key<cKey ? LC:RC;
            temp = curr->child[which];
            n=isNull(temp);
            d=DFlagSet(temp);
            p=PFlagSet(temp);
            next=getAddress(temp);

            if(key == cKey || n)
            {
                s->pLastEdge = pLastEdge;
                s->lastEdge = lastEdge;
                fillTheEdge(&s->injectionEdge,curr,next,which);
                if(key == cKey)
                {
                    return;
                }
                else
                {
                    break;
                }
            }
            if(which == RC)
            {
                anchorRecord->node = curr;
                anchorRecord->key = cKey;
            }
            pLastEdge = lastEdge;
            fillTheEdge(&lastEdge,curr,next,which);
            curr = next;
        }
        aKey = andKey(anchorRecord->node->markedKey);
        if(anchorRecord->key == aKey)
        {
            temp = anchorRecord->node->child[RC];
            d=DFlagSet(temp);
            p=PFlagSet(temp);
            if(!d && !p)
            {
                return;
            }
            if(pAnchorRecord->node == anchorRecord->node && pAnchorRecord->key == anchorRecord->key)
            {
                copySeekRecord(s,&t->pSeekRecord);
                return;
            }
        }
        copySeekRecord(&t->pSeekRecord,s);
        pAnchorRecord->node = anchorRecord->node;
        pAnchorRecord->key = anchorRecord->key;
    }
}

void inject(struct args* t,struct stateRecord* state)
{
    struct node* parent;
    struct node* node;
    struct edge targetEdge;
    size_t which;
    bool result;
    bool i,d,p;
    struct node* temp;

    targetEdge = state->targetEdge;
    parent = targetEdge.parent;
    node = targetEdge.child;
    which = targetEdge.which;

    result = CAS(parent,which,node,setIFlag(node));
    if(!result)
    {
        temp = parent->child[which];
        i=IFlagSet(temp);
        d=DFlagSet(temp);
        p=PFlagSet(temp);

        if(i)
        {
            helpTargetNode(t,&targetEdge,1);
        }
        else if(d)
        {
            helpTargetNode(t,&state->pTargetEdge,1);
        }
        else if(p)
        {
            helpSuccessorNode(t,&state->pTargetEdge,1);
        }
        return;
    }

    result = markChildEdge(t,state,LC);
    if(!result)
    {
        return;
    }
    result = markChildEdge(t,state,RC);

    initializeTypeAndUpdateMode(t,state);
    return;
}
void findAndMarkSuccessor(struct args* t, struct stateRecord* state)
{
    struct node* node;
    struct seekRecord* s;
    struct edge successorEdge;
    bool m,n,d,p,result;
    struct node* temp;
    struct node* left;

    node = state->targetEdge.child;
    s = &state->successorRecord;

    while(true)
    {
        m=KeyMarked(node->markedKey);
        findSmallest(t,node,s);
        if(m)
        {
            break;
        }
        successorEdge = s->lastEdge;
        temp = successorEdge.child->child[LC];
        n=isNull(temp);
        p=PFlagSet(temp);
        left=getAddress(temp);
        if(!n)
        {
            continue;
        }
        m=KeyMarked(node->markedKey);
        if(m)
        {
            if(p)
            {
                break;
            }
            else
            {
                continue;
            }
        }
        result = CAS(successorEdge.child,LC,setNull(left),setPFlag(setNull(node)));
        if(result)
        {
            break;
        }
        temp = successorEdge.child->child[LC];
        n = isNull(temp);
        d = DFlagSet(temp);
        p = PFlagSet(temp);

        if(p)
        {
            break;
        }
        if(!n)
        {
            continue;
        }
        if(d)
        {
            helpTargetNode(t,&s->lastEdge,state->depth+1);
        }
    }
    updateMode(t,state);
    return;
}

void removeSuccessor(struct args* t, struct stateRecord* state)
{
    struct node* node;
    struct seekRecord* s;
    struct edge successorEdge;
    struct edge pLastEdge;
    struct node* temp;
    struct node* right;
    struct node* address;
    struct node* oldValue;
    struct node* newValue;
    bool n;
    bool d;
    bool p;
    bool i;
    bool dFlag;
    bool which;
    bool result;

    node = state->targetEdge.child;
    s = &state->successorRecord;
    findSmallest(t,node,s);
    successorEdge = s->lastEdge;
    temp = successorEdge.child->child[LC];
    p=PFlagSet(temp);
    address=getAddress(temp);

    if(address!=node)
    {
        node->readyToReplace = true;
        updateMode(t,state);
        return;
    }
    if(!p)
    {
        node->readyToReplace = true;
        updateMode(t,state);
        return;
    }

    temp = successorEdge.child->child[RC];
    p=PFlagSet(temp);
    if(!p)
    {
        markChildEdge(t,state,RC);
    }
    node->markedKey = setReplaceFlagInKey(successorEdge.child->markedKey);
    while(true)
    {
        if(successorEdge.parent == node)
        {
            dFlag = true;
            which = RC;
        }
        else
        {
            dFlag = false;
            which = LC;
        }

        i=IFlagSet(successorEdge.parent->child[which]);
        temp = successorEdge.child->child[RC];
        n=isNull(temp);
        right=getAddress(temp);
        if(n)
        {
            if(i)
            {
                if(dFlag)
                {
                    oldValue = setIFlag(setDFlag(successorEdge.child));
                    newValue = setNull(setDFlag(successorEdge.child));
                }
                else
                {
                    oldValue = setIFlag(successorEdge.child);
                    newValue = setNull(successorEdge.child);
                }
            }
            else
            {
                if(dFlag)
                {
                    oldValue = setDFlag(successorEdge.child);
                    newValue = setNull(setDFlag(successorEdge.child));
                }
                else
                {
                    oldValue = successorEdge.child;
                    newValue = setNull(successorEdge.child);
                }
            }
            result = CAS(successorEdge.parent,which,oldValue,newValue);
        }
        else
        {
            if(i)
            {
                if(dFlag)
                {
                    oldValue = setIFlag(setDFlag(successorEdge.child));
                    newValue = setDFlag(right);
                }
                else
                {
                    oldValue = setIFlag(successorEdge.child);
                    newValue = right;
                }
            }
            else
            {
                if(dFlag)
                {
                    oldValue = setDFlag(successorEdge.child);
                    newValue = setDFlag(right);
                }
                else
                {
                    oldValue = successorEdge.child;
                    newValue = right;
                }
            }
            result = CAS(successorEdge.parent,which,oldValue,newValue);
        }
        if(result)
        {
            break;
        }
        if(dFlag)
        {
            break;
        }
        temp=successorEdge.parent->child[which];
        d=DFlagSet(temp);
        pLastEdge = s->pLastEdge;
        if(d && pLastEdge.parent != NULL)
        {
            helpTargetNode(t,&pLastEdge,state->depth+1);
        }
        findSmallest(t,node,s);
        if(s->lastEdge.child != successorEdge.child)
        {
            break;
        }
        else
        {
            successorEdge = s->lastEdge;
        }
    }
    node->readyToReplace = true;
    updateMode(t,state);
    return;
}

bool cleanup(struct args* t, struct stateRecord* state)
{
    struct node* parent;
    struct node* node;
    struct node* newNode;
    struct node* left;
    struct node* right;
    struct node* address;
    struct node* temp;
    bool pWhich;
    bool nWhich;
    bool result;
    bool n;

    parent = state->targetEdge.parent;
    node = state->targetEdge.child;
    pWhich = state->targetEdge.which;

    if(state->type == COMPLEX)
    {
        newNode = (struct node*) malloc(sizeof(struct node));
        newNode->markedKey = andKey(node->markedKey);
        newNode->readyToReplace = false;
        left = getAddress(node->child[LC]);
        newNode->child[LC] = left;
        temp=node->child[RC];
        n=isNull(temp);
        right=getAddress(temp);
        if(n)
        {
            newNode->child[RC] = setNull(NULL);
        }
        else
        {
            newNode->child[RC] = right;
        }

        result = CAS(parent,pWhich,setIFlag(node),newNode);
    }
    else
    {
        if(isNull(node->child[LC]))
        {
            nWhich = RC;
        }
        else
        {
            nWhich = LC;
        }
        temp = node->child[nWhich];
        n = isNull(temp);
        address = getAddress(temp);
        if(n)
        {
            result = CAS(parent,pWhich,setIFlag(node),setNull(node));
        }
        else
        {
            result = CAS(parent,pWhich,setIFlag(node),address);
        }
    }
    return result;
}

bool markChildEdge(struct args* t, struct stateRecord* state, bool which)
{
    struct node* node;
    struct edge edge;
    Flag flag;
    struct node* address;
    struct node* temp;
    bool n,i,d,p,result;
    struct edge helpeeEdge;
    struct node* oldValue;
    struct node* newValue;

    if(state->mode == INJECTION)
    {
        edge = state->targetEdge;
        flag = DELETE_FLAG;
    }
    else
    {
        edge = state->successorRecord.lastEdge;
        flag = PROMOTE_FLAG;
    }

    node = edge.child;

    while(true)
    {
        temp = node->child[which];
        n=isNull(temp);
        i=IFlagSet(temp);
        d=DFlagSet(temp);
        p=PFlagSet(temp);
        address=getAddress(temp);
        if(i)
        {
            fillTheEdge(&helpeeEdge,node,address,which);
            helpTargetNode(t,&helpeeEdge,state->depth+1);
            continue;
        }
        else if(d)
        {
            if(flag == PROMOTE_FLAG)
            {
                helpTargetNode(t,&edge,state->depth+1);
                return false;
            }
            else
            {
                return true;
            }
        }
        else if(p)
        {
            if(flag == DELETE_FLAG)
            {
                helpSuccessorNode(t,&edge,state->depth+1);
                return false;
            }
            else
            {
                return true;
            }
        }

        if(n)
        {
            oldValue = setNull(address);
        }
        else
        {
            oldValue = address;
        }

        if(flag == DELETE_FLAG)
        {
            newValue = setDFlag(oldValue);
        }
        else
        {
            newValue = setPFlag(oldValue);
        }
        result = CAS(node,which,oldValue,newValue);
        if(result)
        {
            break;
        }
    }
    return true;
}

void findSmallest(struct args* t,struct node* node, struct seekRecord* s)
{
    struct node* curr;
    struct node* left;
    struct node* right;
    struct node* temp;
    bool n;
    struct edge lastEdge,pLastEdge;

    right = getAddress(node->child[RC]);
    fillTheEdge(&lastEdge,node,right,RC);
    fillTheEdge(&pLastEdge,node,right,RC);
    while(true)
    {
        curr = lastEdge.child;
        temp = curr->child[LC];
        n=isNull(temp);
        left=getAddress(temp);

        if(n)
        {
            break;
        }
        pLastEdge = lastEdge;
        fillTheEdge(&lastEdge,curr,left,LC);
    }
    s->lastEdge = lastEdge;
    s->pLastEdge = pLastEdge;
    return;
}

void initializeTypeAndUpdateMode(struct args* t,struct stateRecord* state)
{
    struct node* node;
    node = state->targetEdge.child;

    if(isNull(node->child[LC]) || isNull(node->child[RC]))
    {
        if(KeyMarked(node->markedKey))
        {
            state->type = COMPLEX;
        }
        else
        {
            state->type = SIMPLE;
        }
    }
    else
    {
        state->type= COMPLEX;
    }
    updateMode(t,state);
}

void updateMode(struct args* t,struct stateRecord* state)
{
    struct node* node;

    node = state->targetEdge.child;

    if(state->type == SIMPLE)
    {
        state->mode = CLEANUP;
    }
    else
    {
        if(node->readyToReplace)
        {
            assert(KeyMarked(node->markedKey));
            state->mode = CLEANUP;
        }
        else
        {
            state->mode = DISCOVERY;
        }
    }
    return;
}

void helpTargetNode(struct args* t, struct edge* helpeeEdge, size_t depth)
{
    struct stateRecord* state;
    bool result;
    state = (struct stateRecord*) malloc(sizeof(struct stateRecord));
    state->targetEdge = *helpeeEdge;
    state->depth = depth;
    state->mode = INJECTION;

    result = markChildEdge(t,state, LC);
    if(!result)
    {
        return;
    }
    markChildEdge(t,state,RC);
    initializeTypeAndUpdateMode(t,state);
    if(state->mode == DISCOVERY)
    {
        findAndMarkSuccessor(t,state);
    }

    if(state->mode == DISCOVERY)
    {
        removeSuccessor(t,state);
    }
    if(state->mode == CLEANUP)
    {
        cleanup(t,state);
    }

    return;
}

void helpSuccessorNode(struct args* t, struct edge* helpeeEdge, size_t depth)
{
    struct node* parent;
    struct node* node;
    struct node* left;
    struct stateRecord* state;
    struct seekRecord* s;
    parent = helpeeEdge->parent;
    node = helpeeEdge->child;
    left=getAddress(node->child[LC]);
    state = (struct stateRecord*) malloc(sizeof(struct stateRecord));
    fillTheEdge(&state->targetEdge,NULL,left,LC);
    state->depth = depth;
    state->mode = DISCOVERY;
    s = &state->successorRecord;
    s->lastEdge = *helpeeEdge;
    fillTheEdge(&s->pLastEdge,NULL,parent,LC);
    removeSuccessor(t,state);
    return;
}

static bool CAS(struct node* parent, size_t which, struct node* oldChild, struct node* newChild)
{
    parent->child[which].compare_exchange_strong(oldChild,newChild,std::memory_order_seq_cst);
}

static struct node* createNode(size_t key)
{
    struct node* node = (struct node*) malloc(sizeof(struct node));
    node->markedKey = key;
    node->child[LC] = setNull(NULL);
    node->child[RC] = setNull(NULL);
    node->readyToReplace=false;
    return(node);
}

struct node* createSENTINodes()
{
    R = createNode(SENTI_R);
    R->child[RC] = createNode(SENTI_S);
    S = R->child[RC];
    S->child[RC] = createNode(SENTI_T);
    T = S->child[RC];
    return(R);
}

bool isBSTValid(struct node* node, unsigned long min, unsigned long max)
{
    if(isNull(node))
    {
        return true;
    }
    struct node* address = getAddress(node);
    size_t nKey = andKey(address->markedKey);
    if(nKey > min && nKey < max && isBSTValid(address->child[LC],min,nKey) && isBSTValid(address->child[RC],nKey,max))
    {
        return true;
    }
    return false;
}

void fillTheEdge(struct edge* e, struct node* parent, struct node* child, bool which)
{
    e->parent = parent;
    e->child = child;
    e->which = which;
}

void copyEdge(struct edge* d, struct edge* s)
{
    d->parent = s->parent;
    d->child = s->child;
    d->which = s->which;
}

void copySeekRecord(struct seekRecord* d, struct seekRecord* s)
{
    copyEdge(&d->lastEdge,&s->lastEdge);
    copyEdge(&d->pLastEdge,&s->pLastEdge);
    copyEdge(&d->injectionEdge,&s->injectionEdge);
}

#endif //PROJECT_FINAL_HELPER_3_H
