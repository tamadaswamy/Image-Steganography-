#include <stdio.h>
#include "decode.h"
#include "types.h"
#include<string.h>
#include "common.h"

/* Function Definitions */



Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    
    char *decode_point;
    if((decode_point=strstr(argv[2],"."))!=NULL)
    {
        if(strcmp(decode_point,".bmp")==0)
        {
            decInfo->decode_image_fname=argv[2];
        }
        else
        {
            printf("❌ Invalid file! No extension found.(.bmp)\n");
            return e_failure;
        }
        
    }
    else
    {
        printf("Invalid! The file extension should be .extension\n");
        return e_failure;
    }
    if(argv[3]!=NULL)
    {
        char *output_token=strtok(argv[3],".");
        strcpy(decInfo->output_fname,output_token);
    }
    else
    {
        strcpy(decInfo->output_fname,"output");
        //printf("%s",decInfo->output_fname);
    }
    return e_success;
}

Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->fptr_decode_image=fopen(decInfo->decode_image_fname,"r");
    if(decInfo->fptr_decode_image==NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->decode_image_fname);
        
        return e_failure;
    }
    fseek(decInfo->fptr_decode_image,54,SEEK_CUR);
    if(ftell(decInfo->fptr_decode_image)==54)
    {
        printf("Successfully skip 54 bytes of header\n");
        return e_success;
    }
    else
    {
        printf("The doesn't skip the 54 bytes header\n");
        return e_failure;
    }
}

Status decode_magic_string(const char *magic_string,DecodeInfo *decInfo)
{
    int magic_length=strlen(magic_string); 
    char decode_magic_string[magic_length+1];
    char ch;
    char magic_string_buffer[8];
    for(int i=0;i<magic_length;i++)
    {
        fread(magic_string_buffer,8,1,decInfo->fptr_decode_image);
        decode_byte_from_lsb(&ch, magic_string_buffer);
        decode_magic_string[i]=ch;
    }
    decode_magic_string[magic_length]='\0';
    // printf("%s",decode_magic_string);
    if(strcmp(magic_string,decode_magic_string) == 0)
    {
        printf("The magic string matched\n");
        return e_success;
    }
    else
    {
        printf("The magic string doesn't match\n");
        return e_failure;
    }

}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char exten_size_buffer[32];
    fread(exten_size_buffer,32,1,decInfo->fptr_decode_image);
    int size=0;
    decode_size_from_lsb(exten_size_buffer,&size);
    // printf("%d",size);
    decInfo->extn_secret_size=size;
    return e_success;

}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char decode_secret_extn_buffer[decInfo->extn_secret_size+1];
    char extn_buffer[8];
    char ch;
    for(int i=0;i<decInfo->extn_secret_size;i++)
    {
        fread(extn_buffer,8,1,decInfo->fptr_decode_image);
        decode_byte_from_lsb(&ch, extn_buffer);
        decode_secret_extn_buffer[i]=ch;
    }
    decode_secret_extn_buffer[decInfo->extn_secret_size]='\0';
    if(strlen(decode_secret_extn_buffer) ==decInfo->extn_secret_size)
    {
        strcat(decInfo->output_fname,decode_secret_extn_buffer);
        decInfo->fptr_output_file=fopen(decInfo->output_fname,"w");
        printf("extract extension length is equal to the extension size\n");
        if(decInfo->fptr_output_file==NULL)
        {
            perror("fopen");
            fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->decode_image_fname);
        
            return e_failure;
        }
        return e_success;
    }
    else
    {
        printf("extract extension length is not  equal to the extension size\n");
        e_failure;
    }

}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char secret_data_size[32];
    int size=0;
    fread(secret_data_size,32,1,decInfo->fptr_decode_image);
    decode_size_from_lsb(secret_data_size,&size);
    decInfo->secret_data_size=size;
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char secret_data_buffer[8];
    char secret_file_data[decInfo->secret_data_size+1];
    char ch;
    for(int i=0;i<decInfo->secret_data_size;i++)
    {
        fread(secret_data_buffer,8,1,decInfo->fptr_decode_image);
        decode_byte_from_lsb(&ch, secret_data_buffer);
        secret_file_data[i]=ch;
    }
    secret_file_data[decInfo->secret_data_size]='\0';
    if(strlen(secret_file_data)==decInfo->secret_data_size)
    {
        fwrite(secret_file_data,decInfo->secret_data_size,1,decInfo->fptr_output_file);
        printf("Extract secret data length is equal to secret data size\n");
        return e_success; 
    }
    else
    {
        printf("Extract secret data length is not equal to secret data size\n");
        return e_failure;
    }
}
Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    char res=0;
    for(int i=0;i<8;i++)
    {
        int get_bit=image_buffer[i]&1;
        res=res|(get_bit<<i);
    }
    *data=res;
}

Status decode_size_from_lsb(char *buffer,int *size)
{
    int res=0;
    for(int i=0;i<32;i++)
    {
        int get_bit=buffer[i]&1;
        res=res|(get_bit<<i);
    }
    *size=res;
}
Status do_decoding(DecodeInfo *decInfo)
{
    if(open_decode_files(decInfo) == e_success)
    {
        printf("Successfully files opened\n");
    }
    else
    {
        return e_failure;
    }
    if(decode_magic_string(MAGIC_STRING,decInfo) == e_success)
    {
        printf("Magic string successfully decoded\n");
    }
    else
    {
        return e_failure;
    }
    if(decode_secret_file_extn_size(decInfo)== e_success)
    {
        printf("Secret file extension successfully decoded\n");
    }
    else
    {
        return e_failure;
    }
    if(decode_secret_file_extn(decInfo) == e_success)
    {
        printf("successfully secret file extension decoded and file extension add to the output file and file opend\n");
    }
    else
    {
        return e_failure;
    }
    if(decode_secret_file_size(decInfo) == e_success)
    {
        printf("successfully extract the secret file data size\n");
    }
    else
    {
        return e_failure;
    }
    if(decode_secret_file_data(decInfo) == e_success)
    {
        printf("successfully secret data decoded and stored in output file\n");
        fclose(decInfo->fptr_output_file);
        fclose(decInfo->fptr_decode_image);
        
    }
    else
    {
        return e_failure;
    }
    return e_success;
}

