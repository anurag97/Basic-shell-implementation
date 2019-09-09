
#include<unistd.h>
#include<ctype.h>
#include<stdlib.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string.h>
#include<stdio.h>
#include<sys/wait.h>
#include<errno.h>



int pipcnt =0;



char input_buff[1024];

char *cmd[100];

int flag=0;
static char *argv[1024];




int status;

char currdir[500];






static int choice(int , int , int , char *cmd);

void change_directory()
{
char *h="/home";   
if(argv[1]==NULL)
        chdir(h);
else if ((strcmp(argv[1], "~")==0) || (strcmp(argv[1], "~/")==0))
        chdir(h);
else if(chdir(argv[1])<0)
    printf("error %s", argv[1]);

}

char cwd[1024];

void currdirfunc(){
    if(getcwd(cwd, sizeof(cwd))!=NULL)
    printf("%s\n", cwd);
    else
     printf("current directory error!!\n"); 
}



static char* removespace(char *inp){
     while (isspace(*inp)) ++inp;
     

    return inp;

}
char* removeinvertedcommas(char* input)
{
  int i=0, j=0;
  char temp[100];
  while(input[i++]!='\0'){
    if(input[i-1]!='"')
    temp[j++]=input[i-1];
            }
        temp[j]='\0';
        input = strdup(temp);
  
  return input;
}

void basetokenise(char* cmd){

    int i=1;

    argv[0] = strtok(cmd," ");
    while((argv[i]=strtok(NULL," "))!=NULL)
    i++;

}


int output_redir_flag =0;

int input_redir_flag =0;

char *inpredir_file;

char *opredir_file;
void token_io(char *cmd)
{

      char *temp;  
  temp=strdup(cmd);

  
  char *token[100];

  int i=1;
 token[0]=strtok(temp,"<");       
  while((token[i]=strtok(NULL,">"))!=NULL)
        i++;

  token[1]=removespace(token[1]);

  token[2]=removespace(token[2]);

  inpredir_file=strdup(token[1]);

  opredir_file=strdup(token[2]);

  basetokenise(token[0]);
  
}
void inputredirection_tokens( char *cmd){
    char *token[100];

    char *temp;
    
    temp = strdup(cmd);

    int i=1;
    token[0] =strtok(temp,"<");
    
    while((token[i] = strtok(NULL,"<"))!=NULL)
    i++;

    token[1]=removespace(token[1]);
    inpredir_file = strdup(token[1]);

    basetokenise(token[0]);
    
}

void outputredirection_tokens( char *cmd){
    char *token[100];

    char *temp;
    
    temp = strdup(cmd);

    int i=1;
    token[0] =strtok(temp,">");
    
    while((token[i] = strtok(NULL,">"))!=NULL)
    i++;

    token[1]=removespace(token[1]);
    opredir_file = strdup(token[1]);

    basetokenise(token[0]);
    
}

 static int execute(char *cmd, int input, int first, int last){
    char *temp;

    temp = strdup(cmd);

    int i=1;
    argv[0] = strtok(cmd," ");

    while((argv[i]=strtok(NULL," "))!=NULL)
    i++;

    argv[i] = NULL;

    if(argv[0]!=NULL){
       if(strcmp(argv[0],"echo")==0){
               cmd  = removeinvertedcommas(temp);
                      int i=1;
                      argv[0]=strtok(cmd," ");       
                      while((argv[i]=strtok(NULL," "))!=NULL)
                                i++;
                      argv[i]=NULL;
           }
           if(strcmp("cd",argv[0])==0)
                    {
                    change_directory();
                    return 1;
                    }
           else if(strcmp(argv[0],"pwd")==0){
               currdirfunc();
               return 1;
           }
            else if(strcmp(argv[0],"exit")==0)
           {
               printf("bye!!");
               exit(0);
           }
           else if(strcmp(argv[0],"$$\n")==0){
            int procid = getpid(); 
          printf("%d\n",procid);
          }
    }
    return choice(input,first,last,temp);

    }


void executepipe() {
    int i=0;
    int input =0;
    int first =1;
    int j=1;
    
    cmd[0] = strtok(input_buff,"|");
   while((cmd[j] = strtok(NULL,"|"))!=NULL) j++;

   cmd[j] =NULL;
   pipcnt = j-1;

   for(i=0;i<j-1;i++){
       input = execute(cmd[i],input,first,0);
       first =0;
   }
   input = execute(cmd[i],input, first,1);
   input =0;
   return;

}
pid_t pid;
static int choice(int input, int first, int last, char *cmd){
    int pfd[2];
    int ifd;
    int ofd;

   
   int ret = pipe(pfd);

   if(ret == -1){
       printf("error");
       return 1;
   }
   
   pid= fork();

   if(pid ==0){
       if(first==1 && last ==0 && input == 0){
           dup2(pfd[1],1);
       }
       else if(first ==0 && last ==0 && input !=0){
           dup2(input,0);
           dup2(pfd[1],1);
       }
       else{
           dup2(input,0);
       }


   if(strchr(cmd,'<'))
   {
       input_redir_flag =1;
       inputredirection_tokens(cmd);

   }
   else if(strchr(cmd,'>')){
       output_redir_flag =1;
       outputredirection_tokens(cmd);
   }

   if(input_redir_flag==1){
        ifd=open(inpredir_file,O_RDONLY, 0);
                         if (ifd < 0)
                          {
                          fprintf(stderr, "Failed to open %s for reading\n", inpredir_file);
                          return 0;
                          }
                        dup2(ifd, 0);
                        close(ifd);
                        input_redir_flag=0;
   }
   else if(output_redir_flag == 1)
                {                    
                        ofd= creat(opredir_file, 0644);
                        if (ofd < 0)
                          {
                          fprintf(stderr, "Can't open %s\n", opredir_file);
                          return(EXIT_FAILURE);
                          }
                        dup2(ofd, 1);
                        close(ofd);
                        output_redir_flag=0;
                }
 else if(execvp(argv[0], argv)<0) 
 printf("command doesn't exist!!\n");
 exit(0);
   }

   else{
       waitpid(pid,0,0);
   }
if(last ==1)
close(pfd[0]);
if(input!=0)
close(input);
close(pfd[1]);
return pfd[0];

}

int main(){
 int status;
 char ln[2] = "\n";
 getcwd(currdir,sizeof(currdir));

 while(1){
printf("Shell>> ");
 fgets(input_buff,1024,stdin);

 if(strcmp(input_buff,ln)==0) continue;
 int stlength = strlen(input_buff);
 input_buff[stlength -1] ='\0';

 if(strcmp(input_buff,"exit")==0){
     flag =0;
     break;
 }
executepipe();
waitpid(pid,&status,0);
 }
if(flag ==1){
    printf("Nmaskaar\n");
    exit(0);
    
}
return 0;
}