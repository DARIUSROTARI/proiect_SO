#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/wait.h>
#include <fcntl.h>

#define PATH_MAX 512

//informatii despre un fisier regulat
void informatii_fisier(int fd, char *filename, char *filename_statistica,const char *nume_director_output){

   char cale_completa[100];
   sprintf(cale_completa,"%s/%s",nume_director_output,filename_statistica);
   int stat_fd = open(cale_completa,O_WRONLY | O_CREAT | O_APPEND, 0666);
   if(stat_fd < 0){
      perror("eroare la crearea fisierului de statistica.\n");
      exit(-1);
   }

  struct stat st;

  char buffer[1024];//alocarea unui buffer suficient de mare
  
  fstat(fd, &st);

  char data_modificarii[11];  
  strftime(data_modificarii, sizeof(data_modificarii), "%Y-%m-%d", localtime(&st.st_mtime));

  char user_permissions[4],group_permissions[4],others_permissions[4];

  user_permissions[0] = (st.st_mode & S_IRUSR) ? 'R' : '-';
  user_permissions[1] = (st.st_mode & S_IWUSR) ? 'W' : '-';
  user_permissions[2] = (st.st_mode & S_IXUSR) ? 'X' : '-';
  user_permissions[3] = '\0';
 
  group_permissions[0] = (st.st_mode & S_IRGRP) ? 'R' : '-';
  group_permissions[1] = (st.st_mode & S_IWGRP) ? 'W' : '-';
  group_permissions[2] = (st.st_mode & S_IXGRP) ? 'X' : '-';
  group_permissions[3] = '\0';
  
  others_permissions[0] = (st.st_mode & S_IROTH) ? 'R' : '-';
  others_permissions[1] = (st.st_mode & S_IWOTH) ? 'W' : '-';
  others_permissions[2] = (st.st_mode & S_IXOTH) ? 'X' : '-';
  others_permissions[3] = '\0';

    int bytes_written = snprintf(buffer, sizeof(buffer),"nume fisier : %s\ndimensiune : %lld\nidentificatorul utilizatorului : %d\ntimpul ultimei modificari : %s\ncontorul de legaturi : %ld\ndrepturi de acces user : %s\ndrepturi de acces grup : %s\ndrepturi de acces altii : %s\n\n",filename, (long long)st.st_size, st.st_uid, data_modificarii, (long)st.st_nlink, user_permissions, group_permissions, others_permissions);

    if (bytes_written < 0) {
        perror("eroare la construirea buffer-ului.\n");
        exit(-1);
    }

    // Scrie doar octeții utilizați în fișier
    if (write(stat_fd, buffer, bytes_written) == -1) {
        perror("eroare la scrierea informatiilor despre un fisier regulat.\n");
        exit(-1);
    }

   if(close(stat_fd) < 0){//inchiderea fisierului de statistica
      perror("eroare inchidere fisier de statistica.\n");
      exit(-1);
   }
}

int getHeight(int fd){//functie ce returneaza inaltimea unui fisier cu extensia .bmp
    int height;
     if (lseek(fd, 22, SEEK_SET) < 0){
        perror("Error to move the file cursor\n");
        exit(1);
    }

    if (read(fd, &height, 4) < 0){
        perror("Error to read from file!\n");
        exit(1);
    }
    return height;
}

int getWidth(int fd){//functie ce returneaza latimea unui fisier cu extensia .bmp
    int width;
     if (lseek(fd, 18, SEEK_SET) < 0){
        perror("Error to move the file cursor\n");
        exit(1);
    }

    if (read(fd, &width, 4) < 0){
        perror("Error to read from file!\n");
        exit(1);
    }
    return width;
}
//informatii aditionale despre un fisier regulat un extensia .bmp
void height_and_width(int fd, char *filename_statistica,const char *nume_director_output){
  
  char cale_completa[100];
  sprintf(cale_completa,"%s/%s",nume_director_output,filename_statistica);
  int stat_fd = open(cale_completa,O_WRONLY | O_CREAT | O_APPEND, 0666);
   if(stat_fd < 0){
      perror("eroare la crearea fisierului de statistica.\n");
      exit(-1);
   }

   int height, width;

    if (lseek(fd, 18, SEEK_SET) < 0){
        perror("Error to move the file cursor\n");
        exit(1);
    }

    if (read(fd, &width, 4) < 0){
        perror("Error to read from file!\n");
        exit(1);
    }

     if (read(fd, &height, 4) < 0){
        perror("Error to read from file!\n");
        exit(1);
    }

    char buff[100];//alocare buffer
    sprintf(buff,"inaltime : %d\nlungime : %d\n\n",height,width);
    if(write(stat_fd, buff, sizeof(buff)) == -1){
        perror("eroare la scrierea informatiilor despre un fisier regulat cu extensia .bmp\n");
        exit(-1);
    }

    if(close(stat_fd) < 0){//inchiderea fisierului de statistica
      perror("eroare inchidere fisier de statistica.\n");
      exit(-1);
   }
}

//informatii despre un director
void informatii_director(struct dirent *element, char *filename_statistica, char *dirname,const char *nume_director_output){

   char cale_completa[100];
   sprintf(cale_completa,"%s/%s",nume_director_output,filename_statistica);
   int stat_fd = open(cale_completa,O_WRONLY | O_CREAT | O_APPEND, 0666);
   if(stat_fd < 0){
      perror("eroare la crearea fisierului de statistica.\n");
      exit(-1);
   }

  struct stat fileStat;
  char buffer[1024];//alocarea unui buffer suficient de mare

  if (lstat(element->d_name, &fileStat) < 0) {
    perror("lstat");
    exit(EXIT_FAILURE);
  }
  uid_t uid = fileStat.st_uid;

  char user_permissions[4],group_permissions[4],others_permissions[4];

  user_permissions[0] = (fileStat.st_mode & S_IRUSR) ? 'R' : '-';
  user_permissions[1] = (fileStat.st_mode & S_IWUSR) ? 'W' : '-';
  user_permissions[2] = (fileStat.st_mode & S_IXUSR) ? 'X' : '-';
  user_permissions[3] = '\0';
 
  group_permissions[0] = (fileStat.st_mode & S_IRGRP) ? 'R' : '-';
  group_permissions[1] = (fileStat.st_mode & S_IWGRP) ? 'W' : '-';
  group_permissions[2] = (fileStat.st_mode & S_IXGRP) ? 'X' : '-';
  group_permissions[3] = '\0';

  others_permissions[0] = (fileStat.st_mode & S_IROTH) ? 'R' : '-';
  others_permissions[1] = (fileStat.st_mode & S_IWOTH) ? 'W' : '-';
  others_permissions[2] = (fileStat.st_mode & S_IXOTH) ? 'X' : '-';
  others_permissions[3] = '\0';

  int bytes_written = snprintf(buffer, sizeof(buffer),"nume director : %s\nidentificatorul utilizatorului : %d\ndrepturi de acces user : %s\ndrepturi de acces grup : %s\ndrepturi de acces altii : %s\n\n",dirname, uid, user_permissions, group_permissions, others_permissions);

    if (bytes_written < 0) {
        perror("eroare la construirea buffer-ului.\n");
        exit(-1);
    }

        // Scrie doar octeții utilizați în fișier
    if (write(stat_fd, buffer, bytes_written) == -1) {
        perror("eroare la scrierea informatiilor despre un director.\n");
        exit(-1);
    }   

    if(close(stat_fd) < 0){//inchiderea fisierului de statistica
      perror("eroare inchidere fisier de statistica.\n");
      exit(-1);
    }

}

//informatii despre o legatura simbolica 
void informatii_legatura_simbolica(int fd_link, char *filename_statistica, char *linkname,const char *nume_director_output){
    
    char cale_completa[100];
    sprintf(cale_completa,"%s/%s",nume_director_output,filename_statistica);
    int stat_fd = open(cale_completa,O_WRONLY | O_CREAT | O_APPEND, 0666);
    if(stat_fd < 0){
      perror("eroare la crearea fisierului de statistica.\n");
      exit(-1);
    }

    char buffer[1024];//alocarea unui buffer suficient de mare
          
    struct stat link_stat;
    fstat(fd_link, &link_stat);

    struct stat target_stat;
    fstat(fd_link,&target_stat);

    char user_permissions[4],group_permissions[4],other_permissions[4];

    user_permissions[0] = (target_stat.st_mode & S_IRUSR) ? 'R' : '-';
    user_permissions[1] = (target_stat.st_mode & S_IWUSR) ? 'W' : '-';
    user_permissions[2] = (target_stat.st_mode & S_IXUSR) ? 'X' : '-';
    user_permissions[3] = '\0';

    group_permissions[0] = (target_stat.st_mode & S_IRGRP) ? 'R' : '-';
    group_permissions[1] = (target_stat.st_mode & S_IWGRP) ? 'W' : '-';
    group_permissions[2] = (target_stat.st_mode & S_IXGRP) ? 'X' : '-';
    group_permissions[3] = '\0';

    other_permissions[0] = (target_stat.st_mode & S_IROTH) ? 'R' : '-';
    other_permissions[1] = (target_stat.st_mode & S_IWOTH) ? 'W' : '-';
    other_permissions[2] = (target_stat.st_mode & S_IXOTH) ? 'X' : '-';
    other_permissions[3] = '\0';
    
    int bytes_written = snprintf(buffer, sizeof(buffer),"nume legatura : %s\ndimensiune legatura : %lld\ndimensiune fisier : %lld\ndrepturi de acces user legatura : %s\ndrepturi de acces grup legatura : %s\ndrepturi de acces altii legatura : %s\n\n",linkname, (long long)link_stat.st_size, (long long)target_stat.st_size, user_permissions, group_permissions, other_permissions);

    if (bytes_written < 0) {
        perror("eroare la construirea buffer-ului.\n");
        exit(-1);
    }

    // Scrie doar octeții utilizați în fișier
    if (write(stat_fd, buffer, bytes_written) == -1) {
        perror("eroare la scrierea informatiilor despre o legatura simbolica.\n");
        exit(-1);
    }

    if(close(stat_fd) < 0){//inchiderea fisierului de statistica
      perror("eroare inchidere fisier de statistica.\n");
      exit(-1);
   }
    
}

void parcurge_director(const char *cale_director, const char *director_iesire){

    DIR *director;
    struct dirent *element;
    struct stat st;

    if ((director = opendir(cale_director)) == NULL) {
        perror("Eroare la deschiderea directorului.\n");
        exit(-1);
    }

    
    while ((element = readdir(director)) != NULL) {

         if (strcmp(element->d_name, ".DS_Store") == 0) {
              //Ignoră fișierul DS_Store
              //configurare specifica pentru sistemul meu de operare, macOS
               continue;
        }

        char cale_completa[PATH_MAX];
        snprintf(cale_completa, sizeof(cale_completa), "%s/%s", cale_director, element->d_name);

        if (lstat(cale_completa, &st) == -1){
            perror("Eroare la obținerea informațiilor despre elementul directorului.\n");
            continue;
        }

        if (element->d_type == DT_DIR){//daca este un subdirector
        
            int numar_linii_scrise = 0;
            pid_t pid;
            pid = fork();//crearea procesului

            if(pid == -1){
                perror("error fork.\n");
                exit(-1);
            }

            if(pid == 0){//procesul copil

            if (strcmp(element->d_name, ".") != 0 && strcmp(element->d_name, "..") != 0){
                
                //deschid un alt director
                DIR *dir;
                struct dirent *elem;

                if ((dir = opendir(cale_director)) == NULL) {
                    perror("Eroare la deschiderea directorului.\n");
                    exit(-1);
                }

                elem = readdir(dir);
                char buffer_for_name[100];
                sprintf(buffer_for_name,"%s_statistica.txt",element->d_name);

                informatii_director(elem,buffer_for_name,element->d_name,director_iesire);
                numar_linii_scrise = numar_linii_scrise + 5;

                parcurge_director(cale_completa,director_iesire);// Parcurge recursiv subdirectorul.
                closedir(dir);
            }
            exit(numar_linii_scrise);
        }else { // proces părinte
            int status;
            waitpid(pid, &status, 0); // Așteaptă terminarea procesului copil și obține statusul acestuia

            if (!WIFEXITED(status)) {
                perror("procesul copil s-a terminat cu eroare.\n");
                exit(-1);
            }
        }
        }

        if (element->d_type == DT_REG) {//este un fisier regulat
            
            int numar_linii_scrise = 0;
            pid_t pid;
            pid = fork();//crearea procesului

            if(pid == -1){
                perror("error fork.\n");
                exit(-1);
            }

            if(pid == 0){//procesul copil
        
            int fd = open(cale_completa, O_RDONLY | O_RDWR);//deschid fisierul
            if (fd < 0) {
                perror("Eroare la deschiderea fișierului.\n");
                exit(-1);
            }

            char buffer_for_name[100];
            sprintf(buffer_for_name,"%s_statistica.txt",element->d_name);

            informatii_fisier(fd,element->d_name,buffer_for_name,director_iesire);
            numar_linii_scrise = numar_linii_scrise + 8;
            
            if (strstr(element->d_name, ".bmp") != NULL) {//daca are extensia .bmp

                height_and_width(fd,buffer_for_name,director_iesire);
                numar_linii_scrise = numar_linii_scrise + 2;

                pid_t pid_gray = fork();//crearea procesului pentru fisiere cu extensia .bmp

                if(pid_gray == -1){
                    perror("error fork.\n");
                    exit(-1);
                }

                if (pid_gray == 0) {//procesul copil pentru conversia la tonuri de gri
                
                    int height, width, dataOffset;

                    height = getHeight(fd);
                    width = getWidth(fd);
                    
                    if (lseek(fd, 0, SEEK_SET) < 0){//mutare la pozitia 0 in structura unui fisier .bmp
                        perror("Error to move the file cursor1\n");
                        exit(-1);
                    }

                    if (lseek(fd, 10, SEEK_CUR) < 0){//mutare dupa 10 octeti
                        perror("Error to move the file cursor1\n");
                        exit(-1);
                    }

                    if(read(fd, &dataOffset, 4) < 0){//prin dataOffset accesez Raster Data
                        perror("Error to read from file!\n");
                        exit(-1);
                    }

                    if(lseek(fd,dataOffset,SEEK_SET) < 0){//muta cursorul in locul unde trebuie modificati pixelii
                        perror("Error to move the file cursor3.\n");
                        exit(-1);
                    }

                    int red,green,blue;
                    long long i;

                    for (i = 0; i < width * height; i++) {

                        if (read(fd, &red, 1) < 0){
                            perror("error red.\n");
                            exit(-1);
                        }

                        if (read(fd, &green, 1) < 0){
                            perror("error green.\n");
                            exit(-1);
                        }

                        if (read(fd, &blue, 1) < 0){
                            perror("error blue.\n");
                            exit(-1);
                        }

                        int gray_value = (int)(0.299 * red + 0.587 * green + 0.114 * blue);

                        // Seteaza aceeasi valoare de gri pentru toate cele trei culori
                        red = gray_value;
                        green = gray_value;
                        blue = gray_value;

                        // Muta cursorul înapoi la poziția inițială
                        if (lseek(fd, -3, SEEK_CUR) < 0) {
                            perror("Error to move the file cursor4.\n");
                            exit(-1);
                        }

                        //Scrie noile valori
                        if (write(fd, &red, 1) < 0) {
                            perror("Error to write red value.\n");
                            exit(-1);
                        }

                        if (write(fd, &green, 1) < 0) {
                            perror("Error to write green value.\n");
                            exit(-1);
                        }

                        if (write(fd, &blue, 1) < 0) {
                            perror("Error to write blue value.\n");
                            exit(-1);
                        }
                    
                    }
                   
                    exit(0); // Iesire din procesul copil pentru conversie la tonuri de gri

                }else{//proces parinte

                    int status_gray;
                    pid_t terminated_pid_gray = waitpid(pid_gray, &status_gray, 0); // Asteapta terminarea procesului copil pentru conversie

                    if (terminated_pid_gray == -1) {
                        perror("Error waiting for child process (gray conversion) to terminate.\n");
                        exit(-1);
                    }
                    if (WIFEXITED(status_gray)) {
                       printf("S-a incheiat procesul cu pid-ul %d si codul %d\n",terminated_pid_gray,WEXITSTATUS(status_gray));
                    }else{
                        perror("procesul copil pentru conversia la tonuri de gri s-a terminat cu eroare.\n");
                        exit(-1);
                    }
                    
                }
        
            }

            close(fd);
            exit(numar_linii_scrise);

            }else { // proces părinte
                int status;
                waitpid(pid, &status, 0); // Așteaptă terminarea procesului copil și obține statusul acestuia

                if (!WIFEXITED(status)) {
                    perror("procesul copil s-a terminat cu eroare.\n");
                    exit(-1);
                }
            }
        }

        if (element->d_type == DT_LNK) {//daca este o legatura simbolica
           
            pid_t pid;
            pid = fork();

            if(pid == -1){
                perror("error fork.\n");
                exit(-1);
            }

            if(pid == 0){//proces copil

            int numar_linii_scrise = 0;
            int fd_link = open(cale_completa, O_RDONLY);
            if (fd_link < 0) {
                perror("Eroare la deschiderea legaturii simbolice.\n");
                exit(-1);
            }

            char buffer_for_name[100];
            sprintf(buffer_for_name,"%s_statistica.txt",element->d_name);

            informatii_legatura_simbolica(fd_link, buffer_for_name,element->d_name,director_iesire);
            numar_linii_scrise = numar_linii_scrise + 6;

            close(fd_link);

            exit(numar_linii_scrise);

            }else { // proces părinte
                int status;
                waitpid(pid, &status, 0); // Așteaptă terminarea procesului copil și obține statusul acestuia

                if (!WIFEXITED(status)) {
                    perror("procesul copil s-a terminat cu eroare.\n");
                    exit(-1);
                }
           }
        }

    }
    
    if (closedir(director) < 0){//inchiderea directorului
        perror("eroare inchidere director.\n");
        exit(-1);
    }
}

int main(int argc, char *argv[]){

    if (argc != 3){//numar incorect de argumente
        perror("Bad Usage!\n");
        exit(-1);
    }
    
    parcurge_director(argv[1], argv[2]);
 
    return 0;
}