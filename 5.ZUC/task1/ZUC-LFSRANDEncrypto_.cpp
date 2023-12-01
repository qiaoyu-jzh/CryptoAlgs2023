/*ZUC算法*/
#include <stdio.h>
#include <math.h>
#include <memory.h>
#define MAX 1024 * 1024 // 读入的明文字符缓冲区

typedef unsigned char unit8; //
typedef unsigned int unit32; //
// 全局变量
unit32 P = pow(2, 31) - 1; // 常数2的31次幂-1
// 组件变量
unit32 LFSR[16] = {0}; // LFSR16个寄存器
unit32 BRC_X0;		   // 比特重组4个单元
unit32 BRC_X1;
unit32 BRC_X2;
unit32 BRC_X3;
unit32 F_R1; // F函数两个记忆单元
unit32 F_R2;
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

#define ROT(a, k) (((a) << k) | ((a) >> (32 - k))) // 循环左移操作

unit32 L1(unit32 X) //******************************************线性变换L1
{
	return (X ^ ROT(X, 2) ^ ROT(X, 10) ^ ROT(X, 18) ^ ROT(X, 24));
}

unit32 L2(unit32 X) //******************************************线性变换L2
{
	return (X ^ ROT(X, 8) ^ ROT(X, 14) ^ ROT(X, 22) ^ ROT(X, 30));
}

unit32 MAKEU32(unit8 a, unit8 b, unit8 c, unit8 d) // 4个8位组成32位
{
	unit32 temp = 0;
	temp = a << 24 | b << 16 | c << 8 | d;
	return temp;
}

int datato8(unit32 input, unit8 outputdata[4])
{
	outputdata[0] = (unit8)(input >> 24);
	outputdata[1] = (unit8)(input >> 16);
	outputdata[2] = (unit8)(input >> 8);
	outputdata[3] = (unit8)(input);
	return 0;
}

void Initial_LFSR(unit8 k[16], unit8 iv[16]) /* 将K和iv加载到LFSR中*/
{
	for (int i = 0; i < 16; i++)
	{
		LFSR[i] = k[i] << 23 | EK_d[i] << 8 | iv[i];
	}
}

void BitReorganization() //*********************************比特重组，产生X0，X1,X2.X3
{
	BRC_X0 = ((LFSR[15] & 0x7fff8000) << 1) | (LFSR[14] | 0xffff);
	BRC_X1 = (LFSR[11] & 0xffff) << 16 | (LFSR[9] >> 16);
	BRC_X2 = (LFSR[7] & 0xffff) << 16 | (LFSR[5] >> 16);
	BRC_X3 = (LFSR[2] & 0xffff) << 16 | (LFSR[0] >> 16);
}

unit32 F() //************************************非线性函数F
{
	unit32 W, W1, W2, Sbox1, Sbox2; // W1LW2H, W2LW1H
	unit8 s[4];
	W = (F_R1 ^ BRC_X0) + F_R2;
	W1 = F_R1 + BRC_X1;
	W2 = F_R2 ^ BRC_X2;
	// W1LW2H = (W1 << 16) | (W2 >> 16);
	// W2LW1H = (W2 << 16) | (W1 >> 16);
	Sbox1 = L1((W1 << 16) | (W2 >> 16));
	Sbox2 = L2((W2 << 16) | (W1 >> 16));
	datato8(Sbox1, s);
	F_R1 = MAKEU32(S0[s[0]], S1[s[1]], S0[s[2]], S1[s[3]]);
	datato8(Sbox2, s);
	F_R2 = MAKEU32(S0[s[0]], S1[s[1]], S0[s[2]], S1[s[3]]);
	return W;
}

void LFSRWithInitialisationMode(unit32 u)
{
	unit32 v;
	v = (LFSR[15] << 15 + LFSR[13] << 27 + LFSR[10] << 21 + LFSR[4] << 20 + LFSR[0] << 8 + LFSR[0]) & 0x7fffffff % P;
	v = (v + u) & 0x7fffffff % P;
	if (v == 0)
		v = P;
	for (int i = 0; i < 15; i++)
	{
		LFSR[i] = LFSR[i + 1];
	}
	LFSR[15] = v;
}

void LFSRWithWorkMode()
{
	unit32 v;
	v = (LFSR[15] << 15 + LFSR[13] << 27 + LFSR[10] << 21 + LFSR[4] << 20 + LFSR[0] << 8 + LFSR[0]) % P;
	if (v == 0)
		v = P;
	for (int i = 0; i < 15; i++)
	{
		LFSR[i] = LFSR[i + 1];
	}
	LFSR[15] = v;
}

void Initialization(unit8 k[16], unit8 iv[16]) // 初始化阶段
{
	unit32 u;
	for (int i = 0; i < 32; i++) // 执行32次
	{
		Initial_LFSR(k, iv);
		BitReorganization();
		u = F();
		LFSRWithInitialisationMode(u);
	}
}

void KeystreamGenerate(unit32 *Keystream, int KeystreamLen) // 工作阶段
{
	BitReorganization();
	F(); // 第一遍丢弃
	LFSRWithWorkMode();
	for (int i = 0; i < KeystreamLen; i++)
	{
		BitReorganization();
		Keystream[i] = F() ^ BRC_X3;
		LFSRWithWorkMode();
	}
}

int main()
{
	unit8 k[16] = {15, 5, 56, 1, 8, 25, 6, 9, 8, 36, 41, 22, 71, 12, 24, 10}; // 因为是8位的，所以每个数字都是在0~255之间
	unit8 iv[16] = {8, 4, 16, 1, 6, 45, 7, 9, 3, 36, 42, 1, 32, 17, 5, 23};
	unit32 plain[4] = {0x12345678, 0x34567890, 0x56789012, 0x78901234};
	unit32 Keystream[4] = {0};
	unit32 cipher[4] = {0};
	F_R1 = 0x00000000, F_R2 = 0x000000000; // R1R2初始化

	Initialization(k, iv);

	printf("初始密钥是：\n");
	for (int i = 0; i < 16; i++)
	{
		printf(" %x", k[i]);
	}
	printf("\n");

	printf("初始变量是：\n");
	for (int i = 0; i < 16; i++)
	{
		printf(" %x", iv[i]);
	}
	printf("\n");

	printf("原文是：\n");
	for (int i = 0; i < 4; i++)
		printf("  %x", plain[i]);
	printf("\n");

	printf("密钥是：\n");
	KeystreamGenerate(Keystream, 4);
	for (int i = 0; i < 4; i++)
		printf("  %x", Keystream[i]);
	printf("\n");

	printf("密文是：\n");
	for (int i = 0; i < 4; i++)
		cipher[i] = Keystream[i] ^ plain[i];
	for (int i = 0; i < 4; i++)
		printf("  %x", cipher[i]);
	printf("\n");

	unit32 temp[4] = {0};
	printf("解密后明文是：\n");
	for (int i = 0; i < 4; i++)
		temp[i] = Keystream[i] ^ cipher[i];
	for (int i = 0; i < 4; i++)
		printf("  %x", temp[i]);
	printf("\n");
}
