#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<sys/syscall.h>
#include<linux/kernel.h>
int main()
{
	long int a;
		
	printf("this is testwr execution\n");
	
	char res[120];
	char string[120];
	char path[120];
	scanf("%s",string);
	scanf("%s",path);
	int length=strlen(string)+1;
	int plength=strlen(path)+1;
	

	a = syscall(444,length,string,plength,path);
	
	printf("a=%li\n",a);

	printf("testwr end\n");
	
	printf("this is testwr execution\n");
	a = syscall(442,plength,path);
	printf("a=%li\n",a);

 	a = syscall(443,res,plength,path);
 	
 	printf("%s\n",res);

	printf("a=%li\n",a);

	printf("testwr end\n");
	return 0;
	
}

