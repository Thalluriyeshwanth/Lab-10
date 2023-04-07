/* Sample solution to Lab-7 assignment.
 * To Compile: gcc -Wall -o lab7 lab7_solution.c
 * To Run: ./lab7 commands.txt
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

void createarray(char *buf, char **array) {
	int i, count, len;
	len = strlen(buf);
	buf[len-1] = '\0'; 
	for (i = 0, array[0] = &buf[0], count = 1; i < len; i++) {
		if (buf[i] == ' ') {
		   buf[i] = '\0';
		   array[count++] = &buf[i+1];
		}
	}
	array[count] = (char *)NULL;
}

int main(int argc, char **argv) {
    pid_t pid;
    int status;
    char line[BUFSIZ], buf[BUFSIZ], *args[BUFSIZ];
    time_t t1, t2;
    char y[1000], s[1000];
    int b, u;

    if (argc < 2) {
        printf("Usage: %s <commands file>\n", argv[0]);
        exit(-1);
    }

    FILE *fp1 = fopen(argv[1],"r");
    if (fp1 == NULL) {
	printf("Error opening file %s for reading\n", argv[1]);
	exit(-1);
    }

    FILE *fp2 = fopen("output.log","w");
    if (fp2 == NULL) {
	printf("Error opening file output.log for writing\n");
	exit(-1);
    }

    while (fgets(line, BUFSIZ, fp1) != NULL) {
      strcpy(buf, line); 
      createarray(line, args);
#ifdef DEBUG
      int i;
      printf("%s", buf);
      for (i = 0; args[i] != NULL; i++)
          printf("[%s] ", args[i]);
      printf("\n");
#endif
      time(&t1);
      pid = fork();
      if (pid == 0) { 
      sprintf(y, "%d", getpid());
      sprintf(s, "%d", getpid());
      strcat(y, ".out");
      strcat(s, ".err");
      if ((u = open(y, O_CREAT | O_APPEND | O_WRONLY, 0755)) == -1)
      {
        printf("Error in opening file %s \n", y);
        exit(-1);
      }
      if ((b = open(s, O_CREAT | O_APPEND | O_WRONLY, 0755)) == -1)
      {
        printf("Error in  opening file %s\n", s);
        exit(-1);
      }
   
      dup2(u, 1);
      dup2(b, 2);
        execvp(args[0], args);
	perror("exec");
	exit(-1);
      } else if (pid > 0) {
        printf("Child started at %s", ctime(&t1));
        printf("Wait for the child process to terminate\n");
        wait(&status); 
        time(&t2);
        printf("Child ended at %s", ctime(&t2));
        if (WIFEXITED(status)) { 
	  printf("Child process exited with status = %d\n", WEXITSTATUS(status));
        } else {
	  printf("Child process did not terminate normally!\n");

        }
	buf[strlen(buf) - 1] = '\t'; 
	strcat(buf, ctime(&t1)); 
	buf[strlen(buf) - 1] = '\t'; 
	strcat(buf, ctime(&t2)); 
	fprintf(fp2, "%s", buf);
	fflush(fp2);

  close(u);
      close(b);
      
      } else { 
        perror("fork"); 
        exit(EXIT_FAILURE);
      }
    }

    fclose(fp1);
    fclose(fp2);
    printf("[%ld]: Exiting main program .....\n", (long)getpid());

    return 0;
}
