#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <libgen.h> 
#include "command.h"

#define MAX_BUFFER_SIZE 4096

// ls command
void listDir() {
    DIR *dir;
    struct dirent *line;
    
    // Open directory
    dir = opendir("."); 
    if (dir != NULL) {
        // Read each line in the directory
        while ((line = readdir(dir)) != NULL) { 
            write(1, line->d_name, strlen(line->d_name)); 
            write(1, " ", 1);
        }
        closedir(dir); 
    }
    write(1, "\n", 1);
}

// pwd command
void showCurrentDir(){
    char buf[MAX_BUFFER_SIZE];

    // Get the current directory
    char *pwd = getcwd(buf, sizeof(buf));
    
    if (pwd == NULL) {  
        char *error_msg = "Memory allocation failed: ";
        write(1, error_msg, strlen(error_msg));
        write(1, "\n", 1);
    } else {
        write(1, pwd, strlen(pwd)); 
        write(1, "\n", 1);
    }
    
}


// mkdir command
void makeDir(char *dirName) { 
    mkdir(dirName, 0777);
}


// cd command
void changeDir(char *dirName) {
    chdir(dirName);    
}


// cp command
void copyFile(char *sourcePath, char *destinationPath) {

    char buf[MAX_BUFFER_SIZE];
    int source_file, destination_file;
    ssize_t text_read;

    // Open the source file to read only
    source_file = open(sourcePath, O_RDONLY);
    if(source_file == -1){
        char *error_msg = "cp command failed! Invalid Source file!";
        write(1, error_msg, strlen(error_msg));
        write(1, "\n", 1);
    }
    
    // Open destination file
    destination_file = open(destinationPath, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if(destination_file == -1){
        // Get file name from the path
        char *base = basename(sourcePath);
        char *newDestPath = malloc(strlen(destinationPath) + strlen(base) + 2);
        if (newDestPath == NULL) {
            char *error_msg = "cp command failed!";
            write(1, error_msg, strlen(error_msg));
            write(1, "\n", 1);
        }
        
        // Format destination path
        sprintf(newDestPath, "%s/%s", destinationPath, base);
        destination_file = open(newDestPath, O_WRONLY | O_CREAT | O_TRUNC, 0777);
        free(newDestPath);
    }
    
    // Read source file and write to destination
    while ((text_read = read(source_file, buf, sizeof(buf)))) {
        write(destination_file, buf, text_read);
    }

    close(source_file);
    close(destination_file); 
}




// mv command
void moveFile(char *sourcePath, char *destinationPath) { 
    if (rename(sourcePath, destinationPath) == -1) {
        char *error_msg = "mv command failed!";
	write(1, error_msg, strlen(error_msg));
	write(1, "\n", 1); 
    }
    
}


// rm command
void deleteFile(char *filename) { 
    if(remove(filename) == -1) {
        char *error_msg = "rm command failed!";
        write(1, error_msg, strlen(error_msg)); 
        write(1, "\n", 1); 
   } 
   
   
}


// Cat command
void displayFile(char *filename) {

    char *buf = (char *)malloc(MAX_BUFFER_SIZE);
    // Open file for reading 
    int o_file = open(filename, O_RDONLY);
    
    if(o_file == -1){
        char *error_msg = "CAT failed! Cannot read the file!\n";
	write(1, error_msg, strlen(error_msg));
    
    } else{
        // Read file and write its contents
        while(read(o_file, buf, sizeof(buf))){
	    write(1, buf, sizeof(buf));
	}
    }
    
    write(1, "\n", 1);
    free(buf);
    close(o_file);

}
