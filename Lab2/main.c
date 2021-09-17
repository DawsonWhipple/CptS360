#include <stdio.h>             // for I/O
#include <stdlib.h>            // for I/O
#include <libgen.h>            // for dirname()/basename()
#include <string.h>
#include <stddef.h>

typedef struct node{
         char  name[64];       // node's name string
         char  type;           // 'D' for DIR; 'F' for file
   struct node *child, *sibling, *parent;
}NODE;


NODE *root, *cwd, *start;
char line[128];
char command[16], pathname[64];

//                0        1       2     3     4       5       6     7         8       9       10
char *cmd[] = {"mkdir", "rmdir", "cd", "ls", "pwd", "creat", "rm", "save", "reload", "menu", "quit",  0};

//n_path == normalized path
//p_path == parent path
//b_path == base path

//unedited from professor's code
int findCmd(char *command)
{
   int i = 0;
   while(cmd[i]){
     if (strcmp(command, cmd[i])==0)
         return i;
     i++;
   }
   return -1;
}
//unedited from professor's code
NODE *search_child(NODE *parent, char *name)
{
  NODE *p;
  printf("search for %s in parent DIR\n", name);
  p = parent->child;
  if (p==0)
    return 0;
  while(p){
    if (strcmp(p->name, name)==0)
      return p;
    p = p->sibling;
  }
  return 0;
}
//unedited from professor's code
int insert_child(NODE *parent, NODE *q)
{
  NODE *p;
  printf("insert NODE %s into END of parent child list\n", q->name);
  p = parent->child;
  if (p==0)
    parent->child = q;
  else{
    while(p->sibling)
      p = p->sibling;
    p->sibling = q;
  }
  q->parent = parent;
  q->child = 0;
  q->sibling = 0;
}

int remove_child(NODE *parent, NODE *q){
  NODE *p;
  printf("remove NODE %s into END of parent child list\n", q->name);
  p = parent->child;
  if (p==q){
    parent->child = q->sibling;
    free(q);
    return 0;
  }
  while(p){
    if(p->sibling == q){
      p->sibling = p->sibling->sibling;
      free(q);
      return 0;
    }
    p = p->sibling;
  }
  return -1;
}
//path to node conversion function
NODE *pton(char *pathname){
  if(!pathname || pathname[0] == '\0'){
    return NULL;
  }
  char *name[64];
  char temp[128];
  strcpy(temp, pathname);

  NODE *p;
  int n = 0;

  start = pathname[0] == '/' ? root : cwd;

  p = start;

  printf("pton: %s\n", pathname);
  name[n] = strtok(temp, "/");

  while(name[n]){
    printf("component: %s\n", name[n]);
    n++;
    name[n] = strtok(NULL, "/");
  }

  for(int i = 0; i < n; i++){
    if(!strcmp(name[i], "."))
      continue;
    if(!strcmp(name[i], "..")){
      p = p->parent;
      continue;
    }
    p = search_child(p, name[i]);
    if(!p)
      return NULL;
  }
  return p;
}
//no to path recursive helper function
void ntop_helper(NODE *node, char * name){
  if(node->parent != node)
    ntop_helper(node->parent, name);
  else
    *name = '\0';

  strcat(name, node->name);
  if(node->parent != node)
    strcat(name, "/");
}
//node to path conversion
void ntop(NODE *node, char *name){
  ntop_helper(node, name);
  if(strlen(name) > 1)
    name[strlen(name) - 1] = '\0';
}

void normalize(char *name, char *n_path){
  char *temp = malloc(strlen(name));
  strcpy(temp, name);
  memset(n_path, '\0', strlen(name));
  if(temp[0] != '/')
    ntop(cwd, n_path);
  else 
    n_path[0] = '/';
  
  char *token = strtok(temp, "/");
  while(token){
      if(strcmp(token, ".") == 0 || strlen(token) == 0){
        token - strtok(NULL, "/");
        continue;
      }
      if(strcmp(token, "..") == 0){
        int i = strlen(n_path - 1);
        while(n_path[i] != '/'){
          i--;
        }
        if(!i){
          i++;
        }
        n_path[i] = '\0';
      }
      else{
        if(n_path[strlen(n_path) - 1] != '/'){
          strcat(n_path, "/");
        }
        strcat(n_path, token);
      }
      token = strtok(NULL, "/");
  }
}

void parent_of(char *name, char *p_path){
  int index = -1;
  for(int i = 0; name[i]; i++){
    if(name[i] == '/'){
      index = i;
    }
  }
  if(index == -1){
    strcpy(p_path, cwd->name);
    return;
  }
  if(index == 0){
    index++;
  }
  strncpy(p_path, name, index);
}

int mkdir(char *name)
{
  NODE *p, *q;
  char *n_path = malloc(strlen(name));
  char *p_path = malloc(strlen(name)); //new
  char *b_path;
  printf("mkdir: name=%s\n", name);

  if (!strcmp(name, "/") || !strcmp(name, ".") || !strcmp(name, "..") || !strcmp(name, "")){
    printf("can't mkdir with %s\n", name);
    return -1;
  }
  
  normalize(name, n_path);
  printf("check whether %s already exists\n", name);
  parent_of(n_path, p_path);
  printf("%s, %s\n", n_path, p_path);
  start = pton(p_path);
  if(!start){
    printf("Path doesn not exist\n");
    return -1;
  }
  if(start->type == 'F'){
    printf("Cannot insert directory under a file\n");
    return -1;
  }
  b_path = n_path + strlen(p_path);
  if(strcmp(p_path, "/")){
    b_path++;
  }
  p = search_child(start, b_path);

  if (p){
    printf("name %s already exists, mkdir FAILED\n", name);
    return -1;
  }
  printf("--------------------------------------\n");
  printf("ready to mkdir %s\n", b_path);
  q = (NODE *)malloc(sizeof(NODE));
  q->type = 'D';
  strcpy(q->name, b_path);
  insert_child(start, q);
  printf("mkdir %s OK\n", b_path);
  printf("--------------------------------------\n");

  free(p_path);
    
  return 0;
}

int rmdir(char *name){
  
  printf("rmdir: name = %s\n", name);

  if(!strcmp(name, "/") || !strcmp(name, ".") || !strcmp(name, "..")){
    printf("can't rmdir with %s\n", name);
    return -1;
  }

  printf("check whether %s exists\n", name);
  start = pton(pathname);
  if(!start){
    printf("Path does not exist\n");
    return -1;
  }
  if(start->type == 'F'){
    printf("Cannot remove file with rmdir\n");
    return -1;
  }
  if(start->child){
    printf("Cannot remove non empty directory\n");
    return -1;
  }
  printf("--------------------------------------\n");
  printf("ready to rmdir %s\n", name);
  if(remove_child(start->parent, start)){
    printf("rmdir %s failed\n", name);
    printf("--------------------------------------\n");
    return -1;
  }
  printf("rmdir %s OK\n", name);
  printf("--------------------------------------\n");

  return 0;
}


int cd(char *name){
  NODE *p;
  printf("cd into %s\n", name);
  p = pton(name);
  if(!p){
    printf("path does not exist, please try again\n");
    return -1;
  }
  cwd = p;
  printf("cwd: %s\n", cwd->name);
}

// This ls() list CWD. You MUST improve it to ls(char *pathname)
int ls(char *name)
{
  NODE *p;
  if(!name[0]){
    p = cwd;
  }
  else{
    p = pton(name); //new
  }
  if(!p){
    printf("invalid path\n");
    return -1;
  }

  p = p->child;
  printf("cwd contents = \n");
  while(p){
    printf("[%c %s] \n", p->type, p->name);
    p = p->sibling;
  }
  printf("\n");
}

int pwd(){
  char path[128];
  ntop(cwd, path);
  printf("pwd: %s\n", path);
}

int creat(char *name){
  NODE *p, *q;
  char *n_path = malloc(strlen(name));
  char *p_path = malloc(strlen(name)); //new
  char *b_path;
  printf("creat: name = %s \n", name);

  if(!strcmp(name, "/") || !strcmp(name, ".") || !strcmp(name, "..") || !strcmp(name, "")){
    printf("cant create with %s\n", name);
    return -1;
  }

  normalize(name, n_path);
  printf("check whether %s already exists\n", name);
  parent_of(n_path, p_path);
  start = pton(p_path);
  if(!start){
    printf("Path does not exist\n");
    return -1;
  }
  if(start->type == 'F'){
    printf("Cannot insert file under a file\n");
    return -1;
  }
  b_path = n_path + strlen(p_path);
  if(strcmp(p_path, "/")){
    b_path++;
  }
  p = search_child(start, b_path);

  if (p){
    printf("name %s already exists, creat FAILED\n", name);
    return -1;
  }
  printf("--------------------------------------\n");
  printf("ready to creat %s\n", b_path);
  q = (NODE *)malloc(sizeof(NODE));
  q->type = 'F';
  strcpy(q->name, b_path);
  insert_child(start, q);
  printf("creat %s OK\n", b_path);
  printf("--------------------------------------\n");

  free(p_path);
    
  return 0;
}

int rm(char *name){
  printf("rm: name = %s\n", name);

  if(!strcmp(name, "/") || !strcmp(name, ".") || !strcmp(name, "..")){
    printf("can't rm with %s\n", name);
    return -1;
  }

  printf("check whether %s exists\n", name);
  start = pton(pathname);
  if(!start){
    printf("Path does not exist\n");
    return -1;
  }
  if(start->type == 'D'){
    printf("Cannot remove directory with rm\n");
    return -1;
  }
  printf("--------------------------------------\n");
  printf("ready to rm %s\n", name);
  if(remove_child(start->parent, start)){
    printf("rm %s failed\n", name);
    printf("--------------------------------------\n");
    return -1;
  }
  printf("rm %s OK\n", name);
  printf("--------------------------------------\n");

  return 0;
}

void save_helper(FILE* fp, NODE* node){
  char pathname[128];
  ntop(node, pathname);
  fprintf(fp, "%c %s\n", node->type, pathname);
  if(node->child){
    save_helper(fp, node->child);
  }
  if(node->sibling){
    save_helper(fp, node->sibling);
  }
}

int save(char *filename){
  FILE *fp = fopen(filename, "w+");
  save_helper(fp, root);
  fclose(fp);
}

int reload(char *filename){
  FILE *fp = fopen(filename, "r");
  char line[132];

  while(fgets(line, sizeof(line), fp)){
    line[strlen(line) -1] = '\0';
    if(line[0] == 'D'){
      mkdir(line + 2);
    }
    else{
      creat(line + 2);
    }
  }
  fclose(fp);
}

//displays menu to user
int menu(){
  printf("====================== MENU =========================\n");
  printf("mkdir rmdir ls  cd  pwd  creat  rm  save reload  quit\n");
  printf("=====================================================\n");
}

int quit(){
  save("Lab2.txt");
  printf("Program exit\n");
  exit(0);
  // improve quit() to SAVE the current tree as a Linux file
  // for reload the file to reconstruct the original tree
}
//unedited from profeesor's code
int initialize()
{
    root = (NODE *)malloc(sizeof(NODE));
    strcpy(root->name, "/");
    root->parent = root;
    root->sibling = 0;
    root->child = 0;
    root->type = 'D';
    cwd = root;
    printf("Root initialized OK\n");
}

int main()
{
  int index;

  initialize();

  printf("Enter 'menu' for help menu\n");
  //printf("NOTE: commands = [mkdir|ls|quit]\n");

  while(1){
      printf("Enter command line : ");
      fgets(line, 128, stdin);
      line[strlen(line)-1] = 0;

      command[0] = pathname[0] = 0;
      sscanf(line, "%s %s", command, pathname);
      printf("command=%s pathname=%s\n", command, pathname);
      
      if (command[0]==0) 
         continue;

      index = findCmd(command);

      switch (index){
        case 0: mkdir(pathname);  break;
        case 1: rmdir(pathname);  break;
        case 2: cd(pathname);     break;
        case 3: ls(pathname);     break;
        case 4: pwd();            break;
        case 5: creat(pathname);  break;
        case 6: rm(pathname);     break;
        case 7: save(pathname);   break;
        case 8: reload(pathname); break;
        case 9: menu();           break;
        case 10: quit();          break;
        defualt:
          printf("invalid command\n");
          break;
      }
  }
}