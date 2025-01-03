#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h> //to use open system call
#include<stdlib.h>
#include<sys/stat.h>
#include<sys/types.h>
void check_format(int);
void compile_files(int);
int main(){
	int fd;//file descriptor
	if((fd=open("makefile",O_RDONLY))==-1){
		perror("Makefile not found");
		return 0;
	}

	check_format(fd);
	lseek(fd,0,SEEK_SET);
	
	compile_files(fd);


	

	
}
void check_format(int fd){
	char buff[512]={0};
	FILE *fp=fdopen(fd,"r");
	int l_c=0;
	char check_flag=0;//to check whether the line is a command or a target line
	while(fgets(buff,512,fp)){
		l_c++;	
		if(buff[0]=='\0')
			continue;//for empty lines
		if(buff[0]=='#')
			continue;//for comments

		if(buff[0]=='\t'){//for tab space
			if(!check_flag){
				printf("unexpected command at %d\n",l_c);
				fclose(fp);
				exit(1);
			}
		    check_flag=0;
		    continue;
		}

		if(strchr(buff,':')){
			
			*(strchr(buff,':'))='\0';//to split the target and to validate the target
			char *target=buff;
			
			while(*target && (*target)==' ')
				target++;
			
			if(*target=='\0'){
				printf("missing target on %d\n",l_c);
				fclose(fp);
				exit(1);
			}
		}

		check_flag=1;
		continue;
	    }
		
	
       if(check_flag){
	       printf("missing command in last line\n");
	       fclose(fp);
	       exit(1);
       }

}





void compile_files(int fd){
	FILE *fp=fdopen(fd,"r");
	char final_target[20]={0};
	char final_command[512]={0};
	char tmp_target[20]={0};
	char buff[512]={0};
	int l_c=0;
	char *tmp;
	int flag=0;
        struct stat file_info[2];
	while(fgets(buff,512,fp)!=NULL){
		l_c++;
		if(tmp=strchr(buff,':')){
			*tmp='\0';
			if(l_c==1){
			strncpy(final_target,buff,strlen(buff));
			continue;
			}
			strncpy(tmp_target,buff,strlen(buff));
			continue;

		}

		 if(buff[0]=='\0')
			 l_c--;
                        continue;//for empty lines
                if(buff[0]=='#')
			l_c--;
                        continue;//for comments
                      
	           if(buff[0]=='\t'){
		       if(l_c==2){
			strncpy(final_command,&buff[1],strlen(buff+1));
			
		       }

		       }
		
		stat(tmp_target,&file_info[0]);
		tmp_target[strlen(tmp_target)-1]='c';
		stat(tmp_target,&file_info[1]);

		if(file_info[0].st_mtime<file_info[1].st_mtime){
			system(&buff[1]);
			flag=1;
		}

	}
	if(flag==1){

		system(&final_command[1]);
	}

	printf("success\n");


}
