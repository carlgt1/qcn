
#ifdef _WIN32
   #include "qcn_config_win.h"
#else
   #include "config.h"
#endif

#include "trickleup.h"
#include "filesys.h"

extern double dtime();  // this is from boinc lib
extern void boinc_sleep(double);   

enum e_trigvariety { TRIGGER_VARIETY_FINALSTATS = -2, TRIGGER_VARIETY_QUAKELIST, TRIGGER_VARIETY_NORMAL, TRIGGER_VARIETY_PING, TRIGGER_VARIETY_CONTINUAL };

namespace trickleup {

void qcnTrickleUp(const char* strTrickle, const int iVariety, const char* strWU)
{
	// probably not thread safe, but within a same proc/thread (i.e. main.cpp) we should not have trickles/triggers sent within a second because BOINC
	// can't handle them that fast (i.e. writes out a trickle file name with a timestamp to the nearest second, and trickles within a second
	// write over each other							

	static double dTimeLastTrickle = 0.0;  // set time trickle sent so we can pause if necessary
	
	
	
#ifdef QCNLIVE
  return; // no trickles on gui!
#else

	// just a "sanity check" to make sure we haven't sent too many trickles in a row (i.e. > 1 sec)

	//trickleup::qcnCheckTrickleSleep();
	double dCurTime = dtime();
	if ((dCurTime - dTimeLastTrickle) < 1.0) { // last trickle sent less than a second ago, which would be bad for boinc, so sleep a bit)
		boinc_sleep((dCurTime - dTimeLastTrickle) + .10);
	}
	
   char strVariety[32];
   memset(strVariety, 0x00, 32);
   strcpy(strVariety, "trigger");
   switch (iVariety) {
      case TRIGGER_VARIETY_FINALSTATS:
        strcpy(strVariety, "finalstats"); break;
      case TRIGGER_VARIETY_QUAKELIST:
        strcpy(strVariety, "quakelist"); break;
      case TRIGGER_VARIETY_NORMAL:
        strcpy(strVariety, "trigger"); break;
      case TRIGGER_VARIETY_PING:
        strcpy(strVariety, "ping"); break;
      case TRIGGER_VARIETY_CONTINUAL:
        strcpy(strVariety, "continual"); break;
   }


        // BOINC adds the appropriate workunit/resultid etc and posts to trickle_up table in MySQL
        static bool bInHere = false;
        if (bInHere) return;
        bInHere = true;

        // CMC let's print out trickles in standalone mode so I can see something!
        if (boinc_is_standalone()) {
           char szTFile[32];
           static unsigned long iNum = 0L;
           iNum++;
           sprintf(szTFile, "trickle_%09lu_%s.xml", (unsigned long) iNum, strVariety);
           FILE* fTrickle = boinc_fopen(szTFile, "w");
           if (fTrickle) {
             fwrite(strTrickle, 1, strlen(strTrickle), fTrickle);
             fclose(fTrickle);
           }
        }
        else {
           boinc_send_trickle_up((char*) strVariety, (char*) strTrickle);
        }
        bInHere = false;
#endif
	
	dTimeLastTrickle = dtime();  // set time trickle sent so we can pause if necessary

}

} // namespace trickleup

