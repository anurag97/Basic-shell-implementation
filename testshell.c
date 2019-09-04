#include<unistd.h>
#include<stdio.h>
#include<dirent.h>
#include<string.h>
#include<glob.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<stdlib.h>
#include<errno.h>
//int execvp(const char *file, char *const argv[]);

#define MAX_CMD_LEN 256
#define HISTORY_COUNT 100

 int u =1;
 int gargc;
 char *gargv[64];
 //int current =0;
//int exit_status=0;    
//char* hist[HISTORY_COUNT];
//char* recordarr[1000];

/*void checkExit(){
    if test "$?" != "0"; 
      echo Command $1 exited with abnormal status
      exit(1);
    else echo $?
    
}*/



/*int history(int current){
  int i =current;
  int hist_num=1;
  do{
    if(hist[i]){
      printf("%4d %s\n", hist_num, hist[i]);
      hist_num++;}
      i = (i+1)%HISTORY_COUNT;
  }while(i!=current);
  return 0;
} */

 void  parse_input(char *input, char **argv)
{
     while (*input != '\0') {       
          while (*input == ' ' || *input == '\t' || *input == '\n')
               *input++ = '\0';     
          *argv++ = input;          
          while (*input != '\0' && *input != ' ' && 
                 *input != '\t' && *input != '\n') 
               input++;           
     }
     *argv = '\0';                 
}
void  execute(char **argv)
{
     pid_t  pid;
     int    status;
     
     if ((pid = fork()) < 0) {     
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
     else if (pid == 0) {          
          if (execvp(*argv, argv) < 0) {    
               printf("*** ERROR: exec failed\n");
               exit(1);
          }
     }
     else {                                  
          while (wait(&status) != pid)       
               ;
     }
}

 char* takeinput(){
  char* input = NULL;
  ssize_t buffsize =0;
  getline(&input, &buffsize , stdin);
  return input;
}

char runprompt(int u){
  if(u==0) return '#';
  else    return   '$';
};

char* currdirectory(void){
long path_max;
size_t size;
char *buf;
char *ptr;

path_max = pathconf(".", _PC_PATH_MAX);
if (path_max == -1)
    size = 1024;
else if (path_max > 10240)
    size = 10240;
else
    size = path_max;


for (buf = ptr = NULL; ptr == NULL; size *= 2)
{
    if ((buf = realloc(buf, size)) == NULL)
    {
     continue;   
    }
    ptr = getcwd(buf, size);
    if (ptr == NULL && errno != ERANGE)
    {continue;
    }
}


return ptr;
}

void init_shell_loop(void){
 int status =1; 
 //current =0;
  //for (i = 0; i < HISTORY_COUNT; i++)
    //            hist[i] = NULL;
  while(status){  
   /*************************************************************************/  
   char *ch="\n";
   char *ch1 ="quit\n";

   char* cwd = currdirectory();
   char prompt = runprompt(u);
   printf("%s :%c ", cwd,prompt);
  /*************************************************************************/


     char* input = takeinput();

     if(strcmp(input,ch)==0){
       continue;
     }

    else  if(strcmp(input,ch1)==0){ status =0;break;}
     else if(strcmp(input,"su\n")==0){u=0;continue;}
    else if(strcmp(input,"suexit\n")==0){u=1;continue;}
      else if(strcmp(input,"$$\n")==0){int procid = getpid(); printf("%d\n",procid);}
     else{
       
       if (input[strlen(input) - 1] == '\n')input[strlen(input) - 1] = '\0';
      
      // char* binpat = "/bin/";
       parse_input(input, gargv);
        execute(gargv); 
       /*char *args[]={input,NULL}; 
        execvp(args[0],args); 
      */
        

     }
     /************************HISTORY FUNCTION*************************************
     if (input[strlen(input) - 1] == '\n')input[strlen(input) - 1] = '\0';
      hist[current]=strdup(cmd);
      current = (current+1)%HISTORY_COUNT;
      if (strcmp(cmd, "history") == 0)
      history(hist, current);
      if(input[strlen(input)-1] =='\n') input[strlen(input)-1] = '\0';
    /******************************************************************************/

   
   

     //if(cmd )
      
   };

printf("Thanks for using Anurag's shell! Fir milenge...\n");
 return;
}

      
int main(int argc, char **argv){
    
    init_shell_loop();
    return 0;


}      