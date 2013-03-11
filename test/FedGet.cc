 
#include "FedGet.hh"
#include <stdlib.h>		// for NULL
#include <stdio.h>
#include <string.h>

#include "crc16d64.hh"

FedGet::FedGet( uint32_t* raw, uint32_t size, uint32_t DCCver) {
  m_DCCrev = DCCver;
  m_raw = raw;
  m_size = size;
  if( Evt_Length() * 2 != m_size) {
    fprintf( stderr, "FedGet():  Bad event length!  size=%d  CDF trailer=%d\n",
	    size, Evt_Length()*2 );
    exit(1);
  }
  // build table of pointers to HTR payloads
  uint32_t* p = HTR_Payload();	// start of HTR payloads
  for( int i=0; i<FEDGET_NUM_HTR; i++) {
    if( HTR_nWords(i)) {
      m_htr[i] = p;
      p += HTR_nWords(i);
      m_extra[i] = (uint16_t* )p - 12;	// pointer to 'Extra-Info1' at end of payload
    } else
      m_htr[i] = NULL;
  }
  chan_unpacked = false;
  memset( m_chn, 0, sizeof(m_chn));
  memset( m_capid, 0, sizeof(m_capid));
  memset( m_chn_len, 0, sizeof(m_chn_len));
}

uint16_t* FedGet::HTR_QIE_chan(int h, int c) {
  if( !chan_unpacked) {

    // build table of pointers to channels, respecting zero-suppression
    // also record number of words in each channel
    if( HTR_ndd(h)) {		// any DAQ data?
      int ch0 = -1;		// previous channel
      int ch = 0;			// this channel
      uint16_t* q = HTR_QIE(h);	// point to start of QIE data
      uint16_t* q0 = q;		// point to previous data
      for( int i=0; i<HTR_ndd(h); i++) {
	ch = QIE_fiberID(*q)*3 + QIE_chipID(*q); // current channel
	if( ch != ch0) {	// did the channel change?
	  m_chn[h][ch] = q;	// yes, it's start of a new channel
	  if( ch != 0)		// calculate length
	    m_chn_len[h][ch0] = q - q0;
	  ch0 = ch;
	}
      }
      // now the length for the last channel, watch for 'FFFF' word!
      m_chn_len[h][ch] = m_extra[h] - q;
      if( *(m_extra[h]-1) == 0xffff)
	--m_chn_len[h][ch];
    }
  }
  chan_unpacked = true;
  return m_chn[h][c];
}

bool FedGet::good_capID(int h, int c) {
  if( HTR_QIE_chan(h,c) == NULL)
    return false;
  if( m_capid[h][c])		// capID state known?
    return( (bool)(m_capid[h][c]-1) );	// yes, return 0(bad) or 1(good)
  uint16_t* q = HTR_QIE_chan( h, c);
  // else check it
  if( HTR_QIE_chan_len(h,c) > 1) {
    for( int i=1; i<HTR_QIE_chan_len(h,c); i++) {
      if( ((QIE_capID(q[i-1])+1)%4) != QIE_capID(q[i])) {
	m_capid[h][c] = 1;
	return false;
      }
    }
  }
  m_capid[h][c] = 2;
  return true;
}


// calculate CRC
uint16_t FedGet::CalcCRC() {
  uint16_t crc = 0xFFFF;
    
  for (unsigned int i=0; i < m_size; i+=2) {
    if(i == m_size -2)
      crc = crc16d64_(m_raw[i+1], m_raw[i]&0xFFFF, crc);
    else
      crc = crc16d64_(m_raw[i+1], m_raw[i], crc);
  }

  return crc;
}


// Dump for debug to stdout
void FedGet::Dump( int level) {
  printf("FED: %3d EvN: %06x  BcN: %03x  OrN: %08x  TTS: %d\n",
	 fed_id(), EvN(), BcN(), OrN(), TTS() );
  if( CalcCRC() != CRC())
    printf("CRC Error!  cal=%08x  data=%08x\n", CalcCRC(), CRC() );
  if( level > 1) {
    for( int i=0; i<15; i++) {
      if( HTR_nWords(i)) {
	printf(" %2d: id: %03x EvN: %06x BcN: %03x  OrN: %02x  HDR: %04x ntp: %d  ndd: %d ns: %d\n",
	       i, HTR_id(i), HTR_EvN(i), HTR_BcN(i), HTR_OrN(i), HTR_status(i),
	       HTR_ntp(i), HTR_ndd(i), HTR_ns(i));
	if( level > 2) {
	  if( HTR_ndd(i)) {
	    for( int ch=0; ch<24; ch++) {
	      uint16_t *q;
	      if( (q = HTR_QIE_chan(i,ch)) != NULL) {
		printf("Channel %d:\n", ch);
		for( int s=0; s<HTR_QIE_chan_len(i,ch); s++) {
		  uint16_t r = q[s];
		  printf("  Fib %d QIE %d Cap %d %c%c (%d %2d)\n",
			 QIE_fiberID(r), QIE_chipID(r), QIE_capID(r),
			 QIE_dv(r) ? 'V' : ' ',
			 QIE_err(r) ? 'E' : ' ',
			 QIE_range(r), QIE_mant(r));
		}
	      }
	    }
	  }
	}
      }
    }
  }
}


// decode bits of QIE
uint8_t QIE_mant(uint16_t r) { return r & 0x1f; } ///< QIE mantissa
uint8_t QIE_range(uint16_t r) { return (r>>5) & 3; } ///< QIE range
uint8_t QIE_capID(uint16_t r) { return (r>>7) & 3; } ///< QIE cap ID
uint8_t QIE_chipID(uint16_t r) { return (r>>11) & 3; } ///< QIE chip ID
uint8_t QIE_fiberID(uint16_t r) { return (r>>13) & 7; } ///< QIE fiber ID
uint8_t QIE_err(uint16_t r) { return (r>>10) & 1; } ///< QIE link error bit
uint8_t QIE_dv(uint16_t r) { return( r>>9) & 1; } ///< QIE data valid bit

