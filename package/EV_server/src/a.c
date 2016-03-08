#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/aes.h>
#define AES_BITS 128
#define MSG_LEN 128
#include "data.h"
 u8 Plaintext[19] =
  {
    /*0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
    0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11,
    0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17,*/
		0xab, 0x23, 0x89, 0xff, 0x1f, 0x90, 0xef, 0x89, 
		0x83, 0xa8, 0x5d, 0x67, 0x31, 0xed, 0x00, 0x89,
		0x90, 0xff, 0x12
  };

/* Key to be used for AES encryption/decryption */
u8 Key[16] = 
  {
    /*0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
    0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
    0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b,*/
		0xc4, 0x8C, 0xAE, 0x77, 0x18, 0x83, 0x94,0xF3,
    0x12, 0x4F, 0x1C, 0xF9, 0x12, 0x9A, 0x4B, 0x45
  };

/* Initialization Vector */
u8 IV[16] =
  {
   /* 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,*/
		0xc4, 0x8C, 0xAE, 0x77, 0x18, 0x83, 0x94, 0xF3,
    0x12, 0x4F, 0x1C, 0xF9, 0x12, 0x9A, 0x4B, 0x45
  };

u8 Expected_Ciphertext[] =
  {
 /*   0x4f, 0x02, 0x1d, 0xb2, 0x43, 0xbc, 0x63, 0x3d,
    0x71, 0x78, 0x18, 0x3a, 0x9f, 0xa0, 0x71, 0xe8,
    0xb4, 0xd9, 0xad, 0xa9, 0xad, 0x7d, 0xed, 0xf4,
    0xe5, 0xe7, 0x38, 0x76, 0x3f, 0x69, 0x14, 0x5a,
    0x57, 0x1b, 0x24, 0x20, 0x12, 0xfb, 0x7a, 0xe0,
    0x7f, 0xa9, 0xba, 0xac, 0x3d, 0xf1, 0x02, 0xe0,
    0x08, 0xb0, 0xe2, 0x79, 0x88, 0x59, 0x88, 0x81,
    0xd9, 0x20, 0xa9, 0xe6, 0x4f, 0x56, 0x15, 0xcd,*/ 
		0xf0, 0x41, 0x47, 0x1d, 0xa4, 0x3e, 0x3c, 0x36,
		0xba, 0x70, 0x69, 0x6f, 0x62, 0xdb, 0xa1, 0x75, 
		0xa0, 0x1f, 0x37, 0x99, 0x2e, 0x92, 0x5c, 0x49, 
		0xe3, 0x0a, 0x42, 0xa4, 0x21, 0x5d, 0x12, 0xfb 
};

u32  Remainder = 0;

void Padding(u8 *InputMessage,u32 InputMessageLength ,u8 *OutputMessage,u32 *OutputMessageLength)
{
		u32 len;	
		static u32 Modulu;
		for(len = 0;len < InputMessageLength;len++)
		{
			OutputMessage[len] = InputMessage[len];
		}

		if(InputMessageLength >= 16)
		{
			Remainder = InputMessageLength%16;
			if(Remainder == 0)
			{
				*OutputMessageLength = InputMessageLength + 16;
				for(len = (*OutputMessageLength) - 1;len >= *OutputMessageLength - 16; len--)
				{
					OutputMessage[len] = 16;
				}
			}
			else 
			{
				 Modulu = (16 - Remainder);
				 *OutputMessageLength = InputMessageLength + Modulu;
				len = *OutputMessageLength - 1;
//				for(len = (*OutputMessageLength) - 1; len >= (*OutputMessageLength) - Remainder; len--)
				//{
//					OutputMessage[len] = Modulu;
//				}
				while(Modulu--)
				{
					OutputMessage[len--] = (16 - Remainder);
				}
		        }
		}
		else
		{
			Remainder = 16 - InputMessageLength;
			*OutputMessageLength = InputMessageLength + Remainder;
			for(len = (*OutputMessageLength) - 1; len >= (*OutputMessageLength) - Remainder; len--)
			{
				OutputMessage[len] = Remainder;
			}
		}
}		






void RePadding(u8 *InputMessage,u32 InputMessageLength ,u8 *OutputMessage,u32 *OutputMessageLength)
{
	static u8 Lastbyte = 0;		
	static int len;
	Lastbyte = InputMessage[InputMessageLength - 1];
	if(Lastbyte<=InputMessageLength)
	{
		*OutputMessageLength = InputMessageLength - Lastbyte;
		for(len = 0; len < *OutputMessageLength; len++)
		{
			OutputMessage[len] = InputMessage[len];
		}
	}
	else
	{
		printf("wrong padding length\r\n");
	}
}




int aes_encrypt(char* in, char* key, char* out, int len)//, int olen)可能会设置buf长度
{
    if(!in || !key || !out) return 0;
    unsigned char iv[AES_BLOCK_SIZE];//加密的初始化向量
	int i;  
   for( i=0; i<AES_BLOCK_SIZE; ++i)//iv一般设置为全0,可以设置其他，但是加密解密要一样就行
    	iv[i]=0;//IV[i];
    AES_KEY aes;
    if(AES_set_encrypt_key((unsigned char*)key, 128, &aes) < 0)
    {
        return 0;
    }
//    int len=strlen(in);//这里的长度是char*in的长度，但是如果in中间包含'\0'字符的话

    //那么就只会加密前面'\0'前面的一段，所以，这个len可以作为参数传进来，记录in的长度

    //至于解密也是一个道理，光以'\0'来判断字符串长度，确有不妥，后面都是一个道理。
    AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, len, &aes, iv, AES_ENCRYPT);
    return 1;
}
int my_aes_encrypt(char* in, char* key, char* out, int len)//, int olen)可能会设置buf长度
{
	int i, en_len=0;  
    	if(!in || !key || !out) return 0;
    
   	 AES_KEY aes;
    	if(AES_set_encrypt_key((unsigned char*)key, 128, &aes) < 0)
   	 {
       		 return 0;
    	}
//    int len=strlen(in);//这里的长度是char*in的长度，但是如果in中间包含'\0'字符的话
   	while(en_len < len)
	{
	   	AES_encrypt((unsigned char*)in, (unsigned char*)out, &aes);
		in += AES_BLOCK_SIZE;
		out +=AES_BLOCK_SIZE;
		en_len +=AES_BLOCK_SIZE;
	}
	 return 1;
}

int my_aes_decrypt(char* in, char* key, char* out, int len)  
{  
    	if(!in || !key || !out) return 0;  
    	AES_KEY aes;  
   	 if(AES_set_decrypt_key((unsigned char*)key, 128, &aes) < 0)  
   	 {  
        	return 0;  
    	}  
//    int len=strlen(in), en_len=0;  
	int en_len = 0;  
     	while(en_len<len)  
     	{  
        	AES_decrypt((unsigned char*)in, (unsigned char*)out, &aes);  
        	in+=AES_BLOCK_SIZE;
		printf("AES_BLOCK_SIZE:%d\n", AES_BLOCK_SIZE);  
        	out+=AES_BLOCK_SIZE;  
        	en_len+=AES_BLOCK_SIZE;  
   	 }  
   	 return 1;  
}  




int aes_decrypt(char* in, char* key, char* out, int len)
{
    if(!in || !key || !out) return 0;
    unsigned char iv[AES_BLOCK_SIZE];//加密的初始化向量
	int i; 
   for(i=0; i<AES_BLOCK_SIZE; ++i)//iv一般设置为全0,可以设置其他，但是加密解密要一样就行
    	iv[i]=0;//IV[i];
    AES_KEY aes;
    if(AES_set_decrypt_key((unsigned char*)key, 128, &aes) < 0)
    {
        return 0;
    }
//    int len=strlen(in);
    AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, len, &aes, iv, AES_DECRYPT);
    return 1;
}

int main(int argc,char *argv[])
{
    char sourceStringTemp[MSG_LEN];
    char dstStringTemp[MSG_LEN];
    memset((char*)sourceStringTemp, 0 ,MSG_LEN);
    memset((char*)dstStringTemp, 0 ,MSG_LEN);
//    strcpy((char*)sourceStringTemp, "abcdefghijklmn");
    //strcpy((char*)sourceStringTemp, argv[1]);

    char key[AES_BLOCK_SIZE];
    int i;
    for(i = 0; i < 16; i++)//可自由设置密钥
    {
        key[i] = Key[i];
    }
	
  int len = 0;
   Padding(Plaintext, 19, sourceStringTemp, &len);
    if(!aes_encrypt(sourceStringTemp,key,dstStringTemp, len))
    {
    	printf("encrypt error\n");
    	return -1;
    }
//    printf("enc %d:",strlen((char*)dstStringTemp));
    for(i= 0;dstStringTemp[i];i+=1){
        printf("%#x ",(unsigned char)dstStringTemp[i]);
    }
    memset((char*)sourceStringTemp, 0 ,MSG_LEN);
    if(!aes_decrypt(dstStringTemp,key,sourceStringTemp, len)){
    
    	printf("decrypt error\n");
    	return -1;
    }
    printf("\n");
  //  printf("dec %d:",strlen((char*)sourceStringTemp));
   // printf("%s\n",sourceStringTemp);
  // RePadding(sourceStringTemp)
    for(i= 0;i<32;i+=1){
        printf("%#x ",(unsigned char)sourceStringTemp[i]);
    }
    printf("\n");



	printf("second way:========================================\n");
        memset(sourceStringTemp, 0, sizeof(sourceStringTemp));
   	memset(dstStringTemp, 0, sizeof(dstStringTemp));
   	Padding(Plaintext, 19, sourceStringTemp, &len);
    

 
	 for(i= 0;i<len;i+=1){
            printf("%#x ",(unsigned char)sourceStringTemp[i]);
  	}
	printf("\n"); 

  //  Padding(Plaintext, 19, sourceStringTemp, &len);
      if(!my_aes_encrypt(sourceStringTemp,key,dstStringTemp, len))
      {
    	 printf("encrypt error\n");
    	 return -1;
      }
	printf("encode over...\n");
    
//    printf("enc %d:",strlen((char*)dstStringTemp));
      for(i= 0;i<32;i+=1){
            printf("%#x ",(unsigned char)dstStringTemp[i]);
  	}
	printf("\n"); 
	
	if(!my_aes_decrypt(dstStringTemp, key, sourceStringTemp, len))
	{
		printf("decrypt error...\n");
		return -1;
	}	

    for(i= 0;i<32;i+=1){
        printf("%#x ",(unsigned char)sourceStringTemp[i]);
    }
    printf("\n");


    return 0;
}
