/*
 tcryp1 - Working with public and private keys in a very simple way.
 2025 - Created by Fred Nora.
*/

#include <ctype.h> 
#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
//#include <stdio.h>

#include <stdio.h>
#include <string.h>

// Simple "encryption" with public key (add a value derived from public key)
void encryptWithPublicKey(char *message, int publicKey, char *output) {
    int i = 0;
    while (message[i] != '\0') {
        output[i] = message[i] + publicKey; // "Encrypt" by adding public key value
        i++;
    }
    output[i] = '\0';
}

// Simple "decryption" with private key (subtract related value)
void decryptWithPrivateKey(char *encrypted, int privateKey, char *output) {
    int i = 0;
    while (encrypted[i] != '\0') {
        output[i] = encrypted[i] - privateKey; // "Decrypt" by subtracting private key value
        i++;
    }
    output[i] = '\0';
}

int main(int argc, char *argv[]) 
{
    char message[] = "hello";
    int publicKey = 5;   // Simplified public key (just a number for this demo)
    int privateKey = 5;  // Private key matches public key for reversal (toy example)
    char encrypted[50];
    char decrypted[50];

    // Encrypt with public key
    encryptWithPublicKey(message, publicKey, encrypted);
    printf("Original: %s\n", message);
    
    // Print encrypted (might show odd characters, but it’s fine for demo)
    printf("Encrypted: ");
    for (int i = 0; encrypted[i] != '\0'; i++) {
        printf("%c", encrypted[i]);
    }
    printf("\n");

    // Decrypt with private key
    decryptWithPrivateKey(encrypted, privateKey, decrypted);
    printf("Decrypted: %s\n", decrypted);

    return 0;
}