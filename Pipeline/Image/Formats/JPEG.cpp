#include "PipelinePch.h"
////////////////////////////////////////////////////////////////////////////////////////////////
//
//  jpeg.cpp
//
//  Adapted by: Rob Wyatt
//
//  Fast inline integer code to load a color jpeg file.
//
////////////////////////////////////////////////////////////////////////////////////////////////
#include "Platform/Types.h"
#include "Pipeline/Image/Image.h"
#include "JPEG.h"

using namespace Helium;

//-----------------------------------------------------------------------------
static const uint8_t JPGZig1[64] =
{
  0,0,1,2,1,0,0,1,2,3,4,3,2,1,0,0,1,2,3,4,5,6,5,4,3,2,1,0,0,1,2,3,
  4,5,6,7,7,6,5,4,3,2,1,2,3,4,5,6,7,7,6,5,4,3,4,5,6,7,7,6,5,6,7,7
};

//-----------------------------------------------------------------------------
static const uint8_t JPGZig2[64] =
{
  0,1,0,0,1,2,3,2,1,0,0,1,2,3,4,5,4,3,2,1,0,0,1,2,3,4,5,6,7,6,5,4,
  3,2,1,0,1,2,3,4,5,6,7,7,6,5,4,3,2,3,4,5,6,7,7,6,5,4,5,6,7,7,6,7
};

//-----------------------------------------------------------------------------
static const uint16_t aanscales[64] =
{
  // precomputed values scaled up by 14 bits
  16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
  22725, 31521, 29692, 26722, 22725, 17855, 12299,  6270,
  21407, 29692, 27969, 25172, 21407, 16819, 11585,  5906,
  19266, 26722, 25172, 22654, 19266, 15137, 10426,  5315,
  16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
  12873, 17855, 16819, 15137, 12873, 10114,  6967,  3552,
  8867, 12299, 11585, 10426,  8867,  6967,  4799,  2446,
  4520,  6270,  5906,  5315,  4520,  3552,  2446,  1247
};

//-----------------------------------------------------------------------------
static uint8_t *jpgdata = NULL;
static uint8_t *vram = NULL;
static uint32_t findex;
static uint16_t DCTables;
static uint16_t ACTables;
static uint16_t QTables;
static uint8_t curByte;
static uint8_t curBits;
static uint8_t EOI;
static struct JPGHuffmanEntry HuffmanDC0[256];
static struct JPGHuffmanEntry HuffmanDC1[256];
static struct JPGHuffmanEntry HuffmanAC0[256];
static struct JPGHuffmanEntry HuffmanAC1[256];
static uint16_t QuantTable[2][8][8];    // 2 quantization tables (Y, CbCr)
static struct JpegType JpegImage;

//-----------------------------------------------------------------------------
void JPGToRGB (int16_t y0, int16_t cb0, int16_t cr0, int16_t *r0, int16_t *g0, int16_t *b0);
int16_t JPGReceiveBits (uint16_t cat);

//-----------------------------------------------------------------------------
static inline uint32_t RGB32(uint32_t r, uint32_t g, uint32_t b)
{
  return (uint32_t) ((255<<24)+(r<<16)+(g<<8)+b);
}

//-----------------------------------------------------------------------------
static inline void gfxPixel (int16_t x, int16_t y, uint32_t pixel, uint8_t* vram)
{
  uint32_t *dst = (uint32_t *) vram;
  dst[(y*JpegImage.Cols)+x] = pixel;
}

//-----------------------------------------------------------------------------
static inline uint32_t JPGpower2(uint16_t pwr)
{
  uint8_t i;
  uint16_t total = 1;

  if (pwr>0xff)
    return 0;
  for (i=0; i<pwr; i++)
    total <<= 1;
  return (total);
}

//-----------------------------------------------------------------------------
static inline uint8_t JPGGetByte()
{
  return (jpgdata[findex++]);
}

//-----------------------------------------------------------------------------
static inline uint16_t JPGGetWord()
{
  uint16_t a = (uint16_t) (JPGGetByte()<<8);
  a = (uint16_t) (a+JPGGetByte());
  return a;
}

//-----------------------------------------------------------------------------
static inline uint8_t JPGNextBit (void)
{
  uint8_t NextBit;

  curBits >>= 1;
  NextBit = (uint8_t) ((curByte >> 7) & 1);
  curByte <<= 1;
  if (curBits == 0)
  {
    curBits = 128;
    curByte = JPGGetByte();
    if (curByte == 255)
    {
      if (JPGGetByte() == 0xD9)
      {
        EOI = 1;
        return(0);
      }
    }
  }
  return(NextBit);
}


//-----------------------------------------------------------------------------
static int16_t JPGDecode(struct JPGHuffmanEntry inArray[256])
{
  uint16_t n1; uint16_t n2; uint16_t i; uint16_t l;
  int32_t CurVal;
  int16_t MatchFound;

  if (JPGGetByte() == 255)
  {
    n1 = JPGGetByte();
    findex -= 2;
    if ((n1 >= 0xd0) && (n1 <= 0xd7))
    {
      n2 = (uint16_t) (curBits - 1);
      if ((curByte & n2) == n2)     // if the remaining bits are 1
      {
        EOI = 1;
        return(0);
      }
    }
  }
  else
    findex--;

  CurVal = 0;
  MatchFound = -1;
  for (l=1; l<16+1; l++)    // cycle through 16 possible Huffman lengths
  {
    CurVal = (CurVal << 1) + JPGNextBit();
    if (EOI) return(0);
    for (i=0; i<256; i++)              // look for a match in the Huffman table
    {
      if (inArray[i].Length > l) break;
      if (inArray[i].Length == l)
        if (inArray[i].Index == CurVal)
        {
          MatchFound = i;
          break;
        }
    }

    if (MatchFound > -1) break;
  }

  if (MatchFound == -1)
    return(-1);

  return(inArray[MatchFound].Code);  // return the appropriate code
}

//-----------------------------------------------------------------------------
static void jpeg_idct_ifast (int16_t inarray[8][8], int16_t outarray[8][8], uint16_t QuantNum)
{
  // The following variables only need 16bits precision
  // but the resulting code is smaller if you use int32_t.
  int32_t tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  int32_t tmp10, tmp11, tmp12, tmp13;
  int32_t z5, z10, z11, z12, z13;

  uint32_t ctr;
  int16_t warray[8][8];

  /* Pass 1: process columns from input, store into work array. */

  for (ctr = 0; ctr < 8; ctr++)
  {
    /* Due to quantization, we will usually find that many of the input
    * coefficients are zero, especially the AC terms.  We can exploit this
    * by short-circuiting the IDCT calculation for any column in which all
    * the AC terms are zero.  In that case each output is equal to the
    * DC coefficient (with scale factor as needed).
    * With typical images and quantization tables, half or more of the
    * column DCT calculations can be simplified this way.
    */

    if (inarray[1][ctr] == 0 && inarray[2][ctr] == 0 &&
      inarray[3][ctr] == 0 && inarray[4][ctr] == 0 &&
      inarray[5][ctr] == 0 && inarray[6][ctr] == 0 &&
      inarray[7][ctr] == 0)
    {
      /* AC terms all zero */
      int16_t dcval = (int16_t) (inarray[0][ctr] * QuantTable[QuantNum][0][ctr]);

      warray[0][ctr] = dcval;
      warray[1][ctr] = dcval;
      warray[2][ctr] = dcval;
      warray[3][ctr] = dcval;
      warray[4][ctr] = dcval;
      warray[5][ctr] = dcval;
      warray[6][ctr] = dcval;
      warray[7][ctr] = dcval;

      continue;
    }

    /* Even part */

    tmp0 = inarray[0][ctr] * QuantTable[QuantNum][0][ctr];
    tmp1 = inarray[2][ctr] * QuantTable[QuantNum][2][ctr];
    tmp2 = inarray[4][ctr] * QuantTable[QuantNum][4][ctr];
    tmp3 = inarray[6][ctr] * QuantTable[QuantNum][6][ctr];

    tmp10 = tmp0 + tmp2;	/* phase 3 */
    tmp11 = tmp0 - tmp2;

    tmp13 = tmp1 + tmp3;	/* phases 5-3 */
    tmp12 = (((tmp1 - tmp3)*( FIX_1_414213562)) >> 8) - tmp13; /* 2*c4 */

    tmp0 = tmp10 + tmp13;	/* phase 2 */
    tmp3 = tmp10 - tmp13;
    tmp1 = tmp11 + tmp12;
    tmp2 = tmp11 - tmp12;

    /* Odd part */

    tmp4 = inarray[1][ctr] * QuantTable[QuantNum][1][ctr];
    tmp5 = inarray[3][ctr] * QuantTable[QuantNum][3][ctr];
    tmp6 = inarray[5][ctr] * QuantTable[QuantNum][5][ctr];
    tmp7 = inarray[7][ctr] * QuantTable[QuantNum][7][ctr];

    z13 = tmp6 + tmp5;		/* phase 6 */
    z10 = tmp6 - tmp5;
    z11 = tmp4 + tmp7;
    z12 = tmp4 - tmp7;

    tmp7 = z11 + z13;		/* phase 5 */
    tmp11 = ((z11 - z13) * FIX_1_414213562) >> 8; /* 2*c4 */

    z5 = ((z10 + z12) * FIX_1_847759065) >> 8; /* 2*c2 */
    tmp10 = ((z12 * FIX_1_082392200) >> 8) - z5; /* 2*(c2-c6) */
    tmp12 = ((z10 * - FIX_2_613125930) >> 8) + z5; /* -2*(c2+c6) */

    tmp6 = tmp12 - tmp7;	/* phase 2 */
    tmp5 = tmp11 - tmp6;
    tmp4 = tmp10 + tmp5;

    warray[0][ctr] = (int16_t) (tmp0 + tmp7);
    warray[7][ctr] = (int16_t) (tmp0 - tmp7);
    warray[1][ctr] = (int16_t) (tmp1 + tmp6);
    warray[6][ctr] = (int16_t) (tmp1 - tmp6);
    warray[2][ctr] = (int16_t) (tmp2 + tmp5);
    warray[5][ctr] = (int16_t) (tmp2 - tmp5);
    warray[4][ctr] = (int16_t) (tmp3 + tmp4);
    warray[3][ctr] = (int16_t) (tmp3 - tmp4);
  }

  /* Pass 2: process rows from work array, store into output array. */
  /* Note that we must descale the results by a factor of 8 == 2**3, */
  /* and also undo the PASS1_BITS scaling. */

  for (ctr = 0; ctr < 8; ctr++)
  {
    /* Rows of zeroes can be exploited in the same way as we did with columns.
    * However, the column calculation has created many nonzero AC terms, so
    * the simplification applies less often (typically 5% to 10% of the time).
    * On machines with very fast multiplication, it's possible that the
    * test takes more time than it's worth.  In that case this section
    * may be commented out.
    */

    if (warray[ctr][1] == 0 && warray[ctr][2] == 0 && warray[ctr][3] == 0 && warray[ctr][4] == 0 &&
      warray[ctr][5] == 0 && warray[ctr][6] == 0 && warray[ctr][7] == 0)
    {
      /* AC terms all zero */
      int16_t dcval = (int16_t) ((warray[ctr][0] >> 5)+128);
      if (dcval<0) dcval = 0;
      if (dcval>255) dcval = 255;

      outarray[ctr][0] = dcval;
      outarray[ctr][1] = dcval;
      outarray[ctr][2] = dcval;
      outarray[ctr][3] = dcval;
      outarray[ctr][4] = dcval;
      outarray[ctr][5] = dcval;
      outarray[ctr][6] = dcval;
      outarray[ctr][7] = dcval;
      continue;
    }

    /* Even part */

    tmp10 = warray[ctr][0] + warray[ctr][4];
    tmp11 = warray[ctr][0] - warray[ctr][4];

    tmp13 = warray[ctr][2] + warray[ctr][6];
    tmp12 = (((warray[ctr][2] - warray[ctr][6]) * FIX_1_414213562) >> 8) - tmp13;

    tmp0 = tmp10 + tmp13;
    tmp3 = tmp10 - tmp13;
    tmp1 = tmp11 + tmp12;
    tmp2 = tmp11 - tmp12;

    /* Odd part */

    z13 = warray[ctr][5] + warray[ctr][3];
    z10 = warray[ctr][5] - warray[ctr][3];
    z11 = warray[ctr][1] + warray[ctr][7];
    z12 = warray[ctr][1] - warray[ctr][7];

    tmp7 = z11 + z13;		/* phase 5 */
    tmp11 = ((z11 - z13) * FIX_1_414213562) >> 8; /* 2*c4 */

    z5 = ((z10 + z12) * FIX_1_847759065) >> 8; /* 2*c2 */
    tmp10 = ((z12 * FIX_1_082392200) >> 8) - z5; /* 2*(c2-c6) */
    tmp12 = ((z10 * - FIX_2_613125930) >> 8) + z5; /* -2*(c2+c6) */

    tmp6 = tmp12 - tmp7;	/* phase 2 */
    tmp5 = tmp11 - tmp6;
    tmp4 = tmp10 + tmp5;

    /* Final output stage: scale down by a factor of 8 and range-limit */

    outarray[ctr][0] = (int16_t) (((tmp0 + tmp7) >> 5)+128);
    if ((outarray[ctr][0])<0)  outarray[ctr][0] = 0;
    if ((outarray[ctr][0])>255) outarray[ctr][0] = 255;

    outarray[ctr][7] = (int16_t) (((tmp0 - tmp7) >> 5)+128);
    if ((outarray[ctr][7])<0)  outarray[ctr][7] = 0;
    if ((outarray[ctr][7])>255) outarray[ctr][7] = 255;

    outarray[ctr][1] = (int16_t) (((tmp1 + tmp6) >> 5)+128);
    if ((outarray[ctr][1])<0)  outarray[ctr][1] = 0;
    if ((outarray[ctr][1])>255) outarray[ctr][1] = 255;

    outarray[ctr][6] = (int16_t) (((tmp1 - tmp6) >> 5)+128);
    if ((outarray[ctr][6])<0)  outarray[ctr][6] = 0;
    if ((outarray[ctr][6])>255) outarray[ctr][6] = 255;

    outarray[ctr][2] = (int16_t) (((tmp2 + tmp5) >> 5)+128);
    if ((outarray[ctr][2])<0)  outarray[ctr][2] = 0;
    if ((outarray[ctr][2])>255) outarray[ctr][2] = 255;

    outarray[ctr][5] = (int16_t) (((tmp2 - tmp5) >> 5)+128);
    if ((outarray[ctr][5])<0)  outarray[ctr][5] = 0;
    if ((outarray[ctr][5])>255) outarray[ctr][5] = 255;

    outarray[ctr][4] = (int16_t) (((tmp3 + tmp4) >> 5)+128);
    if ((outarray[ctr][4])<0)  outarray[ctr][4] = 0;
    if ((outarray[ctr][4])>255) outarray[ctr][4] = 255;

    outarray[ctr][3] = (int16_t) (((tmp3 - tmp4) >> 5)+128);
    if ((outarray[ctr][3])<0)  outarray[ctr][3] = 0;
    if ((outarray[ctr][3])>255) outarray[ctr][3] = 255;

  }
}

//-----------------------------------------------------------------------------
static void JPGGetBlock(int16_t vector[8][8], uint16_t HuffDCNum, uint16_t HuffACNum, uint16_t QuantNum, int16_t *dcCoef)
{
  int16_t array2[8][8];
  int32_t d; uint16_t XPos; uint16_t YPos;
  uint16_t bits; uint16_t zeros; int32_t bitVal; uint16_t ACCount;
  uint16_t x; uint16_t y;
  int16_t temp0;
  uint16_t ZigIndex;

  EOI = 0;

  for (x=0; x<8; x++)
    for (y=0; y<8; y++)
      array2[x][y] = 0;

  if (HuffDCNum)
    temp0 = JPGDecode(HuffmanDC1);   // Get the DC coefficient
  else
    temp0 = JPGDecode(HuffmanDC0);   // Get the DC coefficient

  *dcCoef = (int16_t) (*dcCoef + JPGReceiveBits(temp0));
  array2[0][0] = *dcCoef;//* Quant[QuantNum][0][0];

  XPos = 0; YPos = 0;
  ZigIndex = 1;
  ACCount = 1;
  do
  {
    if (HuffACNum)
      d = JPGDecode(HuffmanAC1);
    else
      d = JPGDecode(HuffmanAC0);

    zeros = (uint16_t) (d >> 4);
    bits = (uint16_t) (d & 15);
    bitVal = JPGReceiveBits(bits);

    if (bits)
    {
      ZigIndex = (uint16_t) (ZigIndex+zeros);
      ACCount = (uint16_t) (ACCount+zeros);
      if (ACCount >= 64) break;

      XPos = JPGZig1[ZigIndex];
      YPos = JPGZig2[ZigIndex];
      ZigIndex++;

      //Read(XPos, YPos);
      array2[XPos][YPos] = (int16_t) bitVal; // * Quant[QuantNum][XPos][YPos];
      ACCount++;
    }
    else
    {
      if (zeros != 15) break;
      ZigIndex += 15;
      ACCount += 16;
    }

  }
  while (ACCount < 64);

  jpeg_idct_ifast (array2, vector, QuantNum);

}

//-----------------------------------------------------------------------------
static uint16_t JPGGetHuffTables (void)
{
  uint16_t HuffAmount[17]; //1-16
  uint32_t l0;
  uint16_t c0;
  uint16_t temp0;
  int16_t temp1;
  uint16_t total;
  uint16_t i;
  uint16_t t0;
  int32_t CurNum;
  uint16_t CurIndex;
  uint16_t j;

  l0 = JPGGetWord();
  c0 = 2;
  do
  {
    temp0 = JPGGetByte();
    c0++;
    t0 = (uint16_t) ((temp0 & 16) >> 4);
    temp0 &= 15;
    switch (t0)
    {
    case 0:        // DC Table
      total = 0;
      for (i=1; i<16+1; i++)
      {
        temp1 = JPGGetByte();
        c0++;
        total = (int16_t) (total+temp1);
        HuffAmount[i] = temp1;
      }
      for (i=0; i<total; i++)
      {
        if (temp0)
          HuffmanDC1[i].Code = JPGGetByte();
        else
          HuffmanDC0[i].Code = JPGGetByte();
        c0++;
      }
      CurNum = 0;
      CurIndex = (uint16_t) -1;
      for (i=1; i<16+1; i++)
      {
        for (j=1; j<HuffAmount[i]+1; j++)
        {
          CurIndex++;
          if (temp0)
          {
            HuffmanDC1[CurIndex].Index = (uint16_t) CurNum;
            HuffmanDC1[CurIndex].Length = (uint16_t) i;
          }
          else
          {
            HuffmanDC0[CurIndex].Index = (uint16_t) CurNum;
            HuffmanDC0[CurIndex].Length = (uint16_t) i;
          }
          CurNum++;
        }
        CurNum *= 2;
      }
      DCTables++;
      break;
    case 1:
      total = 0;
      for (i=1; i<16+1; i++)
      {
        temp1 = JPGGetByte();
        c0++;
        total = (uint16_t) (total+temp1);
        HuffAmount[i] = temp1;
      }
      for (i=0; i<total; i++)
      {
        if (temp0)
          HuffmanAC1[i].Code = JPGGetByte();
        else
          HuffmanAC0[i].Code = JPGGetByte();
        c0++;
      }

      CurNum = 0;
      CurIndex = (uint16_t) -1;
      for (i=1; i<16+1; i++)
      {
        for (j=1; j<HuffAmount[i]+1; j++)
        {
          CurIndex++;
          if (temp0)
          {
            HuffmanAC1[CurIndex].Index = (uint16_t) CurNum;
            HuffmanAC1[CurIndex].Length = (uint16_t) i;
          }
          else
          {
            HuffmanAC0[CurIndex].Index = (uint16_t) CurNum;
            HuffmanAC0[CurIndex].Length = (uint16_t) i;
          }
          CurNum++;
        }
        CurNum *= 2;
      }
      ACTables++;
      break;
    }
  }
  while (c0 < l0);

  return(1);
}

//-----------------------------------------------------------------------------
static uint16_t JPGGetImageAttr (void)
{
  uint32_t temp4;
  uint16_t temp0;
  uint16_t temp1;
  uint16_t i;
  uint16_t id;

  temp4 = JPGGetWord();          //Length of segment
  temp0 = JPGGetByte();          // Data precision
  if (temp0 != 8)
    return(0);                  // we do not support 12 or 16-bit samples

  JpegImage.Rows = JPGGetWord();
  JpegImage.Cols = JPGGetWord();

  temp0 = JPGGetByte();          // Number of components
  for (i=1; i<temp0+1; i++)
  {
    id = JPGGetByte();
    switch (id)
    {
    case 1:
      temp1 = JPGGetByte();
      JpegImage.SamplesY = (uint16_t) ((temp1 & 15) * (temp1 >> 4));
      JpegImage.QuantTableY = JPGGetByte();
      break;
    case 2:
    case 3:
      temp1 = JPGGetByte();
      JpegImage.SamplesCbCr = (uint16_t) ((temp1 & 15) * (temp1 >> 4));
      JpegImage.QuantTableCbCr = JPGGetByte();
      break;
    }
  }

  return(1);
}

//-----------------------------------------------------------------------------
static uint8_t JPGGetQuantTables(void)
{
  uint32_t l0 = JPGGetWord();
  uint16_t c0 = 2;
  uint16_t temp0;
  uint16_t xp;
  uint16_t yp;
  uint16_t i;
  uint16_t ZigIndex;

  do
  {
    temp0 = JPGGetByte();
    c0++;
    if (temp0 & 0xf0)
      return(0);        //we don't support 16-bit tables

    temp0 &= 15;
    ZigIndex = 0;
    xp = 0;
    yp = 0;
    for (i=0; i<64; i++)
    {
      xp = JPGZig1[ZigIndex];
      yp = JPGZig2[ZigIndex];
      ZigIndex++;
      /* For AA&N IDCT method, multipliers are equal to quantization
      * coefficients scaled by scalefactor[row]*scalefactor[col], where
      *   scalefactor[0] = 1
      *   scalefactor[k] = cos(k*PI/16) * sqrt(2)    for k=1..7
      */
      QuantTable[temp0][xp][yp] = (uint16_t) ((JPGGetByte() * aanscales[(xp<<3) + yp]) >> 12);
      c0++;
    }
    QTables++;
  }
  while (c0 < l0);

  return(1);
}

//-----------------------------------------------------------------------------
static uint16_t JPGGetSOS (void)
{
  uint32_t temp4;
  uint16_t temp0;
  uint16_t temp1;
  uint16_t temp2;
  uint16_t i;

  temp4 = JPGGetWord();
  temp0 = JPGGetByte();

  if ((temp0 != 1) && (temp0 != 3))
    return(0);
  JpegImage.NumComp = temp0;
  for (i=1; i<temp0+1; i++)
  {
    temp1 = JPGGetByte();
    switch (temp1)
    {
    case 1:
      temp2 = JPGGetByte();
      JpegImage.HuffACTableY = (uint16_t) (temp2 & 15);
      JpegImage.HuffDCTableY = (uint16_t) (temp2 >> 4);
      break;
    case 2:
      temp2 = JPGGetByte();
      JpegImage.HuffACTableCbCr = (uint16_t) (temp2 & 15);
      JpegImage.HuffDCTableCbCr = (uint16_t) (temp2 >> 4);
      break;
    case 3:
      temp2 = JPGGetByte();
      JpegImage.HuffACTableCbCr = (uint16_t) (temp2 & 15);
      JpegImage.HuffDCTableCbCr = (uint16_t) (temp2 >> 4);
      break;
    default:
      return(0);
    }
  }
  findex += 3;
  return(1);
}

//-----------------------------------------------------------------------------
static inline int16_t JPGReceiveBits(uint16_t cat)
{
  uint32_t temp0 = 0;
  uint16_t i;
  int32_t ReceiveBits;

  for (i=0; i<cat; i++)
    temp0 = temp0 * 2 + JPGNextBit();
  if ((temp0*2) >= (JPGpower2(cat)) )
    ReceiveBits = temp0;
  else
    ReceiveBits = (int32_t) (-(int32_t) (JPGpower2(cat) - 1) + temp0);
  return (int16_t) ReceiveBits;
}

//-----------------------------------------------------------------------------
static inline void JPGToRGB (int16_t y0, int16_t cb0, int16_t cr0, int16_t *r0, int16_t *g0, int16_t *b0)
{
  // Do color space conversion from YCbCr to RGB
  *r0 = (int16_t) ((y0) + (((cr0-128) * 45) >> 5));
  *g0 = (int16_t) ((y0) - (((cb0-128) * 11) >> 5) - (((cr0-128) * 23) >> 5));
  *b0 = (int16_t) ((y0) + (((cb0-128) * 57) >> 5));
  if (*r0 > 255) *r0 = 255;
  if (*r0 < 0) *r0 = 0;
  if (*g0 > 255) *g0 = 255;
  if (*g0 < 0) *g0 = 0;
  if (*b0 > 255) *b0 = 255;
  if (*b0 < 0) *b0 = 0;
}

//-----------------------------------------------------------------------------
Image* Image::LoadJPG(const void *src, bool convert_to_linear)
{
  uint8_t x0 = 0;
  uint8_t y0 = 0;
  uint8_t exit = 1;
  int16_t y;
  uint32_t size = 0;
  uint16_t Restart = 0;
  uint16_t XPos; uint16_t YPos;
  int16_t dcY; int16_t dcCb; int16_t dcCr;
  uint16_t xindex; uint16_t yindex;
  uint16_t mcu;
  int16_t YVector1[8][8];              // 4 vectors for Y attribute
  int16_t YVector2[8][8];              // (not all may be needed)
  int16_t YVector3[8][8];
  int16_t YVector4[8][8];
  int16_t CbVector[8][8];              // 1 vector for Cb attribute
  int16_t CrVector[8][8];              // 1 vector for Cr attribute

  uint16_t i,j;
  uint16_t i2; uint16_t j2;
  int16_t cb; int16_t cr;
  uint16_t xj;
  uint16_t yi;
  int16_t r; int16_t g; int16_t b;

  jpgdata = (uint8_t*)src;

  QTables = 0;     // Initialize some checkpoint variables
  ACTables = 0;
  DCTables = 0;

  findex = 0;

  if (JPGGetByte() == 0xff)
  {
    if (JPGGetByte() == 0xd8)
      exit = 0;
  }

  // not a JPEG of the correct type
  if (exit)
    return 0;

  while (!exit)
  {
    if (JPGGetByte() == 0xff)
    {
      switch (JPGGetByte())
      {
      case 0x00: //not important
        break;
      case 0xc0: //SOF0
        JPGGetImageAttr();
        break;
      case 0xc1: //SOF1
        JPGGetImageAttr();
        break;
      case 0xc4: //DHT
        if ((ACTables < 2) || (DCTables < 2))
          JPGGetHuffTables();
        break;
      case 0xc9: //SOF9
        break;
      case 0xd9: //EOI
        exit = 1;
        break;
      case 0xda: //SOS
        JPGGetSOS();
        if ( ((DCTables == 2) &&
          (ACTables == 2) &&
          (QTables == 2)) ||
          (JpegImage.NumComp == 1) )
        {
          EOI = 0;
          exit = 1;        // Go on to secondary control loop
        }
        break;
      case 0xdb: //DQT
        if (QTables < 2)
          JPGGetQuantTables();
        break;
      case 0xdd: //DRI
        Restart = JPGGetWord();
        break;
      case 0xe0: //APP0
        (void) JPGGetWord();        // Length of segment
        findex += 5;
        (void) JPGGetByte();        // Major revision
        (void) JPGGetByte();        // Minor revision
        (void) JPGGetByte();        // Density definition
        (void) JPGGetByte();        // X density
        (void) JPGGetByte();        // Y density
        (void) JPGGetByte();        // Thumbnail width
        (void) JPGGetByte();        // Thumbnail height
        break;
      case 0xfe: //COM
        break;
      }
    }
  }

  XPos = 0;
  YPos = 0;                            // Initialize active variables
  dcY = 0; dcCb = 0; dcCr = 0;
  xindex = 0; yindex = 0; mcu = 0;
  r = 0; g = 0; b = 0;

  size      = JpegImage.Cols*JpegImage.Rows*sizeof(uint32_t);
  uint8_t* vram  = new uint8_t[size];

  curBits = 128;                // Start with the seventh bit
  curByte = JPGGetByte();       // Of the first byte

  switch (JpegImage.NumComp)        // How many components does the image have?
  {
  case 3:                     // 3 components (Y-Cb-Cr)
    {
      switch (JpegImage.SamplesY)   // What's the sampling ratio of Y to CbCr?
      {
      case 4:                 // 4 pixels to 1

        do                    // Process 16x16 blocks of pixels
        {
          JPGGetBlock (YVector1, JpegImage.HuffDCTableY,    JpegImage.HuffACTableY,    JpegImage.QuantTableY,    &dcY);
          JPGGetBlock (YVector2, JpegImage.HuffDCTableY,    JpegImage.HuffACTableY,    JpegImage.QuantTableY,    &dcY);
          JPGGetBlock (YVector3, JpegImage.HuffDCTableY,    JpegImage.HuffACTableY,    JpegImage.QuantTableY,    &dcY);
          JPGGetBlock (YVector4, JpegImage.HuffDCTableY,    JpegImage.HuffACTableY,    JpegImage.QuantTableY,    &dcY);
          JPGGetBlock (CbVector, JpegImage.HuffDCTableCbCr, JpegImage.HuffACTableCbCr, JpegImage.QuantTableCbCr, &dcCb);
          JPGGetBlock (CrVector, JpegImage.HuffDCTableCbCr, JpegImage.HuffACTableCbCr, JpegImage.QuantTableCbCr, &dcCr);
          // YCbCr vectors have been obtained

          for (i=0; i<8; i++)  // Draw top left 8x8 pixels
            for (j=0; j<8; j++)
            {
              y = YVector1[i][j];
              i2 = (uint16_t) (i >> 1);
              j2 = (uint16_t) (j >> 1);
              cb = CbVector[i2][j2];
              cr = CrVector[i2][j2];
              JPGToRGB (y, cb, cr, &r, &g, &b);
              xj = (uint16_t) (xindex + j);
              yi = (uint16_t) (yindex + i);
              if ( (xj < JpegImage.Cols) && (yi < JpegImage.Rows) )
                gfxPixel ((int16_t) (xj + x0), (int16_t) (yi + y0), (uint32_t) RGB32(r, g, b), vram);
            }
            for (i=0; i<8; i++)  // Draw top right 8x8 pixels
              for (j=8; j<16; j++)
              {
                y = YVector2[i][j - 8];
                i2 = (uint16_t) (i >> 1);
                j2 = (uint16_t) (j >> 1);
                cb = CbVector[i2][j2];
                cr = CrVector[i2][j2];
                JPGToRGB (y, cb, cr, &r, &g, &b);
                xj = (uint16_t) (xindex + j);
                yi = (uint16_t) (yindex + i);
                if ((xj < JpegImage.Cols) && (yi < JpegImage.Rows))
                  gfxPixel ((int16_t) (xj + x0), (int16_t) (yi + y0), (uint32_t) RGB32(r, g, b), vram);
              }
              for (i=8; i<16; i++)  // Draw bottom left 8x8 pixels
                for (j=0; j<8; j++)
                {
                  y = YVector3[i - 8][j];
                  i2 = (uint16_t) (i >> 1);
                  j2 = (uint16_t) (j >> 1);
                  cb = CbVector[i2][j2];
                  cr = CrVector[i2][j2];
                  JPGToRGB (y, cb, cr, &r, &g, &b);
                  xj = (uint16_t) (xindex + j);
                  yi = (uint16_t) (yindex + i);
                  if ((xj < JpegImage.Cols) && (yi < JpegImage.Rows))
                    gfxPixel ((int16_t) (xj + x0), (int16_t) (yi + y0), (uint32_t) RGB32(r, g, b), vram);
                }
                for (i=8; i<16; i++)          // Draw bottom right 8x8 pixels
                  for (j=8; j<16; j++)
                  {
                    y = YVector4[i - 8][j - 8];
                    i2 = (uint16_t) (i >> 1);
                    j2 = (uint16_t) (j >> 1);
                    cb = CbVector[i2][j2];
                    cr = CrVector[i2][j2];
                    JPGToRGB (y, cb, cr, &r, &g, &b);
                    xj = (uint16_t) (xindex + j);
                    yi = (uint16_t) (yindex + i);
                    if ((xj < JpegImage.Cols) && (yi < JpegImage.Rows))
                      gfxPixel ((int16_t) (xj + x0), (int16_t) (yi + y0), (uint32_t) RGB32(r, g, b), vram);
                  }
                  xindex += 16;
                  if (xindex >= JpegImage.Cols)
                  {
                    xindex = 0; yindex += 16; mcu = 1;
                  }
                  if ((mcu == 1) && (Restart != 0))    //Execute the restart interval
                  {
                    curByte = JPGGetByte(); curByte = JPGGetByte(); curByte = JPGGetByte();
                    curBits = 128;
                    dcY = 0; dcCb = 0; dcCr = 0; mcu = 0;  //Reset the DC value
                  }
        }
        //           while ((findex < flen) && (yindex < Image.Rows));
        while (yindex < JpegImage.Rows);
        break;
      case 2:           // 2 pixels to 1
        do
        {
          JPGGetBlock (YVector1, JpegImage.HuffDCTableY,    JpegImage.HuffACTableY,    JpegImage.QuantTableY,    &dcY);
          JPGGetBlock (YVector2, JpegImage.HuffDCTableY,    JpegImage.HuffACTableY,    JpegImage.QuantTableY,    &dcY);
          JPGGetBlock (CbVector, JpegImage.HuffDCTableCbCr, JpegImage.HuffACTableCbCr, JpegImage.QuantTableCbCr, &dcCb);
          JPGGetBlock (CrVector, JpegImage.HuffDCTableCbCr, JpegImage.HuffACTableCbCr, JpegImage.QuantTableCbCr, &dcCr);
          // YCbCr vectors have been obtained

          for (i=0; i<8; i++)       // Draw left 8x8 pixels
            for (j=0; j<8; j++)
            {
              y = YVector1[i][j];
              i2 = (uint16_t) (i >> 1);
              j2 = (uint16_t) (j >> 1);
              cb = CbVector[i2][j2];
              cr = CrVector[i2][j2];
              JPGToRGB (y, cb, cr, &r, &g, &b);
              xj = (uint16_t) (xindex + j);
              yi = (uint16_t) (yindex + i);
              if ((xj < JpegImage.Cols) && (yi < JpegImage.Rows))
                gfxPixel ((int16_t) (xj + x0), (int16_t) (yi + y0), (uint32_t) RGB32(r, g, b), vram);
            }

            for (i=0; i<8; i++)       // Draw right 8x8 pixels
              for (j=8; j<16; j++)
              {
                y = YVector2[i][j - 8];
                i2 = (uint16_t) (i >> 1);
                j2 = (uint16_t) (j >> 1);
                cb = CbVector[i2][j2];
                cr = CrVector[i2][j2];
                JPGToRGB (y, cb, cr, &r, &g, &b);
                xj = (uint16_t) (xindex + j);
                yi = (uint16_t) (yindex + i);
                if ((xj < JpegImage.Cols) && (yi < JpegImage.Rows))
                  gfxPixel ((int16_t) (xj + x0), (int16_t)(yi + y0), (uint32_t) RGB32(r, g, b), vram);
              }
              xindex += 16;
              if (xindex >= JpegImage.Cols)
              {
                xindex = 0; yindex += 8; mcu = 1;
              }
              if ((mcu == 1) && (Restart != 0))  // execute the restart interval
              {
                curByte = JPGGetByte(); curByte = JPGGetByte(); curByte = JPGGetByte();
                curBits = 128;
                dcY = 0; dcCb = 0; dcCr = 0; mcu = 0;
              }
        }
        while (yindex < JpegImage.Rows);
        break;
      case 1:        // 1 pixel to 1
        do
        {
          JPGGetBlock (YVector1, JpegImage.HuffDCTableY,    JpegImage.HuffACTableY,    JpegImage.QuantTableY,    &dcY);
          JPGGetBlock (CbVector, JpegImage.HuffDCTableCbCr, JpegImage.HuffACTableCbCr, JpegImage.QuantTableCbCr, &dcCb);
          JPGGetBlock (CrVector, JpegImage.HuffDCTableCbCr, JpegImage.HuffACTableCbCr, JpegImage.QuantTableCbCr, &dcCr);
          // YCbCr vectors have been obtained

          for (i=0; i<8; i++)            // Draw 8x8 pixels
            for (j=0; j<8; j++)
            {
              y = YVector1[i][j];
              i2 = (uint16_t) (i >> 1);
              j2 = (uint16_t) (j >> 1);
              cb = CbVector[i2][j2];
              cr = CrVector[i2][j2];
              JPGToRGB (y, cb, cr, &r, &g, &b);
              xj = (uint16_t) (xindex + j);
              yi = (uint16_t) (yindex + i);
              if ((xj < JpegImage.Cols) && (yi < JpegImage.Rows))
                gfxPixel ((int16_t) (xj + x0), (int16_t) (yi + y0), (uint32_t) RGB32(r, g, b), vram);
            }

            xindex += 8;
            if (xindex >= JpegImage.Cols)
            {
              xindex = 0; yindex += 8; mcu = 1;
            }
            if ((mcu == 1) && (Restart != 0))  // execute the restart interval
            {
              curByte = JPGGetByte(); curByte = JPGGetByte(); curByte = JPGGetByte();
              curBits = 128;
              dcY = 0; dcCb = 0; dcCr = 0; mcu = 0;
            }
        }
        while (yindex < JpegImage.Rows);
        break;
      }  // Ratio
    }
  case 1:
    do
    {
      JPGGetBlock (YVector1, JpegImage.HuffDCTableY, JpegImage.HuffACTableY, JpegImage.QuantTableY, &dcY);
      // Y vector has been obtained

      for (i=0; i<8; i++)           // Draw 8x8 pixels
        for (j=0; j<8; j++)
        {
          y = YVector1[i][j];
          if (y < 0) y = 0;
          if (y > 255) y = 255;
          xj = (uint16_t) (xindex + j);
          yi = (uint16_t) (yindex + i);
          if ((xj < JpegImage.Cols) && (yi < JpegImage.Rows))
            gfxPixel ((int16_t) (xj + x0), (int16_t) (yi + y0), (uint32_t) RGB32(y, y, y), vram);
        }
        xindex += 8;
        if (xindex >= JpegImage.Cols)
        {
          xindex = 0; yindex += 8; mcu = 1;
        }

        if ((mcu == 1) && (Restart != 0))   // execute the restart interval
        {
          curByte = JPGGetByte(); curByte = JPGGetByte(); curByte = JPGGetByte();
          curBits = 128;
          dcY = 0; mcu = 0;
        }
    }
    while (yindex < JpegImage.Rows);
    break;
  }

  Image* result = new Image(JpegImage.Cols,JpegImage.Rows,CF_ARGB8888);
  result->FillFaceData(0, CF_ARGB8888, vram);

  //Clean up
  delete[] vram;

  if(convert_to_linear)
  {
    result->ConvertSrgbToLinear();
  }

  return result;
}
