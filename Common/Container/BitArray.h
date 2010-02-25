#pragma once

#include "../Types.h"
#include "../Assert.h"

//
// A one-dimensional array of bits, similar to STL bitset.
//
// Copyright 2000 Andrew Kirmse.  All rights reserved.
//
// Permission is granted to use this code for any purpose, as long as this
// copyright message remains intact.
//

class BitArray
{
public:

  //
  // Constructors and destructor
  //

  explicit BitArray(unsigned size)
  {
    Init(size);

    // Clear last bits
    Trim();
  }

  BitArray(const BitArray &that)
  {
    mpStore = 0;
    *this = that;
  }

  virtual ~BitArray()
  {
    if (mLength > 1)
      delete[] mpStore;
  }

  //
  // Operators
  //

  class BitProxy;

  BitArray &operator=(const BitArray &that)
  {
    if (this != &that)
    {
      if (mLength > 1)
        delete[] mpStore;

      Init(that.mNumBits);

      memcpy(mpStore, that.mpStore, mLength * sizeof(store_type));
    }
    return *this;
  }

  BitProxy operator[](unsigned pos)
  {
    NOC_ASSERT(pos < mNumBits);
    return BitProxy(*this, pos);
  }

  const BitProxy operator[](unsigned pos) const
  {
    NOC_ASSERT(pos < mNumBits);
    return BitProxy(const_cast<BitArray &>(*this), pos);
  }

  bool operator==(const BitArray &that) const
  {
    if (mNumBits != that.mNumBits)
      return false;

    for (unsigned i = 0; i < mLength; i++)
      if (mpStore[i] != that.mpStore[i])
        return false;
    return true;
  }

  bool operator!=(const BitArray &that) const
  {
    return !(*this == that);
  }

  BitArray &operator&=(const BitArray &that)
  {
    NOC_ASSERT(mNumBits == that.mNumBits);
    for (unsigned i = 0; i < mLength; i++)
      mpStore[i] &= that.mpStore[i];
    return *this;
  }

  BitArray operator|=(const BitArray &that)
  {
    NOC_ASSERT(mNumBits == that.mNumBits);
    for (unsigned i = 0; i < mLength; i++)
      mpStore[i] |= that.mpStore[i];
    return *this;
  }

  BitArray operator^=(const BitArray &that)
  {
    NOC_ASSERT(mNumBits == that.mNumBits);
    for (unsigned i = 0; i < mLength; i++)
      mpStore[i] ^= that.mpStore[i];
    return *this;
  }

  BitArray operator~() const
  {
    return BitArray(*this).FlipAllBits();
  }

  friend BitArray operator&(const BitArray &a1, const BitArray &a2)
  {
    return BitArray(a1) &= a2;
  }

  friend BitArray operator|(const BitArray &a1, const BitArray &a2)
  {
    return BitArray(a1) |= a2;
  }

  friend BitArray operator^(const BitArray &a1, const BitArray &a2)
  {
    return BitArray(a1) ^= a2;
  }

  //
  // Plain English interface
  //

  // Set all bits to false.
  void Clear()
  {
    memset(mpStore, 0, mLength * sizeof(store_type));
  }

  // Set the bit at position pos to true.
  void SetBit(unsigned pos)
  {
    NOC_ASSERT(pos < mNumBits);
    mpStore[GetIndex(pos)] |= 1 << GetOffset(pos); 
  }

  // Set the bit at position pos to false.
  void ClearBit(unsigned pos)
  { 
    NOC_ASSERT(pos < mNumBits);
    mpStore[GetIndex(pos)] &= ~(1 << GetOffset(pos)); 
  }

  // Toggle the bit at position pos.
  void FlipBit(unsigned pos) 
  { 
    NOC_ASSERT(pos < mNumBits);
    mpStore[GetIndex(pos)] ^= 1 << GetOffset(pos); 
  }

  // Set the bit at position pos to the given value.
  void Set(unsigned pos, bool val)
  { 
    val ? SetBit(pos) : ClearBit(pos);
  }

  // Returns true iff the bit at position pos is true.
  bool IsBitSet(unsigned pos) const
  {
    NOC_ASSERT(pos < mNumBits);
    return (mpStore[GetIndex(pos)] & (1 << GetOffset(pos))) != 0;
  }

  // Returns true iff all bits are false.
  bool AllBitsFalse() const
  {
    for (unsigned i=0; i < mLength; i++)
      if (mpStore[i] != 0)
        return false;
    return true;
  }

  // Change value of all bits
  BitArray &FlipAllBits()
  {
    for (unsigned i=0; i < mLength; i++)
      mpStore[i] = ~mpStore[i];

    Trim();
    return *this;
  }

  //
  // Bit proxy (for operator[])
  //

  friend class BitProxy;

  class BitProxy
  {
  public:
    BitProxy(BitArray &array, unsigned pos):
        mArray(array), mPos(pos)
        {}

        BitProxy &operator=(bool value)
        {
          mArray.Set(mPos, value);
          return *this;
        }

        BitProxy &operator=(const BitProxy &that)
        {
          mArray.Set(mPos, that.mArray.IsBitSet(that.mPos));
          return *this;
        }

        operator bool() const
        {
          return mArray.IsBitSet(mPos);
        }

        bool Flip()
        {
          mArray.FlipBit(mPos);
          return mArray.IsBitSet(mPos);
        }

  private:
    BitArray &mArray;
    unsigned  mPos;
  };

public:

  typedef u32 store_type;

  enum
  {
    bits_per_byte = 8,
    cell_size     = sizeof(store_type) * bits_per_byte
  };


  store_type        *mpStore;  
  store_type         mSingleWord; // Use this buffer when mLength is 1
  u32           mLength;     // Length of mpStore in units of store_type
  u32           mNumBits;

  private:
    
  // Get the index and bit offset for a given bit number.
  static u32 GetIndex(unsigned bit_num)
  {
    return bit_num / cell_size;
  }

  static u32 GetOffset(unsigned bit_num)
  {
    return bit_num % cell_size;
  }

  void Init(u32 size)
  {
    mNumBits = size;

    if (size == 0)
      mLength = 0;
    else
      mLength = 1 + GetIndex(size - 1);

    // Avoid allocation if length is 1 (common case)
    if (mLength <= 1)
      mpStore = &mSingleWord;      
    else
      mpStore = new store_type[mLength];

    Clear();
  }

  // Force overhang bits at the end to 0
  inline void Trim()
  {
    unsigned extra_bits = mNumBits % cell_size;
    if (mLength > 0 && extra_bits != 0)
      mpStore[mLength - 1] &= ~((~(store_type) 0) << extra_bits);
  }
};