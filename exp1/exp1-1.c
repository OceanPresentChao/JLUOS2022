#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
int main(){
	pid_t p1,p2;
	printf("current process id = %d \n",getpid());
	while((p1 = fork()) < 0);
	if(p1 == 0){
		printf("current p1 process id = %d \n",getpid());
		putchar('b');
		putchar('\n');
	}else {
		while((p2 = fork()) < 0);
		if(p2 == 0){
			printf("current p2 process id = %d \n",getpid());
			putchar('c');
			putchar('\n');
		}else {
			putchar('a');
			putchar('\n');
		}
	}
}
