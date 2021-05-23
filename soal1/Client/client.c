#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h> 
#include <unistd.h>
#include <netdb.h> 
#include <ctype.h>

int account_check(int fd, char command[]){
    int return_val;
	char id[100], password[100], pesan[100];
	
    printf("Enter ID: "); scanf("%s", id);
    return_val = send(fd, id, sizeof(id), 0);
    printf("Enter Password: "); scanf("%s", password);
    return_val = send(fd, password, sizeof(password), 0);
    return_val = recv(fd, pesan, 100, 0);

    if(!strcmp(pesan, "account_get")) return 1;
    else if(!strcmp(pesan, "user_exists")) {
    	printf("\e[31mID already exists.\e[0m\n");
        return 0;
    }
    else if(!strcmp(pesan, "wrong_account")) {
    	printf("\e[31mID or password is incorrect.\e[0m\n");
        return 0;
    }
}

void addBook(int fd){
    char publisher[100], tahun[10], filepath[128];
    
    printf("Publisher: "); fgets(publisher, sizeof(publisher), stdin);
    publisher[strcspn(publisher, "\n")] = 0;
    
    printf("Tahun Publikasi: "); fgets(tahun, sizeof(tahun), stdin);
    tahun[strcspn(tahun, "\n")] = 0;
    
    printf("Filepath: "); fgets(filepath, sizeof(filepath), stdin);
    filepath[strcspn(filepath, "\n")] = 0;

    int return_val;
    return_val = send(fd, publisher, sizeof(publisher), 0);
    return_val = send(fd, tahun, sizeof(tahun), 0);
    return_val = send(fd, filepath, sizeof(filepath), 0);

    FILE *file = fopen(filepath, "r");
    char file_data[4096] = {0};

    while(fgets(file_data, 4096, file)) {
        if(send(fd, file_data, sizeof(file_data), 0) != -1) bzero(file_data, 4096);
    }
    
    fclose(file);
    printf("\e[32mSuccessfully added file.\e[0m\n");
    send(fd, "OK", 4096, 0);
}

void downloadBook(int fd) {
    int return_val, return_rec;
    char filename[100], filePath[500]={0}, file_data[4096];
    printf("\e[0mInput file name\n> \e[36m");
    fgets(filename, sizeof(filename), stdin);
    printf("\e[0m");
    
    filename[strcspn(filename, "\n")] = 0;
    return_val = send(fd, filename, sizeof(filename), 0);

    sprintf(filePath, "%s%s", "/home/vicky/praktikum/modul3/soal1/Client/", filename);
    while(1) {
        if(recv(fd, file_data, sizeof(file_data), 0) != -1) {
            if(!strcmp(file_data, "404")) {
                printf("\e[31mFile not found.\e[0m\n");
                return;
            }
            if(!strcmp(file_data, "OK")) {
                printf("\e[32mSuccessfully downloaded file.\e[0m\n");
                return;
            }
            
            FILE *file = fopen(filePath, "a");
            fprintf(file, "%s", file_data);
            bzero(file_data, 4096);
            fclose(file);
        }
    }
}

void deleteBook(int fd) {
    int return_value;
    char filename[100], pesan[100];

	printf("\e[0mInput file name\n> \e[36m");
    fgets(filename, sizeof(filename), stdin);
    printf("\e[0m");
    
    filename[strcspn(filename, "\n")] = 0;

    return_value = send(fd, filename, sizeof(filename), 0);
    return_value = recv(fd, pesan, 100, 0);
    
    if(!strcmp(pesan, "OK")) printf("\e[32mSuccessfully deleted file.\e[0m\n");
    if(!strcmp(pesan, "404")) printf("\e[31mDeletion error, file not found.\e[0m\n");
}

void seeBook(int fd) {
    int return_value;
    char filename[1024], publisher[1024], tahun[1024], ext[1024], filepath[1024], pesan[1024];

    while(1){
    	if(recv(fd, pesan, sizeof(pesan), 0) != -1){
    		if(!strcmp(pesan, "OK")) break;
		}
        return_value = recv(fd, filename, 1024, 0);
        printf("%s", filename);
        return_value = recv(fd, publisher, 1024, 0);
        printf("%s", publisher);
        return_value = recv(fd, tahun, 1024, 0);
        printf("%s", tahun);
        return_value = recv(fd, ext, 1024, 0);
        printf("%s", ext);
        return_value = recv(fd, filepath, 1024, 0);
        printf("%s", filepath);
    }
}

void findBook(int fd) {
    int return_value;
    char query[1024], filename[1024], publisher[1024], tahun[1024], ext[1024], filepath[1024], pesan[1024];
	
	printf("\e[0mInput search query\n> \e[36m");
    fgets(query, sizeof(query), stdin);
    printf("\e[0m");
    
    query[strcspn(query, "\n")] = 0;
    return_value = send(fd, query, sizeof(query), 0);
    
    while(1){
    	if(recv(fd, pesan, sizeof(pesan), 0) != -1){
    		if(!strcmp(pesan, "OK")) break;
		}
        return_value = recv(fd, filename, 1024, 0);
        printf("%s", filename);
        return_value = recv(fd, publisher, 1024, 0);
        printf("%s", publisher);
        return_value = recv(fd, tahun, 1024, 0);
        printf("%s", tahun);
        return_value = recv(fd, ext, 1024, 0);
        printf("%s", ext);
        return_value = recv(fd, filepath, 1024, 0);
        printf("%s", filepath);
    }
}

int main(){
    struct hostent *lh;
	struct sockaddr_in server_addr;
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP), return_val;
    char message[100], command[100], buffer[1000]={0};
    
    if (fd == -1) {
		printf("\n Socket creation failed. \n");
		return -1;
	}
    
	server_addr.sin_family = AF_INET;         
    server_addr.sin_port = htons(8080);
    lh = gethostbyname("127.0.0.1");
    server_addr.sin_addr = *((struct in_addr *)lh->h_addr);
    
    return_val = connect(fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
    if (return_val == -1) {
        fprintf(stderr, "Connect failed [%s]\n", hstrerror(errno));
        close(fd);
        return -1;
    }
    
    return_val = recv(fd, message, 100, 0);
    while(!strcmp(message, "wait")) {
        printf("\e[31mServer is full, please wait.\e[0m\n");
        return_val = recv(fd, message, 100, 0);
    }
    printf("\e[33mServer is now listening to your commands.\e[0m\n");
    
    int login=0, i;
    while(1){
    	while(!login) {
            printf("\e[32mInsert your option: login/register\n>\e[0m ");
            scanf("%s", command); getchar();
            
            for(i=0; i<strlen(command); i++) command[i] = tolower(command[i]);
            
            return_val = send(fd, command, sizeof(command), 0);
            if(!strcmp(command, "login") || !strcmp(command, "register")){
            	if(account_check(fd, command)) {
                	login = 1;
                	break;
				}
			} else {
                return_val = recv(fd, message, 100, 0);
                if(!strcmp(message, "not_logged_in\n")) printf("\e[31mPlease login/register first.\e[0m\n");
                else login = 1;
            }
        }
        while(1){
            printf("\e[32mPlease input the operation you would like to do: add/download/delete/see/find\n>\e[0m ");
            scanf("%s", command); getchar();
            
            for(i=0; i<strlen(command); i++) command[i] = tolower(command[i]);
            
            return_val = send(fd, command, sizeof(command), 0);
            if(!strcmp(command, "login") || !strcmp(command, "register"))  printf("\e[32mYou are already logged in.\e[0m\n");
            if(!strcmp(command, "add")) addBook(fd);
            if(!strcmp(command, "download")) downloadBook(fd);
            if(!strcmp(command, "delete")) deleteBook(fd);
            if(!strcmp(command, "see")) seeBook(fd);
            if(!strcmp(command, "find")) findBook(fd);
        }

        sleep(2);
        if(login) break;
	}
	
	printf("\e[31mClosing connection to the server...\e[0m\n\n");
    close(fd);
    return 0;
}
