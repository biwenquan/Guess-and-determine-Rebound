#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h> 
#include <time.h>
#include <stdbool.h>

#define Nb 4
#define BLOCKLEN 16
#define Nk 6
#define Nr 12
#define keyExpSize 208


static const uint8_t sbox[256] = {
  //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

static const uint8_t rsbox[256] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };

static const uint8_t Rcon[11] = {0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};
static const uint8_t Diff_X[5][16] = {
 //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
{0x2f, 0x2f, 0x55, 0x5e, 0x2f, 0x2f, 0x55, 0x48, 0x2f, 0x2f, 0x71, 0x5e, 0x2f, 0x2f, 0x71, 0x48},
{0x00, 0x00, 0x5b, 0x00, 0x00, 0x00, 0x00, 0x69, 0x59, 0x00, 0x00, 0x00, 0x00, 0x86, 0x00, 0x5d},
{0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16}
};
static const uint8_t Diff_Y[5][16] = {
{0x64, 0x77, 0x3b, 0x08, 0x0c, 0xfd, 0x69, 0x25, 0xbc, 0xb3, 0xa5, 0x3b, 0xe8, 0xf0, 0xde, 0x71},
{0x00, 0x00, 0x8a, 0x00, 0x00, 0x00, 0x00, 0xc4, 0xa6, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x2f},
{0x00, 0x00, 0x00, 0x2f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2f},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16}
};

static int value_X[5][16] = {0};
static int value_Y[5][16] = {0};
static int value_Z[5][16] = {0};
static int value_W[5][16] = {0};
static int value_K[5][16] = {0};

static int value_X_1[5][16] = {0};
static int value_Y_1[5][16] = {0};
static int value_Z_1[5][16] = {0};
static int value_W_1[5][16] = {0};
static int value_K_1[5][16] = {0};

static int value_all_X[5][16][4] = {0}; 
static int value_all_Y[5][16][4] = {0};
static int value_all_K[5][16][4] = {0};

int diff_num[5][16] = {0};
static uint8_t test_state[16] = {0};
static uint8_t test_key192[24] = {0};
static uint8_t RoundKey[keyExpSize];

typedef struct{
    int num;
    uint8_t term[4][2];
} node;
int sbox_ddt_ID[256][256] = {0};
node sbox_ddt_term[256][256] = {0};

// return max num of ddt table
int gen_sbox_ddt() {
    int res = 0;
    for(int i = 0; i < 256; i++) {
        for(int j = 0; j < 256; j++) {
            sbox_ddt_ID[i][j] = 0;
        }
    }
    for(int i = 0; i < 256; i++) {
        for(int j = 0; j < 256; j++) {
            sbox_ddt_ID[i^j][sbox[i]^sbox[j]]++;
        }
    }
    for(int i = 0; i < 256; i++) {
        for(int j = 0; j < 256; j++) {
            if(sbox_ddt_ID[i][j] > res && sbox_ddt_ID[i][j] != 256) {
                res = sbox_ddt_ID[i][j];
            }
        }
    }

    return res;
}

void gen_sbox_diff_term() {
    for(int i = 0; i < 256; i++) {
        for(int j = 0; j < 256; j++) {
            sbox_ddt_term[i][j].num = 0;
        }
    }
    //memset(sbox_ddt_term, 0, 256*256*sizeof(node));
    for(int i = 0; i < 256; i++) {
        for(int j = 0; j < 256; j++) {
            if(i == j) continue;
            sbox_ddt_term[i^j][sbox[i]^sbox[j]].term[sbox_ddt_term[i^j][sbox[i]^sbox[j]].num][0] = (uint8_t)i;
            sbox_ddt_term[i^j][sbox[i]^sbox[j]].term[sbox_ddt_term[i^j][sbox[i]^sbox[j]].num][1] = (uint8_t)sbox[i];
            sbox_ddt_term[i^j][sbox[i]^sbox[j]].num++;
        }
    }
    int res = 0;
    for(int i = 0; i < 256; i++) {
        for(int j = 0; j < 256; j++) {
            if(sbox_ddt_term[i][j].num > res && sbox_ddt_term[i][j].num != 256) {
                res = sbox_ddt_term[i][j].num;
            }
        }
    }
    return;

}
// for a trail, the every △ nums, the X0
void get_diff_nums() {
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 16; j++) {
            diff_num[i][j] = sbox_ddt_term[Diff_X[i][j]][Diff_Y[i][j]].num;
        }
    }
    for(int i = 0; i < 5; i++) {
        printf("aes %d  round sbox diff num is \n", i);
        for(int j = 0; j < 16; j++)  {
            printf("%d  ", diff_num[i][j]);
        }printf("\n");
    }
}

void get_diff_values(){
    memset(value_all_X, -1, 5*16*4*sizeof(int));
    memset(value_all_Y, -1, 5*16*4*sizeof(int));
    memset(value_all_K, -1, 5*16*4*sizeof(int));
    for(int i = 0; i < 5; i++) {
        for (int j = 0; j < 16; j++) {
            for(int k = 0; k < diff_num[i][j]; k++){
                value_all_X[i][j][k] = sbox_ddt_term[Diff_X[i][j]][Diff_Y[i][j]].term[k][0];
                value_all_Y[i][j][k] = sbox_ddt_term[Diff_X[i][j]][Diff_Y[i][j]].term[k][1];
            }
        }
    }
}

void test_diff_values(){
    int x,y;int num=0;
    for(int i = 0; i < 5; i++) {
        for (int j = 0; j < 16; j++) {
            for(int k = 0; k < diff_num[i][j]; k++){
                x = k;
                y = (diff_num[i][j] == 2) ? (k+1)%2 : ((k%2 == 0)? k+1 : k-1);//0
                if((value_all_X[i][j][x] != (value_all_X[i][j][y] ^ Diff_X[i][j])) || (value_all_Y[i][j][x] != (value_all_Y[i][j][y] ^ Diff_Y[i][j]))) {
                    printf("mismatch at %d round %d byte %d\n", i, j, k); continue; //
                    printf("the %dth diff value is %x & %x\n", i, value_all_X[i][j][k], value_all_Y[i][j][k]);
                    printf("the %dth else value is %x & %x\n", i, value_all_X[i][j][k]^Diff_X[i][j], value_all_Y[i][j][k]^Diff_Y[i][j]);
                }
                if(value_all_X[i][j][k] == 0 || value_all_Y[i][j][k] == 0) {
                    printf("mistake at %d  %d %d: %x %x \n", i, j, k, value_all_X[i][j][k], value_all_Y[i][j][k]);
                }
                if(value_all_X[i][j][k] == -1){
                    num++;
                    printf("test:%d %2d %d %3d;",i, j, k, value_all_X[i][j][k]);
                }
            }
        }
    } printf("num -1 is  %d\n", num); 
     printf("diff_num[1][15] is %d\n",diff_num[1][15]);
    for(int i = 0; i < diff_num[1][15]; i++) {
        printf("the %dth diff value is %x & %x\n", i, value_all_X[1][15][i], value_all_Y[1][15][i]);
    }
    for(int i = 0; i < diff_num[1][15]; i++) {
        printf("the %dth else value is %x & %x\n", i, value_all_X[1][15][i]^Diff_X[1][15], value_all_Y[1][15][i]^Diff_Y[1][15]);
    }

    printf("diff_num[0][11] is %d\n",diff_num[0][11]);
    for(int i = 0; i < diff_num[0][11]; i++) {
        printf("the %dth diff value is %x & %x\n", i, value_all_X[0][11][i], value_all_Y[0][11][i]);
    }
    for(int i = 0; i < diff_num[0][11]; i++) {
        printf("the %dth else value is %x & %x\n", i, value_all_X[0][11][i]^Diff_X[0][11], value_all_Y[0][11][i]^Diff_Y[0][11]);
    }



}

uint8_t mul2(uint8_t x){
    uint8_t out = ((x<<1) ^ (((x>>7) & 1) * 0x1b));
    return out;
}

uint8_t mul3(uint8_t x){
    uint8_t out = mul2(x) ^ x;
    return out;
}

uint8_t mul4(uint8_t x){
    uint8_t out = mul2(mul2(x));
    return out;
}

uint8_t mul8(uint8_t x){
    uint8_t out = mul2(mul2(mul2(x)));
    return out;
}

uint8_t mul9(uint8_t x){
    uint8_t out = mul8(x) ^ x;
    return out;
}

uint8_t mulB(uint8_t x){
    uint8_t out = mul8(x) ^ mul2(x) ^ x;
    return out;
}

uint8_t mulD(uint8_t md_in){
    uint8_t out = mul8(md_in) ^ mul4(md_in) ^ md_in;
    return out;
}

uint8_t mulE(uint8_t me_in){
    uint8_t out = mul8(me_in) ^ mul4(me_in) ^ mul2(me_in);
    return out;
}

uint8_t mulF6(uint8_t x){
    uint8_t out = mul8(mul8(mul3(x))) ^ mul4(mul4(mul3(x))) ^ mul2(mul3(x));
    return out;
}

void State2Matrix(uint8_t state[4][4], const uint8_t input[16]) { 
    int i,j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            state[j][i] = *input++;  
        }
    }
}

void Matrix2State(uint8_t output[16], uint8_t state[4][4]) { 
    int i,j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            *output++ = state[j][i];
        }
    }
}

void key_exp_192(uint8_t key[24]) {
    uint8_t tempa[4];
    for(int i = 0; i < Nk; i++) {

    }

}

void subBytes(uint8_t state[4][4]) {
    /* i: row, j: col */
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            state[i][j] = sbox[state[i][j]];
        }
    }
}
void invSubBytes(uint8_t state[4][4]) {
    /* i: row, j: col */
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            state[i][j] = rsbox[state[i][j]];
        }
    }
}
void ShiftRows(uint8_t state[4][4]){ 
    int i,j;
    uint8_t temp[4][4];
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
           temp[i][j]=state[i][j];
        }
    }
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
            state[i][j]=temp[i][(j+i)%4]; 
        }
    }
}
void InShiftRows(uint8_t state[4][4]){  
    int i,j;
    uint8_t temp[4][4];
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
           temp[i][j]=state[i][j];
        }
    }
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
            state[i][j]=temp[i][(4+j-i)%4]; 
        }
    }
}

uint8_t XTIME(uint8_t x) {  
	return ((x << 1) ^ ((x & 0x80) ? 0x1b : 0x00));  
}

uint8_t multiply(uint8_t a, uint8_t b) {
	unsigned char temp[8] = { a };
    uint8_t tempmultiply = 0x00;
	int i;
	for (i = 1; i < 8; i++) {
		temp[i] = XTIME(temp[i - 1]);  
	}
	tempmultiply = (b & 0x01) * a;
	for (i = 1; i <= 7; i++) {
		tempmultiply ^= (((b >> i) & 0x01) * temp[i]);  
	}
	return tempmultiply;
}

void MixColumns(uint8_t state[4][4]){
    int i,j;
    uint8_t temp[4][4];
    uint8_t M[4][4]={
        {0x02, 0x03, 0x01, 0x01},
        {0x01, 0x02, 0x03, 0x01},
        {0x01, 0x01, 0x02, 0x03},
        {0x03, 0x01, 0x01, 0x02}
    };
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j){
            temp[i][j] = state[i][j];
        }
    }

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {  
            state[i][j] = multiply(M[i][0], temp[0][j]) ^ multiply(M[i][1], temp[1][j])
                        ^ multiply(M[i][2], temp[2][j]) ^ multiply(M[i][3], temp[3][j]);
        }
    }
}

void InMixColumns (uint8_t state[4][4]){
    int i,j;
    uint8_t temp[4][4];
    uint8_t M[4][4]={
        {0x0E, 0x0B, 0x0D, 0x09},
        {0x09, 0x0E, 0x0B, 0x0D},
        {0x0D, 0x09, 0x0E, 0x0B},
        {0x0B, 0x0D, 0x09, 0x0E}}; 
    for(i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j){
            temp[i][j] = state[i][j];
        }
    }

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            state[i][j] = multiply(M[i][0], temp[0][j]) ^ multiply(M[i][1], temp[1][j])
                        ^ multiply(M[i][2], temp[2][j]) ^ multiply(M[i][3], temp[3][j]);
        }
    }
}

void AddRoundKey(uint8_t state[4][4],uint8_t W[4][4]){  
    int i,j;
    for(i=0;i<4;i++){
        for(j=0;j<4;j++){
            state[j][i]^=W[j][i];
        }
    }
}

static void KeyExpansion16(uint8_t *Key)
{
  uint32_t i, k;
  uint8_t tempa[4]; 

  for (i = 0; i < Nk; ++i)
  {
    RoundKey[(i * 4) + 0] = Key[(i * 4) + 0];
    RoundKey[(i * 4) + 1] = Key[(i * 4) + 1];
    RoundKey[(i * 4) + 2] = Key[(i * 4) + 2];
    RoundKey[(i * 4) + 3] = Key[(i * 4) + 3];
  }

  //i == Nk
  for (; i < Nb * (Nr + 1); ++i)
  {
    {
      tempa[0]=RoundKey[(i-1) * 4 + 0];
      tempa[1]=RoundKey[(i-1) * 4 + 1];
      tempa[2]=RoundKey[(i-1) * 4 + 2];
      tempa[3]=RoundKey[(i-1) * 4 + 3];
    }

    if (i % Nk == 0)
    {
      // Function RotWord()
      {
        k = tempa[0];
        tempa[0] = tempa[1];
        tempa[1] = tempa[2];
        tempa[2] = tempa[3];
        tempa[3] = k;
      }
      // Function Subword()
      {
        tempa[0] = sbox[tempa[0]];
        tempa[1] = sbox[tempa[1]];
        tempa[2] = sbox[tempa[2]];
        tempa[3] = sbox[tempa[3]];
      }

      tempa[0] =  tempa[0] ^ Rcon[i/Nk];
    }
    RoundKey[i * 4 + 0] = RoundKey[(i - Nk) * 4 + 0] ^ tempa[0];
    RoundKey[i * 4 + 1] = RoundKey[(i - Nk) * 4 + 1] ^ tempa[1];
    RoundKey[i * 4 + 2] = RoundKey[(i - Nk) * 4 + 2] ^ tempa[2];
    RoundKey[i * 4 + 3] = RoundKey[(i - Nk) * 4 + 3] ^ tempa[3];
  }
}

static void KeyExpansion(uint8_t *key,uint8_t w[][4][4]){
    int i,j,r;
    KeyExpansion16(key);
    for(r = 0; r <= Nr; r++) {
        for(i=0;i<4;i++){
            for(j=0;j<4;j++){
                w[r][i][j]=RoundKey[r*16+i+j*4];
            }
        }
    } 
}


void subByte16(uint8_t *RoundText){
    for(int i=0;i<16;i++)
        RoundText[i]=sbox[RoundText[i]];
}
void InvSubByte16(uint8_t *RoundText){
    for(int i=0;i<16;i++)
        RoundText[i]=rsbox[RoundText[i]];
}
void ShiftRow16(uint8_t *RoundText) {
    uint8_t t;
    //row1
    t=RoundText[1]; RoundText[1]=RoundText[5]; RoundText[5]=RoundText[9]; RoundText[9]=RoundText[13]; RoundText[13]=t;
    //row2
    t=RoundText[2];RoundText[2]=RoundText[10];RoundText[10]=t;
    t=RoundText[6];RoundText[6]=RoundText[14];RoundText[14]=t;
    //row3
    t=RoundText[15];RoundText[15]=RoundText[11];RoundText[11]=RoundText[7]; RoundText[7]=RoundText[3]; RoundText[3]=t;
}
void ShiftRow16_int(int *RoundText) {
    int t;
    //row1
    t=RoundText[1]; RoundText[1]=RoundText[5]; RoundText[5]=RoundText[9]; RoundText[9]=RoundText[13]; RoundText[13]=t;
    //row2
    t=RoundText[2];RoundText[2]=RoundText[10];RoundText[10]=t;
    t=RoundText[6];RoundText[6]=RoundText[14];RoundText[14]=t;
    //row3
    t=RoundText[15];RoundText[15]=RoundText[11];RoundText[11]=RoundText[7]; RoundText[7]=RoundText[3]; RoundText[3]=t;
}
void InvShiftRow16(uint8_t *RoundText){
    uint8_t t;
    //row1
    t=RoundText[13]; RoundText[13]=RoundText[9];RoundText[9]=RoundText[5]; RoundText[5]=RoundText[1]; RoundText[1]=t;
    //row2
    t=RoundText[2],RoundText[2]=RoundText[10];RoundText[10]=t;
    t=RoundText[6];RoundText[6]=RoundText[14];RoundText[14]=t;
    //row3
    t=RoundText[3]; RoundText[3]=RoundText[7];RoundText[7]=RoundText[11]; RoundText[11]=RoundText[15]; RoundText[15]=t;
}
void MixColumn16(uint8_t* RoundText){
    uint8_t temp[4];
    for(int i=0;i<4;i++){
        temp[0]=mul2(RoundText[4*i])^mul3(RoundText[1+4*i])^RoundText[2+4*i]^RoundText[3+4*i];
        temp[1]=RoundText[4*i]^mul2(RoundText[1+4*i])^mul3(RoundText[2+4*i])^RoundText[3+4*i];
        temp[2]=RoundText[4*i]^RoundText[1+4*i]^mul2(RoundText[2+4*i])^mul3(RoundText[3+4*i]);
        temp[3]=mul3(RoundText[4*i])^RoundText[1+4*i]^RoundText[2+4*i]^mul2(RoundText[3+4*i]);
        RoundText[4*i]=temp[0];
        RoundText[1+4*i]=temp[1];
        RoundText[2+4*i]=temp[2];
        RoundText[3+4*i]=temp[3];
    }
}
void MixColumn16_1column(uint8_t* RoundText, int i){
    uint8_t temp[4];
    temp[0]=mul2(RoundText[4*i])^mul3(RoundText[1+4*i])^RoundText[2+4*i]^RoundText[3+4*i];
    temp[1]=RoundText[4*i]^mul2(RoundText[1+4*i])^mul3(RoundText[2+4*i])^RoundText[3+4*i];
    temp[2]=RoundText[4*i]^RoundText[1+4*i]^mul2(RoundText[2+4*i])^mul3(RoundText[3+4*i]);
    temp[3]=mul3(RoundText[4*i])^RoundText[1+4*i]^RoundText[2+4*i]^mul2(RoundText[3+4*i]);
    RoundText[4*i]=temp[0];
    RoundText[1+4*i]=temp[1];
    RoundText[2+4*i]=temp[2];
    RoundText[3+4*i]=temp[3];
}

void InvMixColumn16(uint8_t* RoundText) {
    uint8_t temp[4];
    for(int i=0;i<4;i++){
        temp[0]=mulE(RoundText[4*i])^mulB(RoundText[1+4*i])^mulD(RoundText[2+4*i])^mul9(RoundText[3+4*i]);
        temp[1]=mul9(RoundText[4*i])^mulE(RoundText[1+4*i])^mulB(RoundText[2+4*i])^mulD(RoundText[3+4*i]);
        temp[2]=mulD(RoundText[4*i])^mul9(RoundText[1+4*i])^mulE(RoundText[2+4*i])^mulB(RoundText[3+4*i]);
        temp[3]=mulB(RoundText[4*i])^mulD(RoundText[1+4*i])^mul9(RoundText[2+4*i])^mulE(RoundText[3+4*i]);
        RoundText[4*i]=temp[0];
        RoundText[1+4*i]=temp[1];
        RoundText[2+4*i]=temp[2];
        RoundText[3+4*i]=temp[3];
    }
}
void AddRoundKey16(uint8_t* RoundText,int round){
    for(int i=0;i<16;i++)
        RoundText[i]^=RoundKey[round*16+i];
}
void AES192encrypt16(uint8_t* RoundText, uint8_t *key, int R){
    KeyExpansion16(key);
    int round=0;
    AddRoundKey16(RoundText,round);
    for(round=1;round<R;round++){
        subByte16(RoundText);
        ShiftRow16(RoundText);
        MixColumn16(RoundText);
        AddRoundKey16(RoundText,round);
    }
    subByte16(RoundText);
    ShiftRow16(RoundText);
    AddRoundKey16(RoundText,round);
}
void aes_192_enc(uint8_t output[16], uint8_t key[24], int round, const uint8_t input[16]) {
    int i,j,k;
    uint8_t state[4][4];
    uint8_t w[13][4][4];
    KeyExpansion(key,w);
    State2Matrix(state,input);  
    AddRoundKey(state,w[0]);  
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++)
            printf("%02x, ", state[j][i]);
    }printf("\n");
    for(i=1;i<=round;i++){
        subBytes(state);
        ShiftRows(state);
        if(i!=round)
            MixColumns(state);
        AddRoundKey(state,w[i]);
    }
    Matrix2State(output,state);
}

void test_aes_192(){
    //uint8_t in[16]  = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};
    uint8_t in[16]  = {0xae,0x2d,0x8a,0x57,0x1e,0x03,0xac,0x9c,0x9e,0xb7,0x6f,0xac,0x45,0xaf,0x8e,0x51};
    //8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b
    //974104846d0ad3ad7734ecb3ecee4eef
    uint8_t key[24] = { 0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
                    0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b};
    const uint8_t out[16] = { 0xbd, 0x33, 0x4f, 0x1d, 0x6e, 0x45, 0xf2, 0x5f, 0xf7, 0x12, 0xa2, 0x14, 0x57, 0x1f, 0xa5, 0xcc};
    uint8_t state[16] = {0xae,0x2d,0x8a,0x57,0x1e,0x03,0xac,0x9c,0x9e,0xb7,0x6f,0xac,0x45,0xaf,0x8e,0x51};
    //aes_192_enc(state, key, 12, in);
    AES192encrypt16(state, key, Nr);
    printf("cipher\n");
    for(int i = 0; i < 16; i++) {
        printf("%02x, ", state[i]);
    }printf("\n");
    
}

void compute_first_round(int value_Y0[16], int value_W0[16]){
    uint8_t tstate[16];
    for(int i = 0; i < 16; i++) {
        tstate[i] = value_Y0[i];
    }
    //subByte16(tstate);
    ShiftRow16(tstate);
    MixColumn16(tstate);
    for(int i = 0; i < 16; i++) {
        value_W0[i] = tstate[i];
    }
}

void compute_k1_from_w0(int K1[16], int W0[16], int X1[16]){
    for(int i = 0; i < 16; i++){
        if(X1[i] != -1) {
            K1[i] = W0[i] ^  X1[i];
        }
    }
}

void compute_possible_key(int K[5][16]){
    K[1][12] = K[0][4] ^ K[1][8];
    K[2][0] = K[1][12] ^ K[0][8];
    K[2][4] = K[0][12] ^ K[2][0];

    K[2][1] = K[1][13] ^ K[0][9];
    K[2][5] = K[0][13] ^ K[2][1];

    K[2][3] = K[1][15] ^ K[0][11];
    K[2][7] = K[0][15] ^ K[2][3];

    K[1][9] = K[1][13] ^ K[0][5];
    K[1][11] = K[0][7] ^ K[1][15];

    K[1][4] = rsbox[K[1][11] ^ K[0][3]];
    K[1][5] = rsbox[K[1][8] ^ K[0][0] ^ Rcon[1]];
    K[1][6] = rsbox[K[1][9] ^ K[0][1]];

    K[1][10] = K[0][2] ^ sbox[K[1][7]];
    K[1][14] = K[0][6] ^ K[1][10];
    K[2][2] = K[0][10] ^ K[1][14];
    K[2][6] = K[0][14] ^ K[2][2];
    K[2][10] = K[1][2] ^ K[2][6];
    K[2][14] = K[1][6] ^ K[2][10];

}

void compute_Z_1_3(int W0[16], int K1[16], int X1[16], int Y1[16], int Z1[16]){
    for(int i = 0; i < 16; i++){
        if(K1[i] != -1) {
            X1[i] = W0[i] ^  K1[i];
            Y1[i] = sbox[X1[i]];
            
        }
        Z1[i] = Y1[i];
    }
    ShiftRow16_int(Z1);
}

void compute_W_1_column(int K[5][16], int X[5][16], int W[5][16], int Z[5][16]){
    W[1][3] = K[2][3] ^ X[2][3];
    Z[1][0] = mulF6(W[1][3] ^ Z[1][1] ^ Z[1][2] ^ mul2(Z[1][3]));
    X[1][0] = rsbox[Z[1][0]];
    K[1][0] = X[1][0] ^ W[0][0];
    K[2][8] = K[2][4] ^ K[1][0];
    K[2][12] = K[2][8] ^ K[1][4];

}

void compute_all_K(int K[5][16], int X[5][16], int Y[5][16], int Z[5][16], int W[5][16]){
    X[1][1] = K[1][1] ^ W[0][1];
    Y[1][1] = sbox[X[1][1]];
    Z[1][13] = Y[1][1];

    uint8_t tstate[16];
    for(int i = 0; i < 16; i++) {
        tstate[i] = Z[1][i];
    }
    MixColumn16_1column(tstate, 0);
    MixColumn16_1column(tstate, 1);
    MixColumn16_1column(tstate, 3);
    for(int i = 0; i < 8; i++) {
        W[1][i] = tstate[i];
    }
    for(int i = 12; i < 16; i++) {
        W[1][i] = tstate[i];
    }

    K[2][15] = W[1][15] ^ X[2][15];
    K[2][11] = K[2][15] ^ K[1][7];
    K[1][3] = K[2][11] ^ K[2][7];
}


void collision_search_192_5(){
    memset(value_X, -1, 5*16*sizeof(int));
    memset(value_Y, -1, 5*16*sizeof(int));
    memset(value_K, -1, 5*16*sizeof(int));
    memset(value_X_1, -1, 5*16*sizeof(int));
    memset(value_Y_1, -1, 5*16*sizeof(int));
    memset(value_K_1, -1, 5*16*sizeof(int));
    for(int j = 0; j < 16; j++){
        value_X[0][j] = value_all_X[0][j][0];
        value_Y[0][j] = value_all_Y[0][j][0];
        value_X_1[0][j] = Diff_X[0][j] ^ value_X[0][j];
        value_Y_1[0][j] = Diff_Y[0][j] ^ value_Y[0][j];
    }
    uint64_t count = 0;
    int count1 = 0;
    int count2 = 0;

    for(int flag0 = 0; flag0 < 2; flag0++){ 
        for(int index0 = 0; index0 < 65536; index0++){
            uint8_t mn0[16] = {0};
            int tt0 = index0;
            for(int s0 = 0; s0 < 16; s0++){
                mn0[s0]= tt0 % 2;
                tt0 = tt0 >> 1;
            }
            for(int s0 = 0; s0 < 16; s0++) {
                value_X[0][s0] = value_all_X[0][s0][mn0[s0]];
                value_Y[0][s0] = value_all_Y[0][s0][mn0[s0]];
                value_X_1[0][s0] = Diff_X[0][s0] ^ value_X[0][s0];
                value_Y_1[0][s0] = Diff_Y[0][s0] ^ value_Y[0][s0];
                if(flag0 == 1) {
                    value_X[0][11] = value_all_X[0][11][mn0[s0]+2];
                    value_Y[0][11] = value_all_Y[0][11][mn0[s0]+2];
                    value_X_1[0][11] = Diff_X[0][11] ^ value_X[0][11];
                    value_Y_1[0][11] = Diff_Y[0][11] ^ value_Y[0][11];
                }
            }
            
            for(int t0 = 0; t0 < 16; t0++){
                value_K[0][t0] = value_X[0][t0];
                value_K_1[0][t0] = value_X_1[0][t0];
            }
            compute_first_round(value_Y[0], value_W[0]);
            compute_first_round(value_Y_1[0], value_W_1[0]);
            
            for(int flag1 = 0; flag1 < 2; flag1++){ 
                for(int index1 = 0; index1 < 32; index1++){ 
                    uint8_t mn1[16] = {0};
                    int a1[5] = {2,7,8,13,15};
                    int tt1 = index1;
                    for(int s1 = 0; s1 < 5; s1++){
                        mn1[a1[s1]]= tt1 % 2;
                        tt1 = tt1 >> 1;
                    }
                    for(int s1 = 0; s1 < 16; s1++) {
                        if(diff_num[1][s1]) {
                            if((flag1 == 1) && (s1 == 15)) {
                                value_X[1][s1] = value_all_X[1][s1][mn1[s1]+2];
                                value_Y[1][s1] = value_all_Y[1][s1][mn1[s1]+2];
                                value_X_1[1][s1] = Diff_X[1][s1] ^ value_X[1][s1];
                                value_Y_1[1][s1] = Diff_Y[1][s1] ^ value_Y[1][s1];
                            } else {
                                value_X[1][s1] = value_all_X[1][s1][mn1[s1]];
                                value_Y[1][s1] = value_all_Y[1][s1][mn1[s1]];
                                value_X_1[1][s1] = Diff_X[1][s1] ^ value_X[1][s1];
                                value_Y_1[1][s1] = Diff_Y[1][s1] ^ value_Y[1][s1];
                            }
                        }
                    }
                
                    compute_k1_from_w0(value_K[1], value_W[0], value_X[1]);
                    compute_k1_from_w0(value_K_1[1], value_W_1[0], value_X_1[1]);

                    compute_possible_key(value_K);
                    compute_possible_key(value_K_1);

                    compute_Z_1_3(value_W[0], value_K[1], value_X[1], value_Y[1], value_Z[1]);
                    compute_Z_1_3(value_W_1[0], value_K_1[1], value_X_1[1], value_Y_1[1], value_Z_1[1]);

                    for(int index2 = 0; index2 < 4; index2++){
                        uint8_t mn2[16] = {0};
                        int a2[2] = {3,15};
                        int tt2 = index2;
                        for(int s2 = 0; s2 < 2; s2++){
                            mn2[a2[s2]] = tt2 % 2;
                            tt2 = tt2 >> 1;
                        }
                        for(int s2 = 0; s2 < 16; s2++) {
                            if(diff_num[2][s2]) {
                                value_X[2][s2] = value_all_X[2][s2][mn2[s2]];
                                value_Y[2][s2] = value_all_Y[2][s2][mn2[s2]];
                                value_X_1[2][s2] = Diff_X[2][s2] ^ value_X[2][s2];
                                value_Y_1[2][s2] = Diff_Y[2][s2] ^ value_Y[2][s2];
                            }
                        }

                        compute_W_1_column(value_K, value_X, value_W, value_Z);
                        compute_W_1_column(value_K_1, value_X_1, value_W_1, value_Z_1);

                        for(int g = 0; g < 256; g++) { 
                            value_K[1][1] = g;
                            value_K_1[1][1] = g; 
                            compute_all_K(value_K, value_X, value_Y, value_Z, value_W);
                            compute_all_K(value_K_1, value_X_1, value_Y_1, value_Z_1, value_W_1);

                            uint8_t tmp[24];
                            for(int i = 0; i < 24; i++){
                                tmp[i] = value_K[i/16][i%16];
                            }count++;
                            uint8_t ttemp[16] = {0};
                            AES192encrypt16(ttemp, tmp, 5);

                            uint8_t tmp1[24];
                            for(int i = 0; i < 24; i++){
                                tmp1[i] = value_K_1[i/16][i%16];
                            }
                            uint8_t ttemp1[16] = {0}; 
                            AES192encrypt16(ttemp1, tmp1, 5);

                            bool test_collision = true;
                            for(int i = 0; i < 16; i++) {
                                if(ttemp1[i] ^ ttemp[i]) {
                                    test_collision = false;
                                }
                            }

                            if(test_collision) {
                                count2++;
                                printf("collison found:%d\n",count);
                                printf("flag0 %d,index0 %d,flag1 %d,index1 %d, index2 %d, g %d\n", flag0, index0, flag1, index1, index2, g);
                                for(int i = 0; i < 24; i++) {
                                    printf("%x, ", tmp[i]);
                                }printf("\n");
                                for(int i = 0; i < 24; i++) {
                                    printf("%x, ", tmp1[i]);
                                }printf("\n");
                               
                            }
                            
                        }

                    }

                }  
            }
        }
    } 
    printf("count2 for collision: %d\n", count2);
    printf("total countt: %llx\n",count);

}


void verify_collison(){
    uint8_t K1[6][24] = {{0x44, 0xd9, 0x6d, 0x84, 0x5d, 0x53, 0x12, 0xc8, 0xf1, 0x9c, 0x36, 0x0, 0x81, 0x4b, 0xa0, 0x31, 0x96, 0xf3, 0x70, 0x56, 0x25, 0xa2, 0x45, 0x2},
    {0x44, 0xf6, 0x6d, 0x84, 0x5d, 0x53, 0x12, 0xc8, 0xf1, 0xb3, 0x36, 0x5e, 0xae, 0x4b, 0xa0, 0x31, 0xb1, 0x58, 0x2b, 0xad, 0x72, 0xa2, 0x14, 0xd8},
    {0x44, 0xf6, 0x38, 0x84, 0x5d, 0x7c, 0x12, 0xc8, 0xf1, 0xb3, 0x36, 0x5e, 0xae, 0x4b, 0xa0, 0x31, 0xc1, 0x3c, 0xd6, 0x1f, 0xde, 0x9, 0x50, 0xb1},
    {0x44, 0xf6, 0x6d, 0xda, 0x5d, 0x7c, 0x47, 0x80, 0xde, 0x9c, 0x47, 0x0, 0xae, 0x4b, 0xd1, 0x79, 0x78, 0xbb, 0x14, 0xbc, 0x68, 0xc2, 0xa9, 0xcc},
    {0x44, 0xd9, 0x6d, 0xda, 0x5d, 0x7c, 0x47, 0x80, 0xde, 0x9c, 0x47, 0x0, 0xae, 0x4b, 0xd1, 0x79, 0x5f, 0xf6, 0x14, 0xb4, 0xa1, 0x1, 0xe7, 0xa5},
    {0x44, 0xd9, 0x38, 0x84, 0x5d, 0x53, 0x12, 0xc8, 0xde, 0xb3, 0x47, 0x0, 0x81, 0x4b, 0xd1, 0x79, 0xf7, 0xff, 0xb2, 0xdb, 0x45, 0x49, 0xc8, 0x6}
    };

    uint8_t K2[6][24] = {{0x6b, 0xf6, 0x38, 0xda, 0x72, 0x7c, 0x47, 0x80, 0xde, 0xb3, 0x47, 0x5e, 0xae, 0x64, 0xd1, 0x79, 0x96, 0xf3, 0x70, 0x40, 0x25, 0xa2, 0x45, 0x2},
    {0x6b, 0xd9, 0x38, 0xda, 0x72, 0x7c, 0x47, 0x80, 0xde, 0x9c, 0x47, 0x0, 0x81, 0x64, 0xd1, 0x79, 0xb1, 0x58, 0x2b, 0xbb, 0x72, 0xa2, 0x14, 0xd8},
    {0x6b, 0xd9, 0x6d, 0xda, 0x72, 0x53, 0x47, 0x80, 0xde, 0x9c, 0x47, 0x0, 0x81, 0x64, 0xd1, 0x79, 0xc1, 0x3c, 0xd6, 0x9, 0xde, 0x9, 0x50, 0xb1},
    {0x6b, 0xd9, 0x38, 0x84, 0x72, 0x53, 0x12, 0xc8, 0xf1, 0xb3, 0x36, 0x5e, 0x81, 0x64, 0xa0, 0x31, 0x78, 0xbb, 0x14, 0xaa, 0x68, 0xc2, 0xa9, 0xcc},
    {0x6b, 0xf6, 0x38, 0x84, 0x72, 0x53, 0x12, 0xc8, 0xf1, 0xb3, 0x36, 0x5e, 0x81, 0x64, 0xa0, 0x31, 0x5f, 0xf6, 0x14, 0xa2, 0xa1, 0x1, 0xe7, 0xa5},
    {0x6b, 0xf6, 0x6d, 0xda, 0x72, 0x7c, 0x47, 0x80, 0xf1, 0x9c, 0x36, 0x5e, 0xae, 0x64, 0xa0, 0x31, 0xf7, 0xff, 0xb2, 0xcd, 0x45, 0x49, 0xc8, 0x6}
    };


    for(int k = 0; k < 6; k++) {
        uint8_t ttemp[16] = {0};
        uint8_t ttemp1[16] = {0}; 
        uint8_t tmp[24];
        for(int i = 0; i < 24; i++){
            tmp[i] = K1[k][i];
        }
        AES192encrypt16(ttemp, tmp, 5);
        for(int i = 0; i < 24; i++) {
            printf("%02x, ", tmp[i]);
        }printf("\n"); 

        uint8_t tmp1[24];
        for(int i = 0; i < 24; i++){
            tmp1[i] = K2[k][i];
        }
        AES192encrypt16(ttemp1, tmp1, 5);
        for(int i = 0; i < 24; i++) {
            printf("%02x, ", tmp1[i]);
        }printf("\n"); 

        for(int i = 0; i < 24; i++) {
            printf("%02x, ", tmp1[i]^tmp[i]);
        }printf("\n"); 

        for(int i = 0; i < 16; i++) {
            printf("%02x, ", ttemp[i]);
        }printf("\n"); 

        for(int i = 0; i < 16; i++) {
            printf("%02x, ", ttemp1[i]);
        }printf("\n");

        for(int i = 0; i < 16; i++) {
            printf("%x, ", ttemp1[i] ^ ttemp[i]);
        }printf("\n\n\n"); 


    }


}



void main()
{
    printf("begin main\n");
    int t = gen_sbox_ddt();
    gen_sbox_diff_term();
    get_diff_nums();
    get_diff_values();

    collision_search_192_5();
    //verify_collison();
    printf("end main\n");
}