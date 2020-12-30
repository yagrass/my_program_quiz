// 1 MMM = ?
// #define MMM 0x0080008000800080
// #define MMM 0x8000800080008000
// #define MMM 0x0808080808080808
#define MMM 0x8080808080808080
// #define MMM 0x8888888888888888
// #define MMM 0xFFFFFFFFFFFFFFFF

// Extension 2
#define IS_UPPER_CASE(ch) (ch>=0x41 && ch<=0x5A)
#define IS_LOWER_CASE(ch) (ch>=0x61 && ch<=0x7A)

// Extension 3 check text
#define IS_TEXT(ch) (ch>=20 && ch<=7E)

// 2 hexchar2val - MASK
// #define MASK 0x10
// #define MASK 0x20
#define MASK 0x40
// #define MASK 0x60
// #define MASK 0x80

// 2 hexchar2val - AAA
#define AAA 3
// #define AAA 2
// #define AAA 1
// #define AAA 0

// 2 hexchar2val - BBB
// #define BBB 7
#define BBB 6
// #define BBB 5
// #define BBB 4

// 2 hexchar2val 32bits MASK64
// #define MASK64    0x4040404040404040
#define MASK64  0x4040404040404040

// 3 quickmod
// #define XXX 0x00F000F000F00080
// #define XXX 0xF000F000F000F000
// #define XXX 0x0F0F0F0F0F0F0F0F
// #define XXX 0xF0F0F0F0F0F0F0F0
// #define XXX 0x8888888888888888
#define XXX 0xFFFFFFFFFFFFFFFF
