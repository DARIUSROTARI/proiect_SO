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
void informatii_fisier(int fd, int stat_fd, char *filename) {
  struct stat st;
  
  dprintf(stat_fd,"nume fisier : %s\n",filename);// Scrie numele fișierului în fișierul de output (stat_fd)

  fstat(fd, &st);
  dprintf(stat_fd,"dimensiune : %lld\n",(long long) st.st_size);//dimensiunea in octeti
  dprintf(stat_fd,"identificatorul utilizatorului : %d\n",st.st_uid);

  char data_modificarii[11];  
  strftime(data_modificarii, sizeof(data_modificarii), "%Y-%m-%d", localtime(&st.st_mtime));

  dprintf(stat_fd,"timpul ultimei modificari : %s\n", data_modificarii);//data ultimei modificari

  dprintf(stat_fd,"contorul de legaturi : %ld\n", (long)st.st_nlink);

  char user_permissions[4],group_permissions[4],others_permissions[4];

  user_permissions[0] = (st.st_mode & S_IRUSR) ? 'R' : '-';
  user_permissions[1] = (st.st_mode & S_IWUSR) ? 'W' : '-';
  user_permissions[2] = (st.st_mode & S_IXUSR) ? 'X' : '-';
  user_permissions[3] = '\0';
  dprintf(stat_fd,"drepturi de acces user : %s\n", user_permissions);//permisiunile user-ului

  group_permissions[0] = (st.st_mode & S_IRGRP) ? 'R' : '-';
  group_permissions[1] = (st.st_mode & S_IWGRP) ? 'W' : '-';
  group_permissions[2] = (st.st_mode & S_IXGRP) ? 'X' : '-';
  group_permissions[3] = '\0';
  dprintf(stat_fd,"drepturi de acces grup : %s\n",group_permissions);//permisiunile grupului

  others_permissions[0] = (st.st_mode & S_IROTH) ? 'R' : '-';
  others_permissions[1] = (st.st_mode & S_IWOTH) ? 'W' : '-';
  others_permissions[2] = (st.st_mode & S_IXOTH) ? 'X' : '-';
  others_permissions[3] = '\0';
  dprintf(stat_fd,"drepturi de acces altii : %s\n",others_permissions);//permisiunile pentru altii

}

//informatii adistionale despre un fisier regulat un extensia .bmp
void height_and_width(int fd, int stat_fd){
  
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

    dprintf(stat_fd, "inălțime: %d\n", height);
    dprintf(stat_fd, "lungime: %d\n", width);

}

//informatii despre un director
void informatii_director(struct dirent *element, int stat_fd){


  struct stat fileStat;
  if (lstat(element->d_name, &fileStat) < 0) {
    perror("lstat");
    exit(EXIT_FAILURE);
  }
  uid_t uid = fileStat.st_uid;
  dprintf(stat_fd, "identificatorul utilizatorului : %d\n", uid);

  char user_permissions[4],group_permissions[4],others_permissions[4];

  user_permissions[0] = (fileStat.st_mode & S_IRUSR) ? 'R' : '-';
  user_permissions[1] = (fileStat.st_mode & S_IWUSR) ? 'W' : '-';
  user_permissions[2] = (fileStat.st_mode & S_IXUSR) ? 'X' : '-';
  user_permissions[3] = '\0';
  dprintf(stat_fd,"drepturi de acces user : %s\n", user_permissions);//permisiunile user-ului

  group_permissions[0] = (fileStat.st_mode & S_IRGRP) ? 'R' : '-';
  group_permissions[1] = (fileStat.st_mode & S_IWGRP) ? 'W' : '-';
  group_permissions[2] = (fileStat.st_mode & S_IXGRP) ? 'X' : '-';
  group_permissions[3] = '\0';
  dprintf(stat_fd,"drepturi de acces grup : %s\n",group_permissions);//permisiunile grupului

  others_permissions[0] = (fileStat.st_mode & S_IROTH) ? 'R' : '-';
  others_permissions[1] = (fileStat.st_mode & S_IWOTH) ? 'W' : '-';
  others_permissions[2] = (fileStat.st_mode & S_IXOTH) ? 'X' : '-';
  others_permissions[3] = '\0';
  dprintf(stat_fd,"drepturi de acces altii : %s\n",others_permissions);//permisiunile pentru altii

}

//informatii despre o legatura simbolica 
void informatii_legatura_simbolica(int fd_link, int stat_fd, char *linkname){
   
    dprintf(stat_fd,"nume legatura : %s\n",linkname);

          
    struct stat link_stat;
    fstat(fd_link, &link_stat);
    dprintf(stat_fd, "dimensiune legătura : %lld \n", (long long)link_stat.st_size);//dimensiunea in octeti a legaturii

    struct stat target_stat;
    fstat(fd_link,&target_stat);
    dprintf(stat_fd, "dimensiune fișier : %lld \n", (long long)target_stat.st_size);//dimensiunea in octeti a fisierului target

    char user_permissions[4],group_permissions[4],other_permissions[4];

    user_permissions[0] = (target_stat.st_mode & S_IRUSR) ? 'R' : '-';
    user_permissions[1] = (target_stat.st_mode & S_IWUSR) ? 'W' : '-';
    user_permissions[2] = (target_stat.st_mode & S_IXUSR) ? 'X' : '-';
    user_permissions[3] = '\0';
    dprintf(stat_fd,"drepturi de acces user legatura : %s\n", user_permissions);//permisiunile user-ului

    group_permissions[0] = (target_stat.st_mode & S_IRGRP) ? 'R' : '-';
    group_permissions[1] = (target_stat.st_mode & S_IWGRP) ? 'W' : '-';
    group_permissions[2] = (target_stat.st_mode & S_IXGRP) ? 'X' : '-';
    group_permissions[3] = '\0';
    dprintf(stat_fd,"drepturi de acces grup legatura : %s\n", group_permissions);//permisiunile grupului

    other_permissions[0] = (target_stat.st_mode & S_IROTH) ? 'R' : '-';
    other_permissions[1] = (target_stat.st_mode & S_IWOTH) ? 'W' : '-';
    other_permissions[2] = (target_stat.st_mode & S_IXOTH) ? 'X' : '-';
    other_permissions[3] = '\0';
    dprintf(stat_fd,"drepturi de acces altii legatura : %s\n", other_permissions);//permisiunile pentru altii
    
}

void parcurge_director(const char *cale_director, int stat_fd){

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

        if (lstat(cale_completa, &st) == -1) {
            perror("Eroare la obținerea informațiilor despre elementul directorului.\n");
            continue;
        }

        if (element->d_type == DT_DIR) {//daca este un subdirector
            if (strcmp(element->d_name, ".") != 0 && strcmp(element->d_name, "..") != 0) {
               
                dprintf(stat_fd,"nume director : %s\n",element->d_name);//scriu in statistica.txt numele sau
                
                //deschid un alt director
                DIR *dir;
                struct dirent *elem;

                if ((dir = opendir(cale_director)) == NULL) {
                    perror("Eroare la deschiderea directorului.\n");
                    exit(-1);
                }

                elem = readdir(dir);
                informatii_director(elem,stat_fd);
                parcurge_director(cale_completa, stat_fd);// Parcurge recursiv subdirectorul.
                dprintf(stat_fd,"\n");
                closedir(dir);
            }
        }

        if (element->d_type == DT_REG) {//este un fisier regulat
            
            int fd = open(cale_completa, O_RDONLY);
            if (fd < 0) {
                perror("Eroare la deschiderea fișierului.\n");
                exit(-1);
            }
            informatii_fisier(fd, stat_fd,element->d_name);
            if (strstr(element->d_name, ".bmp") != NULL) {//daca are extensia .bmp
                height_and_width(fd, stat_fd);
            }
            close(fd);
            dprintf(stat_fd,"\n");
        }

        if (element->d_type == DT_LNK) {//daca este o legatura simbolica
           
            int fd_link = open(cale_completa, O_RDONLY);
            if (fd_link < 0) {
                perror("Eroare la deschiderea legaturii simbolice.\n");
                exit(-1);
            }
            informatii_legatura_simbolica(fd_link, stat_fd,element->d_name);
            dprintf(stat_fd, "\n");
            close(fd_link);
        }

    }
    
    if (closedir(director) < 0){//inchiderea directorului
        perror("eroare inchidere director.\n");
        exit(-1);
    }
}


int main(int argc, char *argv[]){

    if (argc != 2){//numar incorect de argumente
        perror("Bad Usage!\n");
        exit(-1);
    }
    
   const char *cale_director = argv[1];

    int stat_fd = open("statistica.txt",O_WRONLY | O_CREAT | O_APPEND, 0666);
    if(stat_fd < 0){
      perror("eroare la crearea fisierului de statistica.\n");
      exit(-1);
    }
  
    parcurge_director(cale_director,stat_fd);
 
   if(close(stat_fd) < 0){//inchiderea fisierului de statistica
      perror("eroare inchidere fisier de statistica.\n");
      exit(-1);
   }
    return 0;
}