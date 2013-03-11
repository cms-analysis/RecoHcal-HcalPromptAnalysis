#ifndef _MY_FEDGET_HH
#define _MY_FEDGET_HH

#include <stdint.h>
#include <stdlib.h>

#define FEDGET_NUM_HTR 15
#define FEDGET_NUM_CHAN 24


// decode bits of QIE
uint8_t QIE_mant(uint16_t r);
uint8_t QIE_range(uint16_t r);
uint8_t QIE_capID(uint16_t r);
uint8_t QIE_chipID(uint16_t r);
uint8_t QIE_fiberID(uint16_t r);
uint8_t QIE_err(uint16_t r);
uint8_t QIE_dv(uint16_t r);


/** \brief Class to access items within HCAL FED Data with minimal unpacking
 *
 * The constructor takes a pointer to the raw data and it's length.
 * An array of pointers to HTR payloads is set up for quick access.
 * Accessor functions are provided to the various parts of the data payload.
 * An array of pointers to each HTR's channel data is calculated on demand.
 *
 * Some error-checking functions are provided.
 *
 * NOTE!  Does not currently take into account format variations due to
 * HTR firmware revisions.
 */

class FedGet {
public:
  //! Unpack a FED payload.  Set up pointers for fast access.
  /// \param raw raw 32-bit data (first word of CDF header)
  /// \param size size of data in 32-bit words
  /// \param DCC firmware version (16 bits)
  FedGet( uint32_t* raw, uint32_t size, uint32_t DCCversion);

  //! Pointer to raw data
  uint32_t* raw() { return m_raw; }
  //! Size of data in 32-bit words
  uint32_t size() { return m_size; }

  //! Calculate CRC on data (compare with value returned by FedGet::CRC())
  uint16_t CalcCRC();
 
  uint32_t EvN() { return m_raw[1] & 0xffffff;  } ///< Get EvN from CDF header 
  uint32_t BcN() { return (m_raw[0] >> 20) & 0xfff;  } ///< Get BcN from CDF header
  uint32_t fed_id() { return (m_raw[0] >> 8) & 0xfff; } ///< Get FED ID from CDF header
  uint16_t CRC() { return (m_raw[m_size-2] >> 16) & 0xffff; } ///< Get CRC from CDF trailer
  //! Get event length (64-bit words) from CDF trailer
  uint32_t Evt_Length() { return m_raw[m_size-1] & 0xffffff; }
  uint8_t TTS() { return (m_raw[m_size-2] >> 4) & 0xf; } ///< Get TTS state from CDF trailer

  //! Get orbit number from DCC header (N.B. firmware version dependent!)
  uint32_t OrN() { return ( (m_DCCrev>=0x2c10) ? (((m_raw[2] >> 4) & 0xfffffff) |
						((m_raw[3] << 28) & 0xf0000000))
			    : (m_raw[2] & 0xffffffff)); }

  //! Get 1st DCC header low 32 bits (2nd CDF header)
  uint32_t DCCHdr0() { return m_raw[2]; }
  //! Get 1st DCC header high 32 bits (2nd CDF header)
  uint32_t DCCHdr1() { return m_raw[3]; }
  //! Get 2nd DCC header low 32 bits (DCC summary word)
  uint32_t DCCHdr2() { return m_raw[4]; }
  //! Get 2nd DCC header high 32 bits (DCC summary word)
  uint32_t DCCHdr3() { return m_raw[5]; }

  //! HTR header inserted by DCC for HTR h = 0..14
  uint32_t HTR_Hdr( int h) { return m_raw[6+h]; }
  //! Number of (32 bit) words for HTR
  uint16_t HTR_nWords( int h) { return HTR_Hdr(h) & 0x3ff; }
  //! HTR status bits (E,P,B,V,T) field from DCC header
  uint16_t HTR_epbvt( int h) { return (HTR_Hdr(h) >> 11) & 0x1f; }
  //! HTR status/error bits (low 8 bits only) from DCC header
  uint8_t HTR_err( int h) { return (HTR_Hdr(h) >> 24) & 0xff; }
  //! HTR LRB error bits from DCC header
  uint8_t HTR_lrb_err( int h) { return (HTR_Hdr(h) >> 16) & 0xff; }

  // HTR payload area
  uint32_t* HTR_Payload() { return &m_raw[24]; } ///< pointer to start of 1st HTR payload
  uint32_t* HTR_Data(int h) { return m_htr[h]; } ///< pointer to particular HTR payload
  uint32_t* HTR_End(int h) { return &m_htr[h][HTR_nWords(h)]; } ///< pointer to HTR end + 1

  // HTR items
  //! EvN from HTR
  uint32_t HTR_EvN(int h) { return (m_htr[h][0] & 0xff) | ((m_htr[h][0] >> 8) & 0xffff00); }
  //! BcN from HTR
  uint16_t HTR_BcN(int h) { return m_htr[h][2] & 0xfff; }
  //! OrN from HTR
  uint8_t HTR_OrN(int h) { return (m_htr[h][1] >> 27) & 0x1f; }
  //! Submodule ID from HTR
  uint16_t HTR_id(int h) { return (m_htr[h][1] >> 16) & 0x3ff; }
  //! Number of trigger primitives
  uint16_t HTR_ntp(int h) { return (m_htr[h][2] >> 24) & 0xff; }
  //! Number of DAQ data
  uint16_t HTR_ndd(int h) { return HTR_End(h)[-2] & 0x3ff; }
  //! Number of time samples
  uint8_t HTR_ns(int h) { return (HTR_End(h)[-2] >> 11) & 0x1f; }
  //! 16-bit HTR status/error word
  uint16_t HTR_status(int h) { return m_htr[h][1] & 0xffff; }

  // QIE data
  //! Pointer to start of TP for HTR
  uint16_t* HTR_TP(int h) { return (uint16_t*)(&m_htr[h][4]); }
  //! Pointer to start of QIE data for HTR
  uint16_t* HTR_QIE(int h) { return HTR_TP(h)+HTR_ntp(h); }
  //! Pointer to QIE data for a particular channel
  uint16_t* HTR_QIE_chan(int h, int c);
  //! Number of QIE data for a particular channel
  int HTR_QIE_chan_len(int h, int c) { return m_chn_len[h][c]; }

  //! Check if capID rotated for a HTR, channel
  bool good_capID(int h, int c);

  //! Dump for debug
  void Dump( int level);

  //! access raw HTR payload
  uint32_t* HTR_raw(int h) { return ( m_htr[h]); }

private: 
  uint32_t m_DCCrev;		///< DCC revision 
  uint32_t* m_raw;		///< pointer to raw data 
  uint32_t m_size;		///< data size 
  uint32_t* m_htr[FEDGET_NUM_HTR]; ///< pointers to HTR payloads
  uint16_t* m_chn[FEDGET_NUM_HTR][FEDGET_NUM_CHAN]; ///< pointer to channel data
  int m_chn_len[FEDGET_NUM_HTR][FEDGET_NUM_CHAN]; ///< words per channel
  uint16_t* m_extra[FEDGET_NUM_HTR]; ///< pointer to Extra-Info1 at end of HTR payload
  bool chan_unpacked;  ///< Flag:  unpacking done
  uint8_t m_capid[FEDGET_NUM_HTR][FEDGET_NUM_CHAN]; ///< capID: 0=unk 1=bad 2=OK

};
  


#endif
