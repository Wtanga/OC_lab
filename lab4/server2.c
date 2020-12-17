#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
int create_pr_lab2(char **flags)
{
  pid_t pid;
  int rv;
  switch(pid=fork()) {
  case -1:
        perror("fork"); /* произошла ошибка */
        exit(1); /*выход из родительского процесса*/
  case 0:
        printf("  CHILD: Это процесс-потомок!\n");
        printf("  CHILD: Выполняю l2; c флагом %s\n", flags[1]);
        printf("***********************************************\n");
        execv("l2", flags);
		kill(getppid(), SIGUSR1);//передаю родителю пользовательский сигнал SIGUSR1
  default:
        printf("PARENT: Это процесс-родитель!\n");
        printf("PARENT: Я жду, пока потомок не вызовет exit...\n");
        wait(&pid);
        printf("***********************************************\n");
        printf("PARENT: Код возврата потомка:%d\n", WEXITSTATUS(rv));
        printf("PARENT: Выход!\n");
        raise(SIGUSR2);//родитель сам себе посылает пользовательский сигнал SIGUSR1
  }
}

int create_pr_name(const char *name_programm, char *fl1, char *fl2, char *fl3)
{
	pid_t pid;
	int rv;
	switch(pid=fork()) {
	case -1:
		perror("fork"); /* произошла ошибка */
	    exit(1); /*выход из родительского процесса*/
	case 0:
	    printf("  CHILD: Это процесс-потомок!\n");
	    printf("  CHILD: Выполняю %s; c флагом %s\n", name_programm, fl1);
	    execl(name_programm, name_programm, fl1, fl2, fl3, NULL);

	default:
	    printf("PARENT: Это процесс-родитель!\n");
	    printf("PARENT: Я жду, пока потомок не вызовет exit...\n");
	    wait(&pid);
	    printf("PARENT: Код возврата потомка:%d\n", WEXITSTATUS(rv));
	    printf("PARENT: Выход!\n");
		
	}
}

int background_process(char *name_programm, char *arg1, char *arg2, char *arg3) 
{
    pid_t pid;
	int rv;
    pid = fork();
    if (pid == 0) {
        daemon (1, 0);
        execl (name_programm, name_programm, arg1, arg2, arg3, NULL);
        while (1) {
            if (waitpid(pid, &rv, 0) == pid) {
                _exit (EXIT_FAILURE);
            }
        }
    } else if (pid < 0)
        rv = -1;
    else if (waitpid (pid, &rv, 0) != pid)
        rv = -1;

    return rv;
}

int valid_l2_flag(char *fl1)
{
    if (!strcmp(fl1, "-h") || !strcmp(fl1, "--help")    || 
        !strcmp(fl1, "-d") || !strcmp(fl1, "--delete")  || 
        !strcmp(fl1, "-r") || !strcmp(fl1, "--replace") || 
        !strcmp(fl1, "-c") || !strcmp(fl1, "--copy")    || 
        !strcmp(fl1, "-s") || !strcmp(fl1, "--size")    || 
        !strcmp(fl1, "-l") || !strcmp(fl1, "--list")    || 
        !strcmp(fl1, "-p") || !strcmp(fl1, "--procfs"))
        return 1;
    return 0;      
}

int signalhandler(int sig)
{
	switch (sig)
	{
	case SIGUSR1:
		printf("received SIGUSR1\n");
		break;
	case SIGUSR2:
		printf("received SIGUSR2\n");
		break;
	case SIGKILL:
		printf("received SIGKILL\n");
		break;
	case SIGSTOP:
		printf("received SIGSTOP\n");
		break;
	default:
		break;
	}
}

int main(int argc, char *argv[])
{
    signal(SIGINT, signalhandler);
	signal(SIGUSR1, signalhandler);
	signal(SIGUSR2, signalhandler);
	signal(SIGKILL, signalhandler);
	signal(SIGSTOP, signalhandler);

    struct sockaddr_in server, client;
    int sock = socket(AF_INET, SOCK_STREAM, 0); // создание сокета
    int size;
    char buf[64][8];
    // структура для сервера
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    server.sin_port = htons(2019); // порт сервера
    bind(sock, &server, sizeof(server)); // связка с сокетом
    listen(sock,5); // размер очереди
    while(1)
    {
        int newsock, clnlen;
        newsock = accept(sock, &client, &clnlen); // появление нового клиента
        printf("New client: %s\n",inet_ntoa(client.sin_addr));
        while((size = recv(newsock, buf, sizeof(buf), 0)) != 0)
        {
            if (buf == NULL) //Если флаги не введены
            {
                //Ловим сигналы Нибиру
            }
            else if (valid_l2_flag(buf[1])) //Если флаг совпадает с флагами из лаб. 2
            {
                create_pr_lab2(buf);
            }
            else if (!strcmp(buf[1], "--demon")) //Если процесс нужно запустить в фоновом режиме
            {
                background_process(buf[2], buf[3], buf[4], buf[5]);
            }
            else //Если процесс нужно запустить НЕ в фоне
            {
                create_pr_name(buf[1], buf[2], buf[3], buf[4]);
            }
            send(newsock, buf, sizeof(buf), 0); // отправляем эхо
        } // пока получаем от клиента

            
        close(newsock);
    }
    close(sock);
	
    
    return 0;
}

