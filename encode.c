#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include "common.h"
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    long int offset=ftell(fptr);
    return offset;
    // Find the size of secret file data
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char *src_bmp_point;  //--get the address of  first occurance of charcter in string(.bmp)
    
    if((src_bmp_point=strstr(argv[2],".bmp"))!=NULL)
    {
        if((strcmp(src_bmp_point,".bmp"))==0)
        {
            encInfo->src_image_fname=argv[2];
        }
        else
        {
            printf("❌ Invalid file! No extension found.(.bmp)\n");
            return e_failure;
        }
    }
    else
    {
        printf("The file extension missing(.bmp)\n");
        return e_failure;
    }
    char *txt_point=strstr(argv[3],".");
    if((txt_point)!=NULL)
    {
        
        if((strcmp(txt_point,".txt"))==0)
        {
            strcpy(encInfo->extn_secret_file,txt_point);
            encInfo->secret_fname=argv[3];
        }
        else if((strcmp(txt_point,".c"))==0)
        {
            strcpy(encInfo->extn_secret_file,txt_point);
            encInfo->secret_fname=argv[3];
        }
        else if((strcmp(txt_point,".h"))==0)
        {
            strcpy(encInfo->extn_secret_file,txt_point);
            encInfo->secret_fname=argv[3];
        }
        else if((strcmp(txt_point,".sh"))==0)
        {
            strcpy(encInfo->extn_secret_file,txt_point);
            encInfo->secret_fname=argv[3];
        }
        else
        {
            printf("❌ Invalid source file! No extension found.(.txt)\n");
            return e_failure;
        }
    }
    else
    {
        printf("The file extension is missing(.txt,.c,.h----etc)\n");
        return e_failure;
    }
    if(argv[4]!=NULL)  //check int default string have anything or not if it have if condition true or else true
    {
        char *dest_bmp_point;
        
        if((dest_bmp_point=strstr(argv[4],".bmp"))!=NULL)
        {
            if((strcmp(dest_bmp_point,".bmp"))==0)
            {
                encInfo->dest_image_fname=argv[4];
            }
            else
            {
                printf("❌ Invalid  destion file! No extension found.(.bmp)\n");
                return e_failure;
            }
        }
        else
        {
            printf("The file extension missing\n");
            return e_failure;
        }
    }
    else
    {
        encInfo->dest_image_fname="default.bmp";
    }
    return e_success;
}


Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
        
        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_dest_image = fopen(encInfo->dest_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_dest_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->dest_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);  // function call for image capacity(source file)
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);            //function call for secret file capacity
    int hidden_file_size=(encInfo->size_secret_file)*8+(2*8)+(4*8)+(4*8)+(4*8);
    if(encInfo->image_capacity>hidden_file_size)
    {
        //--success
        return e_success;
    }
    else
    {
        printf("image_capacity is lessthan the hidden_file_size data\n");
        //--failure
        return e_failure;
    }

}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
   char buffer_size[54];
   rewind(fptr_src_image);
   fread(buffer_size,54,1,fptr_src_image);
   fwrite(buffer_size,54,1,fptr_stego_image);
   if(ftell(fptr_src_image) == ftell(fptr_stego_image))
   {
        return e_success;
   }
   else
   {
        printf("Both files offset is not in same position\n");
        return e_failure;
   }
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char magic_buffer[8];
    int magic_length=strlen(magic_string);
    for(int i=0;i<magic_length;i++)
    {
        fread(magic_buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i],magic_buffer);
        fwrite(magic_buffer,8,1,encInfo->fptr_dest_image);
    }
    printf("Successfully copy the magic_buffer data into destination file\n");
    if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_dest_image))
    {
        printf("Both file offsets are equal\n");
        return e_success;
    }
    else
    {
        printf("Both file offsets are not equal\n");
        return e_failure;
    }
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char secret_size_buffer[32];
    fread(secret_size_buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(size, secret_size_buffer);
    fwrite(secret_size_buffer,32,1,encInfo->fptr_dest_image);
    printf("The secret_size_buffer data successfuly copied into destination file\n");
    if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_dest_image))
    {
        printf("Both file offsets are equal\n");
        return e_success;
    }
    else
    {
        printf("Both file offsets are not equal\n");
        return e_failure;
    }
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char secret_extn_buffer[8];
    for(int i=0;i<strlen(file_extn);i++)
    {
        fread(secret_extn_buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i],secret_extn_buffer);
        fwrite(secret_extn_buffer,8,1,encInfo->fptr_dest_image);
    }
    printf("Sucessfully copied the secret_file_buffer into destination file\n");
     if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_dest_image))
    {
        printf("Both file offsets are equal\n");
        return e_success;
    }
    else
    {
        printf("Both file offsets are not equal\n");
        return e_failure;
    }
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char secret_data_size_buffer[32];
    fread(secret_data_size_buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, secret_data_size_buffer);
    fwrite(secret_data_size_buffer,32,1,encInfo->fptr_dest_image);
    printf("The secret_data_size_buffer data successfuly copied into destination file\n");
    if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_dest_image))
    {
        printf("Both file offsets are equal\n");
        return e_success;
    }
    else
    {
        printf("Both file offsets are not equal\n");
        return e_failure;
    }

}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);
    //storing secret file data into charcter string
    char secret_data[encInfo->size_secret_file];  
    fread(secret_data,encInfo->size_secret_file,1,encInfo->fptr_secret);
    char secret_data_buffer[8];
    for(int i=0;i<encInfo->size_secret_file;i++)
    {
        fread(secret_data_buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(secret_data[i], secret_data_buffer);
        fwrite(secret_data_buffer,8,1,encInfo->fptr_dest_image);
    }
    printf("Total Secret data successfully copied into destination file");
    if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_dest_image))
    {
        printf("Both file offsets are equal\n");
        return e_success;
    }
    else
    {
        printf("Both file offsets are not equal\n");
        return e_failure;
    }
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while(fread(&ch,1,1,fptr_src)>0)
    {
        fwrite(&ch,1,1,fptr_dest);
    }
    printf("Remaining data in source file successfully copied into destination file\n");
    if(ftell(fptr_src) == ftell(fptr_dest))
    {
        printf("Both file offsets are equal\n");
        return e_success;
    }
    else
    {
        printf("Both file offsets are not equal\n");
        return e_failure;
    }
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
   for(int i=0;i<8;i++)
   {
    int clear_bit=(image_buffer[i])&(~1);
    int get_bit=(data>>i)&1;
    image_buffer[i]=clear_bit|get_bit;
      ///printf("clear_bit=%d\nget_bit=%d\n",clear_bit,get_bit);
        //printf("*****************%d",i);
        //printf("first value of =%x\n",image_buffer[i]);
      //printf("last_value of =%x\n",image_buffer[i]);
   }
}

Status encode_size_to_lsb(int size, char *image_Buffer)
{
    for(int i=0;i<32;i++)
    {
        int clear_bit=(image_Buffer[i])&(~1);
        int get_bit=(size>>i)&1;
        image_Buffer[i]=clear_bit|get_bit;
    }
}

Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_success)
    {
        printf("Successfully files opened\n");
    }
    else
    {
       return e_failure;
    }
    if(check_capacity(encInfo) == e_success)
    {
        printf("image_capacity is greaterthan the hidden file data\n");
    }
    else
    {
        return e_failure;
    }
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_dest_image) == e_success)
    {
        printf("Source file header is successfully copyed into destination file\n");
    }
    else
    {
        return e_failure;
    }
    if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
    {
        printf("Magic_string successfully encoded\n");
    }
    else
    {
        return e_failure;
    }
    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo) == e_success)
    {
        printf("Secret file extension size successfully encoded\n");
    }
    else
    {
        return e_failure;
    }
    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
    {
        printf("Secret file extension  successfully encoded\n");
    }
    else
    {
        return e_failure;
    }
    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)== e_success)
    {
        printf("Secret file data size  successfully encoded\n");
    }
    else
    {
        return e_failure;
    }
    if(encode_secret_file_data(encInfo) == e_success)
    {
        printf("Secret file data  successfully encoded\n");
    }
    else
    {
        return e_failure;
    }
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_dest_image) == e_success)
    {
        printf("All operation completed successfully!\n");
    }
    else
    {
        return e_failure;
    }
    return e_success;
}
