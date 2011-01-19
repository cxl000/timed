#include <string>
#include <sstream>
using namespace std;

#include <pcrecpp.h>
#include <qmlog>

#if F_CSD
#include <NetworkTime>
#include <NetworkOperator>
#endif

#include "f.h"
#include "cellular.h"
#include "misc.h"
#include "tzdata.h"

#if F_CSD
#include "csd.h"
#endif

cellular_operator_t::cellular_operator_t()
{
}

cellular_operator_t::cellular_operator_t(const string &mcc_s, const string &mnc_s) :
  mcc(mcc_s), mnc(mnc_s)
{
  init() ;
}

#if F_CSD
cellular_operator_t::cellular_operator_t(const QString &mcc_s, const QString &mnc_s)
{
  mcc = mcc_s.toStdString(), mnc = mnc_s.toStdString() ;
  init() ;
}

cellular_operator_t::cellular_operator_t(const Cellular::NetworkTimeInfo &cnti)
{
  if (cnti.isValid())
  {
    mcc = cnti.mcc().toStdString(), mnc = cnti.mnc().toStdString() ;
    init() ;
  }
}
#endif

bool cellular_operator_t::operator==(const cellular_operator_t &x) const // same mcc & mnc
{
  return mcc==x.mcc and mnc==x.mnc ;
}

bool cellular_operator_t::operator!=(const cellular_operator_t &x) const
{
  return not operator==(x) ;
}

string cellular_operator_t::id() const
{
  return empty() ? "" : mcc+"/"+mnc ;
}

string cellular_operator_t::location() const
{
  return known_mcc() ? alpha2 : empty() ? "" : "("+id()+")" ;
}

bool cellular_operator_t::known_mcc() const
{
  return not alpha2.empty() ;
}

bool cellular_operator_t::empty() const
{
  return mcc.empty() and mnc.empty() ;
}

string cellular_operator_t::str() const
{
  ostringstream os ;
  os << "{mcc=" << "'" << mcc << "'" ;
  os << ", mnc=" << "'" << mcc << "'" ;
  if (known_mcc())
    os << ", location='" << alpha2 << "'" ;
  os << "}" ;
  return os.str() ;
}

void cellular_operator_t::init()
{
  alpha2 = tzdata::iso_3166_1_alpha2_by_mcc(mcc) ;
#if 0
  static pcrecpp::RE integer = "(\\d+)" ;
  if (p[0]=='\0') //empty string
    mcc_value = 0 ;
  else if (not integer.FullMatch(mcc, &mcc_value))
    mcc_value = -1 ;
#endif
}

cellular_time_t::cellular_time_t() :
  value(0), ts(0)
{
  log_debug("constructed %s by default", str().c_str()) ;
}

#if F_CSD
cellular_time_t::cellular_time_t(const Cellular::NetworkTimeInfo &cnti) :
  value(0), ts(0)
{
  if (cnti.isValid() and cnti.dateTime().isValid())
  {
    value = cnti.dateTime().toTime_t() ;
    ts = nanotime_t::from_timespec (*cnti.timestamp()) ;
  }
  log_debug("constructed %s out of %s", str().c_str(), csd_t::csd_network_time_info_to_string(cnti).c_str()) ;
}
#endif

string cellular_time_t::str() const
{
  if (not is_valid())
    return "{cellular_time_t::invalid}" ;
  else
  {
    ostringstream os ;
    os << "{value=" << value << "=" << str_iso8601(value) ;
    os << ", received=" << ts.str() << "}" ;
    return os.str() ;
  }
}

cellular_offset_t::cellular_offset_t() :
  offset(0), dst(-1), timestamp(0), sender_time(false)
{
  log_debug("constructed %s by default", str().c_str()) ;
}

#if F_CSD
cellular_offset_t::cellular_offset_t(const Cellular::NetworkTimeInfo &cnti) :
  oper(cnti),
  offset(0), dst(-1), timestamp(0), sender_time(false)
{
  log_debug() ;
  if (cnti.isValid())
  {
    offset = cnti.offsetFromUtc() ;

    // first of all check, if we can support this offset
    static const int offset_threshold = 15*3600 ; // 15 hours from Greenwich
    bool too_large = offset < -offset_threshold or offset_threshold < offset ;
    bool not_divisible = offset % (15*60) ;
    if (too_large or not_divisible)
    {
      log_error("GMT offset %d seconds is not supported", offset) ;
      return ;
    }

    dst = cnti.daylightAdjustment() ;

    if (cnti.dateTime().isValid() and cnti.dateTime().timeSpec()==Qt::UTC)
    {
      sender_time = true ;
      timestamp = cnti.dateTime().toTime_t() ;
    }
    else
    {
      // the exact moment of sending isn't clear, let's guess it
      nanotime_t monotonic_received = nanotime_t::from_timespec(*cnti.timestamp()) ;
      nanotime_t system_received = nanotime_t::systime_at_zero() + monotonic_received ;
      timestamp = system_received.sec() ;
    }
  }
  log_debug("constructed %s out of %s", str().c_str(), csd_t::csd_network_time_info_to_string(cnti).c_str()) ;
}
#endif

string cellular_offset_t::str() const
{
  ostringstream os ;
  if (is_valid())
  {
    os << "{offset=" << offset ;
    if (offset)
    {
      int offset_min = offset / 60 ;
      os << "=" << (offset_min<0 ? (offset_min=-offset_min, "-") : "+") ;
      int hour = offset_min / 60 ;
      os << hour ;
      if (int mins = offset_min % 60)
        os << str_printf(":%02d", mins) ;
      else
        os << "h" ;
    }
    os << ", dst=" ;
    if (dst<0)
      os << "n/a" ;
    else
      os << dst ;

    os << (sender_time ? "sender" : "receiver") << " time=" << timestamp << "=" << str_iso8601(timestamp) ;
    os << "}" ;
  }
  else
    os << "{cellular_offset_t::invalid}" ;
  return os.str() ;
}
