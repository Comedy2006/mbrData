#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

int readSector(char*, unsigned int, char*);
void writeToFile(char*, char**);
void outputTerminal(char*, char*);

// -a: automatically determine the bootable drive
// -c choose the drive you want to scan (custom scan)
// -t terminal output
// -s save the output to a file of your desire
// search up the documentation on GitHub if there are any further questions

int main(int argc, char* argv[]) {
    char* buffer = malloc(512);
    char* dsk[22];
    if (strcmp(argv[1], "-a") == 0 && (argc == 3 || argc == 4)){ // Syntax: executable -a -s/-t file.txt (if -s)
        for (int i = 0; i < 26; i++){ // I don't really expect anyone who owns a PC or sth to have more than like 7 drives in it. Still gonna run thorugh 26 of 'em
            sprintf(dsk, "\\\\.\\PhysicalDrive%d", i);
            readSector(buffer, 0, dsk); //we need to start at sector 0, that's why 0 
            if ((unsigned char)buffer[510] == 0x55 && (unsigned char)buffer[511] == 0xAA) {
                break;
            }
        }
        if (strcmp(argv[2], "-s") == 0 && argc == 4) {
            writeToFile(buffer, argv);
        }else if (strcmp(argv[2], "-t") == 0 && argc == 3) {
            outputTerminal(buffer, dsk);
        }
    }
    // Syntax: executable -c -s/-t file.txt(if you put -s. write the filename of the exported file there) argv[3](drive you want to examine)
    if (strcmp(argv[1], "-c") == 0 && (argc >= 4 || argc == 5)){ 
        sprintf(dsk, "\\\\.\\PhysicalDrive%d", atoi(argv[3])); // need to convert that atoi ==> str to int
        readSector(buffer, 0, dsk); //we need to start at sector 0, that's why 0 
        
        if (strcmp(argv[2], "-s") == 0 && argc == 5) {
            writeToFile(buffer, argv);
        }else if (strcmp(argv[2], "-t") == 0 && argc == 4) {
            outputTerminal(buffer, dsk);
        }
    }

    printf("Press Enter to exit program...");
    getchar();
    return EXIT_SUCCESS;
}

int readSector(char* buffer, unsigned int sector, char* dsk) {
    DWORD read;
    HANDLE hDisk = CreateFile(dsk, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    if (hDisk == INVALID_HANDLE_VALUE) {
        CloseHandle(hDisk);
        return ERROR_INVALID_FUNCTION;
    }
    SetFilePointer(hDisk, sector * 512, 0, FILE_BEGIN);
    ReadFile(hDisk, buffer, 512, &read, 0);
    CloseHandle(hDisk);
    return EXIT_SUCCESS;
}

void writeToFile(char* buffer, char* argv[]) {
    FILE* f;
    f = fopen(argv[3], "w");
    if (f != NULL) {
        for (int i = 0; i < 512; i++) {
            fprintf(f, "%02x ", (unsigned char)buffer[i]);
            if ((i + 1) % 16 == 0) {
                fprintf(f, "\n");
            }
        }
        fclose(f);
    }
}

void outputTerminal(char* buffer, char* dsk) {
    HANDLE fontColor = GetStdHandle(STD_OUTPUT_HANDLE);
    
    SetConsoleTextAttribute(fontColor, 14);
    printf("\nBootloader in %s\n\n", dsk);
    if ((unsigned char)buffer[510] == 0x55 && (unsigned char)buffer[511] == 0xAA) {
        SetConsoleTextAttribute(fontColor, 10);
        printf("---- ! Disk is bootable ! ----\n\n");
    }
    else {
        SetConsoleTextAttribute(fontColor, 12);
        printf("---- ! This Disk is not bootable ! ----\n\n");
    }

    SetConsoleTextAttribute(fontColor, 7);
    printf("Hex:\n-----------------------------------------------------------\n");
    for (int i = 0; i < 512; i++) {
        printf("%02x ", (unsigned char)buffer[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("-----------------------------------------------------------\n");
    printf("\n\n");
    printf("ASCII:\n-----------------------------------------------------------\n");
    for (int i = 0; i < 512; i++) {
        printf("%c ", (unsigned char)buffer[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("-----------------------------------------------------------\n");
}