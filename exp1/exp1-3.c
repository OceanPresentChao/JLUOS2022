#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>
#include<sys/file.h>
int main(){
	pid_t p1,p2;
	int i;
	int max = 100;
	int fd = open("./lock.txt",O_RDWR);
	while((p1 = fork()) < 0);
	if(p1 == 0){
		while(lockf(fd,F_TEST,0) == -1);
		lockf(fd,F_LOCK,0);
		for(i = 0;i < max; i++){
			printf("daughter %d\n",i);
		}
		lockf(fd,F_ULOCK,0);
	}else {
		while((p2 = fork()) < 0);
		if(p2 == 0){
			while(lockf(fd,F_TEST,0) == -1);
			lockf(fd,F_LOCK,0);
			for(i = 0; i < max; i++){
						
				printf("son %d\n",i);
			}
		lockf(fd,F_ULOCK,0);
		}else{
			while(lockf(fd,F_TEST,0) == -1);
			lockf(fd,F_LOCK,0);
			for(i = 0; i < max; i++){
						
				printf("parent %d\n",i);
			}
			lockf(fd,F_ULOCK,0);
		}
	}
}
