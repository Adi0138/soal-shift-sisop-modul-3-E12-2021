#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h> 
#include<sys/types.h>
#include<sys/wait.h>
#include<dirent.h>
#define MAX 1000

char cwd[MAX];

int isFileExist(char *path) //berguna untuk mengecek apakah path yang sedang kita tuju file/bukan
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

char *get_filename_ext(char *filename) { //berguna untuk mengambil suatu ekstensi dari suatu file tanpa menyertakan '.'
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

void* playandcount(void *arg){ 

    char *fullname = (void *) arg;
    //cek exist ga
    int file_exist = isFileExist(fullname);
    char source[MAX], dest[MAX],path[MAX];
    if(file_exist){
        //cari extension
        char* ext = get_filename_ext(fullname);
        //jadi huruf kecil
        char *abspath = (void *) arg;
        if(strcmp(ext,"")!= 0){
            strcpy(source,abspath);
            strcpy(dest, cwd);
            strcat(dest, "/");
            char* file_path= strrchr(fullname, '/');
            strcpy(path,file_path);
            for(int i = 0; ext[i];i++){
                ext[i]= tolower(ext[i]);
            }
            mkdir(ext, 0777);
            strcat(dest, (ext));
            strcat(dest,path);
            rename(source,dest);
        }
        else{ 
            printf("2");
            mkdir("Unknown",0777);
            strcpy(dest, cwd);
            strcat(dest, "/");
            strcat(dest,"Unknown"); 
            char* file_path= strrchr(fullname, '/');
            strcat(dest,file_path);
            rename(abspath,dest);
        }
    }
    
    pthread_exit(0);
}

int main(int argc, char** argv) { 
    //getcwd(cwd,sizeof(cwd)); digunakan untuk mendapatkan current working directory pthread_t tid[MAX]; inisialisasi awal thread
    getcwd(cwd,sizeof(cwd));
    pthread_t tid[MAX];
    int i = 0; 
    int test;   
    if(strcmp(argv[1], "-f") == 0){
        for(test = 2; test < argc; test++){ //Cek apakah argumen yang diberikan adalah -f jika true maka lanjut dimana user bisa menambahkan argumen file yang ingin dikategorikan sesuka hati user.
            pthread_create(&(tid[i]),NULL,playandcount,argv[test]);
            pthread_join(tid[i],NULL);
            i++;
        }
    }
    /*Fungsi opendir untuk menjadikan direktori file dijalankan sebagai pusat, 
    lalu readdir untuk membuka direktori sekaligus mengecek apakah direktori kosong atau tidak. 
    Nantinya akan dibuat thread yang meng-passing filePath yang merupakan path dari file c ini dijalankan. 
    disini filePath hanya akan mengecek file, karena sudah diperiksa oleh fungsi isFileExist.*/
    else if(strcmp(argv[1],"*") == 0){
        DIR *d;
        struct dirent *dir;
        d = opendir(".");
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                char filePath[1000];
                if(isFileExist(dir->d_name)){
                    strcpy(filePath, cwd);
                    strcat(filePath, "/");
                    strcat(filePath, dir->d_name);
                    printf("%s\n",filePath);
                    pthread_create(&(tid[i]),NULL,playandcount,filePath); 
                    pthread_join(tid[i],NULL);
                    i++;
                }
            }
            closedir(d);
        }
    }

    /*mengecek dengan opendir apakah direktori yang user masukkan ada/exist
    Ketika direktori berhasil dibuka, akan melakukan iterasi pada seluruh file ataupun folder dalam suatu direktori.
    Lalu direktori exist, maka akan membuat thread yang mempassing path dari suatu file. 
    Disini yang akan diperiksa cuma file saja sebab telah diperiksa oleh fungsi isFileExist.
    */
    else if(strcmp(argv[1],"-d") == 0){
        DIR *d;
        struct dirent *dir;
        d = opendir(argv[2]);
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                char filePath[1000];
                strcpy(filePath,argv[2]);
                strcat(filePath, "/");
                strcat(filePath,dir->d_name);
                if(isFileExist(filePath)){
                    pthread_create(&(tid[i]),NULL,playandcount,filePath); 
                    pthread_join(tid[i],NULL);
                    i++;
                    printf("Direktori sukses disimpan!\n");
                }
            }
            closedir(d);
        }
        else printf("Yah, gagal disimpan :(\n");
    }
}