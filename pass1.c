#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100

int input_count=0, optab_count=0, symtab_count=0, LOCCTR = 0;

struct Instruction {
    char label[10];
    char operation[10];
    char operand[10];
};

struct Optab {
    char mnemonic[10];
    int format;
    char opcode[10];
};

struct SYMTAB{
    char symbol[10];
    int address;
};

void OptabFile(const char *filename, struct Optab *optab)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file) != NULL) {
        sscanf(line, "%s %d %s", optab[optab_count].mnemonic, &optab[optab_count].format, optab[optab_count].opcode);
        optab_count++;
    }
    fclose(file);

}

void InputFile(const char *filename, struct Instruction *instructions) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';

        if (sscanf(line, "%s %s %s", instructions[input_count].label, instructions[input_count].operation, instructions[input_count].operand) == 3) 
        {
        }
        else if (sscanf(line, "%s %s",instructions[input_count].operation, instructions[input_count].operand) == 2) {
            strcpy(instructions[input_count].label, "*");
        }
        else if(sscanf(line, "%s",instructions[input_count].operation) == 1)
        {
            strcpy(instructions[input_count].label, "*");
            strcpy(instructions[input_count].operand, "*");
        }
        input_count++;
    }

    fclose(file);
}

int bytesz(const char *addr) {
    if (addr[0] == 'C') 
    {
        const char *s = addr + 2;
        return strlen(s) - 1;
    } 
    else if (addr[0] == 'X') 
        return 1;
}

int wordsz(const char *addr) {
    return strlen(addr) / 2;
}

void Pass1(struct Instruction *instructions, struct Optab *optab, struct SYMTAB *symtab) 
{
    for (int i = 0; i < input_count; ++i) {
            strcpy(symtab[symtab_count].symbol, instructions[i].label);
            symtab[symtab_count].address = LOCCTR;
            (symtab_count)++;
     
            int format = -1;
            for (int j = 0; j < optab_count; ++j) 
            {
                if (strcmp(instructions[i].operation, optab[j].mnemonic) == 0) 
                {
                    format = optab[j].format;
                    break;
                }
            }

            if (format == -1) 
            {
                if (strcmp(instructions[i].operation, "RESW") == 0) 
                    LOCCTR += atoi(instructions[i].operand) * 3;
                else if (strcmp(instructions[i].operation, "RESB") == 0) 
                    LOCCTR += atoi(instructions[i].operand);
                else if (strcmp(instructions[i].operation, "WORD") == 0) 
                    LOCCTR += wordsz(instructions[i].operand);
                else if (strcmp(instructions[i].operation, "BYTE") == 0) 
                    LOCCTR += bytesz(instructions[i].operand);
                else if (instructions[i].operation[0] == '+') 
                    LOCCTR += 4;
            } 
            else if(format!=-1)
            {
                LOCCTR += format;
            }
    }
}

char* decimalToHexadecimal(int decimalNumber) {
    char *hexadecimalNumber = (char *)malloc(5 * sizeof(char));
    int index = 0;
    while (decimalNumber > 0) {
        int remainder = decimalNumber % 16;
        if (remainder < 10) {
            hexadecimalNumber[index] = remainder + '0';
        } else {
            hexadecimalNumber[index] = remainder + 'A' - 10;
        }
        index++;
        decimalNumber /= 16;
    }
    while (index < 4) {
        hexadecimalNumber[index++] = '0';
    }
    hexadecimalNumber[index] = '\0';
    char *result = (char *)malloc((index + 1) * sizeof(char));
    int i, j = 0;
    for (i = index - 1; i >= 0; i--) {
        result[j++] = hexadecimalNumber[i];
    }
    result[j] = '\0';
    free(hexadecimalNumber);
    return result;
}

int main() {
    const char *filename = "input.txt";
    const char *filename1 = "instructions.txt";
    FILE *symtabFile;
    FILE *interFile;
    struct Instruction instructions[100];
    struct Optab optab[100];
    struct SYMTAB symtab[100];

    OptabFile(filename1, optab);
    InputFile(filename, instructions);
    Pass1(instructions, optab, symtab);

    symtabFile = fopen("symtab.txt", "w");
    for (int i = 0; i < symtab_count; ++i) {
        char *hexAddress = decimalToHexadecimal(symtab[i].address);
        if((strcmp(symtab[i].symbol, "*"))!=0)
            fprintf(symtabFile, "%s\t%s\n", symtab[i].symbol, hexAddress);
        free(hexAddress);
    }
    interFile = fopen("intermediate.txt", "w");
    for (int i = 0; i < symtab_count; ++i) {
        char *hexAddress = decimalToHexadecimal(symtab[i].address);
        if((strcmp(instructions[i].label, "*"))==0)
            strcpy(instructions[i].label, "");
        if((strcmp(instructions[i].operand, "*"))==0)
            strcpy(instructions[i].operand, "");
        // if((strcmp(instructions[i].operation, "BASE"))==0)
        //     fprintf(interFile, "\t%s\t%s\t%s\n", instructions[i].label, instructions[i].operation, instructions[i].operand);
        // else
            fprintf(interFile, "%s\t%s\t%s\t%s\n", hexAddress, instructions[i].label, instructions[i].operation, instructions[i].operand);
        free(hexAddress);
    }

    return 0;
}
