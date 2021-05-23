# Laporan Praktikum Modul 3 Sistem Operasi

Kelompok E12 :
<li>05111840000138 - Gema Adi Perwira
<li>05111940000006 - Daffa Tristan Firdaus
<li>05111940000211 - VICKY THIRDIAN


	
## Soal 2
<br>Crypto (kamu) adalah teman Loba. Suatu pagi, Crypto melihat Loba yang sedang kewalahan mengerjakan tugas dari bosnya. Karena Crypto adalah orang yang sangat menyukai tantangan, dia ingin membantu Loba mengerjakan tugasnya. Detil dari tugas tersebut adalah:
<br>a.  Membuat program perkalian matrix (4x3 dengan 3x6) dan menampilkan hasilnya. Matriks nantinya akan berisi angka 1-20 (tidak perlu dibuat filter angka).
<br>b.  Membuat program dengan menggunakan matriks output dari program sebelumnya (program soal2a.c) (Catatan!: gunakan shared memory). Kemudian matriks tersebut akan dilakukan perhitungan dengan matrix baru (input user) sebagai berikut contoh perhitungan untuk matriks yang a	da. Perhitungannya adalah setiap cel yang berasal dari matriks A menjadi angka untuk faktorial, lalu cel dari matriks B menjadi batas maksimal faktorialnya matri(dari paling besar ke paling kecil) (Catatan!: gunakan thread untuk perhitungan di setiap cel). Ketentuan:
	
```
If a >= b  -> a!/(a-b)!
If b > a -> a!
If 0 -> 0
```
<br>c.  Karena takut lag dalam pengerjaannya membantu Loba, Crypto juga membuat program (soal2c.c) untuk mengecek 5 proses teratas apa saja yang memakan resource komputernya dengan command “ps aux | sort -nrk 3,3 | head -5” (Catatan!: Harus menggunakan IPC Pipes)

## Jawaban Soal 2
<br>a. Pada bagian ini kita ditugaskan untuk membuat program yang dapat menghitung perkalian antara matrix 4x3 dan 3x6. Dengan begitu, akan menghasilkan matrix yang berukuran 4x6. Tetapi, pengerjaan soal bagian ini akan dikaitkan dengan soal nomor 2 bagian b sehingga kita akan menggunakan shared memory yang dapat memungkinkan kita menggunakan hasil program ini untuk dipakai pada program lain. Sehingga fungsi main menjadi seperti berikut:

```c
int main(void) {

    	key_t key = 12345;

    	int shmid = shmget(key, sizeof(int[4][6]), IPC_CREAT | 0666);
    	value = shmat(shmid, NULL, 0);

	printf("Enter Matrix 4x3 :\n");
    	inputMatriksA();
	printf("Enter Matrix 3x6 :\n");
    	inputMatriksB();

    	pthread_t tid[4];

    	for(int i=0; i<4; i++) {
        	pthread_create(&tid[i], NULL, multiplyMatrices, NULL);
    	}
    	for(int i=0; i<4; i++) {
        	pthread_join(tid[i], NULL);
    	}

    	prtScreen();

    	shmdt(value);
}
```	
<br>Lalu, dengan melakukan perkalian matrix seperti biasa akan menghasilkan program yang jika dijalankan akan seperti berikut:
	
```
Enter Matrix 4x3 :
4 1 4
2 1 3
4 2 2
1 1 4
Enter Matrix 3x6 :
2 1 3 2 0 3
1 4 4 0 0 2
1 1 0 1 2 1

Multiplication Result Matrix 4x6:
13    12    16    12    8    18    
8    9    10    7    6    11    
12    14    20    10    4    18    
7    9    7    6    8    9

```	
<br>b. Selanjutnya, dengan menggunakan hasil dari program sebelumnya kita ditugaskan untuk menghitung perkalian faktorial tiap cell di matrix. Setelah memasukkan matrix B, kita harus memperhatikan syarat yang telah dikasih pada soal. Lalu, kita akan membandingkan setiap cell pada matrix A dan B sesuai syaratnya. Terakhir, kita menghasilkan matrix dari perkalian faktorial matrix A dan B.
<br>Program akan menjadi berikut :
	
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define x 4
#define z 6

pthread_t tid[x*z];
pthread_attr_t attr;
int iret[x*z];
key_t key = 12345;
int *matrix;
int cnt=0;
int matrixA[x][z];
int matrixB[x][z];
int total=x*z;

void *faktorial(void *arguments);

struct arg_struct{
    	int arg1;
    	int arg2;
};

void assign_matrixA(){
    	cnt=0;
    	for(int i=0; i<x; i++){
        	for(int j=0; j<z; j++){
            		matrixA[i][j] = matrix[cnt];
            		cnt++;
        	}
    	}
}

void call_thread(){
    	for(int i=1; i<x+1; i++){
        	for(int j=1; j<z+1; j++){
            		struct arg_struct *args = (struct arg_struct *) malloc(sizeof(struct arg_struct));
            		args->arg1 = i-1;
            		args->arg2 = j-1;
            		pthread_attr_init(&attr);
            		iret[cnt] = pthread_create(&tid[cnt], &attr, faktorial, args);
            		if(iret[cnt]){
                		fprintf(stderr,"Error - pthread_create() return code: %d\n", iret[cnt]);
                		exit(EXIT_FAILURE);
            		}
            		pthread_join(tid[cnt], NULL);
            		cnt++;
        	}
        	printf("\n");
    	}

    	for(int i=0; i<total; i++){
        	pthread_join(tid[i], NULL);
    	}
}

int main(void){
    	int shmid = shmget(key, sizeof(matrix), IPC_CREAT | 0666);
    	matrix = shmat(shmid, 0, 0);

    	assign_matrixA();
    	cnt=0;
    	//input matrixB
	    printf("Enter Matrix B 4x6 :\n");
    	for (int i = 0; i < x; i++) {
      		for (int j = 0; j < z; j++) {
         		scanf("%d", &matrixB[i][j]);
      		}
   	  }  
	    printf("Matrix Result 4x6 :\n");
    	call_thread();

    	shmdt(matrix);
    	shmctl(shmid, IPC_RMID, NULL);

    	return 0;
}

void *faktorial(void *arguments){
        struct arg_struct *args = arguments;

        int baris=args->arg1;
        int kolom=args->arg2;
        long long int hasil=1;

        if (matrixA[baris][kolom] == 0 || matrixB[baris][kolom] == 0){
                printf("0");
        }
        else if(matrixA[baris][kolom]>matrixB[baris][kolom]){
        //a!/(a-b)!
                int batas = matrixA[baris][kolom] - matrixB[baris][kolom];
                for (int i = matrixA[baris][kolom]; i > batas; i--){
                        hasil = hasil*i;
                }
                printf("%lld", hasil);

        }
        else if(matrixB[baris][kolom]>matrixA[baris][kolom]){
        //a!
                for (int i = matrixA[baris][kolom]; i > 0; i--){
                        hasil = hasil * matrixA[baris][kolom];
                        matrixA[baris][kolom]--;
                }
                printf("%lld", hasil);
        }
        printf("\t\t");
        pthread_exit(0);
}
```
<br>Hasil Program :
	
```	
Enter Matrix B 4x6 :
14 2 3 8 8 10
7 4 8 5 14 9
9 2 13 5 11 2
8 7 10 4 10 8
Matrix Result 4x6 :
6227020800   	 132   	 3360   	 19958400   		 158789030400   	 
40320   	 3024   	 1814400   	 2520   	 720   	 19958400   	 
79833600   	 182   	 482718652416000   	 30240   	 24306   	 
5040   	 181440   	 5040   	 360   	 40320   	 362880	  
```
<br>c. Untuk bagian c, untuk menghindar terjadinya lag kita ditugaskan untuk mengecek 5 proses teratas yang memakan resource komputernya dengan command ```ps aux | sort -nrk 3,3 | head -5```. Dengan menggunakan IPC Pipes program akan menjadi berikut :

```c	
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<stdlib.h>

int main() {
    	int pipe1[2], pipe2[2];
    	pid_t pid;

    	if(pipe(pipe1) == -1) {
        	fprintf(stderr, "Pipe failed");
        	return 1;
    	}
    	if(pipe(pipe2) == -1) {
        	fprintf(stderr, "Pipe failed");
        	return 1;
    	}

    	pid = fork();
    	if(pid < 0) {
        	fprintf(stderr, "fork failed");
        	return 1;
    	} 
	    else if(pid == 0) {
        	dup2(pipe1[1], 1);

        	close(pipe1[0]);
        	close(pipe1[1]);

        	char *argv[] = {"ps", "aux", NULL};
        	execv("/usr/bin/ps", argv);

        	perror("Bad exec ps");
        	_exit(1);
    	} 
	    else {
        	pid = fork();
        	if(pid < 0) {
            		fprintf(stderr, "fork failed");
            		return 1;
        	} 
		      else if(pid == 0) {
			          dup2(pipe1[0], 0);

            		dup2(pipe2[1], 1);

            		close(pipe1[0]);
            		close(pipe1[1]);
            		close(pipe2[0]);
            		close(pipe2[1]);

            		char *argv[] = {"sort", "-nrk", "3,3", NULL};
            		execv("/usr/bin/sort", argv);

            		perror("Bad exec sort");
            		_exit(1);
        	}		 
		      else { 
            		close(pipe1[0]);
            		close(pipe1[1]);

            		dup2(pipe2[0], 0);

            		close(pipe2[0]);
            		close(pipe2[1]);

            		char *argv[] = {"head", "-5", NULL};
            		execv("/usr/bin/head", argv);

            		perror("Bad exec head");
            		_exit(1);
        	}
    	}
}  
```
<br>Hasil Program :
	
```	
tristan 	2244  1.9 15.1 2818540 307560 ?  	Sl   20:47   0:26 /usr/lib/firefox/firefox -contentproc -childID 5 -isForBrowser -prefsLen 7375 -prefMapSize 233599 -parentBuildID 20210318103112 -appdir /usr/lib/firefox/browser 1658 true tab
tristan 	1658  1.3 14.0 3296812 285216 ?  	Sl   19:54   1:02 /usr/lib/firefox/firefox -new-window
tristan 	1061  0.9 16.7 3442508 340260 ?  	Rsl  19:40   0:49 /usr/bin/gnome-shell
tristan 	1828  0.8  8.6 2592052 176868 ?  	Sl   19:54   0:38 /usr/lib/firefox/firefox -contentproc -childID 4 -isForBrowser -prefsLen 6249 -prefMapSize 233599 -parentBuildID 20210318103112 -appdir /usr/lib/firefox/browser 1658 true tab
tristan 	1774  0.7 16.4 2986028 335088 ?  	Sl   19:54   0:35 /usr/lib/firefox/firefox -contentproc -childID 2 -isForBrowser -prefsLen 177 -prefMapSize 233599 -parentBuildID 20210318103112 -appdir /usr/lib/firefox/browser 1658 true tab
```
## Soal 3
<br>Seorang mahasiswa bernama Alex sedang mengalami masa gabut. Di saat masa gabutnya, ia memikirkan untuk merapikan sejumlah file yang ada di laptopnya. Karena jumlah filenya terlalu banyak, Alex meminta saran ke Ayub. Ayub menyarankan untuk membuat sebuah program C agar file-file dapat dikategorikan. Program ini akan memindahkan file sesuai ekstensinya ke dalam folder sesuai ekstensinya yang folder hasilnya terdapat di working directory ketika program kategori tersebut dijalankan.
<li>a. Program menerima opsi -f seperti contoh di atas, jadi pengguna bisa menambahkan argumen file yang bisa dikategorikan sebanyak yang diinginkan oleh pengguna. 
Output yang dikeluarkan adalah seperti ini :
  
```
File 1 : Berhasil Dikategorikan (jika berhasil)
File 2 : Sad, gagal :( (jika gagal)
File 3 : Berhasil Dikategorikan
```
<li>b. Program juga dapat menerima opsi -d untuk melakukan pengkategorian pada suatu directory. Namun pada opsi -d ini, user hanya bisa memasukkan input 1 directory saja, tidak seperti file yang bebas menginput file sebanyak mungkin. Contohnya adalah seperti ini:
  
```    
$ ./soal3 -d /path/to/directory/
```
  
Perintah di atas akan mengkategorikan file di /path/to/directory, lalu hasilnya akan disimpan di working directory dimana program C tersebut berjalan (hasil kategori filenya bukan di /path/to/directory).
Output yang dikeluarkan adalah seperti ini :
```
Jika berhasil, print “Direktori sukses disimpan!”
Jika gagal, print “Yah, gagal disimpan :(“
```
<li>c. Selain menerima opsi-opsi di atas, program ini menerima opsi *, contohnya ada di bawah ini:

  ```
$ ./soal3 \*
```
Opsi ini akan mengkategorikan seluruh file yang ada di working directory ketika menjalankan program C tersebut.
<li>d. Semua file harus berada di dalam folder, jika terdapat file yang tidak memiliki ekstensi, file disimpan dalam folder “Unknown”. Jika file hidden, masuk folder “Hidden”.
<li>e. Setiap 1 file yang dikategorikan dioperasikan oleh 1 thread agar bisa berjalan secara paralel sehingga proses kategori bisa berjalan lebih cepat.

## Jawaban Soal 3
```c
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
  ```
## Pembahasan
```c
  int isFileExist(char *path) 
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
```
  Berguna untuk mengecek apakah path yang sedang kita tuju file atau bukan.
  
```c
  char *get_filename_ext(char *filename) { //berguna untuk mengambil suatu ekstensi dari suatu file tanpa menyertakan '.'
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}
```
  Berguna untuk mengecek dan mengambil eksistensi dari suatu file. Eksistensi tersebut terletak setelah tanda '.' pada suatu file.
```c
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
```
 Fungsi ini berguna untuk melakukan inti dari yang diminta soal. Pertama akan mengecek apakah path yang dipassing adalah file atau tidak. Jika iya, maka akan mengambil ekstensi dari file tersebut dan mengubah ekstensi tersebut menjadi huruf kecil semua lalu membuat foldernya. Terdapat dua tipe file, yaitu yang mempunyai ekstensi dan yang tidak mempunyai. Jika get_filename_ext() dijalankan pada file yang mempunyai ekstensi akan mengembalikan string/char yang berupa ekstensi itu sendiri yang nantinya akan di compare untuk mengambil lokasi file yang ingin dipindahkan sebagai variable source dan loksai file ini dijalankan yang ditambahkan dengan ekstensinya lalu nantinya dipindahkan dengan fungsi rename(). Jika suatu file tidak mempunyai ekstensi yang berarti get_filename_ext() mengembalikan "", nantinya akan dibuat folder bernama Unknown pada variable dest yang telah ditambah dengan lokasi file ini dijalankan. Lalu diambil juga lokasi file yang ingin dipindahkan ke variable abspath, dan akan dipindahkan file tersebut menggunakan fungsi rename().

```c
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
```
Potongan kode diatas adalah fungsi -f.
                                  
```c
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
```
Pada potongan kode berikut ini berguna untuk mengecek jika mode yang dimasukkan user adalah "*" yang berarti bahwa user menginginkan untuk mengkategorikan file pada lokasi dimana file c ini berada. Fungsi opendir untuk menjadikan direktori file dijalankan sebagai pusat, lalu readdir untuk membuka direktori sekaligus mengecek apakah direktori kosong atau tidak. Nantinya akan dibuat thread yang meng-passing filePath yang merupakan path dari file c ini dijalankan. disini filePath hanya akan mengecek file, karena sudah diperiksa oleh fungsi isFileExist.
  
```c  
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
```  
Potongan kode ini berguna untuk memeriksa apakah mode yang dimasukkan user adalah "-d". Jika iya, maka akan mengecek dengan opendir apakah direktori yang user masukkan ada/exist. Ketika direktori berhasil dibuka, akan melakukan iterasi pada seluruh file ataupun folder dalam suatu direktori. Lalu direktori exist, maka akan membuat thread yang mempassing path dari suatu file. Disini yang akan diperiksa cuma file saja sebab telah diperiksa oleh fungsi isFileExist.  
                                 



