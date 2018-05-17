#include "main.h"

void checkBOINCStatus()
{
        if (!sm || boinc_is_standalone()) return;
        static bool bInHere = false;
        if (bInHere) return;
        bInHere = true;
        // just check the sm->statusBOINC;
        if (sm->statusBOINC.quit_request || sm->statusBOINC.no_heartbeat || sm->statusBOINC.abort_request) {
                fprintf(stderr, "QCN Monitor - %s from BOINC...\n",
                        sm->statusBOINC.no_heartbeat ? "No 'heartbeat'" :
                                sm->statusBOINC.quit_request ?
                                        "Quit request" : "Abort request"
                );
                fflush(stderr);

                // if it was quit_request == 2, really cleanup, i.e. abort workunit
                if (sm->statusBOINC.abort_request)
                {
                    boinc_finish(0); // never returns
                }

/*
#ifdef __APPLE_CC__
                _exit(0);
#else
                exit(0);
#endif
*/
        }
        bInHere = false;
        return;

/*
        BOINC_STATUS s;
        boinc_get_status(&s);
        if (s.quit_request || s.no_heartbeat || s.abort_request) {
                fprintf(stderr, "CPDN Monitor - %s from BOINC...\n",
                        s.no_heartbeat ? "No 'heartbeat'" :
                                s.quit_request ?
                                        "Quit request" : "Abort request"
                );
                fflush(stderr);

                // if it was quit_request == 2, really cleanup, i.e. abort workunit
                if (s.abort_request)
                {
                        finished(10); // never returns
                }

        // need some final cleanup, if aborted it was caught in the above finished(10)
                cleanupModel(true);
                app_cleanup(); // cleanup graphics etc
                //if (g_fileOut) fclose(g_fileOut);
                //if (g_fileErr) fclose(g_fileErr);
#ifdef __APPLE_CC__
                        _ex(0);
#else
                        ex(0);
#endif
        }
        else { // check for suspend state
                if (s.suspended) {
                        if (bModelRunning) { // model is running, better suspend it
#if defined(_WIN32) || defined(_WIN64)
                                SuspendThread(g_handleThread);  // pause thread
        #ifdef PRECIS
                                SuspendThread(g_handleThreadPrecis);  // pause thread
        #endif
#else
                                kill(g_handleProcess, SIGSTOP);
        #ifdef PRECIS
                                kill(g_handleProcessPrecis, SIGSTOP);  // pause thread
        #endif
#endif
                        }
                        // go into a pseudo-suspended loop, the model is really suspended
                        // but the monitor stays alive and checks status every second for boinc quit
                        // messages or of course a resume message, and acts accordingly
                        while (s.suspended) {
                                //fprintf(stdout, "CPDN Monitor suspended...\n");
                                boinc_sleep(LOOP_SLEEP_TIME);
                                boinc_get_status(&s);
                                if (s.quit_request || s.no_heartbeat || s.abort_request) {
                                        //whoops, time to quit!
                                        // stop model, cleanup shared memory and exit
                                        fprintf(stderr, "Suspended CPDN Monitor - %s from BOINC...\n",
                                                s.no_heartbeat ? "No 'heartbeat'" :
                                                        s.quit_request ?
                                                                "Quit request" : "Abort request"
                                        );
                                        fflush(stderr);

                                        if (s.abort_request)
                                        {
                                                finished(11); // never returns, no heartbeat & quit request continue below
                                        }

                        // need some final cleanup, if aborted it was caught in the above finished(10)
                                        cleanupModel(true);
                                        app_cleanup(); // cleanup graphics etc
                                        //if (g_fileOut) fclose(g_fileOut);
                                        //if (g_fileErr) fclose(g_fileErr);

#ifdef __APPLE_CC__
                                        _ex(0);
#else
                                        ex(0);
#endif
                                }
                        }
                        // now get out of suspend state
                        fprintf(stdout, "Resuming CPDN!\n");
                        if (bModelRunning)  { //have to start model back up
#if defined(_WIN32) || defined(_WIN64)
                                ::ResumeThread(g_handleThread);
        #ifdef PRECIS
                                ::ResumeThread(g_handleThreadPrecis);
        #endif
#else
                                kill(g_handleProcess, SIGCONT);

        #ifdef PRECIS
                                kill(g_handleProcessPrecis, SIGCONT);  // resume
        #endif
#endif
                        }
                }
        } // if conditional -- check for exit or suspend

        if (bModelRunning) {
                static int iCheck = 0;
                iCheck++;
                // need to check that the model actually is running
                // (the monitor thinks model running if bModelRunning)
                if (iCheck>5) {
                        check_cpdn_running(pShMem->apidCPDN[PROCESS_MONITOR]);
                        iCheck = 0;
                }
        }

        bInHere = false;

*/

}

