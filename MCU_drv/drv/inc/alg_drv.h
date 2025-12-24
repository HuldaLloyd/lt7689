/*
 * alg_drv.h
 *
 *  Created on: 2017年6月9日
 *      Author: ccore
 */

#ifndef ALG_DRV_H_
#define ALG_DRV_H_
#include "type.h"
#include "memmap.h"

#define SRAM_BEGIN    (UINT32)(0x20000000)
#define SRAM_END      (UINT32)(0x20038000)

#define EFLASH_BEGIN  (UINT32)(0x8000000)
#define EFLASH_END    (UINT32)(0x807F000)

typedef enum
{
	TYPE_ERROR = 0,
	TYPE_SRAM  = 1,
	TYPE_EFLASH= 2,
}TYPE_ADDR;

/*******Hash算法类型编号********/
#define	HASH_SM3				0
#define	HASH_SHA0				1
#define	HASH_SHA1				2
#define	HASH_SHA224				3
#define HASH_SHA256				4
#define HASH_SHA384				5
#define HASH_SHA512				6

/*******Hash算法数据结构********/
typedef	struct Hash_tx
{
	unsigned int  hash_len[4];	   //本次传送数据长度；
	unsigned int  DataBuf[16];     //本次运算结果
	unsigned char HASHMode;	       //Hash算法类型编号
	unsigned char fb_flag[1];      //第一包数据标志,1 - 第一包数据传输完;0 - 未完成

}Hash_tx;

#define SHA0_R_Len				20
#define SHA1_R_Len  			20
#define SHA224_R_Len 			28
#define SHA256_R_Len 			32
#define SM3_R_Len				32
#define SHA384_R_Len 			48
#define SHA512_R_Len 			64

//sm2
#define	SM2_TEST_BITS       	256
//大数运算相关的数据结构定义与运算函数
#define ECC_BIGINT32_MAXLEN 	12

/*******大数的32位数组结构********/
typedef struct
{
 	UINT16   uLen;                         //32位数组的实际长度
	UINT32   auValue[ECC_BIGINT32_MAXLEN]; //32位数组的值，数组的低32位地址存放大数的低32位有效数，高32位地址存放大数的高32位有效数
}ECC_STU_BIGINT32;

/*******SM2参数据结构********/
typedef struct
{
    UINT16 uBits;                               // 模数P的位数
	ECC_STU_BIGINT32  stuPrimeP;				// 模数P
	ECC_STU_BIGINT32  stuCoefficientA;			// 椭圆系数A
	ECC_STU_BIGINT32  stuCoefficientB;			// 椭圆系数B
	ECC_STU_BIGINT32  stuGx;				    // 椭圆基点坐标Gx
	ECC_STU_BIGINT32  stuGy;				    // 椭圆基点坐标Gy
	ECC_STU_BIGINT32  stuPrimeN;				// 椭圆基点坐标G的阶
} SM2_STU_PRAMS;

/******SM2公钥结构********/
typedef struct
{
	ECC_STU_BIGINT32  stuQx;			        // 公钥坐标Qx
	ECC_STU_BIGINT32  stuQy;				    // 公钥坐标Qy
} SM2_STU_PUBKEY;


extern SM2_STU_PRAMS stuEccPrams;
extern SM2_STU_PUBKEY stuPubKey;
extern ECC_STU_BIGINT32 stuPrivKey;
extern ECC_STU_BIGINT32 stuSignatureR;
extern ECC_STU_BIGINT32 stuSignatureS;


//rsa
//#define		RSA_TEST_BITS	512
//#define		RSA_TEST_BITS	1024
#define		RSA_TEST_BITS	2048

//#define MAX_RSA_MODULUS_BITS 512
//#define MAX_RSA_MODULUS_BITS 1024
#define MAX_RSA_MODULUS_BITS 2048

#define MAX_RSA_MODULUS_LEN ((MAX_RSA_MODULUS_BITS + 31) / 32)
#define MAX_RSA_PRIME_BITS	((MAX_RSA_MODULUS_BITS +  1) / 2 )
#define MAX_RSA_PRIME_LEN	((MAX_RSA_PRIME_BITS   + 31) / 32)

/******RSA公钥结构********/
typedef struct
{
	unsigned int bits;                                /* length in bits of modulus */
	unsigned int modulus[MAX_RSA_MODULUS_LEN];        /* modulus */
	unsigned int exponent[MAX_RSA_MODULUS_LEN];       /* public exponent */
} STU_RSA_PUBKEY;

/******RSA私钥结构********/
typedef struct
{
	unsigned int bits;                                  /* length in bits of modulus */
	unsigned int modulus[MAX_RSA_MODULUS_LEN];          /* modulus */
	unsigned int publicExponent[MAX_RSA_MODULUS_LEN];   /* public exponent */
	unsigned int exponent[MAX_RSA_MODULUS_LEN];         /* private exponent */
	unsigned int prime[2][MAX_RSA_PRIME_LEN];           /* prime factors */
	unsigned int primeExponent[2][MAX_RSA_PRIME_LEN];   /* exponents for CRT */
	unsigned int coefficient[MAX_RSA_PRIME_LEN];        /* CRT coefficient */
} STU_RSA_PRIVKEY;

typedef struct
{
	unsigned int  bits;               /*length in bits of modulus*/
	unsigned char *modulus;           /* modulus */
	unsigned char *exponent;          /* public exponent */
	unsigned int  explen;             /* exponent bytes  */
}rsa_pub_key_t;

extern STU_RSA_PUBKEY  publicKeyRSA;
extern STU_RSA_PRIVKEY privateKeyRSA;

/*******************************************************************************
* Function Name  : AES_Enable
* Description    : 使能AES模块
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void AES_Enable(void);

/*******************************************************************************
* Function Name  : AES_Enable
* Description    : 禁用AES模块
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void AES_Disable(void);

/*******************************************************************************
* Function Name  : AES_EnDecrypt
* Description    : 通过AES (128bits、192bits、256bits)进行加解密运算
*
*                 备注：
*                 1：原来的库此函数内部运算之前打开了EDMAC时钟，退出之前关闭了EDMAC时钟;调用完本函数之后，如其它操作使用EDMAC模块，需要先主动打开EDMAC模块时钟；
*                 2：原库与新库参数的顺序有所不同；
*                 3：本库中内部对EDMAC时钟未进行任何操作；
*
* Input          : EncDe_Mode - 加解密选择
*                      0 - 表示加密;
*                      1 - 表示解密.
*                  CTR_ECB_MODE - 模式选择
*                      0 - ECB模式;
*                      1 - CTR模式.
*                  Key  -  加解密密钥
*                  cntIV -  CTR模式初始向量值
*                  key_mode - AES算法密钥长度
*                      0 - 128bit;
*                      1 - 192bit;
*                      2 - 256bit.
*                  srcbuffer - 加解密数据地址
*                  Output - 加解密结果地址
*                  datalen - 加解密数据长度，16字节的整数倍
* Output         : None
* Return         : None
******************************************************************************/
extern void AES_EnDecrypt(U8 EncDe_Mode, U8 CTR_ECB_MODE,U32 *Key, U32 *cntIV,U32 key_mode, U32 *srcbuffer, U32 *Output, U32 datalen);

/*******************************************************************************
* Function Name  : DES_Enable
* Description    : 使能DES模块
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void DES_Enable(void);

/*******************************************************************************
* Function Name  : DES_Disable
* Description    : 禁用DES模块
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void DES_Disable(void);

/*******************************************************************************
* Function Name  : AES_EnDecrypt
* Description    : 通过des/3des (64bits、128bits、192bits) 进行加解密运算
*
*                 备注：
*                 1：原来的库此函数内部运算之前打开了EDMAC时钟，退出之前关闭了EDMAC时钟;调用完本函数之后，如其它操作使用EDMAC模块，需要先主动打开EDMAC模块时钟；
*                 2：本库中内部对EDMAC时钟未进行任何操作；
*
* Input          : EncDe_Mode - 加解密选择
*                      0 - 表示加密;
*                      1 - 表示解密.
*                  ECB_CBC_Mode - 模式选择
*                      0 - ECB模式;
*                      1 - CBC模式
*                  Key  -  加解密密钥
*                  KeyLen - 加解密密钥长度，单位为字节
*                      8  - 64bit的des加解密;
*                      16 - 128bit的3des加解密;
*                      24 - 192bit的3des加解密.
*                  ivbuffer - 初始向量值，固定为8字节长度的值
*                  srcbuffer - 加解密数据地址
*                  Output - 加解密结果地址
*                  datalen - 加解密数据长度，8字节的整数倍
*                  check_parity：输入的key是否需要进行校验，如果校验错误则返回key校验错误不再继续加解密
*                      1 - 打开此功能;
*                      0 - 禁用此功能.
* Output         : None
* Return         : None
******************************************************************************/
extern void DES_EnDecrypt(U8 EncDe_Mode, U8 ECB_CBC_Mode, U32 *Key, U32 KeyLen, U32 *ivbuffer, U32 *srcbuffer, U32 *Output,U32 datalen, U8 check_parity);

/*******************************************************************************
* Function Name  : SMS4_Enable
* Description    : 使能SMS4模块
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void SMS4_Enable(void);

/*******************************************************************************
* Function Name  : SMS4_Disable
* Description    : 禁用SMS4模块
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void SMS4_Disable(void);

/*******************************************************************************
* Function Name  : SMS4_EnDecrypt
* Description    : 通过SMS4模块对数据加解密
*
*                 备注：
*                 1：原来的库此函数内部运算之前打开了EDMAC时钟，退出之前关闭了EDMAC时钟;调用完本函数之后，如其它操作使用EDMAC模块，需要先主动打开EDMAC模块时钟；
*                 2：本库中内部对EDMAC时钟未进行任何操作；
*
* Input          : EncDe_Mode - 加解密选择
*                      0 - 表示加密;
*                      1 - 表示解密.
*                  ECB_CBC_Mode - 模式选择
*                      0 - ECB模式;
*                      1 - CBC模式
*                  KEY - 128位加解密密钥
*                  IV - 128位初始向量
*                  ivbuffer - 初始向量值，固定为8字节长度的值
*                  srcbuffer - 加解密数据地址
*                  Output - 加解密结果地址
*                  datalen - 加解密数据长度，16字节的整数倍
* Output         : None
* Return         : None
******************************************************************************/
extern void SMS4_EnDecrypt (unsigned char EncDe_Mode, unsigned char ECB_CBC_Mode, unsigned int *KEY, unsigned int*IV, unsigned int *srcbuffer, unsigned int *Output, unsigned int datalen);

/*******************************************************************************
* Function Name  : SHA_Init
* Description    : SHA算法模块初始化
*
*                 备注：
*                 1：使用SHA模块函数之前必需先主动打开SHA、EDAMC模块时钟;
*                 2：与原来版本相比较增加Hash_tx *phash_tx参数；
*
* Input          : phash_tx - 哈希算法数据结构
*                  HashModeNum - 哈希算法编号，取值范围0~4
* Output         : None
* Return         : None
******************************************************************************/
extern void SHA_Init(Hash_tx *phash_tx, unsigned char HashModeNum);

/*******************************************************************************
* Function Name  : SHA_Update
* Description    : 发送要进行SHA运算的数据
*
*                 备注：
*                 1：使用SHA模块函数之前必需先主动打开SHA、EDAMC模块时钟；
*                 2：与原来版本相比较增加Hash_tx *phash_tx参数。
*
* Input          : phash_tx - 哈希算法数据结构
*                  pMessageBlock – 指向本次待处理的数据，地址最好4字节对齐
*                  DataLen – 本次待处理数据长度，长度最好为4的整数倍
* Output         : None
* Return         : None
******************************************************************************/
extern void SHA_Update(Hash_tx *phash_tx, unsigned char *pMessageBlock, unsigned int DataLen);

/*******************************************************************************
* Function Name  : SHA_Final
* Description    : 结束SHA操作并获取结果
*
*                 备注：
*                 1：使用SHA模块函数之前必需先主动打开SHA、EDAMC模块时钟；
*                 2：与原来版本相比较增加Hash_tx *phash_tx参数。
*
* Input          : phash_tx - 哈希算法数据结构
*                  pRes – 哈希运算结果
* Output         : None
* Return         : 本次SHA运算结果字节长度
******************************************************************************/
extern unsigned int SHA_Final(Hash_tx *phash_tx, unsigned int *pRes);

/*******************************************************************************
* Function Name  : SM2_Enable
* Description    : SM2模块使能
*
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void SM2_Enable(void);

/*******************************************************************************
* Function Name  : SM2_Disable
* Description    : SM2模块禁用
*
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void SM2_Disable(void);

/*******************************************************************************
* Function Name  : SM2Init
* Description    : SM2模块初始化
*
*                 备注：
*                 原库函数名称为ECCInit(unsigned short bits);
*
* Input          : bits - SM2位数
* Output         : None
* Return         : None
******************************************************************************/
//extern void SM2Init(unsigned short bits);

/*******************************************************************************
* Function Name  : SM2GenerateKeyPair
* Description    : SM2生成的密钥对
*
*                 备注：
*                 1：此函数内部会使用随机数模块，需打开随机数模块时钟并调用InitTrng函数；
*                 2：函数内部使用EDMAC，需要主动打开EDMAC模块时钟；
*                 3：内部使用CRYPTO模块，需要主动打开CRYPTO模块时钟；
*
* Input          : pstuSM2Prams->uBits			           模数P的位数
*                  pstuSM2Prams->stuCoefficientA	椭圆系数A
*                  pstuSM2Prams->stuCoefficientA	椭圆系数B
*                  pstuSM2Prams->stuGx			           椭圆基点坐标Gx
*                  pstuSM2Prams->stuGy			           椭圆基点坐标Gy
*                  pstuSM2Prams->stuPrimeN		           椭圆基点坐标G的阶
*                  pstuPrivKey		                                           存放生成的私钥
*                  pstuPbuKey					           存放生成的公钥
* Output         : None
* Return         : 1 - 密钥对产生正确
*                  0 - 密钥对产生失败
******************************************************************************/
extern char SM2GenerateKeyPair(SM2_STU_PRAMS *pstuSM2Prams, ECC_STU_BIGINT32 *pstuPrivKey,SM2_STU_PUBKEY *pstuPbuKey);

/*******************************************************************************
* Function Name  : SM2Encrypt_V2
* Description    : 使用公钥对明文加密，产生的密文长度 = 明文长度 + 96字节
*                  SM2Encrypt_V2：使用公钥对明文加密，成生的密文存储方式为：C1|C3|C2；
*
*                 备注：
*                 1.函数内部使用EDMAC，需要打开EDMAC模块时钟；
*                 2.函数内部使用SHA模块，需要打开SHA模块时钟；
*                 3.内部使用CRYPTO模块，需要打开CRYPTO模块时钟；
*
* Input          : mes	明文，高字节在后，低字节在前（大端结构），mes[0]为高字节，若最后一个字节不满32位，则在该字节的末尾填0直到满32位;
*                       例如mes若为48bit长的123456789abc，则mes[0]为0x12345678，mes[1]为0x9abc0000，
*                  klen       		明文字节长度
*                  pstuPubKey		公钥
*                  pstuEccPrams   	椭圆曲线参数
*                  cipher		          存放输出的密文，存储方式和明文相同
* Output         : None
* Return         : None
******************************************************************************/
extern void SM2Encrypt_V2(unsigned char *mes, unsigned short klen, SM2_STU_PUBKEY *pstuPubKey,SM2_STU_PRAMS *pstuEccPrams, unsigned char *cipher);

/*******************************************************************************
* Function Name  : SM2Decrypt_V2
* Description    : 使用私钥对密文解密
*                  SM2Decrypt_V2：使用私钥对存储方式为C1|C3|C2的密文进行解密；
*
*                 备注：
*                 1.函数内部使用EDMAC，需要打开EDMAC模块时钟；
*                 2.函数内部使用SHA模块，需要打开SHA模块时钟；
*                 3.内部使用CRYPTO模块，需要打开CRYPTO模块时钟；
*                 4.原库：使用私钥对存储方式为C1|C3|C2的密文进行解密
*
* Input          : cipher		  	 密文，大端结构
*                  klen         	 明文字节长度
*                  stuPrivKey		 私钥
*                  pstuEccPrams     椭圆曲线参数
*                  plain：		           输出明文，大端结构
* Output         : None
* Return         : 1 - 成功
*                  0 - 失败
******************************************************************************/
extern UINT8 SM2Decrypt_V2(unsigned char *cipher, unsigned short klen, ECC_STU_BIGINT32 *stuPrivKey, SM2_STU_PRAMS *pstuEccPrams, unsigned char *plain);

/*******************************************************************************
* Function Name  : SM2Signature
* Description    : 使用私钥对明文进行签名；
*
*                 备注：
*                 1.此函数内部使用随机数模块，需要打开模块时钟;
*                 2.此函数内部使用EDAMC模块，需要打开模块时钟;
*                 3.此函数内部使用CRYPTO模块，需要请打开模块时钟;
*
* Input          : mes				  明文
*                  klen			             明文字节长度
*                  pstuEccPrams	             椭圆曲线参数
*                  pstuPubKey		  公钥
*                  pSignatureR	             输出明文的签名信息R
*                  pSignatureS	             输出明文的签名信息S
* Output         : None
* Return         : None
******************************************************************************/
extern void SM2Signature(unsigned char *mes, unsigned short klen, SM2_STU_PRAMS *pstuEccPrams, ECC_STU_BIGINT32 *pstuPrivKey, ECC_STU_BIGINT32 *pSignatureR, ECC_STU_BIGINT32 *pSignatureS);

/*******************************************************************************
* Function Name  : SM2SignatureWithIDA
* Description    : 使用私钥及身份信息对明文进行签名；
*
*                 备注：
*                 1.此函数内部使用随机数模块，需要打开模块时钟;
*                 2.此函数内部使用SHA模块，需要请打开模块时钟;
*                 3.此函数内部使用EDMAC模块，需要请打开模块时钟;
*                 4.此函数内部使用CRYPTO模块，需要请打开模块时钟;
*
* Input          : mes				  明文
*                  klen			             明文字节长度
*                  IDA				  签名方的身份信息
*                  entla		             签名方身份信息字节长度
*                  pstuEccPrams	             椭圆曲线参数
*                  pstuPubKey		  公钥
*                  pstuPrivKey	             私钥
*                  pSignatureR	             输出明文的签名信息R
*                  pSignatureS	             输出明文的签名信息S
* Output         : None
* Return         : None
******************************************************************************/
extern void SM2SignatureWithIDA(unsigned char *mes, unsigned short klen, unsigned char *IDA, unsigned short entla, SM2_STU_PRAMS *pstuEccPrams, SM2_STU_PUBKEY *pstuPubKey, ECC_STU_BIGINT32 *pstuPrivKey, ECC_STU_BIGINT32 *pSignatureR,ECC_STU_BIGINT32 *pSignatureS);

/*******************************************************************************
* Function Name  : SM2Verification
* Description    : 使用公钥对明文和签名进行验证；
*
*                 备注：
*                 1.此函数内部使用EDAMC模块，需要打开模块时钟;
*                 2.此函数内部使用CRYPTO模块，需要打开模块时钟;
*
* Input          : mes				    明文
*                  klen				    明文字节长度
*                  IDA			              签名方的身份信息
*                  entla			   签名方身份信息的字节长度（备注：原来库为签名方身份信息的bit长度）
*                  pstuEccPrams		   椭圆曲线参数
*                  pstuPubKey		   公钥
*                  pSignatureR		   明文的签名信息R
*                  pSignatureS		   明文的签名信息S
* Output         : None
* Return         : 1 - 验证通过
*                  0 - 验证失败
******************************************************************************/
extern UINT8 SM2Verification(unsigned char *mes, unsigned short klen, SM2_STU_PRAMS *pstuEccPrams, SM2_STU_PUBKEY *pstuPubKey, ECC_STU_BIGINT32 *pSignatureR, ECC_STU_BIGINT32 *pSignatureS);

/*******************************************************************************
* Function Name  : SM2VerificationWithIDA
* Description    : 使用公钥及身份信息对明文和签名进行验证；
*
*                 备注：
*                 1.此函数内部使用CRYPTO模块，需要打开模块时钟;
*                 2.此函数内部使用SHA模块，需要打开模块时钟;
*                 3.此函数内部使用EDMAC模块，需要打开模块时钟;
*                 4.此函数内部使用CRYPTO模块，需要打开模块时钟;
*
* Input          : mes				    明文
*                  klen				    明文字节长度
*                  IDA			              签名方的身份信息
*                  entla			   签名方身份信息的字节长度（备注：原来库为签名方身份信息的bit长度）
*                  pstuEccPrams		   椭圆曲线参数
*                  pstuPubKey		   公钥
*                  pSignatureR		   明文的签名信息R
*                  pSignatureS		   明文的签名信息S
* Output         : None
* Return         : 1 - 验证通过
*                  0 - 验证失败
******************************************************************************/
extern UINT8 SM2VerificationWithIDA(unsigned char *mes, unsigned short klen, unsigned char *IDA, unsigned short entla, SM2_STU_PRAMS *pstuEccPrams, SM2_STU_PUBKEY *pstuPubKey, ECC_STU_BIGINT32 *pSignatureR, ECC_STU_BIGINT32 *pSignatureS);

/*******************************************************************************
* Function Name  : sign
* Description    : 输出用户的身份信息
*
*                 备注：
*                 1.此函数内部使用SHA（SM3）模块，需要打开模块时钟;
*                 2.此函数内部使用EDMAC模块，需要打开模块时钟;
*
* Input          : IDA				           签名方的身份信息
*                  Entla                签名方身份信息的字节长度（备注：原库为签名方身份信息的bit长度）
*                  pstuPubKey			公钥
*                  pstuEccPrams		          椭圆曲线参数
*                  za				          输出用户的身份信息
* Output         : None
* Return         : None
******************************************************************************/
extern void sign(unsigned char *IDA, unsigned short entla, SM2_STU_PUBKEY *pstuPubKey, SM2_STU_PRAMS *pstuEccPrams, unsigned int *za);

/*******************************************************************************
* Function Name  : SM2KeyExchange
* Description    : 交换用户A和用户B的密钥
*
*                 备注：
*                 1.此函数内部使用SHA（SM3）模块，需要打开模块时钟;
*                 2.此函数内部使用EDMAC模块，需要打开模块时钟;
*                 3.此函数内部使用CRYPTO模块，需要打开模块时钟;
*
* Input          : pstuEccPrams			椭圆曲线参数
*                  pSelfPrikey			已方私钥
*                  pSelfTempPubkey		已方临时公钥
*                  pSelfTempPrikey		已方临时私钥
*                  pOtherPubkey			对方公钥
*                  pOtherTempPubkey		对方临时公钥
*                  za				          已方Z值
*                  zb					对方Z值
*                  pAgreedKey			协商密钥
*                  AgreedKeyLen			协商密钥长度
*                  sA					SA或S2
*                  sB					SB或S1
*                  Mode					0表示已方为A方，1表示已方为B方
*
* Output         : None
* Return         : 1 - 验证通过
*                  0 - 验证失败
******************************************************************************/
extern UINT8 SM2KeyExchange(SM2_STU_PRAMS *pstuEccPrams, ECC_STU_BIGINT32 *pSelfPrikey, SM2_STU_PUBKEY *pSelfTempPubkey, ECC_STU_BIGINT32 *pSelfTempPrikey, SM2_STU_PUBKEY*pOtherPubkey,
		SM2_STU_PUBKEY *pOtherTempPubkey, UINT32 *za, UINT32 *zb, UINT32 *pAgreedKey, UINT32 AgreedKeyLen, UINT32 *sA, UINT32 *sB, UINT32 Mode);

/*******************************************************************************
* Function Name  : EccBig32Initial
* Description    : 大数的初始化。数组值全赋值为0，数组长度记为1
*
*
* Input          : None
*
* Output         : pstuA            指向结构体ECC_STU_BIGINT的指针，初始化后的pstuA
* Return         : None
******************************************************************************/
extern void EccBig32Initial(ECC_STU_BIGINT32 *pstuA);

/*******************************************************************************
* Function Name  : EccCharToHex
* Description    : 字符型数据转化成为Hex格式数据
*
*
* Input          : pChar            指向需要转化的字符型数据存储地址
*                  uLen             转化数据的长度，即pChar数据的字节数
* Output         : pHex             指向转化成Hex格式数据的存储地址
* Return         : None
******************************************************************************/
extern void EccCharToHex(UINT8 *pChar, UINT8 *pHex, UINT16 uLen);

/*******************************************************************************
* Function Name  : EccByBufToBigInt32
* Description    : 输入无符号8位整型数组转换为结构体ECC_STU_BIGINT32的大数格式
*                  例如：长度为8的数据：
*                             pbyOutBuf[0]=0x55;
*							  pbyOutBuf[1]=0x66;
*							  pbyOutBuf[2]=0x77;
*							  pbyOutBuf[3]=0x88;
*                             pbyOutBuf[4]=0x11;
*							  pbyOutBuf[5]=0x22;
*							  pbyOutBuf[6]=0x33;
*							  pbyOutBuf[7]=0x44;
*                        转化为 长度为2的大数 ：
*                             pstuInBigInt->auValue[0]=0x11223344;
*  				              pstuInBigInt->auValue[1]=0x55667788;
*
* Input          : pbyInBuf         指向无符号8位字符型数组首地址
*                  uInLen           无符号8位字符型数组的长度
* Output         : pstuOutBigInt    指向结构体ECC_STU_BIGINT32的地址，输出转换后的对应大数
* Return         : None
******************************************************************************/
void EccByBufToBigInt32(UINT8 *pbyInBuf,UINT16 uInLen, ECC_STU_BIGINT32 *pstuOutBigInt);

/*******************************************************************************
* Function Name  : TestKeyChange
* Description    : SM2交换密钥测试函数
*
* Input          : pstuPrivKey_A      己方私钥
*                  pstuPubKey_A       己方公钥
*                  pstuPrivKey_B      对方私钥
*                  pstuPubKey_B       对方公钥
*                  pstuEccPrams       ECC参数
*                  za                 己方z值
*                  zb                 对方z值
*
* Output         : None
* Return         : 1 - 成功
*                  0 - 失败
******************************************************************************/
UINT8 TestKeyChange(ECC_STU_BIGINT32 *pstuPrivKey_A,SM2_STU_PUBKEY *pstuPubKey_A,ECC_STU_BIGINT32 *pstuPrivKey_B,SM2_STU_PUBKEY *pstuPubKey_B,SM2_STU_PRAMS *pstuEccPrams,unsigned int *za,unsigned int *zb);

/*******************************************************************************
* Function Name  : AB_Check
* Description    : 交换用户之间参数检测
*
* Input          : s1
*                  sB
*                  sA
*                  s2
*                  pstuEccPrams     Ecc参数
*
* Output         : None
* Return         : 1 - 成功
*                  0 - 失败
******************************************************************************/
extern UINT8 AB_Check(UINT32 *s1, UINT32 *sB, UINT32 *sA, UINT32 *s2, SM2_STU_PRAMS *pstuEccPrams);

/*******************************************************************************
* Function Name  : SM2VerifySA
* Description    : USER_B verification USER_A
*
* Input          : s2
*                  sA
*                  pstuEccPrams     Ecc参数
*
* Output         : None
* Return         : 1 - 成功
*                  0 - 失败
******************************************************************************/
extern UINT8 SM2VerifySA(UINT32 *s2, UINT32 *sA, SM2_STU_PRAMS *pstuEccPrams);

/*******************************************************************************
* Function Name  : SM2GenerateKA
* Description    : USER_A verification USER_B , then Send choice(SA) to USER_B
*
* Input          : pstuEccPrams
*                  pstuPubKey_B
*                  pstuPrivKey_A
*                  za
*                  zb
*                  rA
*                  sB
*                  sA
*                  RA_x1
*                  RA_y1
*                  RB_x2
*                  RB_y2
*                  kA
*
* Output         : None
* Return         : 1 - 成功
*                  0 - 失败
******************************************************************************/
extern UINT8 SM2GenerateKA(SM2_STU_PRAMS *pstuEccPrams, SM2_STU_PUBKEY *pstuPubKey_B, ECC_STU_BIGINT32 *pstuPrivKey_A, UINT32 *za, UINT32 *zb,
		ECC_STU_BIGINT32 *rA, UINT32 *sB, UINT32 *sA, ECC_STU_BIGINT32 *RA_x1, ECC_STU_BIGINT32 *RA_y1, ECC_STU_BIGINT32 *RB_x2, ECC_STU_BIGINT32 *RB_y2, UINT32 *kA);

/*******************************************************************************
* Function Name  : SM2GenerateRBKB
* Description    : USER_B response USER_A, then Send RB and choice(SB) to USER_A
*
* Input          : pstuEccPrams
*                  pstuPubKey_A
*                  pstuPrivKey_B
*                  sB
*                  s2
*                  za
*                  zb
*                  RA_x1
*                  RA_y1
*                  RB_x2
*                  RB_y2
*                  kB
*
* Output         : None
* Return         : None
******************************************************************************/
extern void SM2GenerateRBKB(SM2_STU_PRAMS *pstuEccPrams, SM2_STU_PUBKEY *pstuPubKey_A, ECC_STU_BIGINT32 *pstuPrivKey_B, UINT32 *sB, UINT32 *s2, UINT32 *za, UINT32 *zb,
		ECC_STU_BIGINT32 *RA_x1, ECC_STU_BIGINT32 *RA_y1, ECC_STU_BIGINT32 *RB_x2, ECC_STU_BIGINT32 *RB_y2, UINT32 *kB);

/*******************************************************************************
* Function Name  : SM2GenerateRBKB
* Description    : USER_B response USER_A, then Send RB and choice(SB) to USER_A
*
* Input          : pstuEccPrams
*                  RA_x1
*                  RA_y1
*                  rA
*
* Output         : None
* Return         : None
******************************************************************************/
extern void SM2GenerateRA(SM2_STU_PRAMS *pstuEccPrams, ECC_STU_BIGINT32 *RA_x1, ECC_STU_BIGINT32 *RA_y1, ECC_STU_BIGINT32 *rA);

/*******************************************************************************
* Function Name  : RsaModuleEnable
* Description    : 使能RSA模块
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void RsaModuleEnable(void);

/*******************************************************************************
* Function Name  : RsaModuleDisable
* Description    : 禁用RSA模块
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void RsaModuleDisable(void);

/*******************************************************************************
* Function Name  : RsaGenerateKeyPair
* Description    : RSA 密钥对生产
* Input          : mode       RSA算法位数，应填写值为，1024、2048等。
*                  fixkey     公钥形式
*                      0 - 公钥不固定，随机产生
*                      1 - 公钥固定为0x10001
*                      2 - 公钥外部导入
*                  crt        RSA密钥格式
*                      0 - 密钥格式为普通格式
*                      1 - 密钥格式为CRT格式
* Output         : pub_key    输出公钥地址
*                  prv_key    输出私钥地址
* Return         : 0 - 成功
*                  1 - 失败
******************************************************************************/
extern unsigned char RsaGenerateKeyPair( unsigned int mode, STU_RSA_PUBKEY * pub_key, STU_RSA_PRIVKEY * prv_key,unsigned char fixkey, unsigned char crt);

/*******************************************************************************
* Function Name  : RsaPubKey
* Description    : RSA 公钥运算
* Input          : input        输入数据地址
*                  inputLen     输入数据长度
*                  publicKey    RSA公钥地址
*
* Output         : output       输出数据地址
*                  outputLen    输出数据长度

* Return         : 0   - 成功
*                  非0 - 失败(0x0401)
******************************************************************************/
extern int RsaPubKey(unsigned int *output, unsigned int *outputLen, unsigned int *input, unsigned int  inputLen, STU_RSA_PUBKEY *publicKey);

/*******************************************************************************
* Function Name  : RsaPubKey
* Description    : RSA 公钥运算
* Input          : input        输入数据地址
*                  inputLen     输入数据长度
*                  privateKey   RSA私钥地址
*
* Output         : output       输出数据地址
*                  outputLen    输出数据长度

* Return         : 0   - 成功
*                  非0 - 失败(0x0401)
******************************************************************************/
extern int RsaPrivKey(unsigned int *output, unsigned int *outputLen, unsigned int *input, unsigned int inputLen, STU_RSA_PRIVKEY * privateKey, ...);

/*******************************************************************************
* Function Name  : RsaPubKey
* Description    : RSA 公钥运算
* Input          : input         输入数据地址
*                  inputLen      输入数据长度
*                  pprivateKey   RSA私钥地址（密钥格式为CRT格式）
*
* Output         : output        输出数据地址
*                  outputLen     输出数据长度

* Return         : 0   - 成功
*                  非0 - 失败(0x0401)
******************************************************************************/
extern int RsaPrivKeyCRT(unsigned int *output, unsigned int *outputLen, unsigned int *input, unsigned int inputLen, STU_RSA_PRIVKEY * pprivateKey);

/****************************************************************************** 
***                            以下为应用调用接口,用户可以调用             ****
*******************************************************************************/

//算法保护的防护等级防护等级
typedef enum {
    dpa_middle,	//中等防护等级
    dpa_high,	//最高防护等级
    dpa_low,	//最低防护等级
    dpa_disable,//没有防护
}dpa_level_t;


#define SUCCESS_ALG			1
#define FAIL_ALG					0
#define ERROR_DATA_LENGTH	-1
#define ERROR_KEY_LENGTH	-2
#define ERROR_KEY_MODE		-3
#define ERROR_ENDE_MODE		-4
#define ERROR_ECBCBC_MODE		-5
#define ERROR_DATA_BUF		-6
#define ERROR_OTHER_PARA	-7

/*******************************************************************************
* Function Name  : Get_AlgLib_Version
* Description    : 算法版本号
* Input          : -NONE
* Output         : -version:算法版本号
* Return         : return 1: success, 0: fail
******************************************************************************/
extern int Get_AlgLib_Version(UINT8*version);

/*******************************************************************************
* Function Name  : des_dpa_ctrl
* Description    : DES防护
* Input          : -level：防护等级middle high low disable
* Output         : None
* Return         : return 1: success, 0: fail
******************************************************************************/
extern int des_dpa_ctrl(dpa_level_t level);

/*******************************************************************************
* Function Name  : des_dpa_ctrl
* Description    : DES防护
* Input          : -level：防护等级middle high low disable
* Output         : None
* Return         : return 1: success, 0: fail
******************************************************************************/
extern int aes_dpa_ctrl(dpa_level_t level);

/*******************************************************************************
* Function Name  : des_dpa_ctrl
* Description    : DES防护
* Input          : -level：防护等级middle high low disable
* Output         : None
* Return         : return 1: success, 0: fail
******************************************************************************/
extern int sm4_dpa_ctrl(dpa_level_t level);

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
extern int AES_EnDecrypt_2(UINT8 EncDe_Mode, 
													 UINT8 CTR_ECB_MODE,UINT8 *key, 
													UINT8 *ctr_iv,
													UINT32 key_mode, 
													UINT8 *src_buf, 
													UINT8 *out_buf, 
													UINT32 data_len);

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
extern int DES_EnDecrypt_2(UINT8 EncDe_Mode, 
													 UINT8 ECB_CBC_Mode, 
													 UINT8 *key, 
													 UINT32 key_len, 
													 UINT8 *iv, 
													 UINT8 *src_buf, 
													 UINT8 *out_buf,
													 UINT32 data_len, 
													 UINT8 check_parity);

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
extern int SMS4_EnDecrypt_2(UINT8 EncDe_Mode,
														UINT8 ECB_CBC_Mode,
														UINT8 *key,
														UINT8 *iv,
														UINT8 *src_buf,
														UINT8 *out_buf,
														UINT32 data_len);

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
extern int SHA_Init_2(Hash_tx *phash_tx, unsigned char HashModeNum);

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
extern int SHA_Update_2(Hash_tx *phash_tx, unsigned char *pMessageBlock, unsigned int DataLen);

/*******************************************************************************
* Function Name  : SHA_Final_2
* Description    :  结束HASH操作并获取结果
* 				         注意：
* 				    None
* Input          : -phash_tx         ：哈希算法数据结构模式
* Output         : -pRes             ：HASH运算结果
* Return         : 执行结果
******************************************************************************/
extern int SHA_Final_2(Hash_tx *phash_tx, unsigned char *pRes);

/*******************************************************************************
* Function Name  : SM2Init
* Description    : SM2模块初始化
* Input          : testbits ： SM2位数
* Output         : None
* Return         : None
******************************************************************************/
extern int SM2Init(UINT16 testbits);

/*******************************************************************************
* Function Name  : SM2GenerateKeyPair_2
* Description    : SM2密钥生成
* Input          : None
* Output         : -pub_key         ：公钥
*                  -pri_key         ：私钥
* Return         : 运算结果
******************************************************************************/
extern int SM2GenerateKeyPair_2(UINT8 *pub_key,UINT8 *pri_key);

/*******************************************************************************
* Function Name  : SM2Encrypt_2
* Description    : SM2加密
* Input          : -mes             ：明文
*                  -len             :明文长度
*                  -pub_key         :公钥
* Output         : -cipher          ：密文
* Return         : 运算结果
******************************************************************************/
extern int SM2Encrypt_2(UINT8 *mes,UINT16 len,UINT8 *pub_key,UINT8 *cipher);

/*******************************************************************************
* Function Name  : SM2Decrypt_2
* Description    : SM2解密
* Input          : -cipher          ：密文
*                  -len             :明文长度
*                  -pri_key         :私钥
* Output         : -mes             ：明文
* Return         : 运算结果
******************************************************************************/
extern int SM2Decrypt_2(UINT8 *cipher,UINT16 len,UINT8 *pri_key,UINT8 *mes);

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
extern int SM2Signature_2(UINT8 *mes,UINT16 len,UINT8 *pri_key,UINT8 *r,UINT8 *s);

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
extern int SM2Verification_2(UINT8 *mes,UINT16 len,UINT8 *pub_key,UINT8 *r,UINT8 *s);

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
extern int SM2GenerateE_2(UINT8 *IDA,UINT16 ida_len,UINT8 *pub_key,UINT8 *pMessage,UINT32 mesLen,UINT8 *res_E);

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
extern int SM2KeyExchange_2(UINT8*self_pri,
														UINT8*self_temp_pub,
														UINT8 *self_temp_pri,
														UINT8*other_pub,
														UINT8*other_temp_pub,
														UINT8*za,
														UINT8*zb,
														UINT8*agree_key,
														UINT32 AgreedKeyLen,
														UINT8*sA,
														UINT8*sB,
														UINT32 Mode);

/*******************************************************************************
* Function Name  : sign_2
* Description    : SM2生成Z值
* Input          : -IDA             ：用户ID
*                  -entla           :用户ID长度
*                  -pub_key         :公钥
* Output         : -za              ：Z值
* Return         : 执行结果
******************************************************************************/
extern int sign_2(UINT8 *IDA, UINT16 entla, UINT8 *pub_key,UINT8*za);

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
extern int rsa_keypair_gen(UINT32 rsa_bit_len, 
													 UINT8 fixkey, 
													 UINT8* n_addr, 
													 UINT8* e_addr, 
													 UINT8 *d_addr, 
													 UINT8* p_addr, 
													 UINT8* q_addr, 
													 UINT8* dp_addr, 
													 UINT8* dq_addr, 
													 UINT8* qinv_addr);

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
extern int RsaPubKey_2(UINT8 fixkey, 
											 UINT8 *n_addr,
											 UINT8* e_addr, 
											 UINT8 *in,UINT32 in_len,
											 UINT8 *out,
											 UINT32 *out_len);

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
extern int RsaPrivKey_2(UINT8*n_addr,UINT8 *d_addr,UINT8 *in,UINT32 in_len,UINT8 *out,UINT32 *out_len);

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
extern int RsaPrivKey_3(UINT8 *n_addr,
												UINT8 *d_addr,
												UINT8 *e_addr,
												UINT8 *in,
												UINT32 in_len,
												UINT8 *out,
												UINT32 *out_len);

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
extern int RsaPrivKeyCRT_2(UINT8*p_addr,
													 UINT8*q_addr,
													 UINT8*dp_addr,
													 UINT8*dq_addr,
													 UINT8*qinv_addr,
													 UINT8 *in,
													 UINT32 in_len,
													 UINT8 *out,
													 UINT32 *out_len);

/*******************************************************************************
* Function Name  : GetRandomWord
* Description    : 随机数生成函数
* Input          : None
* Output         : None
* Return         : 1个UINT32型随机数
******************************************************************************/
UINT32 GetRandomWord(void);

/*******************************************************************************
* Function Name  : Get_Random_Word
* Description    : 随机数生成函数
* Input          : None
* Output         : None
* Return         : 1个UINT32型随机数
******************************************************************************/
UINT32 Get_Random_Word(void);

/*******************************************************************************
* Function Name  : Init_Trng
* Description    : 随机数功能初始化函数。
*                  开启随机数模块时钟，并设置随机数模块时钟频率为系统时钟频率的1/8
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void Init_Trng(void);

/*******************************************************************************
* Function Name  : Trng_Disable
* Description    : 关闭随机数模块时钟
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void Trng_Disable(void);
#endif /* ALG_DRV_H_ */
