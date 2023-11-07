#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/dir.h>

void informatii_fisier(int fd, struct dirent *structura_director){

//   char timestamp[20];
//   strftime(timestamp, sizeof(timestamp), "%d.%m.%Y", localtime(&file_info.st_mtime));

//   mode_t file_mode = file_info.st_mode;
//   char user_permissions[4], group_permissions[4], other_permissions[4];

//   char statistica[512];
//   sprintf(statistica, "nume fisier : %s\ndimensiune : %lu\nidentificatorul utilizatorului : %d\ntimpul ultimei modificari : %s\ncontorul de legaturi: %lu\ndreptui de acces user : RWX\ndrepturi de acces grup : R--\ndrepturi de acces altii : ---\n")

}

void informatii_director(const char *file, const char *statistica){


}

void accesUser(struct stat buf)
{
    if ((buf.st_mode & S_IRUSR) != 0){
        printf("R");
    }
    else{
        printf("-");
    }
    if ((buf.st_mode & S_IWUSR) != 0){
        printf("W");
    }
    else{
        printf("-");
    }
    if ((buf.st_mode & S_IXUSR) != 0){
        printf("X");
    }
    else{
        printf("-");
    }
}

void height_and_width(const char *image_description,const char *statistica){
  
    int height, width;

    if (lseek(image_description, 18, SEEK_SET) < 0){
        perror("Error to move the file cursor\n");
        exit(1);
    }

    if (read(image_description, &width, 4) < 0){
        perror("Error to read from file!\n");
        exit(1);
    }

     if (read(image_description, &height, 4) < 0){
        perror("Error to read from file!\n");
        exit(1);
    }

}

int main(int argc, char *argv[])
{
    if (argc != 2){
        perror("Usage: %s <input_file>\n", argv[0]);
        exit(-1);
    }

    char stat_filename[] = "statistica.txt";
    int stat_fd = open(stat_filename,O_WRONLY | O_CREAT | O_APPEND);

    if(stat_fd < 0){
      perror("eroare la crearea fisierului de statistica");
      closedir(dir);
      exit(-1);
    }

    DIR *dir = NULL;
    struct dirent *structura_director = NULL;
    
    dir = opendir(argv[1]);
    if(dir ==  NULL){
      perror("eroare deschidere director");
      exit(1);
    }

    while((structura_director = readdir(dir)) !=  NULL){

      if(S_ISREG(structura_director.st_mode)){

	    informatii_fisier(stat_fd,structura_director);

	    if(strcmp(strchr(structura_director,"."),"bmp") == 0){

           height_and_width(image_description);

         }
      }

      if(S_ISDIR(structura_director.st_mode)){

        informatii_director(structura_director);
      }

      if(S_ISLINK(structura_director.st_mode)){

        informatii_legatura_simbolica(structura_director);
      }

    }
     
   if (closedir(dir) < 0)
    {
      perror("eroare inchidere director");
      exit(1);
    }
    return 0;

}
