#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>

#include "types.h" // Contains user defined types


typedef struct DecodeInfo
{
    /*Decoding Image info*/
    char *decode_image_fname;
    FILE *fptr_decode_image;

    /*SECRET FILE*/
    int extn_secret_size;
    int secret_data_size;
    char extn_secret_file[5];

    /*SECRET DATA*/
    //int data_size;

    /*OUTPUT*/
    char output_fname[30];
    FILE *fptr_output_file;

} DecodeInfo;

/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

// /* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

// /* Extract Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

// /*Extract extension size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

// /* Extract secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

// /* Extract secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

// /* Extract secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

// /* Extract a byte into LSB of image data array */
Status decode_byte_from_lsb(char *data, char *image_buffer);

// // Extract a size to lsb
Status decode_size_from_lsb( char *Buffer,int *size);


#endif
