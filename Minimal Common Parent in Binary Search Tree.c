//Programm for calculating Minimal Common Point of 
//two DIFFERENT elements in tree with NO equal elements!

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "malloc.h"

typedef struct Node {

	int val; // value or info of Node
	struct Node* right; // right descendant
	struct Node* left; // left descendant
	struct Node* parent; // parent of a Node

} Node;

typedef struct Tree {
	Node* root;
} Tree;

Node* addnode(int info, Node* tree, Node* TempParent) { // add Node to Binary Tree
	if (tree == NULL) { // if theres no root, make it
		if (!(tree = (Node*)malloc(sizeof(Node))))
			return NULL;
		tree->val = info;
		tree->left = NULL;
		tree->right = NULL;
		tree->parent = TempParent;
	}
	else  if (info < tree->val) { // condition for left descendant
		TempParent = tree;
		tree->left = addnode(info, tree->left, TempParent);
	}
	else { // condition for right descendant
		TempParent = tree;
		tree->right = addnode(info, tree->right, TempParent);
	}
	return(tree);
}

Node* FindElementInBTree(Node* root, int value, int* LevelofElem) { // finds element in Binary Tree by its value
	while (root) { // while root is not NULL
		if (root->val > value) { // go to left side of the tree
			root = root->left;
			(*LevelofElem)++; // change level
			continue;
		}
		else if (root->val < value) { // go to the right size of the tree
			root = root->right;
			(*LevelofElem)++; // change level
			continue;
		}
		else {
			return root;
		}
	}
	return NULL;
}

Node* FindMinCommondParent(Node* root, int first, int second) { // find Mminimal common parent in Binary Tree

	int LevelA = 0; // Levels of points
	int LevelB = 0;

	Node* a = FindElementInBTree(root, first, &LevelA); // Finds element in Binary Tree and gives level of it
	Node* b = FindElementInBTree(root, second, &LevelB); // Finds element in Binary Tree and gives level of it

	if (!a || !b) {
		printf("No such element in Binary Tree");
		return NULL; // there is no point a or b in Binary Tree
	}


	if (LevelA > LevelB) { // equalizing level of A
		for (int i = 0; i < (LevelA - LevelB); i++)
			a = a->parent;
	}
	else if (LevelB > LevelA) { // equalizing level of B
		for (int i = 0; i < (LevelB - LevelA); i++)
			b = b->parent;
	}

	while ((a->val) != (b->val)) { // till they are not points to same element ( min parent )  
								   //  checks value because we cant have equal elements in tree
		if (a->parent)
			a = a->parent;
		if (b->parent)
			b = b->parent;
	}
	return a;
}

void freeTree(Node* root) { // free tree
	if (root->left)
		freeTree(root->left);
	if (root->right)
		freeTree(root->right);

	free(root);
}

void main() {

	Tree* tree;

	if (!(tree = (Tree*)malloc(sizeof(Tree)))) {
		return;
	}
	tree->root = NULL;

	int info;
	Node* TempParent = NULL;

	while (1) { // input
		if (!(scanf("%d", &info)))
			break; // when input is not decimal number

		tree->root = addnode(info, tree->root, TempParent);
	}

	printf("\nInput value of elements to find minimal common parent of (sep. by space): ");
	int a, b;

	rewind(stdin);

	if (!scanf("%d %d", &a, &b))
		return;

	if (a == b) {
		printf("\nMinimal parent of %d and %d is %d", a, b, (FindElementInBTree(tree->root, a, &info)->parent)->val);
		return;
	}

	Node* MinCommondParent = FindMinCommondParent(tree->root, a, b);
	if (!MinCommondParent) {
		return;
	}

	printf("Minimal parent of %d and %d is %d", a, b, MinCommondParent->val);

	// free memory section
	freeTree(tree->root);
	free(tree);
	return;
}
