#include <stdlib.h>
#include <stdio.h>
#include "binaryTree.h"

KeyValPair *key_val_pair_construct(void *key, void *val){

    KeyValPair *keyVal = (KeyValPair*) malloc(sizeof(KeyValPair));

    keyVal->key = key;
    keyVal->value = val;
    
    return keyVal;
}

void key_val_pair_destroy(KeyValPair *kvp){
    free(kvp);
}

Node *node_construct(void *key, void *value, Node *left, Node *right){

    Node *node = (Node*) malloc(sizeof(Node));
    
    node->key = key;
    node->value = value;
    node->left = left;
    node->right = right;

    return node;
}

void node_destroy(Node *node){
    free(node);
}

BinaryTree *binary_tree_construct(CmpFn cmp_fn, KeyDestroyFn key_destroy_fn, ValDestroyFn val_destroy_fn){

    BinaryTree *tree= (BinaryTree*) malloc(sizeof(BinaryTree));

    tree->root = NULL;
    tree->cmp_fn = cmp_fn;
    tree->key_destroy_fn = key_destroy_fn;
    tree->val_destroy_fn = val_destroy_fn;

    return tree;
}

void binary_tree_add(BinaryTree *bt, void *key, void *value){
    
    if(bt->root == NULL)
        bt->root = node_construct(key, value, NULL, NULL);
    else{
        
        Node *atual = bt->root;
        Node *pai = NULL;
        
        while(atual!= NULL){
            
            if(bt->cmp_fn(key, atual->key) == 0){
                bt->val_destroy_fn(atual->value);
                bt->key_destroy_fn(atual->key);
                atual->key = key;
                atual->value = value;
                return;
            }
            else if(bt->cmp_fn(key, atual->key) < 0){
                pai = atual;
                atual = atual->left;
            }
            else{
                 pai = atual;
                 atual = atual->right;
            }

        }

        if(bt->cmp_fn(key, pai->key) < 0){
            atual = node_construct(key, value, NULL, NULL);
            pai->left = atual;
        }
        else{
            atual = node_construct(key, value, NULL, NULL);
            pai->right = atual;
        }
    }
}

void *binary_tree_get(BinaryTree *bt, void *key){

    if(bt->root == NULL)
        return NULL;
    

    Node *atual = bt->root;

    while(bt->cmp_fn(key, atual->key) != 0){
        
        if(bt->cmp_fn(key, atual->key) < 0)
            atual = atual->left;
        else
            atual = atual->right;
        
        if(atual == NULL){
            break;
        }
    }
    
    if(atual != NULL)
        return atual->value;
    else{
        return NULL;
    }
}

void binary_tree_destroy(BinaryTree *bt){
    
    while(bt->root != NULL){
        KeyValPair *kvp = binary_tree_pop_min(bt);
        free(kvp);
    }
    
    
    free(bt);
}

int binary_tree_empty(BinaryTree *bt){
    return bt->root == NULL;
}

void _transplant(BinaryTree *bt, Node *pai, Node *atual, Node *v){

    if(pai == NULL)
        bt->root = v;
    else if(pai->left == atual)
        pai->left = v;
    else
        pai->right = v;
   
}

void binary_tree_remove(BinaryTree *bt, void *key){

    if(binary_tree_empty(bt)){
        printf("Arvore vazia!\n");
        exit(1);
    }

    Node *atual = bt->root;
    Node *pai = NULL;

    while(bt->cmp_fn(key, atual->key) != 0){
        
        pai = atual;

        if(bt->cmp_fn(key, atual->key) < 0)
            atual = atual->left;
        else
            atual = atual->right;
    }

       if (atual->left == NULL)
    {
        _transplant(bt, pai, atual, atual->right);
    }
    else if (atual->right == NULL)
    {
        _transplant(bt, pai, atual, atual->left);
    }
    else
    {
        Node *sucessor = atual->right;
        Node *pai_sucessor = NULL;

        while (sucessor->left != NULL)
        {
            pai_sucessor = sucessor;
            sucessor = sucessor->left;
        }

        if (sucessor != atual->right)
        {
            _transplant(bt, pai_sucessor, sucessor, sucessor->right);
            sucessor->right = atual->right;
        }
        _transplant(bt, pai, atual, sucessor);
        sucessor->left = atual->left;
    }
    
    bt->val_destroy_fn(atual->value);
    bt->key_destroy_fn(atual->key);
    node_destroy(atual);
    
}

KeyValPair *binary_tree_min(BinaryTree *bt){

    if(binary_tree_empty(bt)){
        printf("Arvore vazia!\n");
        exit(1);
    }

    Node *atual = bt->root;
    Node *pai = NULL;

    while(atual != NULL){
        pai = atual;
        atual = atual->left;
    }
        
    KeyValPair *kvp = key_val_pair_construct(pai->key, pai->value);

    return kvp;
}

KeyValPair *binary_tree_max(BinaryTree *bt){

    if(binary_tree_empty(bt)){
        printf("Arvore vazia!\n");
        exit(1);
    }

    Node *atual = bt->root;
    Node *pai = NULL;

    while(atual != NULL){
        pai = atual;
        atual = atual->right;
    }
        
    KeyValPair *kvp = key_val_pair_construct(pai->key, pai->value);

    return kvp;
}

KeyValPair *binary_tree_pop_min(BinaryTree *bt){

    KeyValPair *kvp = binary_tree_min(bt);
    binary_tree_remove(bt, kvp->key);

    return kvp; 
}

KeyValPair *binary_tree_pop_max(BinaryTree *bt){

    KeyValPair *kvp = binary_tree_max(bt);
    binary_tree_remove(bt, kvp->key);

    return kvp; 
}

Vector *binary_tree_inorder_traversal(BinaryTree *bt) {
    Vector *vector = vector_construct();
    Vector *stack = vector_construct();

    Node *atual = bt->root;

    while (1) {
    
        while (atual != NULL) {
            vector_push_back(stack, atual); 
            atual = atual->left;
        }

        if (vector_size(stack) == 0)
            break;

        Node *node = vector_pop_back(stack); 
        vector_push_back(vector, node); 
        atual = node->right;
    }

    vector_destroy(stack);
    return vector;
}

Vector *binary_tree_preorder_traversal(BinaryTree *bt) {
    
    Vector *vector = vector_construct();
    Vector *stack = vector_construct();

    Node *atual = bt->root;

    vector_push_back(stack, atual);

    while (vector_size(stack) != 0) {
        
        Node *aux = vector_pop_back(stack); 

        vector_push_back(vector, aux); 

        if (aux->right != NULL) 
            vector_push_back(stack, aux->right); 
        
        if (aux->left != NULL) 
            vector_push_back(stack, aux->left); 
        
    }

    vector_destroy(stack);
    return vector;
}

Vector *binary_tree_postorder_traversal(BinaryTree *bt){

    Vector *vector = vector_construct();
    Vector *stack1 = vector_construct();
    Vector *stack2 = vector_construct();

    Node *atual = bt->root;

    vector_push_back(stack1, atual);

    while (vector_size(stack1) != 0) {
        
        Node *aux = vector_pop_back(stack1); 
        
        if (aux->left != NULL) 
            vector_push_back(stack1, aux->left); 

        if (aux->right != NULL) 
            vector_push_back(stack1, aux->right); 
        
        vector_push_back(stack2, aux);  
    }

    while(vector_size(stack2) != 0){
        
        Node *aux2 = vector_pop_back(stack2);
        vector_push_back(vector, aux2);
    }

    vector_destroy(stack1);
    vector_destroy(stack2);
    
    return vector;
}

Vector *binary_tree_levelorder_traversal(BinaryTree *bt){

    Vector *vector = vector_construct();
    Vector *fila = vector_construct();

    Node *atual = bt->root;

    vector_push_back(fila, atual);

    while(vector_size(fila) != 0){

        Node *aux = vector_pop_front(fila);

        if(aux != NULL){
            vector_push_back(vector, aux);
            vector_push_back(fila, aux->left);
            vector_push_back(fila, aux->right);
        }
    }

    vector_destroy(fila);
    return vector;
}

void _inoder_recursivo(Node *node, Vector *vector){

    if(node == NULL)
        return;

    _inoder_recursivo(node->left, vector);

    vector_push_back(vector, node);

    _inoder_recursivo(node->right, vector);

}
Vector *binary_tree_inorder_traversal_recursive(BinaryTree *bt){

    Vector *vector = vector_construct();

    Node *aux = bt->root;

    _inoder_recursivo(aux, vector);

    return vector;
}

void _preorder_recursive(Node* node, Vector *vector){

    if(node == NULL)
        return;
    
    vector_push_back(vector, node);
    
    _preorder_recursive(node->left, vector);
    
    _preorder_recursive(node->right, vector);

}

Vector *binary_tree_preorder_traversal_recursive(BinaryTree *bt){

    Vector *vector = vector_construct();

    Node *aux = bt->root;

    _preorder_recursive(aux, vector);
    
    return vector;
}

void _postorder_recursive(Node* node, Vector *vector){

    if(node == NULL)
        return;

    _postorder_recursive(node->left, vector);
    
    _postorder_recursive(node->right, vector);

    vector_push_back(vector, node);
}

Vector *binary_tree_postorder_traversal_recursive(BinaryTree *bt){

    Vector *vector = vector_construct();

    Node *aux = bt->root;

    _postorder_recursive(aux, vector);

    return vector;
}

Node *_add_recursivo(Node *node, void *key, void *value, CmpFn comp_fn){

    if(node == NULL)
        return node_construct(key, value, NULL, NULL);
    
    if(comp_fn(key, node->key) < 0)
        node->left = _add_recursivo(node->left, key, value, comp_fn);
    else
        node->right = _add_recursivo(node->right, key, value, comp_fn);
    
    return node;
}

void binary_tree_add_recursive(BinaryTree *bt, void *key, void *value){
    bt->root = _add_recursivo(bt->root, key, value, bt->cmp_fn);
}