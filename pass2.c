#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE_LENGTH 100

int intermediate_count=0,optab_count=0,register_count=0,relocate_count=0,pccheck = 0,pc = 0,base = 0,index_val = 0;

struct Intermediate {
    char addres[10];
    int address;
    char label[10];
    char operation[10];
    char operand[10];
    char object[10];
};
struct Optab {
    char mnemonic[10];
    int format;
    char opcode[10];
};
struct RelocateProg {
    int address;
};
struct Registers {
    char name[3];
    int code;
};

struct Intermediate intermediate[100];
struct Optab optab[100];
struct Registers registers[10];
struct RelocateProg relocateprog[100];

void OptabFile(const char *filename)
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

void RegisterFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];

    // Membaca setiap baris dari file
    while (fgets(line, sizeof(line), file) != NULL) {
        sscanf(line, "%s %d", registers[register_count].name, &registers[register_count].code);
        (register_count)++;
    }
    fclose(file);
}

void InputFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';

        if(sscanf(line, "%s %s %s %s", intermediate[intermediate_count].addres, intermediate[intermediate_count].label, intermediate[intermediate_count].operation, intermediate[intermediate_count].operand)==4){
        }
        else if(sscanf(line, "%s %s %s", intermediate[intermediate_count].addres, intermediate[intermediate_count].operation,intermediate[intermediate_count].operand)==3){
            strcpy(intermediate[intermediate_count].label, "*");
        }
        else if(sscanf(line, "%s %s", intermediate[intermediate_count].addres, intermediate[intermediate_count].operation)==2){
            strcpy(intermediate[intermediate_count].label, "*");
            strcpy(intermediate[intermediate_count].operand, "*");
        }
        intermediate_count++;
    }

    fclose(file);
}

int hextoint(char *hexstring) {
    return strtol(hexstring, NULL, 16);
}

char *bintohex(int a, int b, int c, int d) {
    char *s = (char *)malloc(2 * sizeof(char));
    int sum = 0;
    sum += d * 1;
    sum += c * 2;
    sum += b * 4;
    sum += a * 8;
    sprintf(s, "%X", sum);
    return s;
}

char *intToHex(int x, int b) {
    char *s = (char *)malloc((b + 1) * sizeof(char));
    // Format the hexadecimal representation with leading zeros
    sprintf(s, "%0*X", b, x);
    // Convert to uppercase
    for (int i = 0; i < b; i++) {
        if (s[i] >= 'a' && s[i] <= 'f') {
            s[i] -= 32;
        }
    }
    return s;
}

char *readdrs(char *disp) {
    int x = strtol(disp, NULL, 16);
    if (x - pc > -256 && x - pc < 4096) {
        pccheck = 1;
        return intToHex(x - pc,3);
    } else {
        pccheck = 0;
        return intToHex(x - base,3);
    }
}

void format2(char *result, char *opcode, char *r1, char *r2) {
    char *opcodeValue = "";
    for (int i = 0; i < optab_count; i++) {
        if (strcmp(opcode, optab[i].mnemonic) == 0) {
            opcodeValue = optab[i].opcode;
            break;
        }
    }
    r2=&r1[2];
    r1[1]='\0';
    char r1Code[3] = ""; 
    for (int i = 0; i < 8; i++) {
        if (strcmp(r1, registers[i].name) == 0) {
            sprintf(r1Code, "%d", registers[i].code);
            break;
        }
    }

    char r2Code[3] = ""; 
    if (strlen(r2) > 0) {
        for (int i = 0; i < 8; i++) {
            if (strcmp(r2, registers[i].name) == 0) {
                sprintf(r2Code, "%d", registers[i].code);
                break;
            }
        }
    } else {
        sprintf(r2Code, "0");
    }
    sprintf(result, "%s%s%s", opcodeValue, r1Code, r2Code);
}

int findbase(char *basemn)
{
    for(int i=0;i<intermediate_count;i++)
    {
        if(strcmp(basemn,intermediate[i].label)==0)
            return i;
    }
    return -1;
}

int findoptab(char *mnemonic)
{
    int i;
    for(i=0;i<optab_count;i++)
    {
        if(strcmp(optab[i].mnemonic,mnemonic)==0)
        {
            return i;
        }
    }
    return -1;
}

void format3(char* result, int i)
{
    char addrs[10], res1[10],res2[10],res3[10];
    int flags[6]={}, dr=0, no=0;
    strcpy(addrs, intermediate[i].operand);
    if(addrs[strlen(addrs)-1]=='X'&&addrs[strlen(addrs)-2]==',')
    {
        flags[2]=1;
        addrs[strlen(addrs) - 1] = '\0';
        addrs[strlen(addrs) - 1] = '\0';
    }
    if(addrs[0]=='#')
    {
        flags[1]=1;
        for (int k = 0; k < strlen(addrs); ++k) {
            addrs[k] = addrs[k + 1];
        }
        if(findbase(addrs)!=-1)
            strcpy(res2,intToHex(intermediate[findbase(addrs)].address,3));
        else
        {
            strcpy(res2, addrs);
            dr=1;
        }
        no=1;
    }
    else if(addrs[0]=='@')
    {
        flags[0]=1;
        for (int k = 0; k < strlen(addrs); ++k) 
            addrs[k] = addrs[k + 1];
        no=2;
        int z=intermediate[findbase(addrs)].address,j;
        for(j=0;j<intermediate_count;j++)
            if(intermediate[j].address==z)
                break;
        strcpy(res2,addrs);
        strcpy(addrs,intToHex(intermediate[j].address,3));
        if(strcmp(intermediate[j].operation,"WORD")!=0&&strcmp(intermediate[j].operation,"BYTE")!=0&&strcmp(intermediate[j].operation,"RESW")!=0&&strcmp(intermediate[j].operation,"RESB")!=0)
        {
            strcpy(addrs, intermediate[j].operand);
            z=intermediate[findbase(addrs)].address;
            for(j=0;j<intermediate_count;j++)
                if(strcpy(intermediate[j].label,res2)==0)
                    break;
            strcpy(addrs, intermediate[j].operand);
            strcpy(res2, intToHex(intermediate[findbase(addrs)].address,3));
        }
        else
            strcpy(res2, addrs);
    }
    else if(addrs[0]=='=')
    {
        for (int i = 3; i <= strlen(addrs)-4; ++i) {
            addrs[i] = addrs[i + 4];
        }
        dr=1;
    }
    else
    {
        if(strcmp(addrs,"*")!=0)
            strcpy(res2, intToHex(intermediate[findbase(addrs)].address,3));
        else
            strcpy(res2, intToHex(0,3));
        flags[0]=1;
        flags[1]=1;
        no=3;
    }
    if(dr!=1 && strcmp(addrs,"*")!=0)
    {
        strcpy(res2,readdrs(res2));
        if(strlen(res2)>3)
        {
            int p=0;
            for (int k = strlen(res2) - 3; k <= strlen(res2); ++k) {
                res2[p++] = res2[k];  
            }
        }
        flags[4] = pccheck;
        flags[3] = !pccheck;
    }
    if(flags[2]==1)
    {
        strcpy(res2, intToHex(hextoint(res2)-index_val,3));
    }
    while(strlen(res2)<3)
    {
        char result[5];  
        strcpy(result, "0");
        strcat(result, res2);
        strcpy(res2, result);
    }
    strcpy(res3,optab[findoptab(intermediate[i].operation)].opcode);
    strcpy(res3,intToHex(hextoint(res3)+no,2));
    strcat(res3,bintohex(flags[2], flags[3], flags[4], flags[5]));
    strcat(res3,res2);
    sprintf(result, "%s", res3);
}

void format4(char * result,int i)
{
    char addrs[10], TA[10]="", obcode[10];
    int no=0, flags[6]={0,0,0,0,0};
    strcpy(addrs, intermediate[i].operand);
    if(addrs[0]=='@')
        flags[0]=1;
    if(addrs[0]=='#')
        flags[1]=1;
    if(flags[0]==flags[1])
    {
        flags[0]=!flags[0];
        flags[1]=!flags[1];
    }
    if (addrs[strlen(addrs) - 1] == 'X' && addrs[strlen(addrs) - 2] == ',') {
        flags[2] = 1;
    } 
    flags[3]=flags[4]=0;flags[5]=1;
    if(addrs[0]=='@'||addrs[0]=='#'){
        for (int k = 0; k < strlen(addrs) - 1; k++) {
            addrs[k] = addrs[k + 1];
        }
        addrs[strlen(addrs) - 1] = '\0';
    }
    if(addrs[strlen(addrs) - 1] == 'X' && addrs[strlen(addrs) - 2] == ',')
    {
        addrs[strlen(addrs)-1]='\0';
        addrs[strlen(addrs)-1]='\0';
    }
    if(flags[0]==1&&flags[1]==1)
    {
        char s[10];
        strcpy(s,intToHex(intermediate[findbase(addrs)].address,5));
        for(int k=0;k<intermediate_count;k++)
        {
            if(strcmp(intToHex(intermediate[k].address,5),s)==0)
            {
                if(flags[2]==0)
                    strcpy(TA,s);
                else
                    strcpy(TA, intToHex(hextoint(s)+index_val,5));
            }
        }
        no=3;
    }
    else if(flags[0]==1&&flags[1]==0&&flags[2]==0)
    {
        char s[10],temp[10],temp2[10];
        sprintf(temp,"%d",intermediate[findbase(addrs)].address);
        strcpy(s,temp);
        sprintf(temp2,"%d",intermediate[i].address);
        for(int k=0;k<intermediate_count;k++)
        {
            sprintf(temp2,"%d",intermediate[k].address);
            if(strcmp(temp2,s)==0)
            {
                strcpy(s, intermediate[k].operand);
                for(int j=0;j<intermediate_count;j++)
                {
                    sprintf(temp2,"%d",intermediate[j].address);
                    if(strcmp(temp2,s)==0)
                        strcpy(TA, intermediate[j].operand);
                }
                    
            }
        }
        no=2;
    }
    else if( flags[0]==0&&flags[1]==1)
    {
        if(addrs[0]<65)
            strcpy(TA,intToHex(atoi(addrs),5));
        else
            strcpy(TA,intToHex(intermediate[findbase(addrs)].address,5));
        no=1;
    }

    for (int k = 0; k < strlen(intermediate[i].operation) - 1; k++) {
        intermediate[i].operation[k] = intermediate[i].operation[k + 1];
    }
    intermediate[i].operation[strlen(intermediate[i].operation) - 1]='\0';
    char res3[10];
    strcpy(res3,optab[findoptab(intermediate[i].operation)].opcode);
    strcpy(res3,intToHex(hextoint(res3)+no,2));
    strcat(res3,bintohex(flags[2],flags[3],flags[4],flags[5]));
    strcat(res3,TA);
    sprintf(result, "%s", res3);
}

void pass2()
{
    for(int i=0;i<intermediate_count;i++)
    {
        char s[10]="";
        int j;
        for(j=i;j<intermediate_count;j++)
        {
            if(intermediate[j].address != intermediate[i].address)
                break;
        }
        pc=intermediate[j].address;
        char s2[10];
        strcpy(s2,intermediate[i].operation);
        int format=optab[findoptab(intermediate[i].operation)].format;
        if(format==1)
            strcpy(s,optab[findoptab(intermediate[i].operation)].opcode);
        else if(format==2)
            format2(s, intermediate[i].operation, intermediate[i].operand, "");
        else if(format==3)
            format3(s, i);
        else if(s2[0]=='+')
        {
            format4(s, i);
            char tt[10];
            strcpy(tt, intermediate[i].operand);
            if(tt[0]!='#')
                relocateprog[relocate_count++].address = intermediate[i].address;
        }
        if(strcmp(intermediate[i].operation,"BASE")==0)
            base=intermediate[findbase(intermediate[i].operand)].address;
        else if(strcmp(intermediate[i].operation,"BYTE")==0)
        {
            char adr[10];
            strcpy(adr, intermediate[i].operand);
            strcpy(s, adr + 2);
            int le=strlen(s);
            s[le-1]='\0';
            char s3[10] = "";
            for (int k = 0; k < strlen(s); k++) {
                char c[2] = {s[k], '\0'};
                strcat(s3, intToHex(c[0],2));
            }
            if (adr[0] == 'C') {
                strcpy(s, s3);
            }
        }
        strcpy(intermediate[i].object, s);       
    }
}

int min(int a, int b) {
    return (a < b) ? a : b;
}

void output(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    int flag=0;
    int sz=intermediate_count;
    fprintf(file, "H%s",intermediate[0].label);
    for(int i=0;i<6-strlen(intermediate[0].label);i++)
        fprintf(file, " ");
    fprintf(file, "%s%s\n", intToHex(intermediate[0].address,6), intToHex(intermediate[intermediate_count-1].address-intermediate[0].address,6));
    int check=0;
    for(int i=0;i<intermediate_count;i+=check)
    {
            flag=0;
            check=0;
            long long sum=0;
            for(int j=i;j<i+min(sz-i,16);j++)
            {
                if(sum>29)
                {
                    flag--;
                    sum-=strlen(intermediate[j-1].object)/2;
                    break;
                }
                sum+=strlen(intermediate[j].object)/2;
                flag++;
                if(strcmp(intermediate[j].label,"BUFFER")==0)
                    break;
            }
            fprintf(file,"T%s%s",intToHex(intermediate[i].address,6),intToHex(sum,2));
            for(int j=i;j<i+flag;j++)
            {
                fprintf(file, "%s", intermediate[j].object);
                check++;
                if(strcmp(intermediate[j].label,"BUFFER")==0)
                {
                    break;
                }
            }
            fprintf(file,"\n");
    }
    for(int i=0;i<relocate_count;i++)
    {
        fprintf(file,"M%s05\n",intToHex(relocateprog[i].address+1,6));
    }
    fprintf(file,"E%s",intToHex(intermediate[0].address,6));
}

int main() {
    const char *filename = "intermediate.txt";
    const char *filename1 = "instructions.txt";
    const char *filename2 = "registers.txt";
    const char *filename3 = "output.txt";

    OptabFile(filename1);
    RegisterFile(filename2);
    InputFile(filename);
    for(int i=0;i<intermediate_count;i++)
    {
        intermediate[i].address=hextoint(intermediate[i].addres);
    }
    pass2();
   //for(int i=0;i<obcode_count;i++)
        //printf("%d\n",intermediate[intermediate_count-1].address);
    output(filename3);
}