/*** CMC modified from sactosac.c for QCN project
 *  using this we can hopefully not have to link with g2c & saclib etc
 *
 *             - examines header of a data file written by the SAC
 *                 (Seismic Analysis Code) and switches it to the other
 *                 machine type: little-endian to big-endian
 *                 Based on saclinux2sun by G. A. Ichinose (1996)
 *                 G. Helffrich/U. Bristol/Feb. 11, 1998
 *                    bug in native conversion fixed Feb. 28, 2000
 *                    formatting bug fixed,
 *                    -m option added 4 Apr. 2007
 *   Important note:  SAC files are saved big-endian, so for most machines (i.e Intel's are little-endian) need to swap bytes for words (floats/longs)
 *      the system has been checked for endian-ness in main.cpp, the global g_endian is ENDIAN_LITTLE or ENDIAN_BIG
 ***/

#include "main.h"

// two ways to use zip, boinc_zip lib or the ziparchive lib (CZipArchive class)
#ifdef ZIPARCHIVE
#include "ZipArchive.h"  // new zip program
#else
#include "boinc_zip.h"
#endif

namespace sacio {

// CMC note -- this mimics the SAC 2000 library wsac0 call which writes a 2-D float array x & y (for us time & var)

void wsac0(
   const char* fname, 
   const float* xarray, 
   const float* yarray, 
   int32_t& nerr, 
   const int32_t& npts,
   const struct sac_header* psacdata
)
{ 	
	
  // fname=filename, xarray is usually time dimension, yarray is data per time point, 
  // nerr is error #, npts is # of points in array, psacdata is pointer to the sacdata struct
  // example usage:   wsac0(fname, t, s, nerr, npts, &sacdata);

    size_t sizeWrite = 0L;
    MFILE fp; // maybe try BOINC MFILE?
    //FILE* fp; 
    //if ( (fp = fopen(fname, "wb")) == NULL) {
    if ( fp.open(fname, "wb") ) {
       fprintf(stderr, "Error opening file %s\n", fname);
       return;
    }

    //fseek(fp.f, 0L, SEEK_SET); // go to start of file (which we should be at anyway)
    //fseek(fp, 0L, SEEK_SET); // go to start of file (which we should be at anyway)
    //sizeWrite = fwrite(psacdata, 1, sizeof(struct sac_header), fp);  // write the header
    sizeWrite = fp.write(psacdata, 1, sizeof(struct sac_header));  // write the header
    if (sizeWrite != (size_t) sizeof(struct sac_header)) {
       fprintf(stderr, "wsac0:header:: Expected %ld bytes written but wrote %ld for %s\n", sizeof(struct sac_header), sizeWrite, fname);
    }

    // now write the data -- should be independent data (yarray) first then time data (xarray)
    //sizeWrite = fwrite(yarray, sizeof(float), npts, fp);  // write the y-axis (x0/y0/z0/fsig/)
    sizeWrite = fp.write(yarray, sizeof(float), npts);  // write the y-axis (x0/y0/z0/fsig/)
    if (sizeWrite != (size_t) (npts)) {
       fprintf(stderr, "wsac0:yarray:: Expected %ld bytes written but wrote %ld for %s\n", sizeof(float) * npts, sizeof(float) * sizeWrite, fname);
    }

#ifdef QCN_RAW_DATA
	// write time data out - else assume it's evenly spaced data by dt
    //sizeWrite = fwrite(xarray, sizeof(float), npts, fp);  // write the x-axis (time/t0) as floats differenced from ref (zero) time
    sizeWrite = fp.write(xarray, sizeof(float), npts);  // write the x-axis (time/t0) as floats differenced from ref (zero) time
    if (sizeWrite != (size_t) (npts)) {
       fprintf(stderr, "wsac0:xarray:: Expected %ld bytes written but wrote %ld for %s\n", sizeof(float) * npts, sizeof(float) * sizeWrite, fname);
    }
#endif
	
    //fclose(fp);
    //fp = NULL;
    fp.close();
}

/*
#if !defined(linux) && !defined(BSD4d2) && !defined(__APPLE__)
extern char * sys_errlist[];
char *strerror(n)
   int32_t n;
{
   return sys_errlist[n];
}
#endif
*/

void long_swap(QCN_CBYTE* cbuf, int32_t& lVal)
{
	union {
		QCN_BYTE cval[4];
		int32_t lval;
	} l_union;
	if (sm->bMyOutputSAC) {  // only really need to do swap for SAC files
          if (qcn_main::g_endian == ENDIAN_BIG) {
			l_union.cval[0] = cbuf[0];
			l_union.cval[1] = cbuf[1];
			l_union.cval[2] = cbuf[2];
			l_union.cval[3] = cbuf[3];
          }
          else { // little endian needs to swap
			l_union.cval[0] = cbuf[3];
			l_union.cval[1] = cbuf[2];
			l_union.cval[2] = cbuf[1];
			l_union.cval[3] = cbuf[0];
          }
	}
	else {
		l_union.cval[0] = cbuf[0];
		l_union.cval[1] = cbuf[1];
		l_union.cval[2] = cbuf[2];
		l_union.cval[3] = cbuf[3];
	}
	lVal = l_union.lval;
}

void float_swap(QCN_CBYTE* cbuf, float& fVal)
{
	union {
		QCN_BYTE cval[4];
		float fval;
	} f_union;
	if (sm->bMyOutputSAC) {  // only really need to do swap for SAC files
          if (qcn_main::g_endian == ENDIAN_BIG) {
			f_union.cval[0] = cbuf[0];
			f_union.cval[1] = cbuf[1];
			f_union.cval[2] = cbuf[2];
			f_union.cval[3] = cbuf[3];			
          }
          else { // little endian needs to swap
			f_union.cval[0] = cbuf[3];
			f_union.cval[1] = cbuf[2];
			f_union.cval[2] = cbuf[1];
			f_union.cval[3] = cbuf[0];
          }
	}
	else {
		 f_union.cval[0] = cbuf[0];
		 f_union.cval[1] = cbuf[1];
		 f_union.cval[2] = cbuf[2];
		 f_union.cval[3] = cbuf[3];
	}
	fVal = f_union.fval;
}

void set_sac_null(struct sac_header* psacdata)
{ // sac data is apparently stored as big-endian so we have to flip it around if little-endian machine

   // 14 x 5 floats, 8 x 5 long, 8 x 3 str[8] = 70 * 4 + 40 * 4 + 24 * 8 = 280 + 160 + 192  = 632 bytes  (SAC_NUMHEADBYTES = 632)
   const float fTemp = SAC_NULL_FLOAT;
   const int32_t lTemp  = SAC_NULL_LONG;

    memset(psacdata, 0x00, sizeof(struct sac_header));

   for (int i = 0; i < 70; i++) {
      float_swap((QCN_CBYTE*) &fTemp, psacdata->f[i]);
   }

   for (int i = 0; i < 40; i++) {
      long_swap((QCN_CBYTE*) &lTemp, psacdata->l[i]);
   }
}
	
// example usage:
//     sacio(n1, n2, sm); // OUTPUT SAC FILE OF TRIG DAT

// n1 & n2 are array bounds we want to write out
// sm is the main shared memory pointer

extern int sacio
(
  const int32_t n1, 
  const int32_t n2, 
  struct STriggerInfo& ti,
  const char* strSensorType 
)
{
	
    boinc_begin_critical_section();

    // CMC note:  important -- all SAC float & long values are byte-swapped (i.e. big endian) for storage!

    struct sac_header sacdata;
    int32_t npts, nerr, i, j, lOff;

    char fname[_MAX_PATH];
    int32_t ifname;
    float   *x, *y, *z, *s, *t;
    //double  dTimeOffset = 0.0f, dTimeOffsetTime = 0.0f, dTimeZero = 0.0f;
    double  dTimeZero = 0.0f;
    int32_t lTemp;
    float fTimeTrigger = -1.0f, fTemp = 0.0f, 
         xmin = 10000.0, xmax = -10000.0, 
         ymin = 10000.0, ymax = -10000.0, 
         zmin = 10000.0, zmax = -10000.0, 
         smin = 10000.0, smax = -10000.0;

    nerr = i = j = 0L;
    npts =  n2 - n1;                               //  NUMBER OF POINTS IN OUT FILE

    if (npts<0) {  // must have wrapped around!
       npts = (MAXI-n1) + n2;
    }

/*
    fprintf(stdout, "Writing %s trigger file from t0[%d]=%ld to t0[%d]=%ld (%ld seconds  npts=%ld)\n", 
		ti.strFile, n1, QCN_ROUND(sm->t0[n1]), n2, QCN_ROUND(sm->t0[n2]), QCN_ROUND(sm->t0[n2] - sm->t0[n1]), npts );
	fflush(stdout); 
*/
	
    x = new float[npts];
    y = new float[npts];
    z = new float[npts];
    s = new float[npts];
    t = new float[npts];

    memset(x, 0x00, sizeof(float) * npts);
    memset(y, 0x00, sizeof(float) * npts);
    memset(z, 0x00, sizeof(float) * npts);
    memset(s, 0x00, sizeof(float) * npts);
    memset(t, 0x00, sizeof(float) * npts);

#ifdef QCN_RAW_DATA	
    const double cfFactor = 1.0f;  // sac data is in nanometers per sec^2 
#else
    const double cfFactor = sm->bMyOutputSAC ? 1.0e9f : 1.0f;  // sac data is in nanometers per sec^2 
#endif

    lOff = n1;
	//double dCtr = 0.0;
    for (j = 0; j < npts; j++) {
       if (lOff == MAXI) lOff = 1; // wraparound, skip 0, baseline point

       //qcn_util::getTimeOffset((const double*) sm->dTimeServerTime, (const double*) sm->dTimeServerOffset, (const double) sm->t0[lOff], dTimeOffset, dTimeOffsetTime);

		if (j == 0) {
			dTimeZero = sm->t0[lOff] + qcn_main::g_dTimeOffset;
			t[0] = 0.0f;
		}
		else {
			t[j] = sm->t0[lOff] + qcn_main::g_dTimeOffset - dTimeZero;
		}
			
       // note multiplying by 1e9 as sac format will be nm/s/s  (nanometers)	
       x[j] = sm->x0[lOff] * cfFactor;
       if (xmin > x[j]) xmin = x[j];
       if (xmax < x[j]) xmax = x[j];

       y[j] = sm->y0[lOff] * cfFactor;
       if (ymin > y[j]) ymin = y[j];
       if (ymax < y[j]) ymax = y[j];

       z[j] = sm->z0[lOff] * cfFactor;
       if (zmin > z[j]) zmin = z[j];
       if (zmax < z[j]) zmax = z[j];

       s[j] = sm->fsig[lOff];
       if (smin > s[j]) smin = s[j];
       if (smax < s[j]) smax = s[j];

		fTemp = t[j];
		float_swap((QCN_CBYTE*) &fTemp, t[j]);
		
		fTemp = x[j];
		float_swap((QCN_CBYTE*) &fTemp, x[j]);

		fTemp = y[j];
		float_swap((QCN_CBYTE*) &fTemp, y[j]);

		fTemp = z[j];
		float_swap((QCN_CBYTE*) &fTemp, z[j]);

		fTemp = s[j];
		float_swap((QCN_CBYTE*) &fTemp, s[j]);
		
		if (ti.bReal && lOff == ti.lOffsetEnd) {
			fTimeTrigger = t[j]; 
		}
		
       lOff++;
    }
		
	// note the time & accel aces arrays are all byte-swapped at this point!

    // ref for header values:  http://www.iris.edu/software/sac/manual/file_format.html
    set_sac_null(&sacdata); 

    lTemp = SAC_VERSION;
    long_swap((QCN_CBYTE*) &lTemp, sacdata.l[esl_nvhdr]);   // SAC version (usually 6)

    lTemp = npts;
    long_swap((QCN_CBYTE*) &lTemp, sacdata.l[esl_npts]);   // number of points per data component
	
    lTemp = TRUE;
    long_swap((QCN_CBYTE*) &lTemp, sacdata.l[esl_lovrok]);   // OK to overwrite
    long_swap((QCN_CBYTE*) &lTemp, sacdata.l[esl_lcalda]);   // TRUE if DIST, AZ, BAZ, and GCARC are to be calculated from station and event coordinates

    lTemp = IB;
    long_swap((QCN_CBYTE*) &lTemp, sacdata.l[esl_iztype]);   // IB means it's begin time

#ifdef QCN_SAC_DATA
    lTemp = 0;  // unknown
#else
    lTemp = IACC;
#endif
    long_swap((QCN_CBYTE*) &lTemp, sacdata.l[esl_idep]);   // acceleration in nm/s/s, so multiply x/y/z values by 10^9

    lTemp = IRLDTA;
    long_swap((QCN_CBYTE*) &lTemp, sacdata.l[esl_isynth]);   // real data

    lTemp = ITIME;
    long_swap((QCN_CBYTE*) &lTemp, sacdata.l[esl_iftype]);   // ITIME means it's a time-series file

    fTemp = (float) sm->dt;
    float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_delta]);  // this is the delta in evenly spaced file -- we try for .02 but not guaranteed based on accelerometer grade

#ifdef QCNLIVE // -- now using for live data via lat/lng sent in project prefs
    // if they input a valid lat/lng, enter it
    if (sm && sm->dMyLatitude != NO_LAT && sm->dMyLongitude != NO_LNG && sm->dMyLatitude != 0.0f && sm->dMyLongitude != 0.0f) {
       fTemp = (float) sm->dMyLatitude;
       float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_stla]);  
       fTemp = (float) sm->dMyLongitude;
       float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_stlo]);  
    }
    // put in elevation data
    fTemp = (float) sm->dMyElevationMeter;
    float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_stel]);  
    fTemp = (float) sm->iMyElevationFloor;
    float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_stdp]);  
#else  // may have data from the scheduler request which updates lat/lng/elev info
    if (sm && sm->dTrLatitude != NO_LAT && sm->dTrLongitude != NO_LNG && sm->dTrLatitude != 0.0f && sm->dTrLongitude != 0.0f) {
       fTemp = (float) sm->dTrLatitude;
       float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_stla]);  
       fTemp = (float) sm->dTrLongitude;
       float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_stlo]);  
    }
    // put in elevation data
    fTemp = (float) sm->dTrElevationMeter;
    float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_stel]);  
    fTemp = (float) sm->iTrElevationFloor;
    float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_stdp]);  
#endif
	

#ifdef QCN_SAC_DATA
	lTemp = 0L;
#else
	lTemp = TRUE;
#endif
	long_swap((QCN_CBYTE*) &lTemp, sacdata.l[esl_leven]);   // actually we are going to force even spaced .02s (50Hz) timings
	
    // event origin time -- trigger time I guess?  in seconds relative to ref time; was set in the above loop and clock adjusted
    if (ti.bReal && fTimeTrigger > -1.0f)  { // NB: if it's a demo trigger, don't mark trigger point
	   sacdata.f[esf_o] = fTimeTrigger;  // note already byte-swapped above i.e. all t, s, x, y, z were processed
       strcpy(sacdata.s[ess_ko], "Trigger");
    }
	sacdata.f[esf_b] = t[0];  // note already byte-swapped above i.e. all t, s, x, y, z were processed
	sacdata.f[esf_e] = t[npts-1];  // note already byte-swapped above i.e. all t, s, x, y, z were processed

    strcpy(sacdata.s[ess_knetwk], "QC");  // call the network QC, squeeze wu name in kevnm (QC not QCN, due to tradition of 2-chars)

/* CMC now passing in the sensor type str
    if (qcn_main::g_psms && sm->bSensorFound) {
       strcpy(sacdata.s[ess_kinst], qcn_main::g_psms->getTypeStrShort());
    }
*/
    strncpy(sacdata.s[ess_kinst], strSensorType, 7);

    strcpy(sacdata.s[ess_kuser0],  qcn_main::g_dTimeSync > 0.0f ? "TSYNC" : "NOTSYNC");   // flag that time was synchronized to server or not
    sprintf(sacdata.s[ess_kevnm], "%07d", ti.iWUEvent);   // number of event

#ifdef QCNLIVE
    // if they entered a station ID then use it
    if (strlen((const char*) sm->strMyStation)>0) {
        strlcpy(sacdata.s[ess_kstnm], (const char*) sm->strMyStation, SIZEOF_STATION_STRING);
    }
    else { // just note that it's from qcnlive
        strcpy(sacdata.s[ess_kstnm], "qcnlive");
    }
#else
    // hostid should be in APP_INIT_DATA in BOINC now
    // use their hostid so we can lookup into the database table if needed
    sprintf(sacdata.s[ess_kstnm], "%07d", sm->dataBOINC.hostid);
   //strcpy(sacdata.s[ess_kstnm], "boinc");
#endif

    // use dTimeZero for the reference time
    struct timeval tv;
    time_t ttime = (time_t) dTimeZero;
    struct tm* ptm = gmtime(&ttime);
    tv.tv_sec =  (int32_t) dTimeZero;
    tv.tv_usec = (int32_t) ((dTimeZero - (double) tv.tv_sec) * 1.0e6f);

    lTemp = 1900L + ptm->tm_year; // note need to add 1900 to tm_year in tm struct
    long_swap((QCN_CBYTE*) &lTemp, sacdata.l[esl_nzyear]);  // year of reference time (should this be the time at 0 point?)

    lTemp = ptm->tm_yday + 1L;    // note need to bump up Julian day by one in tm_yday for tm struct
    long_swap((QCN_CBYTE*) &lTemp, sacdata.l[esl_nzjday]);   // julian day of ref (zero) time

    lTemp = ptm->tm_hour;
    long_swap((QCN_CBYTE*) &lTemp, sacdata.l[esl_nzhour]);   // hour of ref time

    lTemp = ptm->tm_min;
    long_swap((QCN_CBYTE*) &lTemp, sacdata.l[esl_nzmin]);     // minute of ref time

    lTemp = ptm->tm_sec;
    long_swap((QCN_CBYTE*) &lTemp, sacdata.l[esl_nzsec]);     // second of ref time

    lTemp = tv.tv_usec/1000L; // note we want milliseconds, not microseconds!
    long_swap((QCN_CBYTE*) &lTemp, sacdata.l[esl_nzmsec]); // header start millisecond

    ZipFileList zfl;
	const char* strZipPath = (const char*) ((ti.bContinual && strlen(qcn_main::g_strPathContinual)>2) ? qcn_main::g_strPathContinual : qcn_main::g_strPathTrigger);
	string strZip(strZipPath);  // note it DOES NOT HAVE appropriate \ or / at the end
    double dTimeNow;
    dTimeNow = dtime();
    ttime = (time_t) dTimeNow;
    ptm = gmtime(&ttime);
    sprintf(sacdata.s[ess_kdatrd], "%02d%02d%02d", ptm->tm_year-100, ptm->tm_mon+1, ptm->tm_mday);
 
    // now print filenames which is workunit name, time, & number of trigger, then send to wsac0
    ifname = (int32_t) strlen((const char*) ti.strFile) - 4; // this is where the zip will be
    memset(fname, 0x00, _MAX_PATH);
    sprintf(fname, "%s%c", strZipPath, qcn_util::cPathSeparator());
    strncat(fname, (const char*) ti.strFile, ifname);
	
	if (sm->bMyOutputSAC) { // SAC output
			strlcat(fname, ".?.sac", _MAX_PATH);

		//fprintf(stdout, "DEBUG: strFile = [%s]\n", ti.strFile);
		//fprintf(stdout, "DEBUG: fnamebf = [%s]\n", fname);

			ifname = (int32_t) strlen(fname) - 5;
		
#if 0   // not saving sig sac file any more as it's only needed for trigger/quake detection
			// Significance section (fsig)
			fname[ifname] = 'S';

			strcpy(sacdata.s[ess_kcmpnm], "HLS");  // component name (axis)

			fTemp = SAC_NULL_FLOAT;   // cmpinc, which should be 0 for z, 90 for x & y, and -12345.0f for sig.
			float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_cmpinc]);

			fTemp = smin;
			float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_depmin]);  // min value of independent variable

			fTemp = smax;
			float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_depmax]);  // max value of independent variable

			zfl.push_back(fname);
			wsac0(fname, t, s, nerr, npts, &sacdata);	 	
#endif // no sig files
		
		// X section
			fname[ifname] = 'X';

			strcpy(sacdata.s[ess_kcmpnm], "BN1");  // component name (axis)

			fTemp = 90.0f;   // cmpinc, which should be 0 for z, 90 for x & y, and -12345.0f for sig.
			float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_cmpinc]);

			fTemp = 90.0f;   // cmpaz, X would be true east ideally (N+90 degrees) cmpaz = 90
			float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_cmpaz]);

			fTemp = xmin;
			float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_depmin]);  // min value of independent variable

			fTemp = xmax;
			float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_depmax]);  // max value of independent variable

			zfl.push_back(fname);
			wsac0(fname, t, x, nerr, npts, &sacdata);

			// Y section
			fname[ifname] = 'Y';

			strcpy(sacdata.s[ess_kcmpnm], "BN2");  // component name (axis)

			fTemp = 90.0f;   // cmpinc, which should be 0 for z, 90 for x & y, and -12345.0f for sig.
			float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_cmpinc]);

			fTemp = 0.0f;   // cmpaz, Y should be true north, cmpaz = 0
			float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_cmpaz]);

			fTemp = ymin;
			float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_depmin]);  // min value of independent variable

			fTemp = ymax;
			float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_depmax]);  // max value of independent variable
		 
			zfl.push_back(fname);
			wsac0(fname, t, y, nerr, npts, &sacdata);

			// Z section
			fname[ifname] = 'Z';

			strcpy(sacdata.s[ess_kcmpnm], "BN3");  // component name (axis)

			fTemp = 0.0f;   // cmpinc, which should be 0 for z, 90 for x & y, and -12345.0f for sig.
			float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_cmpinc]);

			fTemp = 0.0f;   // cmpaz, Z cmpaz = 0
			float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_cmpaz]);

			fTemp = zmin;
			float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_depmin]);  // min value of independent variable

			fTemp = zmax;
			float_swap((QCN_CBYTE*) &fTemp, sacdata.f[esf_depmax]);  // max value of independent variable

			zfl.push_back(fname);

			wsac0(fname, t, z, nerr, npts, &sacdata);

			if (zfl.size()>0) {
			   // now make sure the zip file is stored in strZipPath + ti.strFile
			   strZip += qcn_util::cPathSeparator();
			   strZip += (const char*) ti.strFile;
			   boinc_delete_file(strZip.c_str());  // we may be overwriting, delete first

		#ifdef ZIPARCHIVE
			   CZipArchive ziparch;
			   bool bRetVal = false;
			   try {

				 bRetVal = ziparch.Open(strZip.c_str(), CZipArchive::zipCreate);
				 if (!bRetVal) {
					fprintf(stdout, "Could not create zip archive %s!\n", strZip.c_str());
				 }
				 for (unsigned int i = 0; bRetVal && i < zfl.size(); i++) {
					 bRetVal = ziparch.AddNewFile(zfl[i].c_str(), -1, false);
				 }
				 ziparch.Close(CZipArchive::afWriteDir);
			   }
			   catch(...)
			   { // throws exception on close error, but will write anyway
				  bRetVal = false;
			   }
			   if (!bRetVal) {
				 fprintf(stdout, "sac.cpp: CZip file error %d\n", 1);
				 fflush(stdout);
			   }
		#else  // traditional boinc_zip...
			   int iRetVal = boinc_zip(ZIP_IT, strZip, &zfl);
			   if (iRetVal) {
				 fprintf(stdout, "sac.cpp: Zip file error # %d\n", iRetVal);
				 fflush(stdout);
			   }
		#endif
			}

			// now delete the original files
			for (int ii = 0; ii < (int) zfl.size(); ii++)  {
				boinc_delete_file(zfl[ii].c_str());
			}
	}
	else { // CSV output
		strlcat(fname, ".csv", _MAX_PATH);
		nerr = 0;
		wcsv0(fname, t, x, y, z, nerr, npts, &sacdata, dTimeZero);	
    }  // sac vs csv i/o

	
	
#ifdef _DEBUG   // lots of output below!
	FILE* fileDebug = fopen("sacio.csv", "at");
	if (fileDebug) {
		float_swap((QCN_CBYTE*) &fTimeTrigger, fTemp);  // trigger time offset
		fprintf(fileDebug, "sizeof(float)=%d, sizeof(double)=%d, big_endian=%c, fTimeTrigger=%f\n", 
				(int) sizeof(float), (int) sizeof(double), (qcn_main::g_endian == ENDIAN_BIG) ? 'T' : 'F', fTemp);
		fprintf(fileDebug, "xmin=%f, xmax=%f\n", xmin, xmax);
		fprintf(fileDebug, "ymin=%f, ymax=%f\n", ymin, ymax);
		fprintf(fileDebug, "zmin=%f, zmax=%f\n", zmin, zmax);
		fprintf(fileDebug, "smin=%f, smax=%f\n", smin, smax);
		fprintf(fileDebug, "n1=%d, n2=%d, npts=%d, dt=%f\nt, x, y, z, s\n", n1, n2, npts, sm->dt);
		for (j = 0; j < npts; j++) {
			float ft[5];
			float_swap((QCN_CBYTE*) &t[j], ft[0]);
			float_swap((QCN_CBYTE*) &x[j], ft[1]);
			float_swap((QCN_CBYTE*) &y[j], ft[2]);
			float_swap((QCN_CBYTE*) &z[j], ft[3]);
			float_swap((QCN_CBYTE*) &s[j], ft[4]);
			fprintf(fileDebug, "%f, %f, %f, %f, %f\n",
					ft[0], ft[1], ft[2], ft[3], ft[4]
					);
		}
		fclose(fileDebug);
	}
#endif
	
	
    delete [] x;
    delete [] y;
    delete [] z;
    delete [] s;
    delete [] t;

#ifdef QCNLIVE  // write a message to be picked up on the display
	char* strTmp = new char[512];
	memset(strTmp, 0x00, sizeof(char) * 512);
	sprintf(strTmp, "%s file written to %s",  (ti.bReal ? "Trigger" : "Recorded"), (sm->bMyOutputSAC ? strZip.c_str() : fname));
	strncpy(sm->strDisplay, strTmp, _MAX_PATH);
	delete [] strTmp;
#endif
	
    boinc_end_critical_section();
    return 0;
}

/* format of the csv file:
 
 Station Name: SNAME
 Longitude: -117.12321
 Latitude: 34.92834
 Elevation: 239
 Floor: 0
 File Start Time: 11/07/2008 12:34:20
 File End Time: 11/07/2008 12:35:40
 Time, x, y, z
 0.000, -1.293, 2.393, -0.123
 0.020, -12.129, 129.010, 19.010
 0.040, -8.010, 1.001, -0.999

 wcsv0(fname, t, x, y, z, nerr, npts, &sacdata);	

 */
	
// a csv version of the sac output
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
)
{
	FILE* fCSV;
	nerr = 0;
	fCSV = boinc_fopen(fname, "wt");
	if (!fCSV) {
		nerr = -1;
		return;
	}

    // use dTimeZero for the reference time
    struct timeval tv;
    time_t ttime = (time_t) dTimeZero;
    struct tm* ptm = gmtime(&ttime);
    tv.tv_sec =  (int32_t) ttime;
    tv.tv_usec = (int32_t) ((dTimeZero - (double) tv.tv_sec) * 1.0e6f);
	int iYear[2], iMonth[2], iDay[2], iHour[2], iMin[2], iSec[2];
	iYear[0] = 1900+ptm->tm_year;
	iMonth[0] = ptm->tm_mon+1;
	iDay[0] = ptm->tm_mday;
	iHour[0] = ptm->tm_hour;
	iMin[0] = ptm->tm_min;
	iSec[0] = ptm->tm_sec;
	
    struct timeval tve;
	double dTimeEnd = dTimeZero + t[npts-1];
	time_t ttimee = (time_t) dTimeEnd;
    ptm = gmtime(&ttimee);
    tve.tv_sec =  (int32_t) ttimee;
    tve.tv_usec = (int32_t) ((dTimeEnd - (double) tve.tv_sec) * 1.0e6f);
	iYear[1] = 1900+ptm->tm_year;
	iMonth[1] = ptm->tm_mon+1;
	iDay[1] = ptm->tm_mday;
	iHour[1] = ptm->tm_hour;
	iMin[1] = ptm->tm_min;
	iSec[1] = ptm->tm_sec;
		
	fprintf(fCSV, "Station Name,  %s\n"
				  "Latitude,      %f\n"
				  "Longitude,     %f\n"
			      "Elevation (m), %f\n"
				  "Floor,         %d\n"
				  "Sensor,        %s\n"
			      "Start Time,    %02d-%02d-%4d,  %02d:%02d:%02d.%03ld   ,  %f\n"
				  "End Time  ,    %02d-%02d-%4d,  %02d:%02d:%02d.%03ld   ,  %f\n"
			      "Time, x, y, z\n"
			,
			      sm->strMyStation,
				  sm->dMyLatitude,
			      sm->dMyLongitude,
			      sm->dMyElevationMeter,
				  sm->iMyElevationFloor,
				  psacdata->s[ess_kinst],
			      iMonth[0], iDay[0], iYear[0], iHour[0], iMin[0], iSec[0], tv.tv_usec/1000L,  dTimeZero,
			      iMonth[1], iDay[1], iYear[1], iHour[1], iMin[1], iSec[1], tve.tv_usec/1000L, dTimeEnd
			);			
			
	for (int i = 0; i < npts; i++) {
		fprintf(fCSV, "%f,%f,%f,%f\n", t[i], x[i], y[i], z[i]);
	}
	fclose(fCSV);
}

	
}  // namespace sacio
