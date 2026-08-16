/*
 tpass - Testing simple password hash function.
 2025 - Created by Fred Nora.
*/

#include <ctype.h> 
#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>



unsigned int simpleHashWithSalt(char *str, char *salt) {
    unsigned int hash = 0;
    int i = 0;
    
    // First process the input string
    while (str[i] != '\0') {
        hash += str[i];
        i++;
    }
    
    // Then add the salt
    i = 0;
    while (salt[i] != '\0') {
        hash += salt[i];
        i++;
    }
    
    return hash;
}

int main(int argc, char *argv[]) 
{
    char text[] = "hello";
    char mySalt[] = "xyz";  // The salt can be any string
    unsigned int hashValue = simpleHashWithSalt(text, mySalt);

// Print
    printf("Hash value of '%s' with salt '%s' is: %u\n", 
        text, 
        mySalt, 
        hashValue );

    return 0;
}
