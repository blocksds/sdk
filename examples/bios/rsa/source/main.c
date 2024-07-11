// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Michele Di Giorgio, 2024

// Private key generated:
//
//      openssl genrsa -f4 -out privkey.pem 1024
//
// Then, generate public key to verify the message:
//
//      openssl rsa -in privkey.pem -pubout -outform DER -out pubkey.der
//
// Then, extract the modulus from public key:
//
//      openssl rsa -pubin -inform DER -text -noout < pubkey.der | tail -n 10 | head -n 9 | xxd -r -p | tail -c +2 > pubkey.mod
//
// Then, create a text message:
//
//      printf 'This is a secret message!' > secret.txt
//
// Now, sign the message using the private key:
//
//      openssl dgst -sha1 -sign privkey.pem -out secret.txt.sig secret.txt
//
// To verify the signature using the public key run:
//
//      openssl dgst -sha1 -keyform DER -verify pubkey.der -signature secret.txt.sig secret.txt
//
// Finally, extract the expected hash using the public key:
//
//      openssl rsautl -verify -inkey pubkey.der -keyform DER -pubin -in secret.txt.sig -hexdump
//
// Update EXPECTED_OUT_SIZE and the array ``expected[EXPECTED_OUT_SIZE]`` with
// values from the last command.

#include <stdio.h>

#include <nds.h>
#include <nds/rsa.h>

#include "pubkey_mod_bin.h"
#include "secret_txt_sig_bin.h"

#define EXPECTED_OUT_SIZE 35

void runRSADecryptTest()
{
    const uint8_t expected[EXPECTED_OUT_SIZE] = {
        0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b,
        0x0e, 0x03, 0x02, 0x1a, 0x05, 0x00, 0x04,
        0x14, 0xd8, 0xf5, 0x58, 0x84, 0x40, 0xab,
        0xa2, 0x9f, 0xee, 0xe5, 0x77, 0xe9, 0xa3,
        0x06, 0xe0, 0xab, 0x52, 0xcf, 0x12, 0x01,
    };

    printf("Expected hashed message:\n\n");
    for (int i = 0; i < EXPECTED_OUT_SIZE; ++i)
        printf("%02X", expected[i]);
    printf("\n\n");

    // Create RSA Heap Context
    swiRSAHeapContext_t rsaCtx = { 0 };
    uint8_t *heap = (uint8_t *)malloc(SWI_RSA_DEFAULT_HEAP_SIZE);
    if (heap == NULL) {
        printf("Not enough memory");
        return;
    }
    swiRSAInitHeap(&rsaCtx, heap, SWI_RSA_DEFAULT_HEAP_SIZE);

    // Decrypt signature
    uint8_t hashedMessage[128] = { 0 };
    swiRSADecrypt(&rsaCtx,
                  hashedMessage,
                  secret_txt_sig_bin,
                  pubkey_mod_bin);
    printf("Result:\n\n");
    for (int i = 0; i < EXPECTED_OUT_SIZE; ++i)
        printf("%02X", hashedMessage[i]);
    printf("\n\n");

    // Validate signature
    int match = memcmp(expected, hashedMessage, EXPECTED_OUT_SIZE) == 0;
    printf("Do they match? %s\n", match ? "Yes" : "No");

    // Release heap memory
    free(heap);
}

int main(int argc, char **argv)
{
    consoleDemoInit();

    if (!isDSiMode())
    {
        printf("RSA BIOS functions are only\n");
        printf("available on DSi\n");
    }
    else
    {
        runRSADecryptTest();
    }
    printf("\n");
    printf("Press START to exit");

    while (1)
    {
        swiWaitForVBlank();

        scanKeys();
        if (keysDown() & KEY_START)
            break;
    }
    return 0;
}
