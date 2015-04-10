/**********************************************************
**
**  Written By: Marvin Corro
**
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

//#include "stdafx.h"
#include "wrapper.h"

//#include <crtdbg.h> 

#define TRUE 1
#define FALSE 0
#define SPECIAL_CHAR " %&*,.;!/=+-()[]{}\t\n#"
#define QUIT_ "quit"

/*
#ifdef _MSC_VER
#include <crtdbg.h>  // needed to check for memory leaks (Windows only!)
#endif
*/
typedef struct queNode{
    int lineNumber;
    struct queNode *next;
}QUEUE_NODE;

typedef struct treeNode{
    char *word;
    QUEUE_NODE *que;
    QUEUE_NODE *rear;
    struct treeNode *left;
    struct treeNode *right;
}TREE_NODE;

TREE_NODE *buildData(FILE*);
void printData(TREE_NODE*, FILE *);
void insertToTree(char*, TREE_NODE **, int );
void makeToLower(char*);
int parse(char*);
void toCheck(char*);
void outPutTreeData(TREE_NODE*);
TREE_NODE *find(char*, TREE_NODE*);
void push(QUEUE_NODE**, QUEUE_NODE**, int);
QUEUE_NODE *pop(QUEUE_NODE**, QUEUE_NODE**);
char* getFile();
void menu(TREE_NODE*, FILE *);
TREE_NODE *DestroyTree(TREE_NODE*);

int main()
{
   FILE *fp;
   TREE_NODE *TREE = NULL;
   char* fileName = getFile();
   fp = fopen(fileName, "r");
   TREE = buildData(fp);
   outPutTreeData(TREE);
   TREE = DestroyTree(TREE);
/*   #ifdef _MSC_VER
   printf( _CrtDumpMemoryLeaks() ? "Memory Leak\n" : "No Memory Leak\n");
   #endif */
//   system("pause");
   return 0;
}

void printData(TREE_NODE *word_tree, FILE *fp)
{
   char line[200];
   QUEUE_NODE *temp = NULL;
   QUEUE_NODE *rear = NULL;
   QUEUE_NODE *print;

   printf("This word \"%s\" found in line number\n", word_tree->word);

   while(word_tree->que != NULL)
   {
      print = pop(&(word_tree->que), &(word_tree->rear));

      fseek(fp, print->lineNumber * sizeof(char), SEEK_SET);
      fgets(line, 200, fp);

      printf("%d %s", print->lineNumber, line);

      push(&temp, &rear, print->lineNumber);
      free(print);
   }

   while(temp != NULL)
   {
      print = pop(&temp, &rear);

      push(&(word_tree->que), &(word_tree->rear), print->lineNumber);
      free(print);
   }
}


TREE_NODE *buildData(FILE *fp)
{ 
   TREE_NODE *TREE = NULL;
   TREE_NODE *found = NULL;
   char *word;
   int counter = 1;
   char line[200];
   while(counter < 100)
   {
      fgets(line, 200, fp);
      printf("%d\t:%s", counter, line);
      toCheck(line);
      word = strtok(line, SPECIAL_CHAR);
      while(word)
      {
         found = find(word, TREE);
         if(parse(word))
         {
            if(!found)
               insertToTree(word, &TREE, counter);
            else if(found->rear->lineNumber != counter)                           
               push(&(found->que), &(found->rear), counter);
         }
         word = strtok(NULL, SPECIAL_CHAR);
      }
      counter++;
   }
   return TREE;
}

int parse(char *word)
{
   char *digits = "1234567890";

   if(strspn(word, digits) > 0)
      return FALSE;
   if(word[0] == '<')
      return FALSE;
   size_t length = strlen(word);

   if (  word[length - 1] == '>' ) 
      return FALSE;

   return TRUE;
}

void makeToLower(char *word)
{
   int counter   = 0;
   size_t length = strlen( word );
   for( counter = 0; counter < length; counter++ )
      word[counter] = tolower(word[counter]);
}

void toCheck(char *line)
{
   char *pWalk, *pLast;
   pWalk = strchr(line, '"');
   while(pWalk)
   {
      pLast = strchr(pWalk + 1, '"');
      *pLast = ' ';
      while(pWalk != pLast)
      {
         *pWalk = ' ';
         pWalk++;
      }
      pWalk = strchr(line, '"');
   }

   pWalk = strchr(line, '/');
   if(pWalk && (*(pWalk+1) == '/' || *(pWalk - 1) == '/'))
      *pWalk = '\0';

}

char* getFile()
{
   static char input[100];
   printf("Input file name: ");
   gets( input );

   return input;
}

QUEUE_NODE *pop(QUEUE_NODE **q, QUEUE_NODE **rear)
{
   QUEUE_NODE *current = *q;

   if(*q == NULL) 
      return NULL;
   *q = (*q)->next;
   if(*q == NULL) 
      *rear = NULL;

   current->next = NULL;
   return current;
}

TREE_NODE *DestroyTree(TREE_NODE *TREE)
{
   if(!TREE) 
      return NULL;
   DestroyTree(TREE->left);
   free(TREE->word);

   while( TREE->que != NULL )
      free(pop(&(TREE->que), &(TREE->rear)));

   DestroyTree(TREE->right);
   free(TREE);
   return NULL;
}


void insertToTree(char *word, TREE_NODE **TREE, int counter)
{
   int compare;

   if(!(*TREE)){
      *TREE = (TREE_NODE *) malloc(sizeof(TREE_NODE));
      (*TREE)->word = (char *) malloc(strlen(word) + 1);

      (*TREE)->que = (*TREE)->rear = NULL;

      push(&((*TREE)->que), &((*TREE)->rear), counter);
      (*TREE)->left = (*TREE)->right = NULL;
      strcpy((*TREE)->word, word);
      return;
   }

   compare = strcmp(word, (*TREE)->word);

   if(compare < 0)
      insertToTree(word, &(*TREE)->left, counter);
   else if(compare > 0)
      insertToTree(word, &(*TREE)->right, counter);
}

void outPutTreeData(TREE_NODE *TREE)
{
   if(!TREE)
      return;
   outPutTreeData(TREE->left);
   printf("%s \n", TREE->word);
   outPutTreeData(TREE->right);
}


void push(QUEUE_NODE **q, QUEUE_NODE **rear, int counter)
{
   QUEUE_NODE *newNode = (QUEUE_NODE*) malloc(sizeof(QUEUE_NODE));

   newNode->lineNumber = counter;
   newNode->next = NULL;
   if(*q == NULL) 
      *q = newNode;
   else 
      (*rear)->next = newNode;
   *rear = newNode;
}


TREE_NODE *find(char *word, TREE_NODE *TREE)
{
   int compare;

   if(!TREE) 
      return NULL;
   compare = strcmp(word, TREE->word);
   if(compare < 0) 
      return find(word, TREE->left);
   else if(compare > 0)
      return find(word, TREE->right);
                 
      return TREE;
}

