#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define BUFFERSIZE 100

int exitKjell()
{
    return 0;
}

int main(int argc, char* argv[], char* envp[])
{
    char *tok;
    char line[BUFFERSIZE];
    char cwd[BUFFERSIZE];
    char delims[10] = " \t\n";
    char prompt[10] = "> ";
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
    while(1)
    {   
        printf("%s:%s",cwd,prompt);
        fgets(line, sizeof(line), stdin);
        tok = strtok(line, delims);
        while(tok != NULL)
        {
            if(strcmp(tok,"exit")==0)
            {
                return exitKjell(); 
            }
            else if(strcmp(tok,"pwd")==0)
            {
                getcwd(cwd, sizeof(cwd));
                printf("%s\n", cwd);
                tok = strtok(NULL, delims);
            }
            else if(strcmp(tok,"cd")==0)
            {
                tok = strtok(NULL, delims);
                if(chdir(tok)==0)
                {
                    getcwd(cwd, sizeof(cwd));
                    //printf("%s ", tok);
                }
            }
            else if(strcmp(tok,"checkEnv")==0)
            {
                printf("0");
                char *arguments = "";
                //char *pager = "less";

                //char * new_str ;
                //if((new_str = malloc(strlen(str1)+strlen(str2)+1)) != NULL){
                    //new_str[0] = '\0';   // ensures the memory is an empty string
                    //strcat(new_str,str1);
                    //strcat(new_str,str2);
                //} else {
                    //fprintf(STDERR,"malloc failed!\n");
                    // exit?
                //}


                while(tok=strtok(NULL,delims))
                {
                    printf("1");
                    //strcat(arguments,tok);   
                } 
                if(strcmp(arguments,"")==0)
                {   
                    printf("2");
                    //system("printenv | sort | less");
                    execlp("printenv", "printenv", NULL);
                }
                else
                {   
                    //char * derp;
                    //strcat(derp, "printenv | grep ");
                    //strcat(derp, arguments);
                    //strcat(derp, " | sort | pager");
                    //system(derp);
                }
                //system("printenv | grep %s | less", );
                //while(*envp)
                //{
                    //printf("%s\n",*envp);
                    //envp++;
                //}
                tok = strtok(NULL, delims);
            }
            else
            {
                //printf("%s", tok);
                tok = strtok(NULL, delims);
            }
        }
        //printf("\n");
    }   
}
