#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>
int main(){
	pid_t p1,p2;
	int i;
	int max = 100;
	while((p1 = fork()) < 0);
	if(p1 == 0){
		for(i = 0;i < max; i++){
			printf("daughter %d\n",i);
		}
	}else {
		while((p2 = fork()) < 0);
		if(p2 == 0){
			for(i = 0; i < max; i++){
						
				printf("son %d\n",i);
			}
		}else{
			for(i = 0; i < max; i++){
						
				printf("parent %d\n",i);
			}
		}
	}
}
