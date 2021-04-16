/*
   Copyright 2016 Technical University of Denmark, DTU Compute. 
   All rights reserved.
   
   This file is part of the time-predictable VLIW processor Patmos.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

      1. Redistributions of source code must retain the above copyright notice,
         this list of conditions and the following disclaimer.

      2. Redistributions in binary form must reproduce the above copyright
         notice, this list of conditions and the following disclaimer in the
         documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ``AS IS'' AND ANY EXPRESS
   OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
   NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   The views and conclusions contained in the software and documentation are
   those of the authors and should not be interpreted as representing official
   policies, either expressed or implied, of the copyright holder.
 */

/** \addtogroup libmp
 *  @{
 */

/**
 * \file mp_loopbound.h Loopbound definitions for libmp.
 * 
 * \author Rasmus Bo Soerensen <rasmus@rbscloud.dk>
 *
 * \brief Internal header with loopbounds timing analysis for message
 * passing library
 *
 */

#ifndef _MP_LOOPBOUND_H
#define _MP_LOOPBOUND_H

#ifndef MSG_SIZE_WORDS
#define MSG_SIZE_WORDS 64
#endif

#ifndef NUM_BUF
#define NUM_BUF 3
#endif
#ifndef NUM_BUFMONE
#define NUM_BUFMONE 2
#endif

#ifndef PKT_TRANS_WAIT
#define PKT_TRANS_WAIT 12
#endif

#ifndef SAMPLE_TRANS_WAIT
#define SAMPLE_TRANS_WAIT 768
#endif

#endif /* _MP_LOOPBOUND_H_ */