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
// File name     : EagleCryptSW.c                                               
// Time created  : Wed Sep 12 23:53:44 2018                                     
// Author        : dsijacic (dsijacic@esat.kuleuven.be)                         
// Details       :                                                              
//               :                                                              
// =============================================================================

extern "C" {
#include "KeccakSpongeWidth200.h"
#include "KeccakSponge-common.h"
#include "KetjeJr.h"
#include "Ketje-common.h"
};

#include "EagleCrypt.hpp"
#include <stdio.h>

KetjeJr_Instance ketjeJr;
KeccakWidth200_SpongeInstance sponge;

int
hash(
    const unsigned char*    data, 
    const unsigned int      dataByteLen, 
    unsigned char*          digest, 
    const unsigned int      digestByteLen
) {
    (void) data;
    (void) dataByteLen;
    int retValue;

    retValue = KeccakWidth200_SpongeInitialize(&sponge, RateSizeInBits, StateSizeInBits - RateSizeInBits);
    if (retValue != 0)
        return 1;
    retValue = KeccakWidth200_SpongeSqueeze(&sponge, digest, digestByteLen);

    return 0;
}

int init(
    const unsigned char*    key,
    const unsigned int      keySizeInBits,
    const unsigned char*    nonce,
    const unsigned int      nonceSizeInBits
) {
    return KetjeJr_Initialize(&ketjeJr, key, keySizeInBits, nonce, nonceSizeInBits);
}

int
encrypt(
    const unsigned char*    ad,
    const unsigned int      adByteLen,
    const unsigned char*    pt,
    const unsigned int      ptByteLen,
    unsigned char*          ct,
    unsigned char*          tag,
    const unsigned int      tagByteLen
) {
    int retValue;
    retValue = KetjeJr_FeedAssociatedData(&ketjeJr, ad, adByteLen);
    if (retValue != 0)
        return 1;
    retValue = KetjeJr_WrapPlaintext(&ketjeJr, pt, ct, ptByteLen);
    if (retValue != 0)
        return 1;
    retValue = KetjeJr_GetTag(&ketjeJr, tag, tagByteLen);

    return retValue;
}

int
decrypt(
    const unsigned char*    ad,
    const unsigned int      adByteLen,
    const unsigned char*    ct,
    const unsigned int      ctByteLen,
    unsigned char*          pt,
    unsigned char*          tag,
    const unsigned int      tagByteLen
) {
    int retValue;

    retValue = KetjeJr_FeedAssociatedData(&ketjeJr, ad, adByteLen);
    if (retValue != 0)
        return 1;
    retValue = KetjeJr_UnwrapCiphertext(&ketjeJr, ct, pt, ctByteLen);
    if (retValue != 0)
        return 1;
    retValue = KetjeJr_GetTag(&ketjeJr, tag, tagByteLen);

    return retValue;
}
