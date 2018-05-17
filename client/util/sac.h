#ifndef _SAC_H_
#define _SAC_H_

#ifndef QCN_MAC_USB

#ifndef int32_t
#ifdef _WIN32
#ifdef _WIN64
typedef int int32_t;
#else
typedef long int32_t;
#endif
#else
typedef int int32_t;
#endif
#endif

struct STriggerInfo;

namespace sacio {

void wsac0(
   const char* fname,
   const float* xarray,
   const float* yarray,
   int32_t& nerr,
   const int32_t& npts,
   const struct sac_header* psacdata
);

// a csv/text/Excel-compatible version of the sacio sac output file recording
void wcsv0(
	const char* fname, 
	const float* t,
	const float* x, 
	const float* y, 
	const float* z, 
	int32_t& nerr, 
	const int32_t& npts,
	const struct sac_header* psacdata,
	const double& dTimeZero
);
		
	
// this actually does double-duty as SAC or CSV (text/Excel-compatible) output, based on the sm->bMyOutputSAC boolean
extern int sacio
(
  const int32_t n1, 
  const int32_t n2, 
  struct STriggerInfo& ti,
  const char* strSensorType
);

// sac constants we're using, ref http://www.iris.edu/software/sac/manual/file_format.html
const int32_t ITIME  = 1;
const int32_t IDISP  = 6;
const int32_t IACC   = 8;
const int32_t IB     = 9;
const int32_t IRLDTA = 49;

// 70 * 4  +  40 * 4 = 440 + 24 * 8 = 632 bytes
struct sac_header {                    
  float f[70];
  int32_t l[40];
  char s[24][8];
};

// note the enum prefixes to make things distinct - esf = enum sac float, esl = enum sac long, ess = enum sac string
enum e_sac_float {
esf_delta,     esf_depmin,    esf_depmax,    esf_scale,     esf_odelta,    
esf_b,         esf_e,         esf_o,         esf_a,         esf_fmt, 
esf_t0,        esf_t1,        esf_t2,        esf_t3,        esf_t4,        
esf_t5,        esf_t6,        esf_t7,        esf_t8,        esf_t9,        
esf_f,         esf_resp0,     esf_resp1,     esf_resp2,     esf_resp3,     
esf_resp4,     esf_resp5,     esf_resp6,     esf_resp7,     esf_resp8,     
esf_resp9,     esf_stla,      esf_stlo,      esf_stel,      esf_stdp,      
esf_evla,      esf_evlo,      esf_evel,      esf_evdp,      esf_mag,   
esf_user0,     esf_user1,     esf_user2,     esf_user3,     esf_user4,     
esf_user5,     esf_user6,     esf_user7,     esf_user8,     esf_user9,     
esf_dist,      esf_az,        esf_baz,       esf_gcarc,     esf_sb, 
esf_sdelta,    esf_depmen,    esf_cmpaz,     esf_cmpinc,    esf_xminimum,   
esf_xmaximum,  esf_yminimum,  esf_ymaximum,  esf_unused6,   esf_unused7,   
esf_unused8,   esf_unused9,   esf_unused10,  esf_unused11,  esf_unused12 
}; 

enum e_sac_long {
esl_nzyear,    esl_nzjday,    esl_nzhour,    esl_nzmin,     esl_nzsec,     
esl_nzmsec,    esl_nvhdr,     esl_norid,     esl_nevid,     esl_npts,      
esl_nsnpts,    esl_nwfid,     esl_nxsize,    esl_nysize,    esl_unused15,  
esl_iftype,    esl_idep,      esl_iztype,    esl_unused16,  esl_iinst,     
esl_istreg,    esl_ievreg,    esl_ievtyp,    esl_iqual,     esl_isynth,    
esl_imagtyp,   esl_imagsrc,   esl_unused19,  esl_unused20,  esl_unused21,  
esl_unused22,  esl_unused23,  esl_unused24,  esl_unused25,  esl_unused26,  
esl_leven,     esl_lpspol,    esl_lovrok,    esl_lcalda,    esl_unused27  
};

enum e_sac_str8 {
ess_kstnm,  ess_kevnm, ess_kevnm2,          
ess_khole,  ess_ko,     ess_ka,               
ess_kt0,    ess_kt1,    ess_kt2,              
ess_kt3,    ess_kt4,    ess_kt5,              
ess_kt6,    ess_kt7,    ess_kt8,              
ess_kt9,    ess_kf,     ess_kuser0,           
ess_kuser1, ess_kuser2, ess_kcmpnm,           
ess_knetwk, ess_kdatrd, ess_kinst          
};

} 

#endif // ifndef QCN_MAC_USB

#endif // _SAC_H_

