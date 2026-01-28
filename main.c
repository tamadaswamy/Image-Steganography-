#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"
#include<string.h>
OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
    
    int res;
        if(argc>=2)
        {
            res=(check_operation_type(argv[1]));
        }
        else
        {
            printf("checking for encoding and decoding minimum arguments should be 2\n");
            return e_failure;
        }
        
        if( res == e_encode)
        {
            if(argc>=4)
            {
                EncodeInfo encInfo;
                if((read_and_validate_encode_args(argv, &encInfo)) == e_success)
                {
                    printf("Successfully file stored in respective structure members\n");
                    if(do_encoding(&encInfo) == e_success)
                    {
                        printf("--------------------------------------\n");
                        printf("!!Successfully encoding completed!!\n");
                        printf("--------------------------------------\n");

                    }
                    else
                    {
                        return e_failure;
                    }
                }
                else
                {
                    return e_failure;
                }
                
            }
            else
            {
                printf("!!!HELP\n");
                printf("for encoding ./a.out -e beautiful.bmp secret.txt optional(destination file)\n");
                return e_failure;
            }
            
        }
        else if(res == e_decode)
        {
            if(argc>=3)
            {
                DecodeInfo decInfo;
                if((read_and_validate_decode_args(argv,&decInfo)) == e_success)
                {

                    printf("Successfully file names are stored in structure memebers\n");
                    if(do_decoding(&decInfo)== e_success)
                    {
                        printf("----------------------------------------\n");
                        printf("!! successfully completed decoding  !!\n");
                        printf("----------------------------------------\n");
                        return e_success;
                    }
                    else
                    {
                    
                        return e_failure;
                    }
                }
                else
                {
                    return e_failure;
                }
            }
            else
            {
                printf("for decoding ./a.out -d  destination file optional\n");
                return e_failure;
            }
           
        }
        
    
    else
    {
       
       
    }
    
}

OperationType check_operation_type(char *symbol)
{
    if((strcmp(symbol,"-e"))==0)
    {
        printf("-------------------------\n");
        printf("    Do encoding\n");
        printf("-------------------------\n");
        return e_encode;
    }
    else if((strcmp(symbol,"-d"))==0)
    {
        printf("-------------------------\n");
        printf("    Start decoding\n");
        printf("-------------------------\n");
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}
