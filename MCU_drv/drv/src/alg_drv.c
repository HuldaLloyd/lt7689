/*
 * alg_demo.c
 *
 *  Created on: 2017年6月9日
 *      Author: ccore
 */

#include "cpm_drv.h"
#include "alg_drv.h"
#include "string.h"
#include "uart.h"
#include "trng_drv.h"

const UINT8 charGx[]           = "32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7";
const UINT8 charGy[]           = "BC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0";
const UINT8 charPrimeN[]       = "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123";
const UINT8 charPrimeP[]       = "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF";
const UINT8 charCoefficientA[] = "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC";
const UINT8 charCoefficientB[] = "28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93";

//SM2
SM2_STU_PRAMS    stuEccPrams;
SM2_STU_PUBKEY   stuPubKey;
ECC_STU_BIGINT32 stuPrivKey;
ECC_STU_BIGINT32 stuSignatureR;
ECC_STU_BIGINT32 stuSignatureS;

//rsa
STU_RSA_PUBKEY  publicKeyRSA;
STU_RSA_PRIVKEY privateKeyRSA;

/*******************************************************************************
* Function Name  : IsSRAMAddr
* Description    : 检测为SRAM地址
* Input          : addr  待检测的地址
* Output         : None
* Return         : TRUE or FLASE
******************************************************************************/
static bool IsSRAMAddr(U32 addr)
{
	if((addr < SRAM_BEGIN)||(addr > SRAM_END))
	{
		return FALSE;
	}
	return TRUE;
}

/*******************************************************************************
* Function Name  : IsEFLASHAddr
* Description    : 检测为EFLASH地址
* Input          : addr  待检测的地址
* Output         : None
* Return         : TRUE or FLASE
******************************************************************************/
static bool IsEFLASHAddr(U32 addr)
{
	if((addr < EFLASH_BEGIN)||(addr > EFLASH_END))
	{
		return FALSE;
	}
	return TRUE;
}

/*******************************************************************************
* Function Name  : CheckAddrType
* Description    : 检测地址类型
* Input          : addr  待检测的地址
* Output         : None
* Return         : int类型     0：地址无效    1：SRAM地址   2：EFLASH地址
******************************************************************************/
static TYPE_ADDR CheckAddrType(U32 addr)
{
	if(IsSRAMAddr(addr) == TRUE)
	{
		return TYPE_SRAM;
	}

	if(IsEFLASHAddr(addr) == TRUE)
	{
		return TYPE_EFLASH;
	}
	return TYPE_ERROR;
}

UINT32 Get_Random_Word(void)
{
	return GetRandomWord();
}



/*******************************************************************************
* Function Name  : U32toU8
* Description    : 字类型（小端）转换为BYTE（大端）类型
* Input          : -src：源数据地址
*                  -len：字长度
*                  -dst: 目的地址
* Output         : None
* Return         : UINT16 byte的字节长度
******************************************************************************/
static U16 U32toU8(U32 *src_p,U16 len,U8 *dst_p,U8 append)
{
	U16 j;
	U16 block;
	U8 *p = (U8*)src_p;
	U8 temp[256];

	if(!len) return 0;

	//16 字节块
	block = len/4;

	for(j = 0x0;j < block;j++)
	{
		temp[j*4]   = p[len-1-j*4];
		temp[j*4+1] = p[len-1-j*4-1];
		temp[j*4+2] = p[len-1-j*4-2];
		temp[j*4+3] = p[len-1-j*4-3];
	}
	memcpy(dst_p,temp+append,block*4-append);

	return block*4-append;
}

/*******************************************************************************
* Function Name  : U8toU32
* Description    : BYTE（大端）转换为字类型（小端）类型
* Input          : -src：源数据地址
*                  -len：字长度
*                  -dst: 目的地址
* Output         : None
* Return         : UINT16 字的长度
******************************************************************************/
static U16 U8toU32(u8 *src_p,U16 len,U32 *dst_p)
{
	U16 end_index,i,shift;
	U32 temp;

	if(len < 4) return 0;
	//计算末尾位置
	end_index = len/4;

	temp = 0;
	shift = 0;
	while(len%4)
	{
		temp = (temp << 8) & 0xFFFFFF00;
		temp |= src_p[shift];
		len--;
		shift++;
	}

	if(shift != 0)
	{
		dst_p[end_index] = temp;
		//end_index++;
	}

	for(i = 0; i < end_index; i++)
	{
		temp = (src_p[shift+i*4]<<24)|(src_p[shift+i*4+1]<<16)|(src_p[shift+i*4+2]<<8)|src_p[shift+i*4+3];
		dst_p[end_index-1-i] = temp;
	}
	return end_index;
}

/*******************************************************************************
* Function Name  : wordswap
* Description    : 以字为单位整体翻转，字内不变
* Input          : -dest：目的地址
*                  -src：源地址
*                  -wordLen: 字长度
* Output         : None
* Return         : None
******************************************************************************/
static void wordswap(UINT32 *dest, UINT32 *src, UINT32 wordLen)
{
	UINT32 i;
	UINT32 temp;

	//输入输出使用同一个BUF
	if (src == dest)
	{
		for (i=0; i<wordLen/2; i++)
		{
			temp = dest[i];
			dest[i] = src[wordLen-1-i];
			dest[wordLen-1-i] = temp;
		}
	}
	else
	{
		for (i=0; i<wordLen; i++)
		{
			dest[i] = src[wordLen-1-i];
		}
	}
}

/*******************************************************************************
* Function Name  : AES_EnDecrypt_2
* Description    : AES加解密运算
* 				         注意：
* 				    None
* Input          : -EncDe_Mode      ：加解密模式
* 				   -CBC_ECB_MODE    ：ECB 或 CBC模式
* 				   -key             :密钥
* 				   -cbc_iv          :CBC初始向量
* 				   -key_mode        ：密钥模式
* 				   -src_buf         :源数据buf
* 				   -data_len        :源数据长度
* Output         : -out_buf         :输出数据buf
*
* Return         : 运算结果
******************************************************************************/
int AES_EnDecrypt_2(U8 EncDe_Mode,U8 CBC_ECB_MODE,U8 *key,U8 *cbc_iv,U32 key_mode, U8 *src_buf, U8 *out_buf, U32 data_len)
{
	U32 key_buf[8];
	U32 cbc_iv_buf[4];

	if((key==NULL)  ||(src_buf==NULL) ||(out_buf==NULL))
	{
		return ERROR_DATA_BUF;
	}
	
	if((CheckAddrType((U32)key) == TYPE_ERROR)
	 ||(CheckAddrType((U32)src_buf) != TYPE_SRAM)
	 ||(CheckAddrType((U32)out_buf) != TYPE_SRAM))
	{
		return ERROR_DATA_BUF;
	}

	if ((0x0 == data_len)||((data_len%16) != 0))	//数据长度错误
	{
		return ERROR_DATA_LENGTH;
	}
	if ((EncDe_Mode!=0) && (EncDe_Mode!=1))
	{
		return ERROR_ENDE_MODE;
	}
	if ((CBC_ECB_MODE!=0) && (CBC_ECB_MODE!=1))
	{
		return ERROR_ECBCBC_MODE;
	}
	if ((key_mode!=0) && (key_mode!=1) && (key_mode!=2))
	{
		return ERROR_KEY_MODE;
	}

	memcpy(key_buf,key,16 + (key_mode<<3));
	if(CBC_ECB_MODE)
	{
		if(NULL == cbc_iv)
		{
			return ERROR_DATA_BUF;
		}
		if(CheckAddrType((U32)cbc_iv) == TYPE_ERROR)
		{
			return ERROR_DATA_BUF;
		}
		memcpy(cbc_iv_buf,cbc_iv,16);
	}

	AES_Enable();
	AES_EnDecrypt(EncDe_Mode,CBC_ECB_MODE,key_buf,cbc_iv_buf,key_mode,(UINT32 *)src_buf,(UINT32 *)out_buf,data_len);
	AES_Disable();

	return SUCCESS_ALG;
}

/*******************************************************************************
* Function Name  : DES_EnDecrypt_2
* Description    : DES加解密运算
* 				         注意：
* 				    None
* Input          : -EncDe_Mode      ：加解密模式
* 				   -CBC_ECB_MODE    ：ECB 或 CBC模式
* 				   -key             :密钥
* 				   -key_len         :密钥长度
* 				   -iv              ：初始向量
* 				   -src_buf         :源数据buf
* 				   -data_len        :源数据长度
* 				   -check_parity    :输入的key是否需要进行校验，如果校验错误则返回key校验错误不再继续加解密
* Output         : -out_buf         :输出数据buf
*
* Return         : 运算结果
******************************************************************************/
int DES_EnDecrypt_2(U8 EncDe_Mode, U8 ECB_CBC_Mode, U8 *key, U32 key_len, U8 *iv, U8 *src_buf, U8 *out_buf,U32 data_len, U8 check_parity)
{
	U32 key_buf[6];
	U32 iv_buf[2];

	if((key==NULL)  ||(src_buf==NULL) ||(out_buf==NULL))
	{
		return ERROR_DATA_BUF;
	}

	if((CheckAddrType((U32)key) == TYPE_ERROR)
	 ||(CheckAddrType((U32)src_buf) != TYPE_SRAM)
	 ||(CheckAddrType((U32)out_buf) != TYPE_SRAM))
	{
		return ERROR_DATA_BUF;
	}

	if ((0x0 == data_len)||((data_len%8) != 0))	//数据长度错误
	{
		return ERROR_DATA_LENGTH;
	}
	if ((EncDe_Mode!=0) && (EncDe_Mode!=1))
	{
		return ERROR_ENDE_MODE;
	}
	if ((ECB_CBC_Mode!=0) && (ECB_CBC_Mode!=1))
	{
		return ERROR_ECBCBC_MODE;
	}
	//判断密钥长度
	if((0x08 != key_len)&&(0x10 != key_len)&&(0x18 != key_len))
	{
		return ERROR_KEY_MODE;
	}
	if (check_parity != 0)
	{
		return ERROR_OTHER_PARA;
	}
	if(ECB_CBC_Mode)
	{
		if(NULL == iv)
		{
			return ERROR_DATA_BUF;
		}
		if(CheckAddrType((U32)iv) == TYPE_ERROR)
		{
			return ERROR_DATA_BUF;
		}
		memcpy(iv_buf,iv,8);
	}
	memcpy(key_buf,key,key_len);

	DES_Enable();
	DES_EnDecrypt(EncDe_Mode,ECB_CBC_Mode,key_buf,key_len,iv_buf,(UINT32 *)src_buf,(UINT32 *)out_buf,data_len,check_parity);
	DES_Disable();

	return SUCCESS_ALG;
}

/*******************************************************************************
* Function Name  : SMS4_EnDecrypt_2
* Description    : SMS4加解密运算
* 				         注意：
* 				    None
* Input          : -EncDe_Mode      ：加解密模式
* 				   -CBC_ECB_MODE    ：ECB 或 CBC模式
* 				   -key             :密钥
* 				   -iv              ：初始向量
* 				   -src_buf         :源数据buf
* 				   -data_len        :输出数据长度
* Output         : -out_buf         :输出数据buf
*
* Return         : 运算结果
******************************************************************************/
int SMS4_EnDecrypt_2(U8 EncDe_Mode,U8 ECB_CBC_Mode,U8 *key,U8 *iv,U8 *src_buf,U8 *out_buf,U32 data_len)
{
	U32 key_buf[4];
	U32 iv_buf[4];

	if((key==NULL)  ||(src_buf==NULL) ||(out_buf==NULL))
	{
		return ERROR_DATA_BUF;
	}

	if((CheckAddrType((U32)key)     == TYPE_ERROR)
	 ||(CheckAddrType((U32)src_buf) != TYPE_SRAM)
	 ||(CheckAddrType((U32)out_buf) != TYPE_SRAM))
	{
		return ERROR_DATA_BUF;
	}

	if ((0x0 == data_len)||((data_len%16) != 0))	//数据长度错误
	{
		return ERROR_DATA_LENGTH;
	}
	if ((EncDe_Mode!=0) && (EncDe_Mode!=1))
	{
		return ERROR_ENDE_MODE;
	}
	if ((ECB_CBC_Mode!=0) && (ECB_CBC_Mode!=1))
	{
		return ERROR_ECBCBC_MODE;
	}

	if(ECB_CBC_Mode)
	{
		if(NULL == iv)
		{
			return ERROR_DATA_BUF;
		}
		if((CheckAddrType((U32)iv) == TYPE_ERROR))
		{
			return ERROR_DATA_BUF;
		}
		memcpy(iv_buf,iv,16);
	}
	memcpy(key_buf,key,16);

	SMS4_Enable();
	SMS4_EnDecrypt(EncDe_Mode,ECB_CBC_Mode,key_buf,iv_buf,(UINT32 *)src_buf,(UINT32 *)out_buf,data_len);
	SMS4_Disable();

	return SUCCESS_ALG;
}

/*******************************************************************************
* Function Name  : SHA_Init_2
* Description    : HASH运算初始化
* 				         注意：
* 				    None
* Input          : -phash_tx        ：哈希算法数据结构模式
* 				   -HashModeNum    ：哈希算法编号，取值范围0~4
* Output         : None
* Return         : 结果
******************************************************************************/
int SHA_Init_2(Hash_tx *phash_tx, unsigned char HashModeNum)
{
	if(phash_tx==NULL)
	{
		return ERROR_DATA_BUF;
	}
	if (HashModeNum>HASH_SHA512)
	{
		return ERROR_OTHER_PARA;
	}

	SHA_Init(phash_tx, HashModeNum);
	return SUCCESS_ALG;
}

/*******************************************************************************
* Function Name  : SHA_Update_2
* Description    : 发送要进行HASH运算的数据
* 				         注意：
* 				    None
* Input          : -phash_tx         ：哈希算法数据结构模式
* 				   -pMessageBlock    ：信息块
* 				   -DataLen          ：信息块长度
* Output         : None
* Return         : 执行结果
******************************************************************************/
int SHA_Update_2(Hash_tx *phash_tx, unsigned char *pMessageBlock, unsigned int DataLen)
{
	UINT8 MessageBlock[64];
	UINT32 offset;
	UINT8 len;
	if((phash_tx==NULL) || (pMessageBlock==NULL))
	{
		return ERROR_DATA_BUF;
	}

	if (DataLen == 0)
	{
		return SUCCESS_ALG;
	}
	offset = 0;
	while(offset < DataLen)
	{
		len = (DataLen-offset)> 0x40?0x40:(DataLen-offset);
		memcpy(MessageBlock,pMessageBlock+offset,len);

		SHA_Update(phash_tx, MessageBlock, len);
		offset += len;
	}
	return SUCCESS_ALG;
}

/*******************************************************************************
* Function Name  : SHA_Final_2
* Description    :  结束HASH操作并获取结果
* 				         注意：
* 				    None
* Input          : -phash_tx         ：哈希算法数据结构模式
* Output         : -pRes             ：HASH运算结果
* Return         : 执行结果
******************************************************************************/
int SHA_Final_2(Hash_tx *phash_tx, unsigned char *pRes)
{
	UINT32 sha_result[16];
	UINT32 res_Len;

	if((phash_tx==NULL) || (pRes==NULL))
	{
		return ERROR_DATA_BUF;
	}

	if(CheckAddrType((U32)pRes) != TYPE_SRAM)
	{
		return ERROR_DATA_BUF;
	}

	SHA_Final(phash_tx, sha_result);

	//计算需要返回的数据长度
	switch(phash_tx->HASHMode)
	{
		case HASH_SM3:
			res_Len = SM3_R_Len;
			break;
		case HASH_SHA0:
			res_Len = SHA0_R_Len;
			break;
		case HASH_SHA1:
			res_Len  = SHA1_R_Len;
			break;
		case HASH_SHA224:
			res_Len = SHA224_R_Len;
			break;
		case HASH_SHA256:
			res_Len = SHA256_R_Len;
			break;
		case HASH_SHA384:
			res_Len = SHA384_R_Len;
			break;
		case HASH_SHA512:
			res_Len = SHA512_R_Len;
			break;
		default:
			return ERROR_OTHER_PARA;
	}

	memcpy(pRes, (UINT8*)sha_result, res_Len);
	return SUCCESS_ALG;
}

/*******************************************************************************
* Function Name  : SM2Init
* Description    : SM2模块初始化
* Input          : testbits ： SM2位数
* Output         : None
* Return         : None
******************************************************************************/
int SM2Init(UINT16 testbits)
{
	UINT8 databuf[64];

	if (testbits != 256)
	{
		return ERROR_OTHER_PARA;
	}

	stuEccPrams.uBits = testbits;

	//初始化ECC参数
	EccBig32Initial(&(stuEccPrams.stuPrimeP));
	EccBig32Initial(&(stuEccPrams.stuCoefficientA));
	EccBig32Initial(&(stuEccPrams.stuCoefficientB));
	EccBig32Initial(&(stuEccPrams.stuGx));
	EccBig32Initial(&(stuEccPrams.stuGy));
	EccBig32Initial(&(stuEccPrams.stuPrimeN));

	EccBig32Initial(&(stuPrivKey));
	EccBig32Initial(&(stuPubKey.stuQx));
	EccBig32Initial(&(stuPubKey.stuQy));
	EccBig32Initial(&stuSignatureR);
	EccBig32Initial(&stuSignatureS);

	EccCharToHex((UINT8 *)charGx,databuf,stuEccPrams.uBits/4);
	EccByBufToBigInt32(databuf,stuEccPrams.uBits/8,&(stuEccPrams.stuGx));

	EccCharToHex((UINT8 *)charGy,databuf,stuEccPrams.uBits/4);
	EccByBufToBigInt32(databuf,stuEccPrams.uBits/8,&(stuEccPrams.stuGy));

	EccCharToHex((UINT8 *)charPrimeN,databuf,stuEccPrams.uBits/4);
	EccByBufToBigInt32(databuf,stuEccPrams.uBits/8,&(stuEccPrams.stuPrimeN));

	EccCharToHex((UINT8 *)charPrimeP,databuf,stuEccPrams.uBits/4);
	EccByBufToBigInt32(databuf,stuEccPrams.uBits/8,&(stuEccPrams.stuPrimeP));

	EccCharToHex((UINT8 *)charCoefficientA,databuf,stuEccPrams.uBits/4);
	EccByBufToBigInt32(databuf,stuEccPrams.uBits/8,&(stuEccPrams.stuCoefficientA));

	EccCharToHex((UINT8 *)charCoefficientB,databuf,stuEccPrams.uBits/4);
	EccByBufToBigInt32(databuf,stuEccPrams.uBits/8,&(stuEccPrams.stuCoefficientB));

	return SUCCESS_ALG;
}

/*******************************************************************************
* Function Name  : SM2GenerateKeyPair_2
* Description    : SM2密钥生成
* Input          : None
* Output         : -pub_key         ：公钥
*                  -pri_key         ：私钥
* Return         : 运算结果
******************************************************************************/
int SM2GenerateKeyPair_2(U8 *pub_key,U8 *pri_key)
{
	char flag;

	if((pub_key==NULL)  ||(pri_key==NULL))
	{
		return ERROR_DATA_BUF;
	}

	if((CheckAddrType((U32)pub_key) != TYPE_SRAM)
	 ||(CheckAddrType((U32)pri_key) != TYPE_SRAM))
	{
		return ERROR_DATA_BUF;
	}

	SM2_Enable();
	flag  =  SM2GenerateKeyPair(&stuEccPrams,&stuPrivKey,&stuPubKey);
	SM2_Disable();

	//cpy 密钥
	U32toU8(stuPrivKey.auValue,8*4,pri_key,0);
	U32toU8(stuPubKey.stuQx.auValue,8*4,pub_key,0);
	U32toU8(stuPubKey.stuQy.auValue,8*4,pub_key+32,0);

	//返回密钥对生成结果
	return flag;
}

/*******************************************************************************
* Function Name  : SM2Encrypt_2
* Description    : SM2加密
* Input          : -mes             ：明文
*                  -len             :明文长度
*                  -pub_key         :公钥
* Output         : -cipher          ：密文
* Return         : 运算结果
******************************************************************************/
int SM2Encrypt_2(U8 *mes,U16 len,U8 *pub_key,U8 *cipher)
{
	if((mes==NULL)  ||(pub_key==NULL) ||(cipher==NULL))
	{
		return ERROR_DATA_BUF;
	}

	if((CheckAddrType((U32)pub_key) == TYPE_ERROR)
	 ||(CheckAddrType((U32)mes)     != TYPE_SRAM)
	 ||(CheckAddrType((U32)cipher)  != TYPE_SRAM))
	{
		return ERROR_DATA_BUF;
	}


	if(len > 256*4)		//明文长度不超过1K字节
	{
		return ERROR_DATA_LENGTH;
	}

	stuPubKey.stuQx.uLen = 8;
	stuPubKey.stuQy.uLen = 8;

	U8toU32(pub_key,32,stuPubKey.stuQx.auValue);
	U8toU32(pub_key+32,32,stuPubKey.stuQy.auValue);

	SM2_Enable();
	SM2Encrypt_V2(mes,len,&stuPubKey,&stuEccPrams,cipher);
	SM2_Disable();

	return SUCCESS_ALG;
}

/*******************************************************************************
* Function Name  : SM2Decrypt_2
* Description    : SM2解密
* Input          : -cipher          ：密文
*                  -len             :明文长度
*                  -pri_key         :私钥
* Output         : -mes             ：明文
* Return         : 运算结果
******************************************************************************/
int SM2Decrypt_2(U8 *cipher,U16 len,U8 *pri_key,U8 *mes)
{
	U32 cipher_buf[256+24];	//1024字节明文+96字节C1/C3
	UINT8 result;

	if((mes==NULL)  ||(pri_key==NULL) ||(cipher==NULL))
	{
		return ERROR_DATA_BUF;
	}

	if((CheckAddrType((U32)pri_key) == TYPE_ERROR)
	 ||(CheckAddrType((U32)cipher)  == TYPE_ERROR)
	 ||(CheckAddrType((U32)mes)     != TYPE_SRAM))
	{
		printf("faile\r\n");
		return ERROR_DATA_BUF;
	}

	if(len > 256*4)		//明文长度不超过1K字节
	{
		return ERROR_DATA_LENGTH;
	}

	memcpy(cipher_buf,cipher,len+96);

	stuPrivKey.uLen = 8;
	U8toU32(pri_key,32,stuPrivKey.auValue);

	SM2_Enable();
	result = SM2Decrypt_V2((UINT8*)cipher_buf,len,&stuPrivKey,&stuEccPrams,mes);
	SM2_Disable();

	return result;
}

/*******************************************************************************
* Function Name  : SM2Signature_2
* Description    : SM2签名
* Input          : -mes             ：明文
*                  -len             :明文长度
*                  -pri_key         :私钥
* Output         : -r               ：签名值R块
*                  -s               :签名值S块
* Return         : 运算结果
******************************************************************************/
int SM2Signature_2(U8 *mes,U16 len,U8 *pri_key,U8 *r,U8 *s)
{
	U32 signplain[8];
	//U8  key[32];

	if((mes==NULL)  ||(pri_key==NULL) ||(r==NULL) ||(s==NULL))
	{
		return ERROR_DATA_BUF;
	}

	if((CheckAddrType((U32)mes)    == TYPE_ERROR)
	||(CheckAddrType((U32)pri_key) == TYPE_ERROR)
	||(CheckAddrType((U32)r)       != TYPE_SRAM)
	||(CheckAddrType((U32)s)       != TYPE_SRAM))
	{
		return ERROR_DATA_BUF;
	}

	if(len != 32)
	{
		return ERROR_DATA_LENGTH;
	}

	memcpy((U8*)signplain, mes, 32);
	wordswap(signplain,signplain,8);

	stuPrivKey.uLen = 8;
	U8toU32(pri_key,32,stuPrivKey.auValue);

	EccBig32Initial(&stuSignatureR);
	EccBig32Initial(&stuSignatureS);

	SM2_Enable();
	SM2Signature((U8*)signplain,len,&stuEccPrams,&stuPrivKey,&stuSignatureR,&stuSignatureS);
	SM2_Disable();

	U32toU8(stuSignatureR.auValue,8*4,r,0);
	U32toU8(stuSignatureS.auValue,8*4,s,0);

	return SUCCESS_ALG;
}

/*******************************************************************************
* Function Name  : SM2Verification_2
* Description    : SM2验签
* Input          : -mes             ：明文
*                  -len             :明文长度
*                  -pri_key         :私钥
*                  -r               ：签名值R块
*                  -s               :签名值S块
* Output         : None
* Return         : 验签结果
******************************************************************************/
int SM2Verification_2(U8 *mes,U16 len,U8 *pub_key,U8 *r,U8 *s)
{
	U8 flag;
	U32 signplain[8];

	if((mes==NULL)  ||(pub_key==NULL) ||(r==NULL) ||(s==NULL))
	{
		return ERROR_DATA_BUF;
	}

	if((CheckAddrType((U32)mes)     == TYPE_ERROR)
	 ||(CheckAddrType((U32)pub_key) == TYPE_ERROR)
	 ||(CheckAddrType((U32)r)       != TYPE_SRAM)
	 ||(CheckAddrType((U32)s)       != TYPE_SRAM))
	{
		return ERROR_DATA_BUF;
	}

	if(len != 32)
	{
		return ERROR_DATA_LENGTH;
	}

	memcpy((U8*)signplain, mes, 32);
	wordswap(signplain,signplain,8);

	stuPubKey.stuQx.uLen = 8;
	stuPubKey.stuQy.uLen = 8;

	U8toU32(pub_key,32,stuPubKey.stuQx.auValue);
	U8toU32(pub_key+32,32,stuPubKey.stuQy.auValue);

	stuSignatureR.uLen = 8;
	stuSignatureS.uLen = 8;

	U8toU32(r,32,stuSignatureR.auValue);
	U8toU32(s,32,stuSignatureS.auValue);

	SM2_Enable();
	flag = SM2Verification((U8*)signplain,len,&stuEccPrams,&stuPubKey,&stuSignatureR,&stuSignatureS);
	SM2_Disable();

	//0 : fail
	return flag;
}

/*******************************************************************************
* Function Name  : SM2GenerateE_2
* Description    : SM2计算E值
* Input          : -IDA             ：用户ID
*                  -ida_len         :用户ID长度
*                  -pub_key         :公钥
*                  -pMessage        ：信息
*                  -mesLen          :信息长度
* Output         : -res_E           :E值
* Return         : 执行结果
******************************************************************************/
int SM2GenerateE_2(U8 *IDA,U16 ida_len,U8 *pub_key,UINT8 *pMessage,UINT32 mesLen,U8 *res_E)
{
	U32 za_buf[8];
	U32 i,block,bytelen;
	U8 oneblock=64;
	Hash_tx stru_sm3;

	if((IDA==NULL)  ||(pub_key==NULL) ||(pMessage==NULL) ||(res_E==NULL))
	{
		return ERROR_DATA_BUF;
	}

	if((CheckAddrType((U32)IDA)     != TYPE_SRAM)
	||(CheckAddrType((U32)res_E)    != TYPE_SRAM)
	||(CheckAddrType((U32)pub_key)  == TYPE_ERROR)
	||(CheckAddrType((U32)pMessage) == TYPE_ERROR))
	{
		return ERROR_DATA_BUF;
	}

	if (mesLen==0)
	{
		return ERROR_DATA_LENGTH;
	}

	//公钥
	stuPubKey.stuQx.uLen = 8;
	stuPubKey.stuQy.uLen = 8;

	U8toU32(pub_key,32,stuPubKey.stuQx.auValue);
	U8toU32(pub_key+32,32,stuPubKey.stuQy.auValue);

	SM2_Enable();
	//计算za值
	sign(IDA,ida_len,&stuPubKey,&stuEccPrams,za_buf);
	SM2_Disable();
	
	ModuleClk_On(MODULE_CLK_SHA);
	
	SHA_Init_2(&stru_sm3, HASH_SM3);
	SHA_Update_2(&stru_sm3, (U8*)za_buf, 32);		//先对Z值做SM3运算

	block = mesLen/oneblock;
	bytelen = mesLen%oneblock;
	for(i = 0 ; i < block ; i++)		//再对数据做SM3运算
	{
		SHA_Update_2(&stru_sm3, pMessage + i*oneblock, oneblock);
	}
	if (bytelen!=0)
	{
		SHA_Update_2(&stru_sm3, pMessage + i*oneblock, bytelen);
	}

	SHA_Final_2(&stru_sm3, res_E);
	
	ModuleClk_Off(MODULE_CLK_SHA);
	
	return SUCCESS_ALG;
}

/*******************************************************************************
* Function Name  : SM2KeyExchange_2
* Description    : SM2密钥交换
* Input          : -self_pri        ：己方私钥
*                  -self_temp_pub   :己方临时公钥
*                  -self_temp_pri   :己方临时私钥
*                  -other_pub       ：对方公钥
*                  -other_temp_pub  :对方临时公钥
*                  -za              ：己方Z值
*                  -zb              :对方Z值
*                  -AgreedKeyLen    ：协商密钥的长度
*                  -Mode			：密钥交换发起方。0表示已方为A方，1表示已方为B方
* Output         : -agree_key       :协商密钥
*                  -sA              ：己方S值
*                  -sB              :对方S值
* Return         : 交换结果
******************************************************************************/
int SM2KeyExchange_2(U8*self_pri,U8*self_temp_pub,U8 *self_temp_pri,U8*other_pub,U8*other_temp_pub,U8*za,U8*zb,U8*agree_key,U32 AgreedKeyLen,U8*sA,U8*sB,U32 Mode)
{
	SM2_STU_PUBKEY   self_temp_pub_key;
	ECC_STU_BIGINT32 self_pri_key;
	ECC_STU_BIGINT32 self_temp_pri_key;
	SM2_STU_PUBKEY   other_pub_key;
	SM2_STU_PUBKEY   other_temp_pub_key;

	UINT32 za_buf[8];
	UINT32 zb_buf[8];
	UINT32 sa_buf[8];
	UINT32 sb_buf[8];
	UINT32 key_buf[8];
	UINT8 result;


	if((NULL == self_pri) ||(NULL == self_temp_pub) ||(NULL == self_temp_pri)
	 ||(NULL == other_pub)||(NULL == other_temp_pub)||(NULL == za)||(NULL == zb)
	 ||(NULL == agree_key)||(NULL == sA)||(NULL == sB))
	{
		return ERROR_DATA_BUF;
	}

	if((CheckAddrType((U32)sA)     != TYPE_SRAM)
	||(CheckAddrType((U32)sB)      != TYPE_SRAM)
	||(CheckAddrType((U32)self_pri)== TYPE_ERROR)
	||(CheckAddrType((U32)self_temp_pub)== TYPE_ERROR)
	||(CheckAddrType((U32)self_temp_pri)== TYPE_ERROR)
	||(CheckAddrType((U32)other_pub)== TYPE_ERROR)
	||(CheckAddrType((U32)other_temp_pub)== TYPE_ERROR)
	||(CheckAddrType((U32)za)== TYPE_ERROR)
	||(CheckAddrType((U32)zb)== TYPE_ERROR)
	||(CheckAddrType((U32)agree_key)!= TYPE_SRAM))
	{
		return ERROR_DATA_BUF;
	}

	if ((Mode > 1)||(AgreedKeyLen > 32))
	{
		return ERROR_DATA_LENGTH;
	}

 	EccBig32Initial(&self_temp_pub_key.stuQx);
 	EccBig32Initial(&self_temp_pub_key.stuQy);
 	EccBig32Initial(&other_pub_key.stuQx);
 	EccBig32Initial(&other_pub_key.stuQy);
 	EccBig32Initial(&other_temp_pub_key.stuQx);
 	EccBig32Initial(&other_temp_pub_key.stuQy);
 	EccBig32Initial(&self_pri_key);
 	EccBig32Initial(&self_temp_pri_key);


	//公钥进行转化
	U8toU32(self_temp_pub,32,self_temp_pub_key.stuQx.auValue);
	U8toU32(self_temp_pub+32,32,self_temp_pub_key.stuQy.auValue);
	self_temp_pub_key.stuQx.uLen = 8;
	self_temp_pub_key.stuQy.uLen = 8;

	U8toU32(other_pub,32,other_pub_key.stuQx.auValue);
	U8toU32(other_pub+32,32,other_pub_key.stuQy.auValue);
	other_pub_key.stuQx.uLen = 8;
	other_pub_key.stuQy.uLen = 8;

	U8toU32(other_temp_pub,32,other_temp_pub_key.stuQx.auValue);
	U8toU32(other_temp_pub+32,32,other_temp_pub_key.stuQy.auValue);
	other_temp_pub_key.stuQx.uLen = 8;
	other_temp_pub_key.stuQy.uLen = 8;


	//私钥进行转化
	U8toU32(self_pri,32,self_pri_key.auValue);
	self_pri_key.uLen = 8;

	U8toU32(self_temp_pri,32,self_temp_pri_key.auValue);
	self_temp_pri_key.uLen = 8;

	memcpy(za_buf,za,32);
	memcpy(zb_buf,zb,32);

	SM2_Enable();
	result = SM2KeyExchange(&stuEccPrams,
							&self_pri_key,
							&self_temp_pub_key,
							&self_temp_pri_key,
							&other_pub_key,
							&other_temp_pub_key,
							za_buf,
							zb_buf,
							key_buf,
							AgreedKeyLen,
							sa_buf,
							sb_buf,
							Mode);
	SM2_Disable();
	if(!result) return FAIL_ALG;

	memcpy(sA,sa_buf,32);
	memcpy(sB,sb_buf,32);
	memcpy(agree_key,key_buf,AgreedKeyLen);

	return SUCCESS_ALG;
}

/*******************************************************************************
* Function Name  : sign_2
* Description    : SM2生成Z值
* Input          : -IDA             ：用户ID
*                  -entla           :用户ID长度
*                  -pub_key         :公钥
* Output         : -za              ：Z值
* Return         : 执行结果
******************************************************************************/
int sign_2(U8 *IDA, U16 entla, U8 *pub_key,U8*za)
{
	UINT32 za_buf[8];
	SM2_STU_PUBKEY pub_key_buf;

	if((IDA==NULL)  ||(pub_key==NULL) ||(za==NULL))
	{
		return ERROR_DATA_BUF;
	}

	if((CheckAddrType((U32)IDA)     != TYPE_SRAM)
	 ||(CheckAddrType((U32)pub_key) == TYPE_ERROR)
	 ||(CheckAddrType((U32)za)     != TYPE_SRAM))
	{
		return ERROR_DATA_BUF;
	}

	if (entla==0)
	{
		return ERROR_DATA_LENGTH;
	}
	//公钥需要进行大小端转换
	U8toU32(pub_key,32,pub_key_buf.stuQx.auValue);
	U8toU32(pub_key+32,32,pub_key_buf.stuQy.auValue);
	pub_key_buf.stuQx.uLen = 8;
	pub_key_buf.stuQy.uLen = 8;
	//签名
	SM2_Enable();
	sign(IDA,entla,&pub_key_buf,&stuEccPrams,za_buf);
	SM2_Disable();
	
	memcpy(za,za_buf,32);

	return SUCCESS_ALG;
}

/*******************************************************************************
* Function Name  : rsa_keypair_gen
* Description    : RSA密钥生成
* Input          : -rsa_bit_len     ：RSA算法位数
*                  -fixkey          :E值是否固定
* Output         : -n_addr          ：N
*                  -e_addr          :E
*                  -d_addr          :D
*                  -p_addr          :P
*                  -q_addr          :Q
*                  -dp_addr         :dP
*                  -dq_addr         :dQ
*                  -qinv_addr       :PQ
* Return         : 执行结果
******************************************************************************/
int rsa_keypair_gen(U32 rsa_bit_len, U8 fixkey, U8* n_addr, U8* e_addr, U8 *d_addr, U8* p_addr, U8* q_addr, U8* dp_addr, U8* dq_addr, U8* qinv_addr)
{
	U32 n_len,p_len,q_len;
	U8 result;

	if ((rsa_bit_len<512) ||(rsa_bit_len>2048) ||((rsa_bit_len%32)!=0))	//32的整数倍
	{
		return ERROR_KEY_LENGTH;
	}
	if((n_addr==NULL) ||(d_addr==NULL)  ||(p_addr==NULL) ||(q_addr==NULL) ||(dp_addr==NULL) ||(dq_addr==NULL) ||(qinv_addr==NULL))
	{
		return ERROR_DATA_BUF;
	}

	if((CheckAddrType((U32)n_addr) != TYPE_SRAM)
	 ||(CheckAddrType((U32)d_addr) != TYPE_SRAM)
	 ||(CheckAddrType((U32)p_addr) != TYPE_SRAM)
	 ||(CheckAddrType((U32)q_addr) != TYPE_SRAM)
	 ||(CheckAddrType((U32)dp_addr) != TYPE_SRAM)
	 ||(CheckAddrType((U32)dq_addr) != TYPE_SRAM)
	 ||(CheckAddrType((U32)qinv_addr) != TYPE_SRAM))
	{
		return ERROR_DATA_BUF;
	}

	if ((fixkey!=0) && (fixkey!=1))
	{
		return ERROR_KEY_MODE;
	}
	if ((fixkey == 0) && (e_addr==NULL))		//公钥随机产生
	{
		return ERROR_DATA_BUF;
	}

	if (fixkey == 0)
	{
		if(CheckAddrType((U32)e_addr) != TYPE_SRAM)
		{
			return ERROR_DATA_BUF;
		}
	}

	n_len = rsa_bit_len / 32;
	q_len = n_len/2;
	if(n_len %2)
	{
		q_len++;
	}
	p_len = q_len;


	RsaModuleEnable();
	result = RsaGenerateKeyPair(rsa_bit_len,&publicKeyRSA,&privateKeyRSA, fixkey, 1);
	RsaModuleDisable();

	U32toU8(privateKeyRSA.modulus,n_len*4,n_addr,(rsa_bit_len>>3)%4);
	U32toU8(privateKeyRSA.exponent,n_len*4,d_addr,(rsa_bit_len>>3)%4);

	U32toU8(privateKeyRSA.prime[0],p_len*4,p_addr,(rsa_bit_len>>4)%4);
	U32toU8(privateKeyRSA.prime[1],q_len*4,q_addr,(rsa_bit_len>>4)%4);
	U32toU8(privateKeyRSA.primeExponent[0],p_len*4,dp_addr,(rsa_bit_len>>4)%4);
	U32toU8(privateKeyRSA.primeExponent[1],q_len*4,dq_addr,(rsa_bit_len>>4)%4);
	U32toU8(privateKeyRSA.coefficient,p_len*4,qinv_addr,(rsa_bit_len>>4)%4);

	if (fixkey == 0)		//公钥随机产生
	{
		U32toU8(publicKeyRSA.exponent,n_len*4,e_addr,(n_len>>1)%4);
	}

	if (result == 0)
	{
		return SUCCESS_ALG;
	}
	else
	{
		return FAIL_ALG;
	}
}

/*******************************************************************************
* Function Name  : RsaPubKey_2
* Description    : RSA公钥运算
* Input          : -fixkey          :E值是否固定
*                  -n_addr          ：N
*                  -e_addr          :E
*                  -in              :输入数据
*                  -in_len          :输入数据长度
* Output         : -out             ：输出数据
*                  -out_len         ：输出数据长度
* Return         : 执行结果
******************************************************************************/
int RsaPubKey_2(U8 fixkey, U8 *n_addr,U8* e_addr, U8 *in,U32 in_len,U8 *out,U32 *out_len)
{
	int result;
	U32 in_buf[256/4],out_buf[256/4];

	if ((in_len<64) ||(in_len>256) ||((in_len%4)!=0))	// 4的整数倍
	{
		return ERROR_DATA_LENGTH;
	}
	if((n_addr==NULL)  ||(in==NULL)  ||(out==NULL)  ||(out_len==NULL))
	{
		return ERROR_DATA_BUF;
	}

	if((CheckAddrType((U32)n_addr) == TYPE_ERROR)
	 ||(CheckAddrType((U32)in)     == TYPE_ERROR)
	 ||(CheckAddrType((U32)out)    != TYPE_SRAM))
	{
		return ERROR_DATA_BUF;
	}

	if ((fixkey!=0) && (fixkey!=1))
	{
		return ERROR_KEY_MODE;
	}
	if ((fixkey == 0) && (e_addr==NULL))		//公钥随机产生
	{
		return ERROR_DATA_BUF;
	}
	if(fixkey == 0)
	{
		if(CheckAddrType((U32)e_addr) == TYPE_ERROR)
		{
			return ERROR_DATA_BUF;
		}

	}
	memset(&publicKeyRSA,0x0,sizeof(publicKeyRSA));

	//组织公钥
	publicKeyRSA.bits = (in_len<<3);

	if (fixkey == 1)
	{
		publicKeyRSA.exponent[0] = 0x00010001;
	}
	else
	{
		U8toU32(e_addr,in_len,publicKeyRSA.exponent);
	}
	U8toU32(n_addr,in_len,publicKeyRSA.modulus);

	//组织输入数据
	U8toU32(in,in_len,in_buf);

	RsaModuleEnable();
	//加密
	result = RsaPubKey(out_buf,out_len,in_buf,in_len/4,&publicKeyRSA);
	RsaModuleDisable();

	if (result == 0)
	{
		*out_len = in_len;
		U32toU8(out_buf,in_len,out,in_len%4);
		return SUCCESS_ALG;
	}
	else
	{
		return result;
	}
}

/*******************************************************************************
* Function Name  : RsaPrivKey_2
* Description    : RSA私钥运算
* Input          :-n_addr          ：N
*                 -d_addr          :D
*                  -in              :输入数据
*                  -in_len          :输入数据长度
* Output         : -out             ：输出数据
*                  -out_len         ：输出数据长度
* Return         : 执行结果
******************************************************************************/
int RsaPrivKey_2(U8*n_addr,U8 *d_addr,U8 *in,U32 in_len,U8 *out,U32 *out_len)
{
	int result;
	U32 in_buf[256/4],out_buf[256/4];

	if ((in_len<64) ||(in_len>256) ||((in_len%4)!=0))	// 4的整数倍
	{
		return ERROR_DATA_LENGTH;
	}
	if((n_addr==NULL)  ||(d_addr==NULL) ||(in==NULL)  ||(out==NULL)  ||(out_len==NULL))
	{
		return ERROR_DATA_BUF;
	}

	if((CheckAddrType((U32)n_addr) == TYPE_ERROR)
	 ||(CheckAddrType((U32)d_addr) == TYPE_ERROR)
	 ||(CheckAddrType((U32)in)     == TYPE_ERROR)
	 ||(CheckAddrType((U32)out)    != TYPE_SRAM))
	{
		return ERROR_DATA_BUF;
	}


	memset(&privateKeyRSA,0x0,sizeof(privateKeyRSA));
	//组织私钥
	privateKeyRSA.bits = (in_len<<3);
	//privateKeyRSA.publicExponent[0] = 0x00010001;
	U8toU32(d_addr,in_len,privateKeyRSA.exponent);
	U8toU32(n_addr,in_len,privateKeyRSA.modulus);

	//组织输入数据
	U8toU32(in,in_len,in_buf);

	RsaModuleEnable();
	result = RsaPrivKey(out_buf,out_len,in_buf,in_len/4,&privateKeyRSA);
	RsaModuleDisable();

	if (result == 0)
	{
		*out_len = in_len;
		U32toU8(out_buf,in_len,out,in_len%4);
		return SUCCESS_ALG;
	}
	else
	{
		return result;
	}
}

/*******************************************************************************
* Function Name  : RsaPrivKey_3
* Description    : RSA私钥运算(DPA = High)
* Input          :-n_addr          ：N
*                 -d_addr          :D
*                  -in              :输入数据
*                  -in_len          :输入数据长度
* Output         : -out             ：输出数据
*                  -out_len         ：输出数据长度
* Return         : 执行结果
******************************************************************************/
int RsaPrivKey_3(U8*n_addr,U8 *d_addr,U8 *e_addr,U8 *in,U32 in_len,U8 *out,U32 *out_len)
{
	int result;
	U32 in_buf[256/4],out_buf[256/4];

	if ((in_len<64) ||(in_len>256) ||((in_len%4)!=0))	// 4的整数倍
	{
		return ERROR_DATA_LENGTH;
	}
	if((n_addr==NULL)  ||(d_addr==NULL) ||(e_addr==NULL) ||(in==NULL)  ||(out==NULL)  ||(out_len==NULL))
	{
		return ERROR_DATA_BUF;
	}

	if((CheckAddrType((U32)n_addr) == TYPE_ERROR)
	 ||(CheckAddrType((U32)d_addr) == TYPE_ERROR)
	 ||(CheckAddrType((U32)in)     == TYPE_ERROR)
	 ||(CheckAddrType((U32)out)    != TYPE_SRAM))
	{
		return ERROR_DATA_BUF;
	}


	memset(&privateKeyRSA,0x0,sizeof(privateKeyRSA));
	memset(&publicKeyRSA,0x0,sizeof(publicKeyRSA));
	//组织私钥
	privateKeyRSA.bits = (in_len<<3);
	//privateKeyRSA.publicExponent[0] = 0x00010001;
	U8toU32(d_addr,in_len,privateKeyRSA.exponent);
	U8toU32(n_addr,in_len,privateKeyRSA.modulus);

	U8toU32(e_addr,in_len,publicKeyRSA.exponent);
	U8toU32(n_addr,in_len,publicKeyRSA.modulus);

	//组织输入数据
	U8toU32(in,in_len,in_buf);

	RsaModuleEnable();
	result = RsaPrivKey(out_buf,out_len,in_buf,in_len/4,&privateKeyRSA,&publicKeyRSA);
	RsaModuleDisable();

	if (result == 0)
	{
		*out_len = in_len;
		U32toU8(out_buf,in_len,out,in_len%4);
		return SUCCESS_ALG;
	}
	else
	{
		return result;
	}
}

/*******************************************************************************
* Function Name  : RsaPrivKeyCRT_2
* Description    : RSA CRT模式运算
* Input          : -p_addr          :P
*                  -q_addr          :Q
*                  -dp_addr         :dP
*                  -dq_addr         :dQ
*                  -qinv_addr       :PQ
*                  -in              :输入数据
*                  -in_len          :输入数据长度
* Output         : -out             ：输出数据
*                  -out_len         :输出数据长度
* Return         : 执行结果
******************************************************************************/
int RsaPrivKeyCRT_2(U8*p_addr,U8*q_addr,U8*dp_addr,U8*dq_addr,U8*qinv_addr,U8 *in,U32 in_len,U8 *out,U32 *out_len)
{
	int result;
	U32 in_buf[256/4],out_buf[256/4];
//	U32 n_len;

	if ((in_len<64) ||(in_len>256) ||((in_len%4)!=0))	// 4的整数倍
	{
		return ERROR_DATA_LENGTH;
	}
	if((p_addr==NULL)  ||(q_addr==NULL)  ||(dp_addr==NULL)  ||(dq_addr==NULL)  ||(qinv_addr==NULL) ||(in==NULL)  ||(out==NULL)  ||(out_len==NULL))
	{
		return ERROR_DATA_BUF;
	}

	if((CheckAddrType((U32)p_addr) == TYPE_ERROR)
	 ||(CheckAddrType((U32)q_addr) == TYPE_ERROR)
	 ||(CheckAddrType((U32)dp_addr) == TYPE_ERROR)
	 ||(CheckAddrType((U32)dq_addr) == TYPE_ERROR)
	 ||(CheckAddrType((U32)qinv_addr) == TYPE_ERROR)
	 ||(CheckAddrType((U32)in)     == TYPE_ERROR)
	 ||(CheckAddrType((U32)out)    != TYPE_SRAM))
	{
		return ERROR_DATA_BUF;
	}

	memset(&privateKeyRSA,0x0,sizeof(privateKeyRSA));

	//组织私钥
	privateKeyRSA.bits = (in_len<<3);
	//privateKeyRSA.publicExponent[0] = 0x00010001;
	//注意： CRT模式，这里要置成全FF
	memset(privateKeyRSA.modulus,0xFF,sizeof(privateKeyRSA.modulus));
	memset(privateKeyRSA.prime,0,sizeof(privateKeyRSA.prime[0]));
	memset(privateKeyRSA.prime,0,sizeof(privateKeyRSA.prime[1]));
	memset(privateKeyRSA.primeExponent,0,sizeof(privateKeyRSA.primeExponent[0]));
	memset(privateKeyRSA.primeExponent,0,sizeof(privateKeyRSA.primeExponent[1]));
	memset(privateKeyRSA.coefficient,0,sizeof(privateKeyRSA.coefficient));

//	n_len = in_len /4;
	U8toU32(p_addr,in_len/2,privateKeyRSA.prime[0]);
	U8toU32(q_addr,in_len/2,privateKeyRSA.prime[1]);
	U8toU32(dp_addr,in_len/2,privateKeyRSA.primeExponent[0]);
	U8toU32(dq_addr,in_len/2,privateKeyRSA.primeExponent[1]);
	U8toU32(qinv_addr,in_len/2,privateKeyRSA.coefficient);
	//组织输入数据
	U8toU32(in,in_len,in_buf);

	RsaModuleEnable();
	result = RsaPrivKeyCRT(out_buf,out_len,in_buf,in_len/4,&privateKeyRSA);
	RsaModuleDisable();

	if (result == 0)
	{
		*out_len = in_len;
		U32toU8(out_buf,in_len,out,0);
		return SUCCESS_ALG;
	}
	else
	{
		return result;
	}
}

