// Autumn Hale
// CSCI 4100
// Assignment 6
// C program that does the same as the the UNIX pwd program, but less complex.

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

void inum_to_filename(ino_t inode_num, char *buf, int buf_size);
ino_t filename_to_inum(char *filename);
void display_path(ino_t inode_num);

int main(void) {

  // Get inode number of current working directory
  ino_t inode_num = filename_to_inum(".");

  // Pass to display_path & display absolute path of current working directory
  display_path(inode_num);

  printf("\n");
  
  return 0;
}

// Translates an inode number into the corresponding file name using the current working directory
void inum_to_filename(ino_t inode_num, char *buf, int buf_size) {

  // Open the current working directory
  DIR *dir_ptr = opendir(".");

  // If current directory cannot be opened, give error & exit
  if (dir_ptr == NULL) {
    perror(".");
    exit(EXIT_FAILURE);
  }

  // Reading the first directory entry
  struct dirent *dirent_ptr = readdir(dir_ptr);

  // Reading the next directory continiously, while there is one
  while (dirent_ptr != NULL) {
    /* if inode number in current directory matches one passed in, 
      copy filename into buffer, return from function */
    if (filename_to_inum(dirent_ptr->d_name) == inode_num) {
      strncpy(buf, dirent_ptr->d_name, buf_size);
      buf[buf_size - 1] = '\0';
      return;
    }
    // Continue reading the next directory entry
    dirent_ptr = readdir(dir_ptr);
  }

  // If node number passed in is not in any directory entries, error & exit
  // Executes if all directory entries have been read
  fprintf(stderr, "error looking for inum %d\n", (int)inode_num);
  exit(EXIT_FAILURE);

  // Close directory. Is this optional? Probably not.
  closedir(dir_ptr);
}

// Translates filename to inode number
ino_t filename_to_inum(char *filename) {

  // Reading the infomation from the file's inode into a structure in memory
  struct stat data;
  int result = stat(filename, &data);

  // If error retrieving metadata, give error & exit
  if (result == -1) {
    fprintf(stderr, "Cannot stat ");
    perror(filename);
    exit(EXIT_FAILURE);
  }

  // Return the inode number from the structure in memory
  return data.st_ino;
}

// Recursive function that displays absolute pathname for given inode number
void display_path(ino_t inode_num) {

  // Creating array of characters to use as buffer for name of directory
  char buffer[BUFSIZ];

  // Getting inode number for the parent of current directory
  ino_t parent_inode_num = filename_to_inum("..");

  // If parent inode number same as current inode number, root reached.
  if (parent_inode_num == inode_num) {
    return;
  }
  // If different...
  // Change to parent directory
  int v = chdir("..");
    
  // Find file name for the inode number passed in, store it in buffer
  inum_to_filename(inode_num, buffer, BUFSIZ);

  // Recursive call to display absolute path name of parent directory
  display_path(parent_inode_num);

  printf("/%s", buffer);
}


