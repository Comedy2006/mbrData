#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#define MAX_LEN 100
#define MAX_LST 20

int readSector(char*, unsigned int, char*);
void writeToFile(char*, char**);
void outputTerminal(char*, char*);
void shell();
void parser(char*, char**);
void commands(char**);
void exec_cmd(char**);

int diskNumber;

// -a: automatically determine the bootable drive
// -c choose the drive you want to scan (custom scan)
// -t terminal output
// -s save the output to a file of your desire
// search up the documentation on GitHub if there are any further questions

int main(int argc, char* argv[]) {
    if (argc == 1){
        shell();
        return 0;
    }
    char* buffer = malloc(512);
    char* dsk;
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

// Shell is WIP ==> doesn't contain any functionalities yet

void shell(){
    char cmd[MAX_LEN];
    char *parsecmd[MAX_LST];
    printf("mbrData> ");
    fgets(cmd, MAX_LEN, stdin);
    cmd[strlen(cmd) - 1] = '\0';
    parser(cmd, parsecmd);
    commands(parsecmd);
    exec_cmd(parsecmd);
    
    shell();
}

// need to parse the commands or else the shell won't work as expected
void parser(char* cmd, char** parsedcmd){
    for(int i = 0; i < MAX_LST; i++){
        parsedcmd[i] = strtok(cmd, " ");
        if(parsedcmd[i] == NULL){
            break;
        }
        cmd = NULL;
    }
}

void commands(char** cmd){
    if (strcmp(*cmd, "h") == 0 || strcmp(*cmd, "?") == 0){
        printf( "ls dsk             list every available physical disk on this device\n"
                "sel dsk [NUMBER]   select a physical drive\n"
                "exit               exit the program\n");
    }else if(strcmp(cmd[0], "exit") == 0){
        exit(0);
    }else if((strcmp(cmd[0], "ls") == 0) && (strcmp(cmd[1], "dsk") == 0)){
        printf("Available Disks:\n");
        system("wmic diskdrive list brief");
    }else if((strcmp(cmd[0], "sel") == 0) && (strcmp(cmd[1], "dsk") == 0)){
        diskNumber = atoi(cmd[2]);
    }
}

void exec_cmd(char** parsecmd){
    STARTUPINFO sinfo;
    PROCESS_INFORMATION pinfo;

    ZeroMemory(&sinfo, sizeof(sinfo));
    sinfo.cb = sizeof(sinfo);
    ZeroMemory(&pinfo, sizeof(pinfo));
    if (CreateProcess(NULL, parsecmd[0], NULL, NULL, FALSE, 0, NULL, NULL, &sinfo, &pinfo)) {
      printf("CreateProcess failed (%d).\n", GetLastError());
      return;
    }

    WaitForSingleObject(pinfo.hProcess, INFINITE);

    CloseHandle(pinfo.hProcess);
    CloseHandle(pinfo.hThread);
}