/*
 tcryp2 - Working with public and private keys in a very simple way.
 2025 - Created by Fred Nora.
*/

#include <ctype.h> 
#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
//#include <stdio.h>

#include <stdio.h>
#include <string.h>

// Modular exponentiation (base^exp mod modulus)
int modPow(int base, int exp, int modulus) {
    int result = 1;
    base = base % modulus;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % modulus;
        }
        base = (base * base) % modulus;
        exp /= 2;
    }
    return result;
}

// Encrypt with public key (e, n)
void encryptWithPublicKey(char *message, int e, int n, int *output) {
    int i = 0;
    while (message[i] != '\0') {
        int m = (int)message[i];        // ASCII value of character
        output[i] = modPow(m, e, n);    // c = m^e mod n
        i++;
    }
    output[i] = -1; // Mark end of encrypted array
}

// Decrypt with private key (d, n)
void decryptWithPrivateKey(int *encrypted, int d, int n, char *output) {
    int i = 0;
    while (encrypted[i] != -1) {
        int c = encrypted[i];
        output[i] = (char)modPow(c, d, n); // m = c^d mod n
        i++;
    }
    output[i] = '\0'; // Null terminate the string
}


int main(int argc, char *argv[]) 
{
    // Toy RSA keys with larger modulus
    int p = 13, q = 17;     // Primes
    int n = p * q;          // n = 221
    int phi = (p - 1) * (q - 1); // phi(n) = 12 * 16 = 192
    int e = 5;              // Public exponent (coprime to 192)
    int d = 77;             // Private exponent (e * d = 1 mod phi; 5 * 77 = 385 = 1 mod 192)

    char message[] = "hi";
    int encrypted[50];
    char decrypted[50];

    // Encrypt
    encryptWithPublicKey(message, e, n, encrypted);
    printf("Original: %s\n", message);
    printf("Encrypted: ");
    for (int i = 0; encrypted[i] != -1; i++) {
        printf("%d ", encrypted[i]);
    }
    printf("\n");

    // Decrypt
    decryptWithPrivateKey(encrypted, d, n, decrypted);
    printf("Decrypted: %s\n", decrypted);

    return 0;
}
