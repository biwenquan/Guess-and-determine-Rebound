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
{0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa6, 0x53, 0x00, 0x00},
{0x00, 0x00, 0xf5, 0x00, 0x00, 0x00, 0xf5, 0x00, 0x34, 0xd1, 0x10, 0xe5, 0x00, 0xec, 0xc1, 0x9b},
{0xbd, 0x12, 0x06, 0x14, 0xe4, 0x06, 0x17, 0xf2, 0x0c, 0xf2, 0xe1, 0x6a, 0xb4, 0x0c, 0x6e, 0x97},
{0xdc, 0x00, 0x00, 0x00, 0x00, 0xa8, 0x00, 0x00, 0x00, 0x00, 0xb2, 0x00, 0x00, 0x00, 0x00, 0x14}
};
static const uint8_t Diff_Y[5][16] = {
{0x00, 0x53, 0x00, 0x00, 0x00, 0x00, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9b, 0xe5, 0x00, 0x00},
{0x00, 0x00, 0x85, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x82, 0x0b, 0xda, 0xe8, 0x00, 0x72, 0xf9, 0x67},
{0x44, 0x09, 0x66, 0x09, 0xd4, 0x66, 0x7d, 0x55, 0xab, 0xea, 0x3b, 0xc4, 0x51, 0x2a, 0x9f, 0xc5},
{0xe5, 0x00, 0x00, 0x00, 0x00, 0xed, 0x00, 0x00, 0x00, 0x00, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x1c}
};
static const uint8_t Diff_K[5][16] = {
//0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
{0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf5, 0x00},
{0x00, 0x00, 0xf5, 0x00, 0x00, 0x00, 0xf5, 0x00, 0x00, 0x00, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf5, 0x00},
{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf5, 0x00}
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
int diff_k16_num = 0;
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

void get_diff_k_nums() {
    diff_k16_num = sbox_ddt_term[Diff_K[1][6]][Diff_K[0][1] ^ Diff_K[1][9]].num;
}

void get_diff_K_values(){
    memset(value_all_K, -1, 5*16*4*sizeof(int));
    
    for(int k = 0; k < diff_k16_num; k++){
        value_all_K[1][6][k] = sbox_ddt_term[Diff_K[1][6]][Diff_K[0][1] ^ Diff_K[1][9]].term[k][0];
    }
}

void test_diff_values(){
    int x,y;int num=0;
    for(int i = 0; i < 5; i++) {
        for (int j = 0; j < 16; j++) {
            for(int k = 0; k < diff_num[i][j]; k++){
                x = k;
                y = (diff_num[i][j] == 2) ? (k+1)%2 : ((k%2 == 0)? k+1 : k-1);
                if((value_all_X[i][j][x] != (value_all_X[i][j][y] ^ Diff_X[i][j])) || (value_all_Y[i][j][x] != (value_all_Y[i][j][y] ^ Diff_Y[i][j]))) {
                    printf("mismatch at %d round %d byte %d\n", i, j, k); continue; 
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

uint8_t mul1(uint8_t x){
    uint8_t out = x;
    return out;
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

uint8_t mul5(uint8_t x){
    uint8_t out = mul4(x) ^ (x);
    return out;
}

uint8_t mul6(uint8_t x){
    uint8_t out = mul2(mul3(x));
    return out;
}

uint8_t mul7(uint8_t x){
    uint8_t out = mul4(x) ^ mul3(x);
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

uint8_t mulE1(uint8_t x){
    uint8_t out = mul8(mul8(mul3(x))) ^ mul4(mul4(mul2(x))) ^ x;
    return out;
}

uint8_t mul4F(uint8_t x){
    uint8_t out = mul8(mul8(x)) ^ mul4(mul3(x)) ^ mul3(x);
    return out;
}

uint8_t mulCB(uint8_t x){
    uint8_t out = mul8(mul8(mul3(x))) ^ (mul8(x)) ^ (mul3(x));
    return out;
}

uint8_t mulF6(uint8_t x){
    uint8_t out = mul8(mul8(mul3(x))) ^ mul4(mul4(mul3(x))) ^ mul2(mul3(x));
    return out;
}

uint8_t mul8D(uint8_t x){
    uint8_t out = mul8(mul8(mul2(x))) ^ (mul4(mul3(x))) ^ (x);
    return out;
}

uint8_t mul52(uint8_t x){
    uint8_t out = mul8(mul8(x)) ^ (mul4(mul4(x))) ^ mul2(x);
    return out;
}

uint8_t mulD1(uint8_t x){
    uint8_t out = mul8(mul8(mul3(x))) ^ (mul4(mul4(x))) ^ (x);
    return out;
}

uint8_t mulE5(uint8_t x){
    uint8_t out = mul8(mul8(mul3(x))) ^ mul4(mul4(mul2(x))) ^ mul4(x) ^ (x);
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
    for (i = Nk; i < Nb * (Nr + 1); ++i)
    {
        {
        tempa[0]=RoundKey[(i-1) * 4 + 0];
        tempa[1]=RoundKey[(i-1) * 4 + 1];
        tempa[2]=RoundKey[(i-1) * 4 + 2];
        tempa[3]=RoundKey[(i-1) * 4 + 3];
        }
        if((Nk == 8) && (i % Nk == 4)) {
            tempa[0] = sbox[tempa[0]];
            tempa[1] = sbox[tempa[1]];
            tempa[2] = sbox[tempa[2]];
            tempa[3] = sbox[tempa[3]];
        } else if (i % Nk == 0) {
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
void InvSubByte16_int(int *RoundText){
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
void InvShiftRow16_int(int *RoundText){
    int t;
    //row1
    t=RoundText[13]; RoundText[13]=RoundText[9];RoundText[9]=RoundText[5]; RoundText[5]=RoundText[1]; RoundText[1]=t;
    //row2
    t=RoundText[2],RoundText[2]=RoundText[10];RoundText[10]=t;
    t=RoundText[6];RoundText[6]=RoundText[14];RoundText[14]=t;
    //row3
    t=RoundText[3]; RoundText[3]=RoundText[7];RoundText[7]=RoundText[11]; RoundText[11]=RoundText[15]; RoundText[15]=t;
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

void InvMixColumn16_1column(uint8_t* RoundText, int i){
    uint8_t temp[4]; 
    temp[0]=mulE(RoundText[4*i])^mulB(RoundText[1+4*i])^mulD(RoundText[2+4*i])^mul9(RoundText[3+4*i]);
    temp[1]=mul9(RoundText[4*i])^mulE(RoundText[1+4*i])^mulB(RoundText[2+4*i])^mulD(RoundText[3+4*i]);
    temp[2]=mulD(RoundText[4*i])^mul9(RoundText[1+4*i])^mulE(RoundText[2+4*i])^mulB(RoundText[3+4*i]);
    temp[3]=mulB(RoundText[4*i])^mulD(RoundText[1+4*i])^mul9(RoundText[2+4*i])^mulE(RoundText[3+4*i]);
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

void compute_forward_first(int X[5][16], int Y[5][16], int Z[5][16], int W[5][16], int K[5][16]){
    int tstate[16];
    for(int i = 0; i < 16; i++) {
        tstate[i] = Y[1][i];
    }
    ShiftRow16_int(tstate);
    for(int i = 0; i < 16; i++) {
        Z[1][i] = tstate[i];
    }
    for(int i = 0; i < 16; i++) {
        tstate[i] = Y[2][i];
    }
    ShiftRow16_int(tstate);
    for(int i = 0; i < 16; i++) {
        Z[2][i] = tstate[i];
    }
    for(int i = 0; i < 16; i++) {
        tstate[i] = Y[4][i];
    }
    ShiftRow16_int(tstate);
    for(int i = 0; i < 16; i++) {
        Z[4][i] = tstate[i];
    }
    uint8_t tstate1[16];
    for(int i = 0; i < 16; i++) {
        tstate1[i] = Z[4][i];
    }
    MixColumn16_1column(tstate1, 0);
    for(int i = 0; i < 4; i++) {
        W[4][i] = tstate1[i];
    }

    for(int i = 0; i < 16; i++) {
        tstate[i] = Y[3][i];
    }
    ShiftRow16_int(tstate);
    for(int i = 0; i < 16; i++) {
        Z[3][i] = tstate[i];
    }
    for(int i = 0; i < 16; i++) {
        tstate1[i] = Z[3][i];
    }
    MixColumn16(tstate1);
    for(int i = 0; i < 16; i++) {
        W[3][i] = tstate1[i];
    }
    K[4][0] = X[4][0] ^ W[3][0];
    K[4][5] = X[4][5] ^ W[3][5];
    K[4][10] = X[4][10] ^ W[3][10];
    K[4][15] = X[4][15] ^ W[3][15];

}


void guess_K3_8_12_13(int i, int j, int k, int X[5][16], int Y[5][16], int Z[5][16], int W[5][16], int K[5][16]){
    K[3][8] = i;
    K[3][12] = j;
    K[3][13] = k;
    K[2][4] = K[3][12] ^ K[3][8];
    K[2][8] = K[3][12] ^ K[4][0];
    K[1][0] = K[2][8] ^ K[2][4];

    W[2][8] = X[3][8] ^ K[3][8];
    W[2][12] = X[3][12] ^ K[3][12];
    W[2][13] = X[3][13] ^ K[3][13];

    Z[2][11] = W[2][8] ^ Z[2][10] ^ mul2(Z[2][8]) ^ mul3(Z[2][9]);
    W[2][9] = Z[2][8] ^ Z[2][11] ^ mul2(Z[2][9]) ^ mul3(Z[2][10]);
    W[2][10] = Z[2][8] ^ Z[2][9] ^ mul2(Z[2][10]) ^ mul3(Z[2][11]);
    W[2][11] = Z[2][9] ^ Z[2][10] ^ mul2(Z[2][11]) ^ mul3(Z[2][8]);

    Z[2][13] = mulD1(W[2][12] ^ mul2(W[2][13]) ^ mul7(Z[2][14]) ^ mul3(Z[2][15]));
    Z[2][12] = W[2][13] ^ Z[2][15] ^ mul2(Z[2][13]) ^ mul3(Z[2][14]);
    W[2][14] = Z[2][12] ^ Z[2][13] ^ mul2(Z[2][14]) ^ mul3(Z[2][15]);
    W[2][15] = Z[2][13] ^ Z[2][14] ^ mul2(Z[2][15]) ^ mul3(Z[2][12]);

    W[1][8] = X[2][8] ^ K[2][8];
    K[3][9] = X[3][9] ^ W[2][9];
    K[3][10] = X[3][10] ^ W[2][10];
    K[3][11] = X[3][11] ^ W[2][11];
    K[3][14] = X[3][14] ^ W[2][14];
    K[3][15] = X[3][15] ^ W[2][15];

    Y[2][7] = Z[2][11];
    X[2][7] = rsbox[Y[2][7]];
    Y[2][12] = Z[2][12];
    X[2][12] = rsbox[Y[2][12]];
    Y[2][1] = Z[2][13];
    X[2][1] = rsbox[Y[2][1]];

    K[2][5] = K[3][13] ^ K[3][9];
    K[2][6] = K[3][14] ^ K[3][10];
    K[2][7] = K[3][15] ^ K[3][11];
    W[1][6] = X[2][6] ^ K[2][6];
    W[1][7] = X[2][7] ^ K[2][7];

}

void guess_K2_12_13(int i, int j, int X[5][16], int Y[5][16], int Z[5][16], int W[5][16], int K[5][16]){
    K[2][12] = i;
    K[2][13] = j;
    K[1][4] = K[2][12] ^ K[2][8];
    K[4][4] = K[2][12] ^ K[4][0];
    K[4][1] = K[2][13] ^ K[4][5];
    K[2][9] = K[3][13] ^ K[4][1];
    K[1][5] = K[2][13] ^ K[2][9];
    K[1][1] = K[2][9] ^ K[2][5];

    W[1][9] = X[2][9] ^ K[2][9];
    W[1][12] = X[2][12] ^ K[2][12];
    W[1][13] = X[2][13] ^ K[2][13];
    X[4][1] = W[3][1] ^ K[4][1];
    X[4][4] = W[3][4] ^ K[4][4];
    Y[4][1] = sbox[X[4][1]];
    Y[4][4] = sbox[X[4][4]];
    Z[4][4] = Y[4][4];
    Z[4][13] = Y[4][1];

    Z[1][13] = W[1][12] ^ W[1][13] ^ mul2(Z[1][14]) ^ mul3(Z[1][12]);
    Z[1][15] = W[1][12] ^ Z[1][14] ^ mul2(Z[1][12]) ^ mul3(Z[1][13]);
    W[1][14] = Z[1][12] ^ Z[1][13] ^ mul2(Z[1][14]) ^ mul3(Z[1][15]);
    W[1][15] = Z[1][13] ^ Z[1][14] ^ mul2(Z[1][15]) ^ mul3(Z[1][12]);

    K[2][14] = X[2][14] ^ W[1][14];
    K[2][15] = X[2][15] ^ W[1][15];
    Y[1][1] = Z[1][13];
    X[1][1] = rsbox[Y[1][1]];
    Y[1][11] = Z[1][15];
    X[1][11] = rsbox[Y[1][11]];

    K[2][10] = K[2][14] ^ K[1][6];
    K[1][2] = K[2][10] ^ K[2][6];
    K[4][2] = K[2][10] ^ K[3][14];
    K[4][6] = K[2][14] ^ K[4][2];
    W[1][10] = X[2][10] ^ K[2][10];

    X[4][2] = W[3][2] ^ K[4][2];
    X[4][6] = W[3][6] ^ K[4][6];
    Y[4][2] = sbox[X[4][2]];
    Y[4][6] = sbox[X[4][6]];
    Z[4][10] = Y[4][2];
    Z[4][14] = Y[4][6];

    W[1][11] = mulE1(Z[1][9] ^ mul9(W[1][8]) ^ mulE(W[1][9]) ^ mulB(W[1][10]));
    Z[1][8] = mulD(W[1][10]) ^ mul9(W[1][11]) ^ mulE(W[1][8]) ^ mulB(W[1][9]);
    Z[1][10] = mulD(W[1][8]) ^ mul9(W[1][9]) ^ mulE(W[1][10]) ^ mulB(W[1][11]);
    Z[1][11] = mulD(W[1][9]) ^ mul9(W[1][10]) ^ mulE(W[1][11]) ^ mulB(W[1][8]);

    K[2][11] = X[2][11] ^ W[1][11];
    Y[1][8] = Z[1][8];
    Y[1][2] = Z[1][10];
    Y[1][7] = Z[1][11];
    X[1][8] = rsbox[Y[1][8]];
    X[1][7] = rsbox[Y[1][7]];
    X[1][2] = rsbox[Y[1][2]];

    K[1][3] = K[2][11] ^ K[2][7];
    K[1][7] = K[2][15] ^ K[2][11];
    K[4][3] = K[2][11] ^ K[3][15];
    K[4][7] = K[2][15] ^ K[4][3];
    K[3][2] = K[4][10] ^ sbox[K[4][7]];
    K[1][10] = K[3][2] ^ sbox[K[2][15]];

    W[2][2] = X[3][2] ^ K[3][2];
    X[4][3] = W[3][3] ^ K[4][3];
    X[4][7] = W[3][7] ^ K[4][7];
    Y[4][3] = sbox[X[4][3]];
    Y[4][7] = sbox[X[4][7]];
    Z[4][7] = Y[4][3];
    Z[4][11] = Y[4][7];

}


void guess_K3_0(int i, int X[5][16], int Y[5][16], int Z[5][16], int W[5][16], int K[5][16]){
    K[3][0] = i;
    K[1][8] = K[3][0] ^ sbox[K[2][13]] ^ Rcon[2];
    K[4][8] = K[3][0] ^ sbox[K[4][5]] ^ Rcon[3];
    W[2][0] = X[3][0] ^ K[3][0];
    X[4][8] = W[3][8] ^ K[4][8];
    Y[4][8] = sbox[X[4][8]];
    Z[4][8] = Y[4][8];

    Z[2][1] = W[2][0] ^ mul2(W[2][2]) ^ mul5(Z[2][2]) ^ mul7(Z[2][3]);
    Z[2][0] = W[2][2] ^ Z[2][1] ^ mul2(Z[2][2]) ^ mul3(Z[2][3]);
    W[2][1] = Z[2][0] ^ Z[2][3] ^ mul2(Z[2][1]) ^ mul3(Z[2][2]);
    W[2][3] = Z[2][1] ^ Z[2][2] ^ mul2(Z[2][3]) ^ mul3(Z[2][0]);
    K[3][1] = X[3][1] ^ W[2][1];
    K[3][3] = X[3][3] ^ W[2][3];

    Y[2][0] = Z[2][0];
    Y[2][5] = Z[2][1];
    X[2][0] = rsbox[Y[2][0]];
    X[2][5] = rsbox[Y[2][5]];
    W[1][5] = X[2][5] ^ K[2][5];

    K[1][9] = K[3][1] ^ sbox[K[2][14]];
    K[1][11] = K[3][3] ^ sbox[K[2][12]];
    K[4][9] = K[3][1] ^ sbox[K[4][6]];
    K[4][11] = K[3][3] ^ sbox[K[4][4]];
    K[3][7] = K[4][11] ^ K[4][15];
    K[2][3] = K[3][7] ^ K[3][11];
    K[1][15] = K[3][7] ^ K[3][3];
    W[2][7] = X[3][7] ^ K[3][7];
    X[4][9] = W[3][9] ^ K[4][9];
    X[4][11] = W[3][11] ^ K[4][11];
    Y[4][9] = sbox[X[4][9]];
    Y[4][11] = sbox[X[4][11]];
    Z[4][5] = Y[4][9];
    Z[4][15] = Y[4][11];

}

void guess_K3_4(int i, int X[5][16], int Y[5][16], int Z[5][16], int W[5][16], int K[5][16]){
    K[3][4] = i;
    K[1][12] = K[3][4] ^ K[3][0];
    K[2][0] = K[3][4] ^ K[3][8];
    K[4][12] = K[3][4] ^ K[4][8];
    W[1][0] = X[2][0] ^ K[2][0];
    W[2][4] = X[3][4] ^ K[3][4];
    X[4][12] = W[3][12] ^ K[4][12];
    Y[4][12] = sbox[X[4][12]];
    Z[4][12] = Y[4][12];

    Z[2][4] = mulD1(W[2][7] ^ mul2(W[2][4]) ^ mul7(Z[2][5]) ^ mul3(Z[2][6]));
    Z[2][7] = W[2][4] ^ Z[2][6] ^ mul2(Z[2][4]) ^ mul3(Z[2][5]);
    W[2][5] = Z[2][4] ^ Z[2][7] ^ mul2(Z[2][5]) ^ mul3(Z[2][6]);
    W[2][6] = Z[2][4] ^ Z[2][5] ^ mul2(Z[2][6]) ^ mul3(Z[2][7]);
    K[3][5] = X[3][5] ^ W[2][5];
    K[3][6] = X[3][6] ^ W[2][6];

    Y[2][4] = Z[2][4];
    Y[2][3] = Z[2][7];
    X[2][3] = rsbox[Y[2][3]];
    X[2][4] = rsbox[Y[2][4]];
    W[1][3] = X[2][3] ^ K[2][3];
    W[1][4] = X[2][4] ^ K[2][4];

    K[1][13] = K[3][5] ^ K[3][1];
    K[1][14] = K[3][6] ^ K[3][2];
    K[2][1] = K[3][9] ^ K[3][5];
    K[2][2] = K[3][10] ^ K[3][6];
    K[4][13] = K[4][9] ^ K[3][5];
    K[4][14] = K[4][10] ^ K[3][6];

    W[1][1] = X[2][1] ^ K[2][1];
    W[1][2] = X[2][2] ^ K[2][2];
    X[4][13] = W[3][13] ^ K[4][13];
    X[4][14] = W[3][14] ^ K[4][14];
    Y[4][13] = sbox[X[4][13]];
    Y[4][14] = sbox[X[4][14]];
    Z[4][9] = Y[4][13];
    Z[4][6] = Y[4][14];

    uint8_t tstate1[16];
    for(int i = 0; i < 16; i++) {
        tstate1[i] = Z[4][i];
    }
    MixColumn16_1column(tstate1, 1);
    MixColumn16_1column(tstate1, 2);
    MixColumn16_1column(tstate1, 3);
    for(int i = 4; i < 16; i++) {
        W[4][i] = tstate1[i];
    }

    for(int i = 0; i < 16; i++) {
        tstate1[i] = W[1][i];
    }
    InvMixColumn16_1column(tstate1, 0);
    InvMixColumn16_1column(tstate1, 1);
    for(int i = 0; i < 8; i++) {
        Z[1][i] = tstate1[i];
    }

    int tstate[16];
    for(int i = 0; i < 16; i++) {
        tstate[i] = Z[1][i];
    }
    InvShiftRow16_int(tstate);
    for(int i = 0; i < 16; i++) {
        Y[1][i] = tstate[i];
    }

    InvSubByte16_int(tstate);
    for(int i = 0; i < 16; i++) {
        X[1][i] = tstate[i];
    }

}

void compute_to_start_point(int X[5][16], int Y[5][16], int Z[5][16], int W[5][16], int K[5][16], uint8_t P[16]){
    K[0][0] = K[1][8] ^ sbox[K[1][5]] ^ Rcon[1];
    K[0][1] = K[1][9] ^ sbox[K[1][6]];
    K[0][2] = K[1][10] ^ sbox[K[1][7]];
    K[0][3] = K[1][11] ^ sbox[K[1][4]];
    K[0][4] = K[1][8] ^ K[1][12];
    K[0][5] = K[1][9] ^ K[1][13];
    K[0][6] = K[1][10] ^ K[1][14];
    K[0][7] = K[1][11] ^ K[1][15];
    K[0][8] = K[2][0] ^ K[1][12];
    K[0][9] = K[2][1] ^ K[1][13];
    K[0][10] = K[2][2] ^ K[1][14];
    K[0][11] = K[2][3] ^ K[1][15];
    K[0][12] = K[2][0] ^ K[2][4];
    K[0][13] = K[2][1] ^ K[2][5];
    K[0][14] = K[2][2] ^ K[2][6];
    K[0][15] = K[2][3] ^ K[2][7];

    for(int i = 0; i < 16; i++) {
        W[0][i] = X[1][i] ^ K[1][i];
    }
    uint8_t tstate1[16];
    for(int i = 0; i < 16; i++) {
        tstate1[i] = W[0][i];
    }
    InvMixColumn16(tstate1);
    for(int i = 0; i < 16; i++) {
        Z[0][i] = tstate1[i];
    }
    InvShiftRow16(tstate1);
    for(int i = 0; i < 16; i++) {
        Y[0][i] = tstate1[i];
    }
    InvSubByte16(tstate1);
    for(int i = 0; i < 16; i++) {
        X[0][i] = tstate1[i];
    }
    for(int i = 0; i < 16; i++) {
        P[i] = tstate1[i] ^ K[0][i];
    }

}

void collision_search_192_7_semi_free(){
    memset(value_X, -1, 5*16*sizeof(int));
    memset(value_Y, -1, 5*16*sizeof(int));
    memset(value_Z, -1, 5*16*sizeof(int));
    memset(value_W, -1, 5*16*sizeof(int));
    memset(value_K, -1, 5*16*sizeof(int));
    memset(value_X_1, -1, 5*16*sizeof(int));
    memset(value_Y_1, -1, 5*16*sizeof(int));
    memset(value_Z_1, -1, 5*16*sizeof(int));
    memset(value_W_1, -1, 5*16*sizeof(int));
    memset(value_K_1, -1, 5*16*sizeof(int));
    uint64_t count = 0; 
    int count2 = 0;

    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 16; j++){
            if ((diff_num[i][j]))
            {
                value_X[i][j] = value_all_X[i][j][1];
                value_Y[i][j] = value_all_Y[i][j][1];
                value_X_1[i][j] = Diff_X[i][j] ^ value_X[i][j];
                value_Y_1[i][j] = Diff_Y[i][j] ^ value_Y[i][j];
            }
        }
    }

    value_K[1][6] = value_all_K[1][6][0];
    value_K_1[1][6] = value_K[1][6] ^ Diff_K[1][6];

    compute_forward_first(value_X, value_Y, value_Z, value_W, value_K);
    compute_forward_first(value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);

    for(int k38 = 0; k38 < 16; k38++) {  
        for(int k312 = 0; k312 < 16; k312++) {
            for(int k313 = 0; k313 < 16; k313++) {
                guess_K3_8_12_13(k38, k312, k313, value_X, value_Y, value_Z, value_W, value_K);
                guess_K3_8_12_13(k38, k312, k313, value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);
                for(int k212 = 0; k212 < 16; k212++) {
                    for(int k213 = 0; k213 < 8; k213++) {
                        guess_K2_12_13(k212, k213, value_X, value_Y, value_Z, value_W, value_K);
                        guess_K2_12_13(k212, k213, value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);
                        for(int k30 = 0; k30 < 8; k30++) {
                            guess_K3_0(k30, value_X, value_Y, value_Z, value_W, value_K);
                            guess_K3_0(k30, value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);
                            for(int k34 = 0; k34 < 8; k34++) {
                                guess_K3_4(k34, value_X, value_Y, value_Z, value_W, value_K);
                                guess_K3_4(k34, value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1);
                                count++;
                                uint8_t ttemp[16] = {0}; 
                                uint8_t ttemp1[16] = {0};    
                                uint8_t P0[16] = {0}; 
                                uint8_t P1[16] = {0};                              
                                compute_to_start_point(value_X, value_Y, value_Z, value_W, value_K, ttemp);
                                compute_to_start_point(value_X_1, value_Y_1, value_Z_1, value_W_1, value_K_1, ttemp1);
                                for(int i = 0; i < 16; i++){
                                    P0[i] = ttemp[i];
                                    P1[i] = ttemp1[i];
                                }
                                uint8_t tmp[24];
                                for(int i = 0; i < 24; i++){
                                    tmp[i] = value_K[i/16][i%16];
                                }
                                AES192encrypt16(ttemp, tmp, 7);

                                uint8_t tmp1[24];
                                for(int i = 0; i < 24; i++){
                                    tmp1[i] = value_K_1[i/16][i%16];
                                }
                                AES192encrypt16(ttemp1, tmp1, 7);

                                bool test_collision = true;
                                for(int i = 0; i < 16; i++) {
                                    if((ttemp1[i] ^ ttemp[i]) || (P0[i] ^ P1[i])) {
                                        test_collision = false;
                                    }
                                }


                                if(test_collision) {
                                    count2++;
                                    printf("collison found:%d\n",count);                                    
                                    printf("k38 %d,k312 %d,k313 %d,k212 %d, k213 %d, k30 %d, k34 %d\n", k38, k312, k313, k212, k213, k30, k34);
                                    printf("P0\n");
                                    for(int i = 0; i < 16; i++) {
                                        printf("%x, ", P0[i]);
                                    }printf("\n");
                                    printf("K0\n");
                                    for(int i = 0; i < 24; i++) {
                                        printf("%x, ", tmp[i]);
                                    }printf("\n");
                                    printf("P1\n");
                                    for(int i = 0; i < 16; i++) {
                                        printf("%x, ", P1[i]);
                                    }printf("\n");
                                    printf("K1\n");
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
    }
    printf("count2 for collision: %d\n", count2);
    printf("countt all try: %llx\n",count);
}


void verify_collison(){
    uint8_t P1[6][16] = {{0x64, 0xd6, 0x68, 0x75, 0xc6, 0xb, 0x79, 0xe2, 0xe6, 0x80, 0x73, 0x16, 0x8f, 0x38, 0xcd, 0x68},
    {0x7b, 0xe8, 0x13, 0x82, 0xa1, 0xcf, 0xba, 0x27, 0x72, 0xf5, 0x49, 0x63, 0x1, 0x80, 0x4, 0xea},
    {0xc5, 0xf8, 0x1b, 0x71, 0xa, 0x1a, 0x8d, 0xb5, 0x61, 0xa, 0x8c, 0x61, 0x81, 0xee, 0x87, 0xd7},
    {0xe9, 0x50, 0xea, 0x4d, 0xf, 0x0, 0xb6, 0xf0, 0x9a, 0x42, 0x58, 0x5c, 0xd5, 0x84, 0x2d, 0xdc},
    {0x6a, 0xcb, 0xfa, 0xe0, 0x7d, 0x2a, 0xc, 0xc7, 0x23, 0x1a, 0x41, 0x2c, 0x1f, 0x57, 0x19, 0x7b},
    {0xa9, 0xdf, 0x44, 0x3, 0x5, 0x8c, 0xc6, 0xc, 0x58, 0x9f, 0x9d, 0x45, 0x76, 0x7d, 0x63, 0xe8}
    };

    uint8_t P2[6][24] = {{0x64, 0xd6, 0x68, 0x75, 0xc6, 0xb, 0x79, 0xe2, 0xe6, 0x80, 0x73, 0x16, 0x8f, 0x38, 0xcd, 0x68},
    {0x7b, 0xe8, 0x13, 0x82, 0xa1, 0xcf, 0xba, 0x27, 0x72, 0xf5, 0x49, 0x63, 0x1, 0x80, 0x4, 0xea},
    {0xc5, 0xf8, 0x1b, 0x71, 0xa, 0x1a, 0x8d, 0xb5, 0x61, 0xa, 0x8c, 0x61, 0x81, 0xee, 0x87, 0xd7},
    {0xe9, 0x50, 0xea, 0x4d, 0xf, 0x0, 0xb6, 0xf0, 0x9a, 0x42, 0x58, 0x5c, 0xd5, 0x84, 0x2d, 0xdc},
    {0x6a, 0xcb, 0xfa, 0xe0, 0x7d, 0x2a, 0xc, 0xc7, 0x23, 0x1a, 0x41, 0x2c, 0x1f, 0x57, 0x19, 0x7b},
    {0xa9, 0xdf, 0x44, 0x3, 0x5, 0x8c, 0xc6, 0xc, 0x58, 0x9f, 0x9d, 0x45, 0x76, 0x7d, 0x63, 0xe8}
    };

    uint8_t K1[6][24] = {{0x70, 0x49, 0x6d, 0xb7, 0x7b, 0xb5, 0x88, 0x87, 0x2, 0xdb, 0x85, 0xc4, 0x5, 0xb0, 0x90, 0x70, 0x7, 0x53, 0xb5, 0xf5, 0xf, 0xf3, 0x24, 0x37},
    {0xfc, 0xe8, 0xa3, 0xf2, 0x77, 0xb6, 0x4b, 0x1f, 0x4, 0x45, 0xdc, 0x5c, 0x3, 0x8f, 0xf, 0xea, 0x7, 0x50, 0x46, 0xde, 0xe, 0xf0, 0x24, 0x70},
    {0x91, 0x67, 0x7e, 0x1b, 0x74, 0xae, 0x89, 0x1a, 0x3, 0x47, 0x66, 0x59, 0x0, 0x1f, 0xf7, 0xac, 0x7, 0x50, 0x3b, 0x49, 0x9, 0xf5, 0x24, 0xc6},
    {0xe6, 0x58, 0x9d, 0xa, 0x68, 0x4d, 0xb2, 0x27, 0x7, 0x7e, 0x4, 0x64, 0xb, 0xfd, 0x58, 0xdb, 0x7, 0x57, 0xdc, 0xf1, 0x5, 0xf2, 0x24, 0x3f},
    {0x10, 0xcb, 0xea, 0x5b, 0x7d, 0xe4, 0xfd, 0x6b, 0x2, 0xa4, 0x29, 0xbf, 0x4, 0x1a, 0xda, 0x56, 0x5, 0x53, 0x36, 0x5a, 0xb, 0xf7, 0x24, 0xe0},
    {0x8f, 0xd7, 0x7a, 0xd6, 0x6f, 0xac, 0xc2, 0xcc, 0x7, 0x6, 0xa9, 0x18, 0x0, 0xee, 0x12, 0xf1, 0x5, 0x56, 0xfe, 0xd3, 0x9, 0xf5, 0x24, 0x6}
    };
    
    uint8_t K2[6][24] = {{0x70, 0x41, 0x6d, 0xb7, 0x7b, 0xb5, 0x7d, 0x87, 0x2, 0xdb, 0x85, 0xc4, 0x5, 0xb0, 0x90, 0x70, 0x7, 0x53, 0xb5, 0xf5, 0xf, 0xf3, 0xd1, 0x37},
    {0xfc, 0xe0, 0xa3, 0xf2, 0x77, 0xb6, 0xbe, 0x1f, 0x4, 0x45, 0xdc, 0x5c, 0x3, 0x8f, 0xf, 0xea, 0x7, 0x50, 0x46, 0xde, 0xe, 0xf0, 0xd1, 0x70},
    {0x91, 0x6f, 0x7e, 0x1b, 0x74, 0xae, 0x7c, 0x1a, 0x3, 0x47, 0x66, 0x59, 0x0, 0x1f, 0xf7, 0xac, 0x7, 0x50, 0x3b, 0x49, 0x9, 0xf5, 0xd1, 0xc6},
    {0xe6, 0x50, 0x9d, 0xa, 0x68, 0x4d, 0x47, 0x27, 0x7, 0x7e, 0x4, 0x64, 0xb, 0xfd, 0x58, 0xdb, 0x7, 0x57, 0xdc, 0xf1, 0x5, 0xf2, 0xd1, 0x3f},
    {0x10, 0xc3, 0xea, 0x5b, 0x7d, 0xe4, 0x8, 0x6b, 0x2, 0xa4, 0x29, 0xbf, 0x4, 0x1a, 0xda, 0x56, 0x5, 0x53, 0x36, 0x5a, 0xb, 0xf7, 0xd1, 0xe0},
    {0x8f, 0xdf, 0x7a, 0xd6, 0x6f, 0xac, 0x37, 0xcc, 0x7, 0x6, 0xa9, 0x18, 0x0, 0xee, 0x12, 0xf1, 0x5, 0x56, 0xfe, 0xd3, 0x9, 0xf5, 0xd1, 0x6}
    };

 
    for(int k = 0; k < 6; k++) {
        uint8_t ttemp[16] = {0};
        uint8_t ttemp1[16] = {0};
        for(int i = 0; i < 16; i++){
            ttemp[i] = P1[k][i];
            ttemp1[i] = P2[k][i];
        }

        uint8_t tmp[24];
        for(int i = 0; i < 24; i++){
            tmp[i] = K1[k][i];
        }
        for(int i = 0; i < 16; i++) {
            printf("%02x, ", ttemp[i]);
        }printf("\n");
        AES192encrypt16(ttemp, tmp, 7);
        for(int i = 0; i < 24; i++) {
            printf("%02x, ", tmp[i]);
        }printf("\n"); 

        uint8_t tmp1[24];
        for(int i = 0; i < 24; i++){
            tmp1[i] = K2[k][i];
        }
        for(int i = 0; i < 16; i++) {
            printf("%02x, ", ttemp1[i]);
        }printf("\n");
        AES192encrypt16(ttemp1, tmp1, 7);
        for(int i = 0; i < 24; i++) {
            printf("%02x, ", tmp1[i]);
        }printf("\n"); 

        for(int i = 0; i < 24; i++) {
            printf("%02x, ", tmp1[i]^tmp[i]);
        }printf("\n"); 
        for(int i = 0; i < 16; i++) {
            printf("%02x, ", ttemp1[i]);
        }printf("\n");
        for(int i = 0; i < 16; i++) {
            printf("%02x, ", ttemp[i]);
        }printf("\n");

        for(int i = 0; i < 16; i++) {
            printf("%x, ", ttemp1[i] ^ ttemp[i]);
        }
        printf("\n\n\n"); 


    }


}


void main()
{
    printf("begin main\n");
    int t = gen_sbox_ddt();
    gen_sbox_diff_term();
    get_diff_nums();
    get_diff_values();
    get_diff_k_nums();
    get_diff_K_values();

    collision_search_192_7_semi_free();
    verify_collison();
    printf("end main\n");
}