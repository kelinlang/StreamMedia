//
// Created by dengjun on 2019/4/26.
//

#include <malloc.h>
#include <CommonLib/log/cl_sdl_log.h>
#include "Tree.h"


void createBiTree(BiTree * T){
    *T = (BiTree)malloc(sizeof(BiTNode));
    (*T)->data = 1;
    (*T)->lchild =  (BiTree)malloc(sizeof(BiTNode));
    (*T)->rchild =  (BiTree)malloc(sizeof(BiTNode));

    (*T)->lchild->data = 2;

    (*T)->lchild->lchild = (BiTree)malloc(sizeof(BiTNode));
    (*T)->lchild->lchild->data = 4;
    (*T)->lchild->lchild->lchild = NULL;
    (*T)->lchild->lchild->rchild = NULL;

    (*T)->lchild->rchild = (BiTree)malloc(sizeof(BiTNode));
    (*T)->lchild->rchild->data = 5;
    (*T)->lchild->rchild->lchild = NULL;
    (*T)->lchild->rchild->rchild = NULL;


    (*T)->rchild->data = 3;

    (*T)->rchild->lchild = (BiTree)malloc(sizeof(BiTNode));
    (*T)->rchild->lchild->data = 6;
    (*T)->rchild->lchild->lchild = NULL;
    (*T)->rchild->lchild->rchild = NULL;

    (*T)->rchild->rchild = (BiTree)malloc(sizeof(BiTNode));
    (*T)->rchild->rchild->data = 7;
    (*T)->rchild->rchild->lchild = NULL;
    (*T)->rchild->rchild->rchild = NULL;
}


void PreOrderTraverse(BiTree T){
    if(T){
        LOGI("value : %d",T->data);
        PreOrderTraverse(T->lchild);
        PreOrderTraverse(T->rchild);
    }
}

void PreOrderTraverseStack(BiTree T){
    if(T){
        int top = -1;
        BiTree a[20];
        BiTree tmpBiTree;

        a[++top] = T;
        LOGI(" top go to stack");
        while (top != -1){
            tmpBiTree = a[top];
            LOGI("get top element");
            top--;//弹栈
            while (tmpBiTree){
                LOGI("value : %d",tmpBiTree->data);
                if(tmpBiTree->rchild){
                    a[++top] = tmpBiTree->rchild;
                }
                tmpBiTree = tmpBiTree->lchild;
            }
        }
    }
}

void INOrderTraverse(BiTree T){
    if(T){
        INOrderTraverse(T->lchild);
        LOGI("value : %d",T->data);
        INOrderTraverse(T->rchild);
    }
}

void INOrderTraverseStack(BiTree T){
    int top = -1;
    BiTree a[20];
    BiTree tmpBiTree;

    a[++top] = T;
    LOGI(" top go to stack");
    while (top != -1){
        while ((tmpBiTree = a[top])&&tmpBiTree){//取栈顶元素，且不能为NULL
            a[++top] = tmpBiTree->lchild;//入栈
        }
        top--;//跳出循环，栈顶元素肯定为NULL,将NULL弹栈
        if(top != -1){
            tmpBiTree = a[top];
            top--;
            LOGI("value : %d",tmpBiTree->data);
            a[++top] = tmpBiTree->rchild;//入栈
        }
    }
}

void INOrderTraverseStack2(BiTree T){
    int top = -1;
    BiTree a[20];
    BiTree tmpBiTree;

    tmpBiTree = T;
    LOGI(" top go to stack");
    while (tmpBiTree || top != -1){
        if(tmpBiTree){
            a[++top] = tmpBiTree;//入栈
            tmpBiTree = tmpBiTree->lchild;
        } else{
            tmpBiTree = a[top];
            top--;
            LOGI("value : %d",tmpBiTree->data);
            tmpBiTree = tmpBiTree->rchild;
        }
    }
}

void PostOrderTraverse(BiTree T){
    if(T){
        PostOrderTraverse(T->lchild);
        PostOrderTraverse(T->rchild);
        LOGI("value : %d",T->data);
    }
}


//后序遍历非递归算法
typedef struct SNode{
    BiTree p;
    int tag;
}SNode;

void PostOrderTraverseStack(BiTree Tree){
    SNode a[20];//定义一个顺序栈
    BiTNode * p;//临时指针
    int tag;
    SNode sdata;
    p=Tree;
    int top = -1;

    while (p||top!=-1) {
        while (p) {
            //为该结点入栈做准备
            sdata.p=p;
            sdata.tag=0;//由于遍历是左孩子，设置标志位为0
            a[++top] = sdata;//入栈;//压栈
            p=p->lchild;//以该结点为根结点，遍历左孩子
        }
        sdata=a[top];//取栈顶元素
        top--;//栈顶元素弹栈
        p=sdata.p;
        tag=sdata.tag;
        //如果tag==0，说明该结点还没有遍历它的右孩子
        if (tag==0) {
            sdata.p=p;
            sdata.tag=1;
            a[++top] = sdata;//更改该结点的标志位，重新压栈
            p=p->rchild;//以该结点的右孩子为根结点，重复循环
        }
            //如果取出来的栈顶元素的tag==1，说明此结点左右子树都遍历完了，可以调用操作函数了
        else{
            LOGI("value : %d",p->data);
            p=NULL;
        }
    }
}