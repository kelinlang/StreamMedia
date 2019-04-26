//
// Created by dengjun on 2019/4/26.
//

#ifndef STREAMMEDIA_TREE_H
#define STREAMMEDIA_TREE_H

#define  TlemType int

typedef struct  BiTNode{
    TlemType data;
    struct BiTNode *lchild,*rchild;
}BiTNode,*BiTree;


void createBiTree(BiTree * T);
//前序递归
void PreOrderTraverse(BiTree T);
//前序栈
void PreOrderTraverseStack(BiTree T);

//中序递归
void INOrderTraverse(BiTree T);
void INOrderTraverseStack(BiTree T);
void INOrderTraverseStack2(BiTree T);

//后序递归
void PostOrderTraverse(BiTree Tree);
void PostOrderTraverseStack(BiTree Tree);

#endif //STREAMMEDIA_TREE_H
