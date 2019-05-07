// =============================================================================
//                     This confidential and proprietary code                   
//                       may be used only as authorized by a                    
//                         licensing agreement from                             
//                     KU Leuven, ESAT Department, COSIC Group                  
//                    https://securewww.esat.kuleuven.be/cosic/                 
//                        _____  ____    ____   ____  _____                     
//                       / ___/ / __ \  / __/  /  _/ / ___/                     
//                      / /__  / /_/ / _\ \   _/ /  / /__                       
//                      \___/  \____/ /___/  /___/  \___/                       
//                                                                              
//                              ALL RIGHTS RESERVED                             
//        The entire notice above must be reproduced on all authorized copies.  
// =============================================================================
// File name     : EagleCrypt.h                                                 
// Time created  : Wed Sep 12 23:53:47 2018                                     
// Author        : dsijacic (dsijacic@esat.kuleuven.be)                         
// Details       :                                                              
//               :                                                              
// =============================================================================

#ifdef __cplusplus
extern "C" {
#endif

#define RateSizeInBits          16
#define RateSizeInBytes         (RateSizeInBits / 8)
#define StateSizeInBits         200

/**
  * Hashing based on Keccak200 permutation.
  *
  * @param  data                Pointer to the data buffer.
  * @param  dataByteLen         The size of the data in bytes.
  * @param  digest              Pointer to the digest buffer of at least 
  *                             DigestSizeInBytes must not overlap with 
  *                             @param data.
  * @param  digestByteLen       The size of the digest in bytes.
  *
  * @return 0 if successful, 1 otherwise.
  */
int
hash(
    const unsigned char*    data, 
    const unsigned int      dataByteLen, 
    unsigned char*          digest, 
    const unsigned int      digestByteLen
);

/**
  * KetjeJr AEAD scheme, encrypt.
  *
  * @param key              Pointer to the key buffer
  * @param keySizeInBits    Bit size of the key
  * @param nonce            Pointer to the nonce buffer
  * @param nonceSizeInBits  Bit size of the nonce
  *
  @return 0 if successful, 1 otherwise
  */
int init(
    const unsigned char*    key,
    const unsigned int      keySizeInBits,
    const unsigned char*    nonce,
    const unsigned int      nonceSizeInBits
);

/**
  * KetjeJr AEAD scheme, encrypt.
  *
  * @param ad               Pointer to the associated data buffer
  * @param adByteLen        Byte size of the associated data
  * @param pt               Pointer to the plaintext buffer
  * @param ptByteLen        Byte size of the plaintext
  * @param ct               Pointer to the ciphertext buffer
  * @param tag              Pointer to the tag buffer
  * @param tagByteLen       Byte size of the tag
  *
  @return 0 if successful, 1 otherwise
  */
int
encrypt(
    const unsigned char*    ad,
    const unsigned int      adByteLen,
    const unsigned char*    pt,
    const unsigned int      ptByteLen,
    unsigned char*          ct,
    unsigned char*          tag,
    const unsigned int      tagByteLen
);

/**
  * KetjeJr AEAD scheme, decrypt.
  *
  * @param ad               Pointer to the associated data buffer
  * @param adByteLen        Byte size of the associated data
  * @param ct               Pointer to the ciphertext buffer
  * @param ctByteLen        Byte size of the ciphertext
  * @param pt               Pointer to the plaintext buffer
  * @param tag              Pointer to the tag buffer
  * @param tagByteLen       Byte size of the tag
  *
  @return 0 if successful, 1 otherwise
  */
int
decrypt(
    const unsigned char*    ad,
    const unsigned int      adByteLen,
    const unsigned char*    ct,
    const unsigned int      ctByteLen,
    unsigned char*          pt,
    unsigned char*          tag,
    const unsigned int      tagByteLen
);

#ifdef __cplusplus
}
#endif