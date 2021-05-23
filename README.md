# Laporan Praktikum Modul 3 Sistem Operasi

Kelompok E12 :
<li>05111840000138 - Gema Adi Perwira
<li>05111940000006 - Daffa Tristan Firdaus
<li>05111940000211 - VICKY THIRDIAN

## Soal 1
<br>Keverk adalah orang yang cukup ambisius dan terkenal di angkatannya. Sebelum dia menjadi ketua departemen di HMTC, dia pernah mengerjakan suatu proyek dimana keverk tersebut meminta untuk membuat server database buku. Proyek ini diminta agar dapat digunakan oleh pemilik aplikasi dan diharapkan bantuannya dari pengguna aplikasi ini. Di dalam proyek itu, Keverk diminta:
<li>a. Secara garis besar, bagian ini diperintahkan untuk pada saat client sudah tersambung dengan server dapat melakukan 2 pilihan antara register dan login. Jika memilih login maka user akan mengisikan ID serta passwordnya yang akan dikirimkan ke server. Apabila memilih login maka user akan diminta untuk memasukkan ID dan passwordnya lalu server akan mengecek apakah akun tersebut yang berisikan ID dan password itu sudah ada atau belum di dalam server tersebut. Semua akun yang terdaftar akan tersimpan di akun.txt dengan format : 
```id:password```
Pertama-tama kita harus menghubungkan koneksi socket terlebih dahulu dari client dan server dengan potongan code seperti ini:
##Server
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
##Server
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
##Client
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
	
## Soal 2
<br>Crypto (kamu) adalah teman Loba. Suatu pagi, Crypto melihat Loba yang sedang kewalahan mengerjakan tugas dari bosnya. Karena Crypto adalah orang yang sangat menyukai tantangan, dia ingin membantu Loba mengerjakan tugasnya. Detil dari tugas tersebut adalah:
<li>a.  Membuat program perkalian matrix (4x3 dengan 3x6) dan menampilkan hasilnya. Matriks nantinya akan berisi angka 1-20 (tidak perlu dibuat filter angka).
<li>b.  Membuat program dengan menggunakan matriks output dari program sebelumnya (program soal2a.c) (Catatan!: gunakan shared memory). Kemudian matriks tersebut akan dilakukan perhitungan dengan matrix baru (input user) sebagai berikut contoh perhitungan untuk matriks yang a	da. Perhitungannya adalah setiap cel yang berasal dari matriks A menjadi angka untuk faktorial, lalu cel dari matriks B menjadi batas maksimal faktorialnya matri(dari paling besar ke paling kecil) (Catatan!: gunakan thread untuk perhitungan di setiap cel). 
<br>Ketentuan:
	
```
If a >= b  -> a!/(a-b)!
If b > a -> a!
If 0 -> 0
```
<li>c.  Karena takut lag dalam pengerjaannya membantu Loba, Crypto juga membuat program (soal2c.c) untuk mengecek 5 proses teratas apa saja yang memakan resource komputernya dengan command “ps aux | sort -nrk 3,3 | head -5” (Catatan!: Harus menggunakan IPC Pipes)

## Jawaban Soal 2

  
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
                                 



