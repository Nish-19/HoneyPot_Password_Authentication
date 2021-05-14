#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <openssl/md5.h>
#include "supportserver.c"
#define MAX 100
#define honeymin 6
#define honeymax 8

FILE *flogmain;

int n;
int nu;
int p;

struct f2
{
	int index;
	char hash[33];
};
struct f1
{
	int k;
	char name[MAX];
	int list[10];
	char hashlist[10][33];
};

struct f2 f2file[MAX];
struct f1 f1file[MAX];

char f1details[MAX][MAX];
char userdetails[50][MAX];
char passwddetails[50][MAX];

char *str2md5(const char *str, int length) {
    int n;
    MD5_CTX c;
    unsigned char digest[16];
    char *out = (char*)malloc(33);

    MD5_Init(&c);

    while (length > 0) {
        if (length > 512) {
            MD5_Update(&c, str, 512);
        } else {
            MD5_Update(&c, str, length);
        }
        length -= 512;
        str += 512;
    }

    MD5_Final(digest, &c);

    for (n = 0; n < 16; ++n) {
        snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
    }

    return out;
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

void readF1()
{
	FILE *fp1 = fopen("f1.txt", "r");
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int count=0;
    while ((read = getline(&line, &len, fp1)) != -1) 
    {
    	strcpy(f1details[count], line);
    	// printf("Retrieved line of length %zu:\n", read);
    	line[strlen(line)-1] = '\0';
    	char** tokens;
    	tokens = str_split(line, ':');
    	if (tokens)
    	{
	        int i;
	        for (i = 0; *(tokens + i); i++)
	        {
	        	if (i==0)
	        	{
	        		strcpy(f1file[count].name, *(tokens + i));
	        		fprintf(flogmain, "%s:", *(tokens + i));
	        	}
	        	else if (i==1)
	        	{
	        		// need to process these numbers one by one
	        		char **listnums;
	        		listnums = str_split(*(tokens + i), ',');
	        		if (listnums)
		    		{
				        int k;
				        for (k = 0; *(listnums + k); k++)
				        {
				        	int lstnumh = atoi(*(listnums + k));
				        	f1file[count].list[k] = lstnumh;
				        	// strncpy(f1file[count].hashlist[k], f2file[lstnumh-1].hash, 33);
				        	fprintf(flogmain, "%d,", lstnumh);
				        }
				        fprintf(flogmain, "\nValue of k in readF1 is %d\n", k);
				        // assinging value of f1file.k = k
				        f1file[count].k = k;
	        		}
	        		free(listnums);
	        	}
	            free(*(tokens + i));
    		}
    		fprintf(flogmain, "%s\n", "");
		    free(tokens);
	    	fprintf(flogmain, "Line retrived is %s\n\n", line);
	    	count++;
    	}
    
	}
	fprintf(flogmain, "Value of count is %d\n", count);
	fclose(fp1);
    if (line)
        free(line);
}

void printF1()
{
	// printing the F1 structure array
	for (int i=0; i<n; i++)
	{
		fprintf(flogmain, "%s:", f1file[i].name);
		for (int j=0; j<f1file[i].k; j++)
		{
			fprintf(flogmain, "%d,", f1file[i].list[j]);
		}
		fprintf(flogmain, "\n");
		for (int j=0; j<f1file[i].k; j++)
		{
			fprintf(flogmain, "%s,", f2file[f1file[i].list[j]-1].hash);
		}
		fprintf(flogmain, "%s\n\n", "");
	}
}

void printF2()
{
	// printing the F2 structure array
	for (int i=0; i<n; i++)
	{
		fprintf(flogmain, "%d:%s\n", f2file[i].index, f2file[i].hash);
	}
}

void readF2()
{
	FILE *fp2 = fopen("f2.txt", "r");
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int count=1;
    while ((read = getline(&line, &len, fp2)) != -1) 
    {
    	// printf("Retrieved line of length %zu:\n", read);
    	f2file[count-1].index = count;
    	char temphash[33];
    	int intial = (int)read - 33;
    	strncpy(temphash, line+intial, 32);
    	temphash[33] = '\0';
    	strncpy(f2file[count-1].hash, temphash, 33);
    	fprintf(flogmain, "Line retrived is %s", line);
    	count++;
    }
    n = count-1;
    fclose(fp2);
    if (line)
        free(line);
}

int findusernum(char username[], int mode)
{
	int i;
	int flag=0;
	int usernum=-1;
	for (i=0; i<n ; i++)
	{
		if (strcmp(f1file[i].name, username)==0)
		{
			flag=1;
			usernum = i;
			break;
		}
	}
	if (flag == 0 && mode == 2)
	{
		printf("Entered user not registered\n");
		exit(0);
	}
	return usernum;
}

// A utility function to swap to integers
void swap (int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}
  
// A utility function to print an array
void printArray (int arr[], int n)
{
    for (int i = 0; i < n; i++)
        fprintf(flogmain, "%d ", arr[i]);
    fprintf(flogmain, "%s\n", "");
}
  
// A function to generate a random permutation of arr[]
void randomize ( int arr[], int n )
{
    // Use a different seed value so that we don't get same
    // result each time we run this program
    srand ( time(NULL) );
  
    // Start from the last element and swap one by one. We don't
    // need to run for the first element that's why i > 0
    for (int i = n-1; i > 0; i--)
    {
        // Pick a random index from 0 to i
        int j = rand() % (i+1);
  
        // Swap arr[i] with the element at random index
        swap(&arr[i], &arr[j]);
    }
}

void generateRandoms(int lower, int upper,	int count, int pivot, int randoms[])
{
	int i, j;
	//int limit = (upper - lower)+1;
	int visited[upper];
	for (i=lower; i<=upper;i++)
		visited[i]=0;
	j = 0;
	for (i = 0; i < count; i++) 
	{
		int num = (rand() %(upper - lower + 1)) + lower;
		if (pivot == num || visited[num] == 1)
		{
			i=i-1;
			continue;
		}
		visited[num] = 1;
		randoms[j++] = num;
	}
}

void update_entries(char username[], char hashpass[])
{
	int userfind = findusernum(username, 1);
	if (userfind != -1)
	{
		printf("Username already exists. Please enter new username again\n");
		return;
	}
	srand(time(0));
	fprintf(flogmain, "Number of current users are %d\n", u);
	fprintf(flogmain, "Number of current honeypots are %d\n", h);
	int randnum[1];
	generateRandoms(honeymin, honeymax, 1, MAX, randnum); //generating a random number for k
	int k = randnum[0] - 1;
	fprintf(flogmain, "Using number of honeywords as %d\n", k+1);
	//int k = K - 1; // total number of honeywords
	int usernum = k / 2;
	int potsnum = k - usernum;
	int userrandoms[usernum];
	int potsrandoms[potsnum];
	// generating user randoms
	generateRandoms(0, u-1,	usernum, n+1, userrandoms);
	// generating pots random
	generateRandoms(0, h-1,	potsnum, n+1, potsrandoms);
	// printing user randoms
	int i;
	fprintf(flogmain, "printing random users\n");
	for (i=0; i<usernum; i++)
		fprintf(flogmain, "%d ", users[userrandoms[i]]);
	// printing pots random
	fprintf(flogmain, "\nprinting random pots\n");
	for (i=0; i<potsnum; i++)
		fprintf(flogmain, "%d ", honeypots[potsrandoms[i]]);
	fprintf(flogmain, "\nThe correct index of the new user is %d\n", (n+1));
	// Constructing the sweet indices
	int sweetindices[k+1];
	sweetindices[0] = n+1;
	for(i=0;i<usernum;i++)
		sweetindices[i+1] = users[userrandoms[i]];
	for(i=0;i<potsnum;i++)
		sweetindices[i+1+usernum] = honeypots[potsrandoms[i]];
	fprintf(flogmain, "The final sweetindices array is\n");
	for(i=0;i<k+1;i++)
		fprintf(flogmain, "%d ", sweetindices[i]);
	fprintf(flogmain, "\nTrying to randomize the same\n");
	randomize (sweetindices, k+1);
    printArray(sweetindices, k+1);
	fprintf(flogmain, "%s\n", "");
	char snum[5];

	// converting integer to string
	sprintf(snum, "%d", (n+1));
	// print our string
	fprintf(flogmain, "%s is the string and the length is %d\n", snum, strlen(snum));
	// form string for adding to f2
	int num_len = strlen(snum);
	int f2len = num_len + 33;
	char *f2string = (char*)(malloc(f2len*sizeof(char)));
	for (i=0;i<num_len;i++)
		f2string[i] = snum[i];
	f2string[num_len] = ':';
	for (i=0;i<strlen(hashpass);i++)
		f2string[i+num_len+1] = hashpass[i];
	fprintf(flogmain, "The final f2string is %s\n", f2string);

	// Writing to users.txt
	FILE *userfile = fopen("honeyinfo/honeypots.txt", "a");
	fprintf(userfile, "%s\n", snum);
	fclose(userfile);
	// Writing to f2.txt
	FILE *f2 = fopen("f2.txt", "a");
	fprintf(f2, "%s\n", f2string);
	fclose(f2);

	// Constructing input for f1 file
	char f1string[100];
	int uname_len = strlen(username);
	for (i=0;i<uname_len;i++)
		f1string[i] = username[i];
	f1string[uname_len] = ':';
	int f1len = uname_len + 1;
	// adding sweet indices data
	for (i=0; i<k+1; i++)
	{
		int j;
		char chnum[10];
		// converting integer to string
		sprintf(chnum, "%d", sweetindices[i]);
		for (j = 0; j<strlen(chnum); j++)
			f1string[j+f1len] = chnum[j];
		f1len+=strlen(chnum);
		if (i != k)
		{
			f1string[f1len] = ',';
			f1len+=1;
		}

	}
	fprintf(flogmain, "%s is the f1string and the length is %d\n", f1string, strlen(f1string));

	// Trying to write somehwere in between
	FILE *f1 = fopen("f1.txt", "w");
	FILE *crridxfile = fopen("honeyinfo/correctindex.txt", "w");
	int randloc[1];
	generateRandoms(0, n-1,	1, n+1, randloc);
	fprintf(flogmain, "Value of randloc is %d\n", randloc[0]);
	// Print the set of f1strings
	fprintf(flogmain, "Printing the f1 strings\n");
	for (i=0;i<n;i++)
	{
		if (i == randloc[0])
			fprintf(f1, "%s\n", f1string);
		fprintf(f1, "%s", f1details[i]);
	}
	// Print the set of users
	fprintf(flogmain, "Printing the correctindex strings\n");
	for (i=0;i<n;i++)
	{
		if (i == randloc[0])
			fprintf(crridxfile, "%s\n", snum);
		fprintf(crridxfile, "%s", crrctkeydetails[i]);
	}
	// close the files
	fclose(f1);
	fclose(crridxfile);

	return;
}

void read_newusers()
{
	FILE *fp = fopen("upusers.txt", "r");
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int count=1;
    while ((read = getline(&line, &len, fp)) != -1) 
    {
    	// printf("Retrieved line of length %zu:\n", read);
    	line[strcspn(line, "\n")] = 0;
    	strcpy(userdetails[count-1], line);
    	//printf("Line retrived is %s\n", line);
    	count++;
    }
    nu = count - 1;
    fclose(fp);
    if (line)
        free(line);
    fprintf(flog, "Total %d Users succesfully read\n", u);
}

void read_passwds()
{
	FILE *fp = fopen("uppasswds.txt", "r");
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int count=1;
    while ((read = getline(&line, &len, fp)) != -1) 
    {
    	// printf("Retrieved line of length %zu:\n", read);
    	line[strcspn(line, "\n")] = 0;
    	strcpy(passwddetails[count-1], line);
    	//printf("Line retrived is %s\n", line);
    	count++;
    }
    p = count - 1;
    fclose(fp);
    if (line)
        free(line);
    fprintf(flog, "Total %d Users succesfully read\n", u);
}

void update_tables()
{
	int i;
	for (i=0; i<nu;i++)
	{
		char *output = str2md5(passwddetails[i], strlen(passwddetails[i]));
		printf("Username is %s\n", userdetails[i]);
		printf("Password is %s and hash is %s\n", passwddetails[i], output);
		update_entries(userdetails[i], output);
		readF2();
	    printF2();
	    // Read File F1
	    readF1();
	    printF1();
	    // Setup the honeychecker server
	    prepare_server();
		free(output);
	}
}


int main()
{
	// Opening the log file
	flogmain = fopen("addhoneylog.txt", "w");
	// Step1: Finding number of entries
	int count=0;
	char c;
	FILE *fp2 = fopen("f2.txt", "r");
    for (c = getc(fp2); c != EOF; c = getc(fp2))
    if (c == '\n') // Increment count if this character is newline
        count = count + 1;
    fclose(fp2);
    fprintf(flogmain, "The F2 file has %d lines\n ", count);
    n = count;
    // Read File F2
    readF2();
    printF2();
    // Read File F1
    readF1();
    printF1();
    // Setup the honeychecker server
    prepare_server();
    // Now take input from the files and add here
    read_newusers();
    read_passwds();
    update_tables();
	return 0;
}