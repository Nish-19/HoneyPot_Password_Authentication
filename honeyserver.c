/*
Author: Nischal A
Roll Num: 1801cs33
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAX 1000
#define SIZE 1024

int n;
int u;
int h;
int new_sock;

FILE *flog;

struct correctkey
{
	int usernum;
	int sugarindex;
};

struct correctkey keys[MAX];

char crrctkeydetails[MAX][20];

int users[MAX];
int honeypots[MAX];

int verify_with_honeyserver(int usernum, int index)
{
	int i;
	int flag=0;
	for (i=0; i<n; i++)
	{
		if (keys[i].usernum == usernum && index == keys[i].sugarindex)
		{
			//printf("Correct password. Login Successfull\n");
			flag=1;
			break;
		}
	}
	if (flag == 1)
		return 1;
	else
	{
		// diagnosing the problem
		// CASE1: IF user account
		for (i=0; i<u; i++)
		{
			if (users[i] == index)
			{
				flag = 2;
				break;
			}
		}
		if (flag == 2)
		{
			fprintf(flog, "%s\n", "BREACH (USER ACCOUNT TYPE DETECTED). NOTING DOWN IP.");
			return flag;
		}
		// Case2: If honeypot account
		for (i=0; i<h; i++)
		{
			if (honeypots[i] == index)
			{
				flag = 3;
				break;
			}
		}
		if (flag == 3)
		{
			fprintf(flog, "%s\n", "BREACH (HONEYPOT ACCOUNT TYPE DETECTED). NOTING DOWN IP.");
			return flag;
		}
		else
		{
			fprintf(flog, "%s\n", "Some honeyserver error detected.");
			return -1;
		}
	}
}

void print_keys()
{
	fprintf(flog, "%s", "Printing the keys\n");
	int i;
	for (i=0; i<n; i++)
	{
		fprintf(flog, "%d:%d\n", keys[i].usernum, keys[i].sugarindex);
	}
}

void print_users()
{
	fprintf(flog, "%s", "Printing the users\n");
	int i;
	for (i=0; i<u; i++)
	{
		fprintf(flog, "%d\n", users[i]);
	}
}

void print_pots()
{
	fprintf(flog, "%s", "Printing the pots\n");
	int i;
	for (i=0; i<h; i++)
	{
		fprintf(flog, "%d\n", honeypots[i]);
	}
}

void read_keys()
{
	FILE *fp = fopen("honeyinfo/correctindex.txt", "r");
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int count=1;
    while ((read = getline(&line, &len, fp)) != -1) 
    {
    	strcpy(crrctkeydetails[count-1], line);
    	// printf("Retrieved line of length %zu:\n", read);
    	keys[count-1].usernum = count;
    	line[strlen(line)-1] = '\0';
    	int ackey = atoi(line);
    	keys[count-1].sugarindex = ackey;
    	//printf("Line retrived is %s\n", line);
    	count++;
    }
    n = count - 1;
    fclose(fp);
    if (line)
        free(line);
    fprintf(flog, "Total %d Keys succesfully read\n", n);
}

void read_users()
{
	FILE *fp = fopen("honeyinfo/users.txt", "r");
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int count=1;
    while ((read = getline(&line, &len, fp)) != -1) 
    {
    	// printf("Retrieved line of length %zu:\n", read);
    	line[strlen(line)-1] = '\0';
    	int ackey = atoi(line);
    	users[count-1] = ackey;
    	//printf("Line retrived is %s\n", line);
    	count++;
    }
    u = count - 1;
    fclose(fp);
    if (line)
        free(line);
    fprintf(flog, "Total %d Users succesfully read\n", u);
}

void read_pots()
{
	FILE *fp = fopen("honeyinfo/honeypots.txt", "r");
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int count=1;
    while ((read = getline(&line, &len, fp)) != -1) 
    {
    	// printf("Retrieved line of length %zu:\n", read);
    	line[strlen(line)-1] = '\0';
    	int ackey = atoi(line);
    	honeypots[count-1] = ackey;
    	//printf("Line retrived is %s\n", line);
    	count++;
    }
    h = count - 1;
    fclose(fp);
    if (line)
        free(line);
    fprintf(flog, "Total %d HoneyPots succesfully read\n", h);
}

void configure_server()
{
	char *ip = "127.0.0.1";
	int port = 8080;
	int e;

	int sockfd, new_sock;
	struct sockaddr_in server_addr, new_addr;
	socklen_t addr_size;
	char buffer[SIZE] = {0};
	char new_buffer[SIZE] = {0};
	char* filename = "recv.txt";

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
	perror("[-]Error in socket");
	exit(1);
	}
	printf("%s", "[+]Server socket created successfully.\n");

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = port;
	server_addr.sin_addr.s_addr = inet_addr(ip);

	int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    	perror("setsockopt(SO_REUSEADDR) failed");


	e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(e < 0) {
	perror("[-]Error in bind");
	exit(1);
	}
	printf("%s", "[+]Binding successfull.\n");

	if(listen(sockfd, 10) == 0){
	printf("[+]Listening....\n");
	}else{
	perror("[-]Error in listening");
	exit(1);
	}

	addr_size = sizeof(new_addr);
	new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
	// Start receiving from here - Initial Setup
	printf("Output only visible to the ADMINISTRATOR\n");
	recv(new_sock, new_buffer, SIZE, 0);
	printf("%s\n", new_buffer);
	bzero(new_buffer, SIZE);
	strcpy(buffer, "Honeyserver accepts request and start");
	printf("%s\n", buffer);
	send(new_sock, buffer, strlen(buffer), 0);
	bzero(buffer, SIZE);
	// recv the usernum and the indexnum
	int usernum, indexnum;
	recv(new_sock, &usernum, sizeof(usernum), 0);
	recv(new_sock, &indexnum, sizeof(indexnum), 0);
	printf("Recieved Value of usernum %d and indexnum %d\n", usernum, indexnum);
	if (usernum==-1 && indexnum==-1)
	{
		printf("Main program handled the situation.\n");
		int res=-1;
		send(new_sock, &res, sizeof(res), 0);
	}
	else
	{
		int res;
		res = verify_with_honeyserver(usernum, indexnum);
		//send back this res
		printf("Sending computed result back\n");
		send(new_sock, &res, sizeof(res), 0);
	}
	close(new_sock);
}

void prepare_server()
{
	read_keys();
	print_keys();
	read_users();
	print_users();
	read_pots();
	print_pots();
	configure_server();
}

int main()
{
	flog = fopen("honeyserverlog.txt", "w");
	// reading key file
	prepare_server();
}