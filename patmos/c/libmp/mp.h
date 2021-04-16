/*
   Copyright 2014 Technical University of Denmark, DTU Compute. 
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
 * \file mp.h Definitions for libmp.
 * 
 * \author Rasmus Bo Soerensen <rasmus@rbscloud.dk>
 *
 * \brief Message passing library for the T-CREST platform
 *
 */

#ifndef _MP_H_
#define _MP_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <machine/patmos.h>
#include <machine/spm.h>
#include <machine/boot.h>
#include <machine/rtc.h>
#include "libnoc/noc.h"
#include "libnoc/coreset.h"
//#define TRACE_LEVEL WARNING
//#define DEBUG_ENABLE
#include "include/debug.h"

/// \brief Aligns X to word size
//#define WALIGN(X) (((X)+0x3) & ~0x3)

#define MAX_CHANNELS  256

#ifdef NOINLINE
 #define INLINING __attribute__ ((noinline))
#else
 #define INLINING __attribute__ ((always_inline))
#endif

/// \brief A type to identify a core. Supports up to 256 cores in the platform
typedef char coreid_t;

////////////////////////////////////////////////////////////////////////////
// Data structures for storing state information
// of the message passing channels
////////////////////////////////////////////////////////////////////////////

typedef enum {SOURCE, SINK} direction_t;

/// \struct LOCK_T
/// \brief Lock type placed in local scratchpad memory
struct _SPM_LOCK_T; // forward decl
typedef struct _SPM_LOCK_T _SPM LOCK_T;
struct _SPM_LOCK_T {
  volatile unsigned int remote_entering;
  volatile unsigned int remote_number;
  unsigned int local_entering;
  unsigned int local_number;
  //struct _SPM_LOCK_T _SPM * remote_ptr;
  LOCK_T * remote_ptr;
  unsigned char remote_cpuid;
};

LOCK_T * initialize_lock(unsigned remote);
void acquire_lock(LOCK_T * lock) INLINING;
void release_lock(LOCK_T * lock) INLINING;

/// \struct qpd_t
/// \brief Queuing port descriptor.
///
/// The struct is used to store the data describing the massage passing channel.
/// This struct is used to describe both the sending and receiving ends of a
/// communication channel.
struct _qpd_t; // forward decl
typedef struct _qpd_t _SPM qpd_t;
struct _qpd_t {
  /*-- Shared variables --*/
  /** The type of port, source or sink */
  direction_t direction_type;
  /** The ID of the remote core */
  coreid_t remote;
  /** The address of the receiver buffer structure */
  volatile void _SPM * recv_addr;
  /** The address of the recv_count at the sender */
  volatile unsigned int _SPM * send_recv_count;
  /** The size of a buffer in bytes */
  unsigned int buf_size;
  /** The number of buffers at the receiver */
  unsigned int num_buf;
  /** The following fields depend on the direction of the port */
  union {
    /** Sender specific fields */
    struct {
      /** A pointer to the free write buffer */
      volatile void _SPM * write_buf;
      /** A pointer to the used write buffer */
      volatile void _SPM * shadow_write_buf;
      /** The number of messages sent by the sender */
      unsigned int send_count;
      /** A pointer to the tail of the receiving queue */
      unsigned int send_ptr;
    };
    /** Recevier specific fields */
    struct {
      /** A pointer to the currently free read buffer */
      volatile void _SPM * read_buf;
      /** The number of messages received by the receiver */
      volatile unsigned int _SPM * recv_count;
      /** A pointer to the head of the receiving queue */
      unsigned int recv_ptr;
    };
  };

};

/// \struct spd_t
/// \brief Sample port descriptor.
///
/// The struct is used to store the data describing the sampling port.
/// This struct is used to describe both the writer and reader of the
/// sampling port.
struct _spd_t; // forward decl
typedef struct _spd_t _SPM spd_t;
struct _spd_t {
  /*-- Shared variables --*/
  /** The type of port, source or sink */
  direction_t direction_type;
  /** The ID of the remote core */
  coreid_t remote;
  /** The sample port descriptor for the remote core */
  spd_t * remote_spd;
  /** The address of the read buffer structure */
  volatile void _SPM * read_bufs;
  /** The address of the shared memory read buffer structure */
  volatile void * read_shm_buf;
  /** The size of a buffer in bytes */
  unsigned int sample_size;
  /** Pointer to lock*/
  LOCK_T * lock;

  int padding;
  /** The following fields depend on the direction of the port */
  union {
    /** writer specific fields */
    struct {
      /** Value specifying which buffer the reader is reading */
      volatile unsigned int reading;
      /** A pointer to the currently free read buffer */
      unsigned int next;
      ///** The number of buffers at the receiver */
      //unsigned int num_readers;
    };
    /** Reader specific fields */
    struct {
      /** Value specifying which buffer is the newest to be read */
      volatile unsigned int newest;
      /** Value specifying which buffer is the newest to be read */
      volatile unsigned int next_reading;
    };
  };

} ;


/// \cond PRIVATE
/// \struct communicator_t
/// \brief Describes at set of communicating processors.
///
/// The struct is used to store all necessary information on the set of
/// communicating processors.
typedef struct {
  coreset_t barrier_set;
  unsigned int count;
  unsigned int msg_size;
  volatile void _SPM ** addr;
} communicator_t __attribute__((aligned(16)));
/// \endcond


////////////////////////////////////////////////////////////////////////////
// Functions for memory management in the communication SPM
////////////////////////////////////////////////////////////////////////////

/// \brief Initialize message passing library.
///
/// #mp_init is a static constructor and not intended to be called directly.
void mp_init(void) __attribute__((constructor(102),used));

/// \brief Static memory allocation on the communication scratchpad.
/// No mp_free function
void _SPM * mp_alloc(const size_t size) __attribute__ ((noinline));

////////////////////////////////////////////////////////////////////////////
// Functions for initializing the communication channels of the
// message passing API. The initialization happens in two steps.
// The first step is to register all the channels that needs to be
// initialized and then initializing them.
////////////////////////////////////////////////////////////////////////////

/// \brief Initialize the state of a communication channel
///
/// \param qpd_ptr A pointer the the message passing descriptor
/// \param remote The core id of the remote processor
/// \param buf_size The size of the message buffer
/// \param num_buf The number of buffers in the receiving scratchpad
///
/// \return The function returns a pointer to the created message passing
/// descriptor #qpd_t. If the function fails, the pointer is NULL.
/// Remember to check if the returned pointer is different from NULL.
qpd_t * mp_create_qport( const unsigned int chan_id, const direction_t direction_type,
                         const size_t msg_size, const size_t num_buf);

/// \brief Initialize the state of a communication channel
///
/// \param qpd_ptr A pointer the the message passing descriptor
/// \param remote The core id of the remote processor
/// \param buf_size The size of the message buffer
/// \param num_buf The number of buffers in the receiving scratchpad
///
/// \return The function returns a pointer to the created sampling port
/// descriptor #spd_t. If the function fails, the pointer is NULL.
/// Remember to check if the returned pointer is different from NULL.
spd_t * mp_create_sport(const unsigned int chan_id, const direction_t direction_type,
                        const size_t sample_size);

/// \breif Initializing all the channels that have been registered.
///
/// \retval 0 The initialization of one or more communication channels failed.
/// \retval 1 The initialization of all the communication channels succeeded.
int mp_init_ports();

/// \cond PRIVATE
/// \brief Initialize the communicator
///
/// \param comm A pointer to the communicator structure
/// \param member_ids An array of member ids.
/// \param member_addrs An array of COM SPM addresses for the members.
///
/// \retval 0 The address is not aligned  to words.
/// \retval 1 The initialization of the communicator_t succeeded.
int mp_communicator_init(communicator_t* comm, const unsigned int count,
              const coreid_t member_ids [], const unsigned int msg_size);
/// \endcond
////////////////////////////////////////////////////////////////////////////
// Functions for queuing point-to-point transmission of data
////////////////////////////////////////////////////////////////////////////

/// \brief Non-blocking function for passing a message to a remote processor
/// under flow control. The data to be passed by the function should be in the
/// local buffer in the communication scratch pad before the function
/// is called.
///
/// \param qpd_ptr A pointer to the message passing data structure
/// for the given message passing channel.
///
/// \retval 0 The send did not succeed, either there was no space in the
/// receiving buffer or there was no free DMA to start a transfer
/// \retval 1 The send succeeded.
int mp_nbsend(qpd_t * qpd_ptr) INLINING  ;

/// \brief A function for passing a message to a remote processor under
/// flow control. The data to be passed by the function should be in the
/// local buffer in the communication scratch pad before the function
/// is called.
///
/// \param qpd_ptr A pointer to the message passing data structure
/// for the given message passing channel.
/// \param time_usecs The time out time in microseconds, if parameter is 0
/// the timeout is infinite
///
/// \retval 0 The function timed out.
/// \retval 1 The function succeeded sending the message.
int mp_send(qpd_t * qpd_ptr, const unsigned int time_usecs) INLINING ;

/// \brief Non-blocking function for receiving a message from a remote processor
/// under flow control. The data that is received is placed in a message buffer
/// in the communication scratch pad, when the received message is no
/// longer used the reception of the message should be acknowledged with
/// the #mp_ack()
///
/// \param qpd_ptr A pointer to the message passing data structure
/// for the given message passing channel.
///
/// \retval 0 No message has been received yet.
/// \retval 1 A message has been received and dequeued. The call has to be
/// followed by a call to #mp_ack() when the data is no longer used.
int mp_nbrecv(qpd_t * qpd_ptr)  INLINING ;

/// \brief A function for receiving a message from a remote processor under
/// flow control. The data that is received is placed in a message buffer
/// in the communication scratch pad, when the received message is no
/// longer used the reception of the message should be acknowledged with
/// the #mp_ack()
///
/// \param qpd_ptr A pointer to the message passing data structure
/// for the given message passing channel.
/// \param time_usecs The time out time in microseconds, if parameter is 0
/// the timeout is infinite
///
/// \retval 0 The function timed out.
/// \retval 1 The function succeeded receiving the message.
int mp_recv(qpd_t * qpd_ptr, const unsigned int time_usecs)  INLINING ;

/// \brief Non-blocking function for acknowledging the reception of a message.
/// This function should be used with extra care, if no acknowledgement is sent
/// the communication channel will be blocked until an acknowledgement is sent.
/// This function shall be called to release space in the receiving
/// buffer when the received data is no longer used.
/// It is not necessary to call #mp_ack() after each #mp_recv() call.
/// It is possible to work on 2 or more incoming messages at the same
/// time with out them being overwritten.
///
/// \param qpd_ptr A pointer to the message passing data structure
/// for the given message passing channel.
///
/// \retval 0 No acknowledgement has been sent.
/// \retval 1 An acknowledgement has been sent.
int mp_nback(qpd_t * qpd_ptr) INLINING ;

/// \brief A function for acknowledging the reception of a message.
/// This function shall be called to release space in the receiving
/// buffer when the received data is no longer used.
/// It is not necessary to call #mp_ack() after each #mp_recv() call.
/// It is possible to work on 2 or more incoming messages at the same
/// time with out them being overwritten.
///
/// \param qpd_ptr A pointer to the message passing data structure
/// for the given message passing channel.
/// \param time_usecs The time out time in microseconds, if parameter is 0
/// the timeout is infinite
///
/// \retval 0 The function timed out.
/// \retval 1 The function succeeded acknowledging the message.
int mp_ack(qpd_t * qpd_ptr, const unsigned int time_usecs) INLINING ;
int mp_ack_n(qpd_t * qpd_ptr, const unsigned int time_usecs, unsigned int num_acks) INLINING ;

////////////////////////////////////////////////////////////////////////////
// Functions for sampling point-to-point transmission of data
////////////////////////////////////////////////////////////////////////////

/// \brief A function for writing a sampled value to the remote location
/// at the receiving end of the channel
int mp_write(spd_t * sport, volatile void _SPM * sample)   __attribute__ ((noinline));

/// \breif A function for reading a sampled value from the remote location
/// at the sending end of the channel
int mp_read(spd_t * sport, volatile void _SPM * sample)   __attribute__ ((noinline));

/// \breif A function for reading a sampled value from the remote location
/// at the sending end of the channel. The function requires that the read
/// value has not been read before.
///
/// \retval 0 The read value has been read before.
/// \retval 1 The read value has not been read before.
/// \retval 2 There is no value to read.
/// \returns The function returns when a value has been read.
//int mp_read_updated(spd_t * spd_ptr);

////////////////////////////////////////////////////////////////////////////
// Functions for collective communication
////////////////////////////////////////////////////////////////////////////
/// \cond PRIVATE
/// \brief A function to synchronize the cores described in the communicator
/// to a barrier.
///
/// \param comm A pointer to the communicator struct that describes 
/// the group of processing cores involved in the broadcast.
///
/// \returns The function returns after all processing cores has
/// called at the barrier function.
void mp_barrier(communicator_t* comm);

/// \brief A function for broadcasting a message to all members of
/// a communicator
///
/// \param comm A pointer to the communicator struct that describes 
/// the group of processing cores involved in the broadcast.
/// \param root The core ID of the processing core that should broadcast
/// data. All processing cores in the communicator group has to call the
/// #mp_broadcast() and specify the same #root core ID.
///
/// \returns The function returns in the root when the root has sent all the data to
/// the other cores and in the other cores when each core has received the data
/// from the #root core
void mp_broadcast(communicator_t* comm, const coreid_t root);
/// \endcond

#endif /* _MP_H_ */

/** @}*/
