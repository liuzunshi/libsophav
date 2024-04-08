// Automaticlly generated by: ::SceMiMsg
// DO NOT EDIT
// C++ Class with SceMi Message passing for type:  TLM3Defines::TLMSecurity

#pragma once

#include "bxactors/xat_scemi.h"

/// C++ class representing the hardware enum TLM3Defines::TLMSecurity.
class TLMSecurity : public XATType {
 public:

  enum E_TLMSecurity {
      e_SECURE,
      e_NON_SECURE
    } ;
  
  E_TLMSecurity m_val;

  /// Default constructor for enumeration
  TLMSecurity (E_TLMSecurity v = e_SECURE)
  : m_val(v)
  {}

  /**
   * @brief Constructor for enumeration from a SceMiMessageData object
   *
   * @param msg -- the scemi message object
   * @param off -- the starting bit offset, updated to next bit position
   */
  TLMSecurity (const SceMiMessageDataInterface *msg, unsigned int & off) {
    m_val = (E_TLMSecurity) msg->GetBitRange ( off, 0 );
    off = off + 1;
  }

  /**
   * @brief Converts enumeration into its bit representation for sending as a SceMi message
   *
   * @param msg -- the message object written into
   * @param off -- bit position offset in message
   * @return next free bit position in the message for writing
   */
  unsigned int setMessageData( SceMiMessageDataInterface &msg, const unsigned int off) const {
    msg.SetBitRange ( off, 0, (SceMiU32) m_val );
    return off + 1;
  }

  /// overload operator == for enumeration classes with values
  bool operator== (const E_TLMSecurity &x) const { return m_val == x ;}
  /// overload operator != for enumeration classes with values
  bool operator!= (const E_TLMSecurity &x) const { return m_val != x ;}
  /// overload operator == for enumeration classes
  bool operator== (const TLMSecurity &x) const { return m_val == x.m_val ;}
  /// overload operator != for enumeration classes
  bool operator!= (const TLMSecurity &x) const { return m_val != x.m_val ;}

  /// overload the put-to operator for TLMSecurity
  friend std::ostream & operator<< (std::ostream &os, const TLMSecurity & e) {
    XATType::PutTo * override = lookupPutToOverride ( e.getClassName() );
    if ( override != 0 ) {
       return override(os, e );
    }
    switch (e.m_val) {
      case e_SECURE: os << "SECURE" ; break ;
      case e_NON_SECURE: os << "NON_SECURE" ; break ;
      default: os << "Enum value error for TLMSecurity: " << (int) e.m_val << "." ;
    }
    return os;
  };

  /**
   * @brief Accessor for symbolic name of the class' enumeration value
   * @return the name as a char *
   */
  const char * getName() const {
    const char *ret = "" ;
    switch (m_val) {
      case e_SECURE: ret = "SECURE" ; break ;
      case e_NON_SECURE: ret = "NON_SECURE" ; break ;
      default: std::cerr << "Enum value error for TLMSecurity: " << (int) m_val << "." ;
    }
    return ret;
  };

  /**
   * @brief Adds to the stream the bit representation of this object
   *
   * @param os -- the ostream object which to append
   * @return the ostream object
   */
  virtual std::ostream & getBitString (std::ostream &os) const {
    unsigned int data = (unsigned int) m_val;
    for ( unsigned int i = 1; i > 0; --i) {
      unsigned int bitidx = i - 1;
      bool v = 0 != (data & (0x1 << bitidx));
      os << (v ? '1' : '0') ;
    }
  return os;
  }

  /**
   * @brief Accessor for the XATType name for this object
   *
   * @param os -- the ostream object which to append
   * @return the ostream object
   */
  virtual std::ostream & getXATType (std::ostream & os) const {
    os << "TLM3Defines::TLMSecurity" ;
    return os;
  }

  /**
   * @brief Accessor on the size of the object in bits
   * @return the bit size
   */
  virtual unsigned int getBitSize () const {
    return 1;
  }

  /// returns the class name for this object
  virtual const char * getClassName() const {
    return "TLMSecurity" ;
  }

  /// returns the XATKind for this object
  virtual XATKind getKind() const {
    return XAT_Enum ;
  }

  /// Accessor for the count of members in object
  virtual unsigned int getMemberCount() const {return 0;};
  /**
   * @brief Accessor to member objects
   *
   * @param idx -- member index
   * @return XATType * to this object or null
   */
  virtual XATType * getMember (unsigned int idx) {return 0;};
  
  /**
   * @brief Accessor for symbolic member names
   *
   * @param idx -- member index
   * @return char* to this name or null
   */
  virtual const char * getMemberName (unsigned int idx) const {return 0;};
};
