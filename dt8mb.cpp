/*
    Discord Teleporter 8mb
    by: Wassimulator

A basic program that disects a file into a set of 8mb chunks to send through Discord and bypass the very low limit.
The program also assimilates the chunks back into a file.

Usage:
    (it is best to add the file to environment variables for easy use)

    Send:
        dt8mb send FILE.ext
    Receive:
        dt8mb receive FILE.ext

    Sending creates the chunks. receive assimilates the chunks under a target file with the extention [ext]. 
    It is important that the [FILE] name in the call is identical to the received chunks minus the counter. You are free to set the extention.
    example: chunk_001.dt8mb, [File] == chunk
*/

#include <iostream>
#include <Windows.h>
#include <fileapi.h>

#define EIGHT_MB 8388608

float clamp(uint64_t input, uint64_t min, uint64_t max)
{
    if (input < min)
        return min;
    else if (input > max)
        return max;
    else
        return input;
}
void ErrorUsage()
{
    printf("WRONG PARAMETERS. Usage: dt8mb [send/receive] file.extention\n");
    exit(1);
}

int main(int argc, char **argv)
{
    if (argc != 3)
        ErrorUsage();

    if (strncmp(argv[1], "send", 4) == 0)
    {
        // seperate file name
        char *originalfile = (char *)malloc(sizeof(char) * strlen(argv[2]));
        strcpy(originalfile, argv[2]);
        int len = strlen(originalfile);

        // open file
        FILE *f = fopen(originalfile, "rb");
        if (f == NULL)
        {
            printf("File not found\n");
            return 1;
        }

        // remove extention
        while (originalfile[len] != '.')
        {
            originalfile[len] = '\0';
            len--;
        }
        originalfile[len] = '\0';
        len--;

        // get size and read into buffer
        fseek(f, 0, SEEK_END);
        uint64_t size = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *buffer = (char *)malloc(size);
        fread(buffer, size, 1, f);
        fclose(f);
        if (size < EIGHT_MB)
        {
            printf("File is less than 8mb and needs no dissection.\n");
            return 1;
        }

        // dissect buffer
        int count = 0;
        uint64_t at = 0;
        uint64_t remaining = size;
        while (at < size)
        {
            char name[256];
            sprintf(name, "%s_%03d.dt8mb", originalfile, count);
            FILE *targetfile = fopen(name, "wb");
            if (targetfile == NULL)
            {
                printf("Failed to create file %s at %i count\n", name, count);
                return 1;
            }

            uint64_t toWrite = min(remaining, EIGHT_MB);
            fwrite(buffer + at, toWrite, 1, targetfile);
            fclose(targetfile);
            remaining -= toWrite;
            at += toWrite;
            count++;
        }
        printf("File dissection complete. %i files created at target file folder.\n", count);
        return 0;
    }
    else if (strncmp(argv[1], "receive", 7) == 0)
    {
        // seperate file name and extention
        char *targetfilename = (char *)malloc(sizeof(char) * strlen(argv[2]));
        char *extention = (char *)malloc(sizeof(char) * strlen(argv[2]));
        strcpy(targetfilename, argv[2]);
        int len = strlen(targetfilename);
        int at = len;
        while (targetfilename[at] != '.')
            at--;
        targetfilename[at] = '\0';
        strcpy(extention, targetfilename + at + 1);

        // open file
        char *targetpath = (char *)malloc(sizeof(char) * (strlen(targetfilename) + strlen(extention) + 1));
        sprintf(targetpath, "%s.%s", targetfilename, extention);
        FILE *targetFILE = fopen(targetpath, "wb");
        if (targetFILE == NULL)
        {
            printf("Failed to create file %s\n", targetpath);
            return 1;
        }

        // loop and assimilate
        int count = 0;
        while (true)
        {
            char sourcename[256];
            sprintf(sourcename, "%s_%03d.dt8mb", targetfilename, count);
            FILE *sourceFILE = fopen(sourcename, "rb");
            if (count == 0)
            {
                if (sourceFILE == NULL)
                {
                    printf("First dt8mb file not found\n");
                    return 1;
                }
            }
            else
            {
                if (sourceFILE == NULL)
                    break;
            }
            //get size of source and read into memory
            fseek (sourceFILE , 0 , SEEK_END);
            uint64_t size = ftell(sourceFILE);
            fseek(sourceFILE, 0, SEEK_SET);
            char *buffer = (char *)malloc(size);
            fread(buffer, size, 1, sourceFILE);
            fclose(sourceFILE);
            remove(sourcename);

            fwrite(buffer, size, 1, targetFILE);
            count++;
            free(buffer);
        }
        fclose(targetFILE);
        printf("File assimilated in: %s\n", targetpath);

        return 0;
    }
    else
        ErrorUsage();
}