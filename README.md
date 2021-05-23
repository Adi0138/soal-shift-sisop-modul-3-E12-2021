# Laporan Praktikum Modul 3 Sistem Operasi

Kelompok E12 :
<li>05111840000138 - Gema Adi Perwira
<li>05111940000006 - Daffa Tristan Firdaus
<li>05111940000211 - VICKY THIRDIAN

## Soal 1
<br>Keverk adalah orang yang cukup ambisius dan terkenal di angkatannya. Sebelum dia menjadi ketua departemen di HMTC, dia pernah mengerjakan suatu proyek dimana keverk tersebut meminta untuk membuat server database buku. Proyek ini diminta agar dapat digunakan oleh pemilik aplikasi dan diharapkan bantuannya dari pengguna aplikasi ini. Di dalam proyek itu, Keverk diminta:
(Sebelumnya pengerjaan soal ini dilakukan menggunakan wsl)
<li>Secara garis besar, bagian ini diperintahkan untuk pada saat client sudah tersambung dengan server dapat melakukan 2 pilihan antara register dan login. Jika memilih login maka user akan mengisikan ID serta passwordnya yang akan dikirimkan ke server. Apabila memilih login maka user akan diminta untuk memasukkan ID dan passwordnya lalu server akan mengecek apakah akun tersebut yang berisikan ID dan password itu sudah ada atau belum di dalam server tersebut.
	
Pertama-tama kita harus menghubungkan koneksi socket terlebih dahulu dari client dan server dengan potongan code seperti ini:
## Server
```c
int createSocket() {
    struct sockaddr_in serv_addr;
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP), return_val;

    if (fd == -1) {
        printf("\n Socket creation failed. \n");
		return -1;
    }
    printf("Socket creation success with fd: %d\n", fd);

    serv_addr.sin_family = AF_INET;         
    serv_addr.sin_port = htons(8080);     
    serv_addr.sin_addr.s_addr = INADDR_ANY; 

	return_val = bind(fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
    if (return_val != 0) {
       	fprintf(stderr, "Binding failed [%s]\n", strerror(errno));
        close(fd); return -1;
    }
    
	return_val = listen(fd, 5);
    if (return_val != 0) {
        fprintf(stderr, "Listen failed [%s]\n", strerror(errno));
        close(fd); return -1;
    }
    return fd;
}
```
  Lalu untuk membuat direktori FILES, files.tsv, akun.txt, running.log. Maka program akan mengecek terlebih dahulu jika belum ada nantinya akan di create saat program dijalankan
```c
void checkFile() {
    if(access("akun.txt", F_OK)) {
		FILE *file = fopen("akun.txt", "w+");
		fclose(file);
	} 
    if(access("files.tsv", F_OK)) {
		FILE *file = fopen("files.tsv", "w+");
        fprintf(file, "Publisher\tTahun Publikasi\tFilepath\n");
		fclose(file);
	}
	if(access("running.log", F_OK )) {
		FILE *file = fopen("running.log", "w+");
		fclose(file);
	}

    struct stat s;
    stat("./FILES", &s);

    if(!S_ISDIR(s.st_mode)) mkdir("./FILES", 0777);
}
```
## Client
```c
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
```
Maka program akan berjalan seperti ini
	![image](https://user-images.githubusercontent.com/54606856/119263505-89f20800-bc09-11eb-8050-d57ef4c9ae98.png)
	
<li>Lalu dalam menambahkan fitur command baru seperti add, download, delete, see, find. Maka dibuat fungsi-fungsi tiap commandnya seperti ini
	
```c  
void addFeature(int client, char userdata[128]) {
    char fname[100], publisher[100], tahun[10], fp[100], message[100], fullpath[256], file_data[4096];
    int return_publisher, return_tahun, return_filepath, return_stat, return_receive;
    return_publisher = recv(client, publisher, sizeof(publisher), 0);
    return_tahun = recv(client, tahun, sizeof(tahun), 0);
    return_filepath = recv(client, fp, sizeof(fp), 0);

    get_file_name(fp, fname);
    sprintf(fullpath, "%s%s", S_PATH, fname);
    
    printf("%s, %s, %s\n", publisher, tahun, fullpath);

    FILE *files_tsv = fopen("files.tsv", "a");
    fclose(files_tsv);

    FILE *file = fopen(fullpath, "w+");
    while(1) {
        return_receive = recv(client, file_data, 4096, 0);
    	fflush(stdout);
        if(return_receive != -1){
            if(!strcmp(file_data, "OK")) break;
		}            
        fprintf(file, "%s", file_data);
        bzero(file_data, 4096);
    }
    fclose(file);

    FILE *log = fopen("running.log", "a");
    fprintf(log, "Tambah : %s (%s)\n", fname, userdata);
    fclose(log);
    fflush(stdout);
}
	void downloadFeature(int client) {
    char fpath[512];
    char fname[256];

    int return_fn = recv(client, fname, sizeof(fname), 0);

	sprintf(fpath, "%s%s", S_PATH, fname);
    printf("%s\n", fpath);

    if(find_file(fname)) {
        FILE *book = fopen(fpath, "r");
        char file_data[4096] = {0};

        while(fgets(file_data, 4096, book) != NULL) {
            if(send(client, file_data, sizeof(file_data), 0) != -1)  bzero(file_data, 4096);
        }
        fclose(book);
        printf("\e[32mFile sent successfully.\e[0m\n");
        send(client, "OK", 4096, 0);
    }
	else send(client, "404", 4096, 0);
}

void deleteFeature(int client, char userdata[128]) {
    char filename[128], new_path[256], old_path[256];
    int return_client= recv(client, filename, sizeof(filename), 0), isFound=0;
    
    find_in_tsv(&isFound, filename);
    if(isFound) {
        return_client = send(client, "OK", 100, 0);
        sprintf(new_path, "%sold-%s", S_PATH, filename);
        sprintf(old_path, "%s%s", S_PATH, filename);
        rename(old_path, new_path);

	    FILE *log = fopen("running.log", "a");
	    fprintf(log, "Hapus : %s (%s)\n", filename, userdata);
	    fclose(log);
    } else return_client = send(client, "404", 100, 0);
}

void seeFeature(int client) {
    FILE *tsv_file = fopen("files.tsv", "r");
    char file_data[1024], file[100], filename[64], publisher[64], tahun[64], ext[64], filepath[256],
		filename_send[1024], publisher_send[1024], tahun_send[1024], ext_send[1024], filepath_send[1024], *p;
    int i=0, ret_c;
    
    while(fgets(file_data, 1024, tsv_file) != NULL) {
        if(i != 0) {
            strcpy(publisher, strtok_r(file_data, "\t", &p));
            strcpy(tahun, strtok_r(NULL, "\t", &p));
            strcpy(filepath, strtok_r(NULL, "\t", &p));
            filepath[strlen(filepath)-1] = '\0';
            sprintf(filepath_send, "Filepath: %s\n\n", filepath);

            get_file_name(filepath, file);

            strcpy(filename, strtok_r(file, ".", &p));
            strcpy(ext, strtok_r(NULL, ".", &p));

            sprintf(filename_send, "Nama: %s\n", filename);
            sprintf(publisher_send, "Publisher : %s\n", publisher);
            sprintf(tahun_send, "Tahun publishing: %s\n", tahun);
            sprintf(ext_send, "Ekstensi File: %s\n", ext);
			
			send(client, "next", 1024, 0);
            send(client, filename_send, 1024, 0);
            send(client, publisher_send, 1024, 0);
            send(client, tahun_send, 1024, 0);
            send(client, ext_send, 1024, 0);
            send(client, filepath_send, 1024, 0);
            printf("%s\n%s\n%s\n%s\n%s\n", filename, publisher, tahun, ext, filepath);
            sleep(1);
        }
        i++;
        bzero(file_data, sizeof(file_data));
    }
    fclose(tsv_file);
	ret_c = send(client, "OK", 1024, 0);
	fflush(stdout);
}

void findFeature(int client) {
    FILE *tsv_file = fopen("files.tsv", "r");
    char query[1024], file_data[1024], file[100], filename[64], publisher[64], tahun[64], ext[64], filepath[256],
		filename_send[1024], publisher_send[1024], tahun_send[1024], ext_send[1024], filepath_send[1024], *p;
    int i=0, ret_c;
    
    ret_c = recv(client, query, 1024, 0);
    
    while(fgets(file_data, 1024, tsv_file) != NULL) {
        if(i != 0) {
            strcpy(publisher, strtok_r(file_data, "\t", &p));
            strcpy(tahun, strtok_r(NULL, "\t", &p));
            strcpy(filepath, strtok_r(NULL, "\t", &p));
            filepath[strlen(filepath)-1] = '\0';
            sprintf(filepath_send, "Filepath: %s\n\n", filepath);

            get_file_name(filepath, file);

            strcpy(filename, strtok_r(file, ".", &p));
            strcpy(ext, strtok_r(NULL, ".", &p));
            
			if(strstr(file, query)) {
	            sprintf(filename_send, "Nama: %s\n", filename);
	            sprintf(publisher_send, "Publisher : %s\n", publisher);
	            sprintf(tahun_send, "Tahun publishing: %s\n", tahun);
	            sprintf(ext_send, "Ekstensi File: %s\n", ext);
				
				send(client, "next", 1024, 0);
	            send(client, filename_send, 1024, 0);
	            send(client, publisher_send, 1024, 0);
	            send(client, tahun_send, 1024, 0);
	            send(client, ext_send, 1024, 0);
	            send(client, filepath_send, 1024, 0);
	            printf("%s\n%s\n%s\n%s\n%s\n", filename, publisher, tahun, ext, filepath);
	            sleep(1);
	    	}
        }
        i++;
        bzero(file_data, sizeof(file_data));
    }
    fclose(tsv_file);
	ret_c = send(client, "OK", 1024, 0);
	fflush(stdout);
}
```  
Jika command add digunakan maka program akan berjalan seperti ini
	![image](https://user-images.githubusercontent.com/54606856/119264355-063a1a80-bc0d-11eb-87ed-7f58e7d26ab1.png)

Jika command delete digunakan maka program akan berjalan seperti ini	
	![tes](https://user-images.githubusercontent.com/54606856/119264500-adb74d00-bc0d-11eb-990d-1024c7003c94.jpg)
	
Jika command see digunakan maka program akan berjalan seperti ini
	![see](https://user-images.githubusercontent.com/54606856/119264579-00910480-bc0e-11eb-9c6f-dc786f237925.jpg)

Jika command find digunakan maka program akan berjalan seperti ini
	![find](https://user-images.githubusercontent.com/54606856/119264676-59609d00-bc0e-11eb-8472-7f4045f2566f.jpg)

Namun pada program ini masih terdapat kesalahan pada lognya yang tidak dapat menampilkan user yang melakukan command
	
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
                                 



