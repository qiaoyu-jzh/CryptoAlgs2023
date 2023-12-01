/*ZUC算法*/
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <memory.h>
// #include <arpa/inet.h>
#include <windows.h>
#pragma comment(lib, "wsock32.lib")

typedef unsigned char unit8; //
typedef unsigned int unit32; //
// 全局变量
unit32 COUNT = 0x12345678; // 计数器
unit32 BEARER = 0x13;	   //
unit32 DIRECTION = 0x1;
unit32 P = pow(2, 31) - 1; // 常数2的31次幂-1
unit32 Keystream[1024];	   // 产生的密钥流
unit32 buffer[1024];	   // 文件读入的字符缓冲区
// 组件变量
unit32 LFSR[16] = {0}; // LFSR16个寄存器
unit32 F_R1;		   // F函数两个记忆单元
unit32 F_R2;
unit32 BRC_X0; // 比特重组4个单元
unit32 BRC_X1;
unit32 BRC_X2;
unit32 BRC_X3;

/* the s-boxes  S盒*/
unit8 S0[256] = {
	0x3e, 0x72, 0x5b, 0x47, 0xca, 0xe0, 0x00, 0x33, 0x04, 0xd1, 0x54, 0x98, 0x09, 0xb9, 0x6d, 0xcb,
	0x7b, 0x1b, 0xf9, 0x32, 0xaf, 0x9d, 0x6a, 0xa5, 0xb8, 0x2d, 0xfc, 0x1d, 0x08, 0x53, 0x03, 0x90,
	0x4d, 0x4e, 0x84, 0x99, 0xe4, 0xce, 0xd9, 0x91, 0xdd, 0xb6, 0x85, 0x48, 0x8b, 0x29, 0x6e, 0xac,
	0xcd, 0xc1, 0xf8, 0x1e, 0x73, 0x43, 0x69, 0xc6, 0xb5, 0xbd, 0xfd, 0x39, 0x63, 0x20, 0xd4, 0x38,
	0x76, 0x7d, 0xb2, 0xa7, 0xcf, 0xed, 0x57, 0xc5, 0xf3, 0x2c, 0xbb, 0x14, 0x21, 0x06, 0x55, 0x9b,
	0xe3, 0xef, 0x5e, 0x31, 0x4f, 0x7f, 0x5a, 0xa4, 0x0d, 0x82, 0x51, 0x49, 0x5f, 0xba, 0x58, 0x1c,
	0x4a, 0x16, 0xd5, 0x17, 0xa8, 0x92, 0x24, 0x1f, 0x8c, 0xff, 0xd8, 0xae, 0x2e, 0x01, 0xd3, 0xad,
	0x3b, 0x4b, 0xda, 0x46, 0xeb, 0xc9, 0xde, 0x9a, 0x8f, 0x87, 0xd7, 0x3a, 0x80, 0x6f, 0x2f, 0xc8,
	0xb1, 0xb4, 0x37, 0xf7, 0x0a, 0x22, 0x13, 0x28, 0x7c, 0xcc, 0x3c, 0x89, 0xc7, 0xc3, 0x96, 0x56,
	0x07, 0xbf, 0x7e, 0xf0, 0x0b, 0x2b, 0x97, 0x52, 0x35, 0x41, 0x79, 0x61, 0xa6, 0x4c, 0x10, 0xfe,
	0xbc, 0x26, 0x95, 0x88, 0x8a, 0xb0, 0xa3, 0xfb, 0xc0, 0x18, 0x94, 0xf2, 0xe1, 0xe5, 0xe9, 0x5d,
	0xd0, 0xdc, 0x11, 0x66, 0x64, 0x5c, 0xec, 0x59, 0x42, 0x75, 0x12, 0xf5, 0x74, 0x9c, 0xaa, 0x23,
	0x0e, 0x86, 0xab, 0xbe, 0x2a, 0x02, 0xe7, 0x67, 0xe6, 0x44, 0xa2, 0x6c, 0xc2, 0x93, 0x9f, 0xf1,
	0xf6, 0xfa, 0x36, 0xd2, 0x50, 0x68, 0x9e, 0x62, 0x71, 0x15, 0x3d, 0xd6, 0x40, 0xc4, 0xe2, 0x0f,
	0x8e, 0x83, 0x77, 0x6b, 0x25, 0x05, 0x3f, 0x0c, 0x30, 0xea, 0x70, 0xb7, 0xa1, 0xe8, 0xa9, 0x65,
	0x8d, 0x27, 0x1a, 0xdb, 0x81, 0xb3, 0xa0, 0xf4, 0x45, 0x7a, 0x19, 0xdf, 0xee, 0x78, 0x34, 0x60};

unit8 S1[256] = {
	0x55, 0xc2, 0x63, 0x71, 0x3b, 0xc8, 0x47, 0x86, 0x9f, 0x3c, 0xda, 0x5b, 0x29, 0xaa, 0xfd, 0x77,
	0x8c, 0xc5, 0x94, 0x0c, 0xa6, 0x1a, 0x13, 0x00, 0xe3, 0xa8, 0x16, 0x72, 0x40, 0xf9, 0xf8, 0x42,
	0x44, 0x26, 0x68, 0x96, 0x81, 0xd9, 0x45, 0x3e, 0x10, 0x76, 0xc6, 0xa7, 0x8b, 0x39, 0x43, 0xe1,
	0x3a, 0xb5, 0x56, 0x2a, 0xc0, 0x6d, 0xb3, 0x05, 0x22, 0x66, 0xbf, 0xdc, 0x0b, 0xfa, 0x62, 0x48,
	0xdd, 0x20, 0x11, 0x06, 0x36, 0xc9, 0xc1, 0xcf, 0xf6, 0x27, 0x52, 0xbb, 0x69, 0xf5, 0xd4, 0x87,
	0x7f, 0x84, 0x4c, 0xd2, 0x9c, 0x57, 0xa4, 0xbc, 0x4f, 0x9a, 0xdf, 0xfe, 0xd6, 0x8d, 0x7a, 0xeb,
	0x2b, 0x53, 0xd8, 0x5c, 0xa1, 0x14, 0x17, 0xfb, 0x23, 0xd5, 0x7d, 0x30, 0x67, 0x73, 0x08, 0x09,
	0xee, 0xb7, 0x70, 0x3f, 0x61, 0xb2, 0x19, 0x8e, 0x4e, 0xe5, 0x4b, 0x93, 0x8f, 0x5d, 0xdb, 0xa9,
	0xad, 0xf1, 0xae, 0x2e, 0xcb, 0x0d, 0xfc, 0xf4, 0x2d, 0x46, 0x6e, 0x1d, 0x97, 0xe8, 0xd1, 0xe9,
	0x4d, 0x37, 0xa5, 0x75, 0x5e, 0x83, 0x9e, 0xab, 0x82, 0x9d, 0xb9, 0x1c, 0xe0, 0xcd, 0x49, 0x89,
	0x01, 0xb6, 0xbd, 0x58, 0x24, 0xa2, 0x5f, 0x38, 0x78, 0x99, 0x15, 0x90, 0x50, 0xb8, 0x95, 0xe4,
	0xd0, 0x91, 0xc7, 0xce, 0xed, 0x0f, 0xb4, 0x6f, 0xa0, 0xcc, 0xf0, 0x02, 0x4a, 0x79, 0xc3, 0xde,
	0xa3, 0xef, 0xea, 0x51, 0xe6, 0x6b, 0x18, 0xec, 0x1b, 0x2c, 0x80, 0xf7, 0x74, 0xe7, 0xff, 0x21,
	0x5a, 0x6a, 0x54, 0x1e, 0x41, 0x31, 0x92, 0x35, 0xc4, 0x33, 0x07, 0x0a, 0xba, 0x7e, 0x0e, 0x34,
	0x88, 0xb1, 0x98, 0x7c, 0xf3, 0x3d, 0x60, 0x6c, 0x7b, 0xca, 0xd3, 0x1f, 0x32, 0x65, 0x04, 0x28,
	0x64, 0xbe, 0x85, 0x9b, 0x2f, 0x59, 0x8a, 0xd7, 0xb0, 0x25, 0xac, 0xaf, 0x12, 0x03, 0xe2, 0xf2};

/* 固定参数D ，16个常规字符串*/
unit32 EK_d[16] = {
	0x44D7, 0x26BC, 0x626B, 0x135E, 0x5789, 0x35E2, 0x7135, 0x09AF,
	0x4D78, 0x2F13, 0x6BC4, 0x1AF1, 0x5E26, 0x3C4D, 0x789A, 0x47AC};

unit32 AddM(unit32 a, unit32 b) //
{
	return ((a & 0x7FFFFFFF) + (b & 0x7FFFFFFF)) % P;
}

void LFSRWithInitialisationMode(unit32 u)
{
	unit32 v = LFSR[0];
	v = AddM(v, LFSR[0] << 8);
	v = AddM(v, LFSR[4] << 20);
	v = AddM(v, LFSR[10] << 21);
	v = AddM(v, LFSR[13] << 17);
	v = AddM(v, LFSR[15] << 15);
	// if(v==0) v=P;
	v = AddM(v, u);
	if (v == 0)
		v = P;

	for (int i = 0; i < 15; i++)
	{
		LFSR[i] = LFSR[i + 1];
		// LFSR[i]=temp;
	}
	LFSR[15] = v;
}

void LFSRWithWorkMode()
{
	unit32 temp, v = LFSR[0];
	v = AddM(v, LFSR[0] << 8);
	v = AddM(v, LFSR[4] << 20);
	v = AddM(v, LFSR[10] << 21);
	v = AddM(v, LFSR[13] << 17);
	v = AddM(v, LFSR[15] << 15);
	if (v == 0)
		v = P;

	for (int i = 0; i < 15; i++)
	{
		temp = LFSR[i + 1];
		LFSR[i] = temp;
	}
	LFSR[15] = v;
}

void BitReorganization() //*********************************比特重组，产生X0，X1,X2.X3
{
	BRC_X0 = ((LFSR[15] & 0x7FFF8000) << 1) | (LFSR[14] & 0xFFFF);
	BRC_X1 = ((LFSR[11] & 0xFFFF) << 16) | (LFSR[9] >> 15);
	BRC_X2 = ((LFSR[7] & 0xFFFF) << 16) | (LFSR[5] >> 15);
	BRC_X3 = ((LFSR[2] & 0xFFFF) << 16) | (LFSR[0] >> 15);
}

#define ROT(a, k) (((a) << k) | ((a) >> (32 - k)))
unit32 L1(unit32 X) //******************************************线性变换L1
{
	return (X ^ ROT(X, 2) ^ ROT(X, 10) ^ ROT(X, 18) ^ ROT(X, 24));
}

unit32 L2(unit32 X) //******************************************线性变换L2
{
	return (X ^ ROT(X, 8) ^ ROT(X, 14) ^ ROT(X, 22) ^ ROT(X, 30));
}

#define MAKEU32(a, b, c, d) (((unit32)(a) << 24) | ((unit32)(b) << 16) | ((unit32)(c) << 8) | ((unit32)(d))) // 将取出的4个8位数值连接在一起
unit32 F()																									 //************************************非线性函数F
{
	unit32 W, W1, W2, u, v;
	W = (BRC_X0 ^ F_R1) + F_R2;
	W1 = F_R1 + BRC_X1;
	W2 = F_R2 ^ BRC_X2;
	u = L1((W1 << 16) | (W2 >> 16)); // 将32位的W1和W2的高、低16位重组
	v = L2((W2 << 16) | (W1 >> 16));
	F_R1 = MAKEU32(S0[u >> 24], S1[(u >> 16) & 0xFF], S0[(u >> 8) & 0xFF], S1[u & 0xFF]); // 作用是分别取出每两位。例：8a4f07bd ，分别取出8a、4f、07、bd
	F_R2 = MAKEU32(S0[v >> 24], S1[(v >> 16) & 0xFF], S0[(v >> 8) & 0xFF], S1[v & 0xFF]);
	return W;
}

#define MAKEU31(a, b, c) (((unit32)(a) << 23) | ((unit32)(b) << 8) | (unit32)(c)) // si=ki||di||ivi。位数31=8+15+8
void Initialization(unit8 *k, unit8 *iv)										  /* initialize 将K和iv加载到LFSR中*/
{
	for (int i = 0; i < 16; i++)
	{
		LFSR[i] = MAKEU31(k[i], EK_d[i], iv[i]);
	}
	F_R1 = 0; // F函数寄存器置0
	F_R2 = 0;

	unit32 z = 0;
	for (int j = 0; j < 32; j++)
	{
		BitReorganization();
		z = F();
		z = (z >> 1) | (z << 31); // 循环右移
		LFSRWithInitialisationMode(z);
	}
}

void KeystreamGenerate(unit32 *Keystream, int KeystreamLen) //****************生成32bit密钥序列
{
	BitReorganization();
	F();
	LFSRWithWorkMode();
	for (int i = 0; i < KeystreamLen; i++)
	{
		BitReorganization();
		Keystream[i] = F() ^ BRC_X3;
		LFSRWithWorkMode();
	}
}

/*-----------------------------------------------------------------------------------*/
/*---------------------基于祖冲之密码的完整性算法128-EIA3---------------------------*/
/*-----------------------------------------------------------------------------------*/

void Init_IV(unit32 COUNT, unit32 BEARER, unit32 DIRECTION, unit8 IV[16]) // 根据COUNT\BEARER\DIRECTION初始化IV
{
	IV[0] = COUNT >> 24 & 0xff;
	IV[1], IV[9] = COUNT >> 16 & 0xff;
	IV[2], IV[10] = COUNT >> 8 & 0xff;
	IV[3], IV[11] = COUNT & 0xff;
	IV[4], IV[12] = BEARER << 3;
	IV[8] = IV[0] ^ (DIRECTION << 7);
	IV[14] = IV[6] ^ (DIRECTION << 7);
	IV[5], IV[6], IV[7], IV[13], IV[15] = 0x0;
}

void EIV3(unit8 *IK, unit32 COUNT, unit32 BEARER, unit32 DIRECTION, unit32 LENGTH, unit8 *M, unit32 &MAC)
{ // 输入：密钥IK，COUNT， BEARER， DIRECTION，明文比特数 LENGTH，明文字节数组M
	// 输出：32bit的MAC值

	unit32 L = (LENGTH + 32 - 1) / 32 + 2;
	unit8 iv[16] = {0};
	unit32 Z[L] = {0};
	Init_IV(COUNT, BEARER, DIRECTION, iv); // zuc iv 初始化
	Initialization(IK, iv);
	KeystreamGenerate(Z, L);

	// 取出比特流，将每一个Z中的32位bit值（0 or 1）
	unit8 zbit[32 * L] = {0};
	for (int i = 0; i < L; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			zbit[j + i * 32] = (Z[i] >> (31 - j)) & 0x1;
		}
	}
	// 重组zi
	unit32 z[L*32] = {0};
	for (int i = 0; i < L*32; i++)
	{
		z[i] = zbit[i] << 31;
		for (int k = 1; k < 32; k++)
		{
			z[i] = z[i] | zbit[k + i] << (31 - k);
		}
		// printf("bbb");
	}
	// get m[i]
	unit8 m[LENGTH] = {0};

	// 累加T 生成MAC
	unit32 T = 0;
	for (int i = 0; i < LENGTH; i++)
	{
		for (int j = 0; j < 8;j++){
			m[i * 8 + j] = *(M + i / 8) >> (7 - j) & 0x01;
		}
		if(m[i]==1){
			T = T ^ z[i];
		}
	}
	T = T ^ z[LENGTH];
	MAC = T ^ z[32 * L - 32];
}

int main()
{
	unit8 k[16] = {15, 5, 56, 1, 8, 25, 6, 9, 8, 36, 41, 22, 71, 12, 24, 10}; // 因为是8位的，所以每个数字都是在0~255之间
	char *m = (char *)"ZUC Integrity Algorithm 128-EIA3";
	unit32 MAC = {0};
	printf("***********ZUC 128-EIA3完整性算法************\n");
	printf("明文：%s\n", m);
	printf("密钥KI：");
	for (int i = 0; i < 16; i++)
		printf("%02x", k[i]);
	EIV3(k, COUNT, BEARER, DIRECTION, strlen(m) * 8, (unit8 *)m, MAC);
	printf("\n消息认证码:%08x", MAC);

	return 0;
}
