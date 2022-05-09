#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
//for the cd command
#define GetCurrentDir getcwd

//global file pointer
FILE *file;

void red()
{
  //changing color of error message in red
  printf("\033[3;31m");
}

void white()
{
  //reseting color
  printf("\033[0m");
}

///////////////////////////////////////////////////////

//handling input errors in execvp
void errcall()
{
  //print for the user the error
  printf("Shell: %s\n",strerror(errno));
  //kill this child to avoid unneeded child
  kill(getpid(),SIGKILL);
}

///////////////////////////////////////////////////////

//handling signals when a child is killed
void handleKillSignal()
{
file=fopen("childlog.txt","a");
if(file==NULL)
{red();
printf("\nThere was an error while opening your log file\n");
white();
}
else
{
fprintf(file,"A child process has been terminated\n");
fclose(file);
}
}


void sigchld_handler(int sig)
{
handleKillSignal();
}


///////////////////////////////////////////////

int main(int argc, char*argv[])
{

//declaring variables
char *string=(char *)malloc(100);
char *args[100];
char *temp=malloc(100);
int j,pid;


//getting the current path of file (home directory) to save for the cd command
char cCurrentPath[FILENAME_MAX];

//handling error of getting the current file path name
 if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
     {
     return errno;
     }
//setting last char of the path name to NULL to avoid errors
cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';


//opening new log file for the program
file=fopen("childlog.txt","w");
if(file==NULL)
{red();
printf("\nThere was an error while opening your log file\n");
white();
}
else
{
fprintf(file,"Program has started\n");
fclose(file);
}



//loop to make the program on going till the user exits
while(1){
white();
//welcome message to signal for the user to enter his input
START:printf("shell>>");
//scanning the input from the user
gets(string);

//checking if the user entered nothing
if(strcmp(string,"")==0)
{
goto START;
}


//checking if the user typed exit to terminate the program
if(strcmp(string,"exit")==0)
{
white();
printf("\nThank you for using shell\n<<shell>>\n");
free(string); //freeing saved space
free(temp);
exit(EXIT_SUCCESS);
}


//diving the string input by spaces
temp=strtok(string," ");
//reseting the args counter
j=0;

//entering the separate words in the args list
while(temp!=NULL)
{
args[j++]=temp;
temp=strtok(NULL," ");
}

//setting the last input for args by NULL
args[j]=NULL;


//checking if the command is cd
if(strcmp(args[0],"cd")==0)
{
if(args[1]==NULL)
chdir(cCurrentPath);
else
{
if(chdir(args[1])!=0)
{
perror("\033[3;31mShell");

}
}
//skip the next the part as the parent performed the command no need for a child process
continue;
}

//creating a child
pid=fork();


if(pid>0) //parent code
{
//check to see if a child has been terminated to update log file
signal(SIGCHLD,sigchld_handler);

//checking to see if there is an & in the args to see if the parent should wait for this child or not
if(strcmp(args[j-1],"&")!=0)
{
int child_status;
waitpid(pid,&child_status,0); //waiting for a specific child using pid
}
else
args[j-1]=NULL; //removing the & before sending the args and skipping the waiting line
}


if(pid==0) //child code
{
execvp(args[0],args);
//handling input error
if(argc!=2) 
    {

       errno=EINVAL;
       red();
       errcall();
       white();
    }
    
}

if(pid==-1) //fork returned -1 and no process was created
{red();
printf("There was an error while creating your process\n");
white();
}
}

return 0;

}
