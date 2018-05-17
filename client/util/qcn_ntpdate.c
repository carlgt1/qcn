/*
 * ntpdate - set the time of day by polling one or more NTP servers
*/

/*  CMC modified to call from a thread in QCN rather than external process */
/* to hopefully suppress including the QCN config.h */
#define QCN_CONFIG_H   
#define NO_MAIN_ALLOWED

/* ensure that this config.h is really ../../ntp-4.2.4p5/config.h in the Makefile! */
/* namely -I../../ntp-4.2.4p5 -I../../ntp-4.2.4p5/include -I../../ntp-4.2.4p5/ntpdate */

/* arguments used by qcn:
   "usage: %s [-46bBdqsuv] [-a key#] [-e delay] [-k file] [-p samples] [-o version#] [-t timeo] server ...\n",
     -p 8 -t 20 -u -b -q qcn-upl.stanford.edu
  bypass the -b?
*/

#ifdef HAVE_CONFIG_H
#ifdef _WIN32
#include <config.h>   // go to the special windows build ntp-4.2.4p/port/winnt

// CMC add the freeaddrinfo() & getaddrinfo() & in6addr() functions for windows
// so don't have to link to ws2_32.lib/dll (Winsock2) which may not be
// found on Windows 2000 & older Windows
// forward declarations for Win32

void freeaddrinfo_win(struct addrinfo *ai);
int getaddrinfo_win(const char *nodename, const char *servname,
	const struct addrinfo *hints, struct addrinfo **res);
static int do_nodename(
	const char *nodename,
	struct addrinfo *ai,
	const struct addrinfo *hints);
int DNSlookup_name(
	const char *name,
	int ai_family,
	struct hostent **Addresses
);
int getnameinfo_win(const struct sockaddr *sa, u_int salen, char *host,
	size_t hostlen, char *serv, size_t servlen, int flags);

#else
#include "../../ntp-4.2.4p5/config.h"
#endif
#endif

#ifdef HAVE_NETINFO
#undef HAVE_NETINFO
#endif // bypass netinfo data, if any

/* also note below -- CMC note changed verbose to ntpdate_verbose since verbose is a symbol in the boinc libs */
/*    another mod - msyslog(LOG_NOTICE, "%s", QCN_VERSION_STRING);   CMC changed from Version to QCN_VERSION_STRING */
/* end CMC mods */


#ifdef HAVE_NETINFO
#include <netinfo/ni.h>
#endif

#include "ntp_machine.h"
#include "ntp_fp.h"
#include "ntp.h"
#include "ntp_io.h"
#include "ntp_unixtime.h"
#include "ntpdate.h"
#include "ntp_string.h"
#include "ntp_syslog.h"
#include "ntp_select.h"
#include "ntp_stdlib.h"

/* Don't include ISC's version of IPv6 variables and structures */
#define ISC_IPV6_H 1
#include "isc/net.h"
#include "isc/result.h"
#include "isc/sockaddr.h"

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#ifdef HAVE_POLL_H
# include <poll.h>
#endif
#ifndef SYS_WINNT
# ifdef HAVE_SYS_SIGNAL_H
#  include <sys/signal.h>
# else
#  include <signal.h>
# endif
# ifdef HAVE_SYS_IOCTL_H
#  include <sys/ioctl.h>
# endif
#endif /* SYS_WINNT */
#ifdef HAVE_SYS_RESOURCE_H
# include <sys/resource.h>
#endif /* HAVE_SYS_RESOURCE_H */

#include <arpa/inet.h>

#ifdef SYS_VXWORKS
# include "ioLib.h"
# include "sockLib.h"
# include "timers.h"

/* select wants a zero structure ... */
struct timeval timeout = {0,0};
#elif defined(SYS_WINNT)
/*
 * Windows does not abort a select select call if SIGALRM goes off
 * so a 200 ms timeout is needed
 */
struct timeval timeout = {0,1000000/TIMER_HZ};
#else
struct timeval timeout = {60,0};
#endif

#ifdef HAVE_NETINFO
#include <netinfo/ni.h>
#endif

#include "recvbuff.h"

#ifdef SYS_WINNT
#define EPROTONOSUPPORT WSAEPROTONOSUPPORT
#define EAFNOSUPPORT	WSAEAFNOSUPPORT
#define EPFNOSUPPORT	WSAEPFNOSUPPORT
#define TARGET_RESOLUTION 1  /* Try for 1-millisecond accuracy
				on Windows NT timers. */
#pragma comment(lib, "winmm")
isc_boolean_t ntp_port_inuse(int af, u_short port);
UINT wTimerRes;
#endif /* SYS_WINNT */

/*
 * Scheduling priority we run at
 */
#ifndef SYS_VXWORKS
# define	NTPDATE_PRIO	(-12)
#else
# define	NTPDATE_PRIO	(100)
#endif

#if defined(HAVE_TIMER_SETTIME) || defined (HAVE_TIMER_CREATE)
/* POSIX TIMERS - vxWorks doesn't have itimer - casey */
static timer_t ntpdate_timerid;
#endif

/*
 * Compatibility stuff for Version 2
 */
#define NTP_MAXSKW	0x28f	/* 0.01 sec in fp format */
#define NTP_MINDIST	0x51f	/* 0.02 sec in fp format */
#define PEER_MAXDISP	(64*FP_SECOND)	/* maximum dispersion (fp 64) */
#define NTP_INFIN	15	/* max stratum, infinity a la Bellman-Ford */
#define NTP_MAXWGT	(8*FP_SECOND)	/* maximum select weight 8 seconds */
#define NTP_MAXLIST	5	/* maximum select list size */
#define PEER_SHIFT	8	/* 8 suitable for crystal time base */

/*
 * for get_systime()
 */
s_char	sys_precision;		/* local clock precision (log2 s) */

/*
 * Debugging flag
 */
volatile int debug = 0;

/*
 * File descriptor masks etc. for call to select
 */

int ai_fam_templ;
int nbsock;             /* the number of sockets used */
SOCKET fd[MAX_AF];
int fd_family[MAX_AF];	/* to remember the socket family */
#ifdef HAVE_POLL_H
struct pollfd fdmask[MAX_AF];
#else
fd_set fdmask;
SOCKET maxfd;
#endif
int polltest = 0;

/*
 * Initializing flag.  All async routines watch this and only do their
 * thing when it is clear.
 */
int initializing = 1;

/*
 * Alarm flag.	Set when an alarm occurs
 */
volatile int alarm_flag = 0;

/*
 * Simple query flag.
 */
int simple_query = 0;

/*
 * Unprivileged port flag.
 */
int unpriv_port = 0;

/*
 * Program name.
 */
char *progname;

/*
 * Systemwide parameters and flags
 */
int sys_samples = DEFSAMPLES;	/* number of samples/server */
u_long sys_timeout = DEFTIMEOUT; /* timeout time, in TIMER_HZ units */
struct server *sys_servers;	/* the server list */
int sys_numservers = 0; 	/* number of servers to poll */
int sys_authenticate = 0;	/* true when authenticating */
u_int32 sys_authkey = 0;	/* set to authentication key in use */
u_long sys_authdelay = 0;	/* authentication delay */
int sys_version = NTP_VERSION;	/* version to poll with */

/*
 * The current internal time
 */
u_long current_time = 0;

/*
 * Counter for keeping track of completed servers
 */
int complete_servers = 0;

/*
 * File of encryption keys
 */

#ifndef KEYFILE
# ifndef SYS_WINNT
#define KEYFILE 	"/etc/ntp.keys"
# else
#define KEYFILE 	"%windir%\\ntp.keys"
# endif /* SYS_WINNT */
#endif /* KEYFILE */

#ifndef SYS_WINNT
const char *key_file = KEYFILE;
#else
char key_file_storage[MAX_PATH+1], *key_file ;
#endif	 /* SYS_WINNT */

/*
 * Miscellaneous flags
 */
/* CMC note changed verbose to ntpdate_verbose since verbose is a symbol in the boinc libs */
int ntpdate_verbose = 0;
int always_step = 0;
int never_step = 0;

/* CMC added a pointer to double pdTimeOffset this function can set */
/* also note the const bool pointer which references the qcn_main::g_bStop var to check if we need to exit */
int ntpdatemain P((double volatile *, const int volatile *, int, char **));

static	void	transmit	P((struct server *));
static	void	receive 	P((struct recvbuf *));
static	void	server_data P((struct server *, s_fp, l_fp *, u_fp));
static	void	clock_filter	P((struct server *));
static	struct server *clock_select P((const int volatile *));
/* CMC added a pointer to double pdTimeOffset this function can set */
static	int clock_adjust	P((double volatile *, const int volatile *));
static	int addserver	P((volatile const int *, char *));   /* CMC changed to return int and added param to check for stop within this function */
static	struct server *findserver P((struct sockaddr_storage *));
void	timer	P((void));
static	void	init_alarm	P((void));
#ifndef SYS_WINNT
static	RETSIGTYPE alarming P((int));
#endif /* SYS_WINNT */
static	void	init_io 	P((void));
static	void	sendpkt 	P((struct sockaddr_storage *, struct pkt *, int));
void	input_handler	P((void));

static	int l_adj_systime	P((l_fp *));
static	int l_step_systime	P((l_fp *));

/* CMC took out
static	void	printserver P((struct server *, FILE *));
*/

#ifdef SYS_WINNT
int 	on = 1;
WORD	wVersionRequested;
WSADATA	wsaData;
HANDLE	TimerThreadHandle = NULL;
#endif /* SYS_WINNT */

#ifdef NO_MAIN_ALLOWED

/* CALL(ntpdate,"ntpdate",ntpdatemain);  CMC let's take this out */

void clear_globals()
{
  /*
   * Debugging flag
   */
  debug = 0;

  ntp_optind = 0;
  /*
   * Initializing flag.  All async routines watch this and only do their
   * thing when it is clear.
   */
  initializing = 1;

  /*
   * Alarm flag.  Set when an alarm occurs
   */
  alarm_flag = 0;

  /*
   * Simple query flag.
   */
  simple_query = 0;

  /*
   * Unprivileged port flag.
   */
  unpriv_port = 0;

  /*
   * Systemwide parameters and flags
   */
  sys_numservers = 0;	  /* number of servers to poll */
  sys_authenticate = 0;   /* true when authenticating */
  sys_authkey = 0;	   /* set to authentication key in use */
  sys_authdelay = 0;   /* authentication delay */
  sys_version = NTP_VERSION;  /* version to poll with */

  /*
   * The current internal time
   */
  current_time = 0;

  /*
   * Counter for keeping track of completed servers
   */
  complete_servers = 0;
  ntpdate_verbose = 0;
  always_step = 0;
  never_step = 0;
}
#endif

#ifdef HAVE_NETINFO
static ni_namelist *getnetinfoservers P((void));
#endif

/*
 * Main program.  Initialize us and loop waiting for I/O and/or
 * timer expiries.
 */
#ifndef NO_MAIN_ALLOWED
int
main(
	int argc,
	char *argv[]
	)
{
	return ntpdatemain (argc, argv);
}
#endif /* NO_MAIN_ALLOWED */

/* CMC note I added a pointer-to-double here so we can return the offset time */
/* also note the pointer to const bool so I can get the qcn_main::g_bStop value and return gracefully if shutting down in the middle of a call */
int
ntpdatemain (
	double volatile *pdTimeOffset,
	const int volatile *pbStop,
    int argc,
	char *argv[]
)
{
	int was_alarmed;
	int tot_recvbufs;
	struct recvbuf *rbuf;
	l_fp tmp;
	int errflg;
	int c;
	int nfound;

/*
#ifdef HAVE_NETINFO
	ni_namelist *netinfoservers;
#endif
*/

#ifdef SYS_WINNT
	HANDLE process_handle; 

	wVersionRequested = MAKEWORD(1,1);
	if (WSAStartup(wVersionRequested, &wsaData)) {
		netsyslog(LOG_ERR, "No useable winsock.dll: %m");
		return 1; /* CMC here - changed exit to return */
	}

	key_file = key_file_storage;

	if (*pbStop) { return -1; }  /* CMC stop requested */

	if (!ExpandEnvironmentStrings(KEYFILE, key_file, MAX_PATH))
	{
		msyslog(LOG_ERR, "ExpandEnvironmentStrings(KEYFILE) failed: %m\n");
	}
#endif /* SYS_WINNT */

#ifdef NO_MAIN_ALLOWED
	clear_globals();
#endif

	if (*pbStop) { return -1; }  /* CMC stop requested */

	/* Check to see if we have IPv6. Otherwise force the -4 flag */
	if (isc_net_probeipv6() != ISC_R_SUCCESS) {
		ai_fam_templ = AF_INET;
	}

	errflg = 0;
	progname = argv[0];
	syslogit = 0;

	/*
	 * Decode argument list
	 */
	while ((c = ntp_getopt(argc, argv, "46a:bBde:k:o:p:qst:uv")) != EOF)
		switch (c)
		{
		case '4':
			ai_fam_templ = AF_INET;
			break;
		case '6':
			ai_fam_templ = AF_INET6;
			break;
		case 'a':
			c = atoi(ntp_optarg);
			sys_authenticate = 1;
			sys_authkey = c;
			break;
		case 'b':
//		        always_step++;
//		        never_step = 0;
			break;
		case 'B':
	//		never_step++;
//			always_step = 0;
			break;
		case 'd':
			++debug;
			break;
		case 'e':
			if (!atolfp(ntp_optarg, &tmp)
			|| tmp.l_ui != 0) {
				(void) fprintf(stderr,
					   "%s: encryption delay %s is unlikely\n",
					   progname, ntp_optarg);
				errflg++;
			} else {
				sys_authdelay = tmp.l_uf;
			}
			break;
		case 'k':
			key_file = ntp_optarg;
			break;
		case 'o':
			sys_version = atoi(ntp_optarg);
			break;
		case 'p':
			c = atoi(ntp_optarg);
			if (c <= 0 || c > NTP_SHIFT) {
				(void) fprintf(stderr,
					   "%s: number of samples (%d) is invalid\n",
					   progname, c);
				errflg++;
			} else {
				sys_samples = c;
			}
			break;
		case 'q':
			simple_query = 1;
			break;
		case 's':
			syslogit = 1;
			break;
		case 't':
			if (!atolfp(ntp_optarg, &tmp)) {
				(void) fprintf(stderr,
					   "%s: timeout %s is undecodeable\n",
					   progname, ntp_optarg);
				errflg++;
			} else {
				sys_timeout = ((LFPTOFP(&tmp) * TIMER_HZ)
					   + 0x8000) >> 16;
				if (sys_timeout == 0)
				sys_timeout = 1;
			}
			break;
		case 'v':
			ntpdate_verbose = 0;  /* CMC here -- supress verbose */
			break;
		case 'u':
			unpriv_port = 1;
			break;
		case '?':
			++errflg;
			break;
		default:
			break;
	    }

        /* CMC HERE	
        fprintf(stdout, "ntpdate settings timeout=%d  verbose=%d  unpriv=%d  simple_query=%d  always_step=%d  never_step=%d\n",
                sys_timeout, ntpdate_verbose, unpriv_port, simple_query, always_step, never_step
        );
        fflush(stdout); */
	if (*pbStop) { return -1; }  /* CMC stop requested */

	if (errflg) {
		(void) fprintf(stderr,
		    "usage: %s [-46bBdqsuv] [-a key#] [-e delay] [-k file] [-p samples] [-o version#] [-t timeo] server ...\n",
		    progname);
		return 2; /* CMC here - changed exit to return */
	}

	if (debug || simple_query) {
#ifdef HAVE_SETVBUF
		static char buf[BUFSIZ];
#ifdef SYS_WINNT
		/* Win32 does not implement line buffering */
		setvbuf(stdout, NULL, _IONBF, BUFSIZ);
#else
		setvbuf(stdout, buf, _IOLBF, BUFSIZ);
#endif	/* SYS_WINNT */
#else
		setlinebuf(stdout);
#endif
	}

	if (*pbStop) { return -1; }  /* CMC stop requested */

#if 0
	/* CMC REMOVED LOGGING
	 * Logging.  Open the syslog if we have to
	 */
	if (syslogit) {
#if !defined (SYS_WINNT) && !defined (SYS_VXWORKS) && !defined SYS_CYGWIN32
# ifndef	LOG_DAEMON
		openlog("ntpdate", LOG_PID);
# else

#  ifndef	LOG_NTP
#	define	LOG_NTP LOG_DAEMON
#  endif
		openlog("ntpdate", LOG_PID | LOG_NDELAY, LOG_NTP);
		if (debug)
			setlogmask(LOG_UPTO(LOG_DEBUG));
		else
			setlogmask(LOG_UPTO(LOG_INFO));
# endif /* LOG_DAEMON */
#endif	/* SYS_WINNT */
	}
#endif

/*
  CMC changed from Version to QCN_VERSION_STRING 
	if (debug || ntpdate_verbose)
		msyslog(LOG_NOTICE, "%s", QCN_VERSION_STRING);   
*/

	/* CMC commented out netinfo stuff
	 * Add servers we are going to be polling
#ifdef HAVE_NETINFO
	netinfoservers = getnetinfoservers();
#endif
	 */

	if (*pbStop) { return -1; }  /* CMC stop requested */
	for ( ; ntp_optind < argc; ntp_optind++) {
		int iRetVal = addserver(pbStop, argv[ntp_optind]);
	if (*pbStop || iRetVal == -1) { return -1; }  /* CMC stop requested */
        }

#ifdef HAVE_NETINFO
	if (netinfoservers) {
		if ( netinfoservers->ni_namelist_len &&
		    *netinfoservers->ni_namelist_val ) {
			u_int servercount = 0;
			while (servercount < netinfoservers->ni_namelist_len) {
				if (debug) msyslog(LOG_DEBUG,
						   "Adding time server %s from NetInfo configuration.",
						   netinfoservers->ni_namelist_val[servercount]);
				addserver(netinfoservers->ni_namelist_val[servercount++]);
             /* CMC here - print out server data 
   fprintf(stdout, "   ntpdate netinfo server: %s\n", netinfoservers->ni_namelist_val[servercount]);
   fflush(stdout); */
			}
		}
		ni_namelist_free(netinfoservers);
		free(netinfoservers);
	}
#endif
	if (sys_numservers == 0) {
		msyslog(LOG_ERR, "no servers can be used, exiting");
		return 1; /* CMC here - changed exit to return */
	}

	/*
	 * Initialize the time of day routines and the I/O subsystem
	 */
	if (*pbStop) { return -1; }  /* CMC stop requested */
	if (sys_authenticate) {
		init_auth();
		if (!authreadkeys(key_file)) {
			msyslog(LOG_ERR, "no key file <%s>, exiting", key_file);
		        return 1; /* CMC here - changed exit to return */
		}
		authtrust(sys_authkey, 1);
		if (!authistrusted(sys_authkey)) {
			msyslog(LOG_ERR, "authentication key %lu unknown",
				(unsigned long) sys_authkey);
		        return 1; /* CMC here - changed exit to return */
		}
	}
	if (*pbStop) { return -1; }  /* CMC stop requested */
	init_io();
	if (*pbStop) { return -1; }  /* CMC stop requested */
	init_alarm();
	if (*pbStop) { return -1; }  /* CMC stop requested */

	/*  CMC bypass the priority!
	 * Set the priority.
#ifdef SYS_VXWORKS
	taskPrioritySet( taskIdSelf(), NTPDATE_PRIO);
#endif
#if defined(HAVE_ATT_NICE)
	nice (NTPDATE_PRIO);
#endif
#if defined(HAVE_BSD_NICE)
	(void) setpriority(PRIO_PROCESS, 0, NTPDATE_PRIO);
#endif
#ifdef SYS_WINNT
	process_handle = GetCurrentProcess();
	if (!SetPriorityClass(process_handle, (DWORD) REALTIME_PRIORITY_CLASS)) {
		msyslog(LOG_ERR, "SetPriorityClass failed: %m");
	}
#endif 
	 */
	if (*pbStop) { return -1; }  /* CMC stop requested */

	initializing = 0;
	was_alarmed = 0;

	while (complete_servers < sys_numservers) {
#ifdef HAVE_POLL_H
		struct pollfd* rdfdes;
		rdfdes = fdmask;
#else
		fd_set rdfdes;
		rdfdes = fdmask;
#endif

		if (alarm_flag) {		/* alarmed? */
			was_alarmed = 1;
			alarm_flag = 0;
		}
	if (*pbStop) { return -1; }  /* CMC stop requested */
		tot_recvbufs = full_recvbuffs();	/* get received buffers */

		if (!was_alarmed && tot_recvbufs == 0) {
			/*
			 * Nothing to do.	 Wait for something.
			 */
	        if (*pbStop) { return -1; }  /* CMC stop requested */
#ifdef HAVE_POLL_H
			/* CMC HERE, this timeout.tv_sec * 1000 seems odd? */
                        nfound = poll(rdfdes, (unsigned int)nbsock, timeout.tv_sec * 1000);
#else
			nfound = select((int) maxfd, &rdfdes, (fd_set *)0,
					(fd_set *)0, &timeout);
#endif
	if (*pbStop) { return -1; }  /* CMC stop requested */
			if (nfound > 0)
				input_handler();
			else if (nfound == SOCKET_ERROR)
			{
/* CMC comment out
#ifndef SYS_WINNT
				if (errno != EINTR)
#else
				if (WSAGetLastError() != WSAEINTR)
#endif
					netsyslog(LOG_ERR,
#ifdef HAVE_POLL_H
						"poll(1) error: %m"
#else
						"select() error: %m"
#endif
						);
*/
			}
/* CMC comment out
			} else if (errno != 0) {
#ifndef SYS_VXWORKS
				netsyslog(LOG_DEBUG,
#ifdef HAVE_POLL_H
					"poll(2): nfound = %d, error: %m",
#else
					"select(): nfound = %d, error: %m",
#endif
					nfound);
#endif
			}
*/
			if (alarm_flag) {		/* alarmed? */
				was_alarmed = 1;
				alarm_flag = 0;
			}
			tot_recvbufs = full_recvbuffs();	/* get received buffers */
         	if (*pbStop) { return -1; }  /* CMC stop requested */
		}

		/*
		 * Out here, signals are unblocked.  Call receive
		 * procedure for each incoming packet.
		 */
		rbuf = get_full_recv_buffer();
	if (*pbStop) { return -1; }  /* CMC stop requested */
		while (rbuf != NULL)
		{
	if (*pbStop) { return -1; }  /* CMC stop requested */
			receive(rbuf);
			freerecvbuf(rbuf);
			rbuf = get_full_recv_buffer();
		}

		/*
		 * Call timer to process any timeouts
		 */
		if (was_alarmed) {
	if (*pbStop) { return -1; }  /* CMC stop requested */
			timer();
			was_alarmed = 0;
		}

		/*
		 * Go around again
		 */
	if (*pbStop) { return -1; }  /* CMC stop requested */
	}

	/*
	 * When we get here we've completed the polling of all servers.
	 * Adjust the clock, then exit.
	 */
#ifdef SYS_WINNT
	WSACleanup();
#endif
#ifdef SYS_VXWORKS
	close (fd);
	timer_delete(ntpdate_timerid);
#endif
        *pdTimeOffset = 0.0f;   /* CMC note - initialize */
	if (*pbStop) { return -1; }  /* CMC stop requested */
	return clock_adjust(pdTimeOffset, pbStop);  /* CMC note we put the pointer-to-double here which will get set to a real offset if clock_adjust == 0 */
}


/*
 * transmit - transmit a packet to the given server, or mark it completed.
 *		This is called by the timeout routine and by the receive
 *		procedure.
 */
static void
transmit(
	register struct server *server
	)
{
	struct pkt xpkt;

	if (debug)
		printf("transmit(%s)\n", stoa(&(server->srcadr)));

	if (server->filter_nextpt < server->xmtcnt) {
		l_fp ts;
		/*
		 * Last message to this server timed out.  Shift
		 * zeros into the filter.
		 */
		L_CLR(&ts);
		server_data(server, 0, &ts, 0);
	}

	if ((int)server->filter_nextpt >= sys_samples) {
		/*
		 * Got all the data we need.  Mark this guy
		 * completed and return.
		 */
		server->event_time = 0;
		complete_servers++;
		return;
	}

	/*
	 * If we're here, send another message to the server.  Fill in
	 * the packet and let 'er rip.
	 */
	xpkt.li_vn_mode = PKT_LI_VN_MODE(LEAP_NOTINSYNC,
					 sys_version, MODE_CLIENT);
	xpkt.stratum = STRATUM_TO_PKT(STRATUM_UNSPEC);
	xpkt.ppoll = NTP_MINPOLL;
	xpkt.precision = NTPDATE_PRECISION;
	xpkt.rootdelay = htonl(NTPDATE_DISTANCE);
	xpkt.rootdispersion = htonl(NTPDATE_DISP);
	xpkt.refid = htonl(NTPDATE_REFID);
	L_CLR(&xpkt.reftime);
	L_CLR(&xpkt.org);
	L_CLR(&xpkt.rec);

	/*
	 * Determine whether to authenticate or not.	If so,
	 * fill in the extended part of the packet and do it.
	 * If not, just timestamp it and send it away.
	 */
	if (sys_authenticate) {
		int len;

		xpkt.exten[0] = htonl(sys_authkey);
		get_systime(&server->xmt);
		L_ADDUF(&server->xmt, sys_authdelay);
		HTONL_FP(&server->xmt, &xpkt.xmt);
		len = authencrypt(sys_authkey, (u_int32 *)&xpkt, LEN_PKT_NOMAC);
		sendpkt(&(server->srcadr), &xpkt, (int)(LEN_PKT_NOMAC + len));

		if (debug > 1)
			printf("transmit auth to %s\n",
			   stoa(&(server->srcadr)));
	} else {
		get_systime(&(server->xmt));
		HTONL_FP(&server->xmt, &xpkt.xmt);
		sendpkt(&(server->srcadr), &xpkt, LEN_PKT_NOMAC);

		if (debug > 1)
			printf("transmit to %s\n", stoa(&(server->srcadr)));
	}

	/*
	 * Update the server timeout and transmit count
	 */
	server->event_time = current_time + sys_timeout;
	server->xmtcnt++;
}


/*
 * receive - receive and process an incoming frame
 */
static void
receive(
	struct recvbuf *rbufp
	)
{
	register struct pkt *rpkt;
	register struct server *server;
	register s_fp di;
	l_fp t10, t23, tmp;
	l_fp org;
	l_fp rec;
	l_fp ci;
	int has_mac;
	int is_authentic;

	if (debug)
		printf("receive(%s)\n", stoa(&rbufp->recv_srcadr));
	/*
	 * Check to see if the packet basically looks like something
	 * intended for us.
	 */
	if (rbufp->recv_length == LEN_PKT_NOMAC)
		has_mac = 0;
	else if (rbufp->recv_length >= LEN_PKT_NOMAC)
		has_mac = 1;
	else {
		if (debug)
			printf("receive: packet length %d\n",
			   rbufp->recv_length);
		return; 		/* funny length packet */
	}

	rpkt = &(rbufp->recv_pkt);
	if (PKT_VERSION(rpkt->li_vn_mode) < NTP_OLDVERSION ||
		PKT_VERSION(rpkt->li_vn_mode) > NTP_VERSION) {
		return;
	}

	if ((PKT_MODE(rpkt->li_vn_mode) != MODE_SERVER
		 && PKT_MODE(rpkt->li_vn_mode) != MODE_PASSIVE)
		|| rpkt->stratum >= STRATUM_UNSPEC) {
		if (debug)
			printf("receive: mode %d stratum %d\n",
			   PKT_MODE(rpkt->li_vn_mode), rpkt->stratum);
		return;
	}

	/*
	 * So far, so good.  See if this is from a server we know.
	 */
	server = findserver(&(rbufp->recv_srcadr));
	if (server == NULL) {
		if (debug)
			printf("receive: server not found\n");
		return;
	}

	/*
	 * Decode the org timestamp and make sure we're getting a response
	 * to our last request.
	 */
	NTOHL_FP(&rpkt->org, &org);
	if (!L_ISEQU(&org, &server->xmt)) {
		if (debug)
			printf("receive: pkt.org and peer.xmt differ\n");
		return;
	}

	/*
	 * Check out the authenticity if we're doing that.
	 */
	if (!sys_authenticate)
		is_authentic = 1;
	else {
		is_authentic = 0;

		if (debug > 3)
			printf("receive: rpkt keyid=%ld sys_authkey=%ld decrypt=%ld\n",
			   (long int)ntohl(rpkt->exten[0]), (long int)sys_authkey,
			   (long int)authdecrypt(sys_authkey, (u_int32 *)rpkt,
				LEN_PKT_NOMAC, (int)(rbufp->recv_length - LEN_PKT_NOMAC)));

		if (has_mac && ntohl(rpkt->exten[0]) == sys_authkey &&
			authdecrypt(sys_authkey, (u_int32 *)rpkt, LEN_PKT_NOMAC,
			(int)(rbufp->recv_length - LEN_PKT_NOMAC)))
			is_authentic = 1;
		if (debug)
			printf("receive: authentication %s\n",
			   is_authentic ? "passed" : "failed");
	}
	server->trust <<= 1;
	if (!is_authentic)
		server->trust |= 1;

	/*
	 * Looks good.	Record info from the packet.
	 */
	server->leap = PKT_LEAP(rpkt->li_vn_mode);
	server->stratum = PKT_TO_STRATUM(rpkt->stratum);
	server->precision = rpkt->precision;
	server->rootdelay = ntohl(rpkt->rootdelay);
	server->rootdispersion = ntohl(rpkt->rootdispersion);
	server->refid = rpkt->refid;
	NTOHL_FP(&rpkt->reftime, &server->reftime);
	NTOHL_FP(&rpkt->rec, &rec);
	NTOHL_FP(&rpkt->xmt, &server->org);

	/*
	 * Make sure the server is at least somewhat sane.	If not, try
	 * again.
	 */
	if (L_ISZERO(&rec) || !L_ISHIS(&server->org, &rec)) {
		transmit(server);
		return;
	}

	/*
	 * Calculate the round trip delay (di) and the clock offset (ci).
	 * We use the equations (reordered from those in the spec):
	 *
	 * d = (t2 - t3) - (t1 - t0)
	 * c = ((t2 - t3) + (t1 - t0)) / 2
	 */
	t10 = server->org;		/* pkt.xmt == t1 */
	L_SUB(&t10, &rbufp->recv_time); /* recv_time == t0*/

	t23 = rec;			/* pkt.rec == t2 */
	L_SUB(&t23, &org);		/* pkt->org == t3 */

	/* now have (t2 - t3) and (t0 - t1).	Calculate (ci) and (di) */
	/*
	 * Calculate (ci) = ((t1 - t0) / 2) + ((t2 - t3) / 2)
	 * For large offsets this may prevent an overflow on '+'
	 */
	ci = t10;
	L_RSHIFT(&ci);
	tmp = t23;
	L_RSHIFT(&tmp);
	L_ADD(&ci, &tmp);

	/*
	 * Calculate di in t23 in full precision, then truncate
	 * to an s_fp.
	 */
	L_SUB(&t23, &t10);
	di = LFPTOFP(&t23);

	if (debug > 3)
		printf("offset: %s, delay %s\n", lfptoa(&ci, 6), fptoa(di, 5));

	di += (FP_SECOND >> (-(int)NTPDATE_PRECISION))
		+ (FP_SECOND >> (-(int)server->precision)) + NTP_MAXSKW;

	if (di <= 0) {		/* value still too raunchy to use? */
		L_CLR(&ci);
		di = 0;
	} else {
		di = max(di, NTP_MINDIST);
	}

	/*
	 * Shift this data in, then transmit again.
	 */
	server_data(server, (s_fp) di, &ci, 0);
	transmit(server);
}


/*
 * server_data - add a sample to the server's filter registers
 */
static void
server_data(
	register struct server *server,
	s_fp d,
	l_fp *c,
	u_fp e
	)
{
	u_short i;

	i = server->filter_nextpt;
	if (i < NTP_SHIFT) {
		server->filter_delay[i] = d;
		server->filter_offset[i] = *c;
		server->filter_soffset[i] = LFPTOFP(c);
		server->filter_error[i] = e;
		server->filter_nextpt = (u_short)(i + 1);
	}
}


/*
 * clock_filter - determine a server's delay, dispersion and offset
 */
static void
clock_filter(
	register struct server *server
	)
{
	register int i, j;
	int ord[NTP_SHIFT];

	/*
	 * Sort indices into increasing delay order
	 */
	for (i = 0; i < sys_samples; i++)
		ord[i] = i;

	for (i = 0; i < (sys_samples-1); i++) {
		for (j = i+1; j < sys_samples; j++) {
			if (server->filter_delay[ord[j]] == 0)
				continue;
			if (server->filter_delay[ord[i]] == 0
				|| (server->filter_delay[ord[i]]
				> server->filter_delay[ord[j]])) {
				register int tmp;

				tmp = ord[i];
				ord[i] = ord[j];
				ord[j] = tmp;
			}
		}
	}

	/*
	 * Now compute the dispersion, and assign values to delay and
	 * offset.	If there are no samples in the register, delay and
	 * offset go to zero and dispersion is set to the maximum.
	 */
	if (server->filter_delay[ord[0]] == 0) {
		server->delay = 0;
		L_CLR(&server->offset);
		server->soffset = 0;
		server->dispersion = PEER_MAXDISP;
	} else {
		register s_fp d;

		server->delay = server->filter_delay[ord[0]];
		server->offset = server->filter_offset[ord[0]];
		server->soffset = LFPTOFP(&server->offset);
		server->dispersion = 0;
		for (i = 1; i < sys_samples; i++) {
			if (server->filter_delay[ord[i]] == 0)
				d = PEER_MAXDISP;
			else {
				d = server->filter_soffset[ord[i]]
					- server->filter_soffset[ord[0]];
				if (d < 0)
					d = -d;
				if (d > PEER_MAXDISP)
					d = PEER_MAXDISP;
			}
			/*
			 * XXX This *knows* PEER_FILTER is 1/2
			 */
			server->dispersion += (u_fp)(d) >> i;
		}
	}
	/*
	 * We're done
	 */
}


/*
 * clock_select - select the pick-of-the-litter clock from the samples
 *		  we've got.
 */
static struct server *
clock_select(const int volatile *pbStop)
{
	register struct server *server;
	register int i;
	register int nlist;
	register s_fp d;
	register int j;
	register int n;
	s_fp local_threshold;
	struct server *server_list[NTP_MAXCLOCK];
	u_fp server_badness[NTP_MAXCLOCK];
	struct server *sys_server;

	/*
	 * This first chunk of code is supposed to go through all
	 * servers we know about to find the NTP_MAXLIST servers which
	 * are most likely to succeed.	We run through the list
	 * doing the sanity checks and trying to insert anyone who
	 * looks okay.	We are at all times aware that we should
	 * only keep samples from the top two strata and we only need
	 * NTP_MAXLIST of them.
	 */
	nlist = 0;	/* none yet */
	for (server = sys_servers; server != NULL; server = server->next_server) {
		if (server->delay == 0) {
			if (debug)
				printf("%s: Server dropped: no data\n", ntoa(&server->srcadr));
			continue;	/* no data */
		}
		if (server->stratum > NTP_INFIN) {
			if (debug)
				printf("%s: Server dropped: strata too high\n", ntoa(&server->srcadr));
			continue;	/* stratum no good */
		}
		if (server->delay > NTP_MAXWGT) {
			if (debug)
				printf("%s: Server dropped: server too far away\n", 
					ntoa(&server->srcadr));
			continue;	/* too far away */
		}
		if (server->leap == LEAP_NOTINSYNC) {
			if (debug)
				printf("%s: Server dropped: Leap not in sync\n", ntoa(&server->srcadr));
			continue;	/* he's in trouble */
		}
		if (!L_ISHIS(&server->org, &server->reftime)) {
			if (debug)
				printf("%s: Server dropped: server is very broken\n", 
				       ntoa(&server->srcadr));
			continue;	/* very broken host */
		}
		if ((server->org.l_ui - server->reftime.l_ui)
		    >= NTP_MAXAGE) {
			if (debug)
				printf("%s: Server dropped: Server has gone too long without sync\n", 
				       ntoa(&server->srcadr));
			continue;	/* too long without sync */
		}
		if (server->trust != 0) {
			if (debug)
				printf("%s: Server dropped: Server is untrusted\n",
				       ntoa(&server->srcadr));
			continue;
		}

		/*
		 * This one seems sane.  Find where he belongs
		 * on the list.
		 */
		d = server->dispersion + server->dispersion;
		for (i = 0; i < nlist; i++)
			if (server->stratum <= server_list[i]->stratum)
			break;
		for ( ; i < nlist; i++) {
			if (server->stratum < server_list[i]->stratum)
				break;
			if (d < (s_fp) server_badness[i])
				break;
		}

		/*
		 * If i points past the end of the list, this
		 * guy is a loser, else stick him in.
		 */
		if (i >= NTP_MAXLIST)
			continue;
		for (j = nlist; j > i; j--)
			if (j < NTP_MAXLIST) {
				server_list[j] = server_list[j-1];
				server_badness[j]
					= server_badness[j-1];
			}

		server_list[i] = server;
		server_badness[i] = d;
		if (nlist < NTP_MAXLIST)
			nlist++;
	}

	/*
	 * Got the five-or-less best.	 Cut the list where the number of
	 * strata exceeds two.
	 */
	j = 0;
	for (i = 1; i < nlist; i++)
		if (server_list[i]->stratum > server_list[i-1]->stratum)
		if (++j == 2) {
			nlist = i;
			break;
		}

	/*
	 * Whew!  What we should have by now is 0 to 5 candidates for
	 * the job of syncing us.  If we have none, we're out of luck.
	 * If we have one, he's a winner.  If we have more, do falseticker
	 * detection.
	 */

	if (nlist == 0)
		sys_server = 0;
	else if (nlist == 1) {
		sys_server = server_list[0];
	} else {
		/*
		 * Re-sort by stratum, bdelay estimate quality and
		 * server.delay.
		 */
        for (i = 0; i < nlist-1; i++) {
	if (*pbStop) { return NULL; }  /* CMC stop requested */
			for (j = i+1; j < nlist; j++) {
				if (server_list[i]->stratum
				< server_list[j]->stratum)
				break;	/* already sorted by stratum */
				if (server_list[i]->delay
				< server_list[j]->delay)
				continue;
				server = server_list[i];
				server_list[i] = server_list[j];
				server_list[j] = server;
			}
        }

		/*
		 * Calculate the fixed part of the dispersion limit
		 */
		local_threshold = (FP_SECOND >> (-(int)NTPDATE_PRECISION))
			+ NTP_MAXSKW;

		/*
		 * Now drop samples until we're down to one.
		 */
		while (nlist > 1) {
	if (*pbStop) { return NULL; }  /* CMC stop requested */
			for (n = 0; n < nlist; n++) {
				server_badness[n] = 0;
				for (j = 0; j < nlist; j++) {
					if (j == n) /* with self? */
						continue;
					d = server_list[j]->soffset
						- server_list[n]->soffset;
					if (d < 0)	/* absolute value */
						d = -d;
					/*
					 * XXX This code *knows* that
					 * NTP_SELECT is 3/4
					 */
					for (i = 0; i < j; i++)
						d = (d>>1) + (d>>2);
					server_badness[n] += d;
				}
			}

			/*
			 * We now have an array of nlist badness
			 * coefficients.	Find the badest.  Find
			 * the minimum precision while we're at
			 * it.
			 */
			i = 0;
			n = server_list[0]->precision;;
			for (j = 1; j < nlist; j++) {
				if (server_badness[j] >= server_badness[i])
					i = j;
				if (n > server_list[j]->precision)
					n = server_list[j]->precision;
			}

			/*
			 * i is the index of the server with the worst
			 * dispersion.	If his dispersion is less than
			 * the threshold, stop now, else delete him and
			 * continue around again.
			 */
			if ( (s_fp) server_badness[i] < (local_threshold
							 + (FP_SECOND >> (-n))))
				break;
			for (j = i + 1; j < nlist; j++)
				server_list[j-1] = server_list[j];
			nlist--;
		}

		/*
		 * What remains is a list of less than 5 servers.  Take
		 * the best.
		 */
		sys_server = server_list[0];
	}

	/*
	 * That's it.  Return our server.
	 */
	return sys_server;
}


/*
 * clock_adjust - process what we've received, and adjust the time
 *		 if we got anything decent.
 */
static int
clock_adjust(double volatile *pdTimeOffset, const int volatile *pbStop)
{
	register struct server *sp, *server;
	s_fp absoffset;
	int dostep;

    for (sp = sys_servers; sp != NULL; sp = sp->next_server) {
		clock_filter(sp);
    	if (*pbStop) { return -1; }  /* CMC stop requested */
    }
    server = clock_select(pbStop);
    if (server == NULL) return -1;  /* CMC we probably need to quit */

/* CMC here, no printing required, we just want to set *pdTimeOffset
	if (debug || simple_query) {
		for (sp = sys_servers; sp != NULL; sp = sp->next_server)
			printserver(sp, stdout);
	}
*/

	if (server == 0) {
		msyslog(LOG_ERR,
			"no server suitable for synchronization found");
		return 1;
	}

	if (*pbStop) { return -1; }  /* CMC stop requested */
	if (always_step) {
		dostep = 1;
	} else if (never_step) {
		dostep = 0;
	} else {
		absoffset = server->soffset;
		if (absoffset < 0)
			absoffset = -absoffset;
		dostep = (absoffset >= NTPDATE_THRESHOLD || absoffset < 0);
	}

	if (dostep) {
		if (simple_query || debug || l_step_systime(&server->offset)){
/* CMC HERE -- this is where we want the offset value set */
               *pdTimeOffset = atof(lfptoa(&server->offset, 6));
/* CMC commented msg out
			msyslog(LOG_NOTICE, "step time server %s offset %s sec",
				stoa(&server->srcadr),
			lfptoa(&server->offset, 6));
*/
		}
	} else {
#if !defined SYS_WINNT && !defined SYS_CYGWIN32
		if (*pbStop) { return -1; }  /* CMC stop requested */
	        if (simple_query || l_adj_systime(&server->offset)) {
/* CMC HERE -- this is where we want the offset value set */
               *pdTimeOffset = atof(lfptoa(&server->offset, 6));
/* CMC commented msg out
			msyslog(LOG_NOTICE, "adjust time server %s offset %s sec",
		          stoa(&server->srcadr),
				lfptoa(&server->offset, 6));
*/
		}
#else
		/* The NT SetSystemTimeAdjustment() call achieves slewing by
		 * changing the clock frequency. This means that we cannot specify
		 * it to slew the clock by a definite amount and then stop like
		 * the Unix adjtime() routine. We can technically adjust the clock
		 * frequency, have ntpdate sleep for a while, and then wake
		 * up and reset the clock frequency, but this might cause some
		 * grief if the user attempts to run ntpd immediately after
		 * ntpdate and the socket is in use.
		 */
		printf("\nThe -b option is required by ntpdate on Windows NT platforms\n");
		return 1; /* CMC here - changed exit to return */
#endif /* SYS_WINNT */
	}
	return 0;
}


/*
 * is_unreachable - check to see if we have a route to given destination
 *		    (non-blocking).
 */
static int
is_reachable (struct sockaddr_storage *dst)
{
	SOCKET sockfd;

	sockfd = socket(dst->ss_family, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		return 0;
	}

	if(connect(sockfd, (struct sockaddr *)dst, SOCKLEN(dst))) {
		closesocket(sockfd);
		return 0;
	}
	closesocket(sockfd);
	return 1;
}



/* XXX ELIMINATE: merge BIG slew into adj_systime in lib/systime.c */
/*
 * addserver - determine a server's address and allocate a new structure
 *		for it.

 CMC changed to int so get retval for a stop as this can take awhile

 */
static int     
addserver(
     volatile const int *pbStop,
	char *serv
)
{
	register struct server *server;
	/* Address infos structure to store result of getaddrinfo */
	struct addrinfo *addrResult, *ptr;
	/* Address infos structure to store hints for getaddrinfo */
	struct addrinfo hints;
	/* Error variable for getaddrinfo */
	int error;
	/* Service name */
	char service[5];
	strcpy(service, "ntp");

	/* Get host address. Looking for UDP datagram connection. */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = ai_fam_templ;
	hints.ai_socktype = SOCK_DGRAM;

#ifdef DEBUG
	if (debug)
		printf("Looking for host %s and service %s\n", serv, service);
#endif
 
        if (*pbStop) return -1;   /* CMC added check to return if we got a stop request from the main QCN program */
#ifdef _WIN32
	error = getaddrinfo_win(serv, service, &hints, &addrResult);
#else
	error = getaddrinfo(serv, service, &hints, &addrResult);
#endif
	if (error != 0) {
		/* Conduct more refined error analysis */
		if (error == EAI_FAIL || error == EAI_AGAIN){
			/* Name server is unusable. Exit after failing on the
			   first server, in order to shorten the timeout caused
			   by waiting for resolution of several servers */
			fprintf(stderr, "Name server cannot be used, exiting");
			msyslog(LOG_ERR, "name server cannot be used, reason: %s\n", gai_strerror(error));
		        return 0; /* CMC here - changed exit to return */
		}
		fprintf(stderr, "Error : %s\n", gai_strerror(error));
		msyslog(LOG_ERR, "can't find host %s\n", serv);
		return 0;
	}
#ifdef DEBUG
	else if (debug) {
		fprintf(stderr, "host found : %s\n", stohost((struct sockaddr_storage*)addrResult->ai_addr));
	}
#endif
#ifdef _WIN32
        if (*pbStop) { freeaddrinfo_win(addrResult); return -1; };   /* CMC added check to return if we got a stop request from the main QCN program */
#else
        if (*pbStop) { freeaddrinfo(addrResult); return -1; };   /* CMC added check to return if we got a stop request from the main QCN program */
#endif

	/* We must get all returned server in case the first one fails */
	for (ptr = addrResult; ptr != NULL; ptr = ptr->ai_next) {
#ifdef _WIN32
        if (*pbStop) { freeaddrinfo_win(addrResult); return -1; };   /* CMC added check to return if we got a stop request from the main QCN program */
#else
        if (*pbStop) { freeaddrinfo(addrResult); return -1; };   /* CMC added check to return if we got a stop request from the main QCN program */
#endif
		if (is_reachable ((struct sockaddr_storage *)ptr->ai_addr)) {
			server = (struct server *)emalloc(sizeof(struct server));
			memset((char *)server, 0, sizeof(struct server));

			memset(&(server->srcadr), 0, sizeof(struct sockaddr_storage));
			memcpy(&(server->srcadr), ptr->ai_addr, ptr->ai_addrlen);
			server->event_time = ++sys_numservers;
			if (sys_servers == NULL)
				sys_servers = server;
			else {
				struct server *sp;

				for (sp = sys_servers; sp->next_server != NULL;
				     sp = sp->next_server) ;
				sp->next_server = server;
			}
#ifdef _WIN32
        if (*pbStop) { freeaddrinfo_win(addrResult); return -1; };   /* CMC added check to return if we got a stop request from the main QCN program */
#else
        if (*pbStop) { freeaddrinfo(addrResult); return -1; };   /* CMC added check to return if we got a stop request from the main QCN program */
#endif
		}
	}
#ifdef _WIN32
        freeaddrinfo_win(addrResult);  /* CMC added check to return if we got a stop request from the main QCN program */
#else
        freeaddrinfo(addrResult);   /* CMC added check to return if we got a stop request from the main QCN program */
#endif
        return 0;
}


/*
 * findserver - find a server in the list given its address
 * ***(For now it isn't totally AF-Independant, to check later..)
 */
static struct server *
findserver(
	struct sockaddr_storage *addr
	)
{
	struct server *server;
	struct server *mc_server;
	isc_sockaddr_t laddr;
	isc_sockaddr_t saddr;

	if(addr->ss_family == AF_INET) {
		isc_sockaddr_fromin( &laddr, &((struct sockaddr_in*)addr)->sin_addr, 0);
	}
	else {
		isc_sockaddr_fromin6(&laddr, &((struct sockaddr_in6*)addr)->sin6_addr, 0);
	}


	mc_server = NULL;
	if (htons(((struct sockaddr_in*)addr)->sin_port) != NTP_PORT)
		return 0;

	for (server = sys_servers; server != NULL; 
	     server = server->next_server) {
		
		if(server->srcadr.ss_family == AF_INET) {
			isc_sockaddr_fromin(&saddr, &((struct sockaddr_in*)&server->srcadr)->sin_addr, 0);
		}
		else {
			isc_sockaddr_fromin6(&saddr, &((struct sockaddr_in6*)&server->srcadr)->sin6_addr, 0);
		}
		if (isc_sockaddr_eqaddr(&laddr, &saddr) == ISC_TRUE)
			return server;

		if(addr->ss_family == server->srcadr.ss_family) {
			if (isc_sockaddr_ismulticast(&saddr) == ISC_TRUE)
				mc_server = server;
		}
	}

	if (mc_server != NULL) {	

		struct server *sp;

		if (mc_server->event_time != 0) {
			mc_server->event_time = 0;
			complete_servers++;
		}

		server = (struct server *)emalloc(sizeof(struct server));
		memset((char *)server, 0, sizeof(struct server));

		memcpy(&server->srcadr, addr, sizeof(struct sockaddr_storage));

		server->event_time = ++sys_numservers;

		for (sp = sys_servers; sp->next_server != NULL;
		     sp = sp->next_server) ;
		sp->next_server = server;
		transmit(server);
	}
	return NULL;
}


/*
 * timer - process a timer interrupt
 */
void
timer(void)
{
	struct server *server;

	/*
	 * Bump the current idea of the time
	 */
	current_time++;

	/*
	 * Search through the server list looking for guys
	 * who's event timers have expired.  Give these to
	 * the transmit routine.
	 */
	for (server = sys_servers; server != NULL; 
	     server = server->next_server) {
		if (server->event_time != 0
		    && server->event_time <= current_time)
			transmit(server);
	}
}


/*
 * The code duplication in the following subroutine sucks, but
 * we need to appease ansi2knr.
 */

#ifndef SYS_WINNT
/*
 * alarming - record the occurance of an alarm interrupt
 */
static RETSIGTYPE
alarming(
	int sig
	)
{
	alarm_flag++;
}
#else
void CALLBACK 
alarming(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	alarm_flag++;
}
#endif /* SYS_WINNT */


#ifdef SYS_WINNT
static void
callTimeEndPeriod(void)
{
	timeEndPeriod( wTimerRes );
	wTimerRes = 0;
}
#endif /* SYS_WINNT */


/*
 * init_alarm - set up the timer interrupt
 */
static void
init_alarm(void)
{
#ifndef SYS_WINNT
# ifndef HAVE_TIMER_SETTIME
	struct itimerval itimer;
# else
	struct itimerspec ntpdate_itimer;
# endif
#else
	TIMECAPS tc;
	UINT wTimerID;
# endif /* SYS_WINNT */
#if defined SYS_CYGWIN32 || defined SYS_WINNT
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	DWORD dwUser = 0;
#endif /* SYS_WINNT */

	alarm_flag = 0;

#ifndef SYS_WINNT
# if defined(HAVE_TIMER_CREATE) && defined(HAVE_TIMER_SETTIME)
	alarm_flag = 0;
	/* this code was put in as setitimer() is non existant this us the
	 * POSIX "equivalents" setup - casey
	 */
	/* ntpdate_timerid is global - so we can kill timer later */
	if (timer_create (CLOCK_REALTIME, NULL, &ntpdate_timerid) ==
#  ifdef SYS_VXWORKS
		ERROR
#  else
		-1
#  endif
		)
	{
		fprintf (stderr, "init_alarm(): timer_create (...) FAILED\n");
		return;
	}

	/*  TIMER_HZ = (5)
	 * Set up the alarm interrupt.	The first comes 1/(2*TIMER_HZ)
	 * seconds from now and they continue on every 1/TIMER_HZ seconds.
	*/
	(void) signal_no_reset(SIGALRM, alarming);
	ntpdate_itimer.it_interval.tv_sec = ntpdate_itimer.it_value.tv_sec = 0;
	ntpdate_itimer.it_interval.tv_nsec = 1000000000/TIMER_HZ;
	ntpdate_itimer.it_value.tv_nsec = 1000000000/(TIMER_HZ<<1);
	timer_settime(ntpdate_timerid, 0 /* !TIMER_ABSTIME */, &ntpdate_itimer, NULL);
# else
	/*  
	 * Set up the alarm interrupt.	The first comes 1/(2*TIMER_HZ)
	 * seconds from now and they continue on every 1/TIMER_HZ seconds.
	 */
	(void) signal_no_reset(SIGALRM, alarming);
	itimer.it_interval.tv_sec = itimer.it_value.tv_sec = 0;
	itimer.it_interval.tv_usec = 1000000/TIMER_HZ;
	itimer.it_value.tv_usec = 1000000/(TIMER_HZ<<1);

	setitimer(ITIMER_REAL, &itimer, (struct itimerval *)0);
# endif
#if defined SYS_CYGWIN32
	/*
	 * Get privileges needed for fiddling with the clock
	 */

	/* get the current process token handle */
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		msyslog(LOG_ERR, "OpenProcessToken failed: %m");
		        return 1; /* CMC here - changed exit to return */
	}
	/* get the LUID for system-time privilege. */
	LookupPrivilegeValue(NULL, SE_SYSTEMTIME_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;		/* one privilege to set */
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	/* get set-time privilege for this process. */
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,(PTOKEN_PRIVILEGES) NULL, 0);
	/* cannot test return value of AdjustTokenPrivileges. */
	if (GetLastError() != ERROR_SUCCESS)
		msyslog(LOG_ERR, "AdjustTokenPrivileges failed: %m");
#endif
#else	/* SYS_WINNT */
	_tzset();

/* CMC here - bypass this */
#if 0
    //
	 // Get privileges needed for fiddling with the clock
	//

	// get the current process token handle 
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		msyslog(LOG_ERR, "OpenProcessToken failed: %m");
		        return; // CMC here - changed exit to return 
	}
	// get the LUID for system-time privilege. 
	LookupPrivilegeValue(NULL, SE_SYSTEMTIME_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;		// one privilege to set 
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	//  get set-time privilege for this process. 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,(PTOKEN_PRIVILEGES) NULL, 0);
	// cannot test return value of AdjustTokenPrivileges. 
	if (GetLastError() != ERROR_SUCCESS)
		msyslog(LOG_ERR, "AdjustTokenPrivileges failed: %m");
#endif // bypass this CMC

	/*
	 * Set up timer interrupts for every 2**EVENT_TIMEOUT seconds
	 * Under Win/NT, expiry of timer interval leads to invocation
	 * of a callback function (on a different thread) rather than
	 * generating an alarm signal
	 */

	/* determine max and min resolution supported */
	if(timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) {
		msyslog(LOG_ERR, "timeGetDevCaps failed: %m");
		        return; /* CMC here - changed exit to return */
	}
	wTimerRes = min(max(tc.wPeriodMin, TARGET_RESOLUTION), tc.wPeriodMax);
	/* establish the minimum timer resolution that we'll use */
	timeBeginPeriod(wTimerRes);
	// atexit(callTimeEndPeriod);

	/* start the timer event */
	wTimerID = timeSetEvent(
		(UINT) (1000/TIMER_HZ),		/* Delay */
		wTimerRes,			/* Resolution */
		(LPTIMECALLBACK) alarming,	/* Callback function */
		(DWORD) dwUser,			/* User data */
		TIME_PERIODIC);			/* Event type (periodic) */
	if (wTimerID == 0) {
		msyslog(LOG_ERR, "timeSetEvent failed: %m");
		        return; /* CMC here - changed exit to return */
	}
#endif /* SYS_WINNT */
}




/*
 * We do asynchronous input using the SIGIO facility.  A number of
 * recvbuf buffers are preallocated for input.	In the signal
 * handler we poll to see if the socket is ready and read the
 * packets from it into the recvbuf's along with a time stamp and
 * an indication of the source host and the interface it was received
 * through.  This allows us to get as accurate receive time stamps
 * as possible independent of other processing going on.
 *
 * We allocate a number of recvbufs equal to the number of servers
 * plus 2.	This should be plenty.
 */


/*
 * init_io - initialize I/O data and open socket
 */
static void
init_io(void)
{
	struct addrinfo *res, *ressave;
	struct addrinfo hints;
	char service[5];
	int optval = 1;
  /* CMC here -- should we bypass the next line check? */
	int check_ntp_port_in_use = !debug && !simple_query && !unpriv_port;

	/*
	 * Init buffer free list and stat counters
	 */
	init_recvbuff(sys_numservers + 2);

	/*
	 * Open the socket
	 */

	strcpy(service, "ntp");

	/*
	 * Init hints addrinfo structure
	 */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = ai_fam_templ;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_DGRAM;

#ifdef _WIN32
	if(getaddrinfo_win(NULL, service, &hints, &res) != 0) {
#else
	if(getaddrinfo(NULL, service, &hints, &res) != 0) {
#endif
		msyslog(LOG_ERR, "getaddrinfo() failed: %m");
	       return ; /* CMC here - changed exit to return */
	       /*NOTREACHED*/
	}

#ifdef SYS_WINNT
	if (check_ntp_port_in_use && ntp_port_inuse(AF_INET, NTP_PORT)){
		netsyslog(LOG_ERR, "the NTP socket is in use, exiting: %m");
		        return; /* CMC here - changed exit to return */
	}
#endif

	/* Remember the address of the addrinfo structure chain */
	ressave = res;

	/*
	 * For each structure returned, open and bind socket
	 */
	for(nbsock = 0; (nbsock < MAX_AF) && res ; res = res->ai_next) {
	/* create a datagram (UDP) socket */
		fd[nbsock] = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (fd[nbsock] == SOCKET_ERROR) {
#ifndef SYS_WINNT
		if (errno == EPROTONOSUPPORT || errno == EAFNOSUPPORT ||
		    errno == EPFNOSUPPORT)
#else
		int err = WSAGetLastError();
		if (err == WSAEPROTONOSUPPORT || err == WSAEAFNOSUPPORT ||
		    err == WSAEPFNOSUPPORT)
#endif
			continue;
		netsyslog(LOG_ERR, "socket() failed: %m");
		        return; /* CMC here - changed exit to return */
		/*NOTREACHED*/
		}
		/* set socket to reuse address */
		if (setsockopt(fd[nbsock], SOL_SOCKET, SO_REUSEADDR, (void*) &optval, sizeof(optval)) < 0) {
				netsyslog(LOG_ERR, "setsockopt() SO_REUSEADDR failed: %m");
		        return; /* CMC here - changed exit to return */
				/*NOTREACHED*/
		}
#ifdef IPV6_V6ONLY
		/* Restricts AF_INET6 socket to IPv6 communications (see RFC 2553bis-03) */
		if (res->ai_family == AF_INET6)
			if (setsockopt(fd[nbsock], IPPROTO_IPV6, IPV6_V6ONLY, (void*) &optval, sizeof(optval)) < 0) {
				   netsyslog(LOG_ERR, "setsockopt() IPV6_V6ONLY failed: %m");
		        return; /* CMC here - changed exit to return */
					/*NOTREACHED*/
		}
#endif

		/* Remember the socket family in fd_family structure */
		fd_family[nbsock] = res->ai_family;

		/*
		 * bind the socket to the NTP port
		 */
		if (check_ntp_port_in_use) {
			if (bind(fd[nbsock], res->ai_addr, SOCKLEN(res->ai_addr)) < 0) {
#ifndef SYS_WINNT
				if (errno == EADDRINUSE)
#else
				if (WSAGetLastError() == WSAEADDRINUSE)
#endif /* SYS_WINNT */
					netsyslog(LOG_ERR, "the NTP socket is in use, exiting");
				else
					netsyslog(LOG_ERR, "bind() fails: %m");
		        return; /* CMC here - changed exit to return */
			}
		}

#ifdef HAVE_POLL_H
		fdmask[nbsock].fd = fd[nbsock];
		fdmask[nbsock].events = POLLIN;
#else
		FD_SET(fd[nbsock], &fdmask);
		if (maxfd < fd[nbsock]+1) {
			maxfd = fd[nbsock]+1;
		}
#endif

		/*
		 * set non-blocking,
		 */
#ifndef SYS_WINNT
# ifdef SYS_VXWORKS
		{
		int on = TRUE;

		if (ioctl(fd[nbsock],FIONBIO, &on) == ERROR) {
		  netsyslog(LOG_ERR, "ioctl(FIONBIO) fails: %m");
		        return; /* CMC here - changed exit to return */
		}
		}
# else /* not SYS_VXWORKS */
#  if defined(O_NONBLOCK)
		if (fcntl(fd[nbsock], F_SETFL, O_NONBLOCK) < 0) {
			netsyslog(LOG_ERR, "fcntl(FNDELAY|FASYNC) fails: %m");
		        return; /* CMC here - changed exit to return */
			/*NOTREACHED*/
		}
#  else /* not O_NONBLOCK */
#	if defined(FNDELAY)
		if (fcntl(fd[nbsock], F_SETFL, FNDELAY) < 0) {
			netsyslog(LOG_ERR, "fcntl(FNDELAY|FASYNC) fails: %m");
		        return; /* CMC here - changed exit to return */
			/*NOTREACHED*/
		}
#	else /* FNDELAY */
#	 include "Bletch: Need non blocking I/O"
#	endif /* FNDELAY */
#  endif /* not O_NONBLOCK */
# endif /* SYS_VXWORKS */
#else /* SYS_WINNT */
		if (ioctlsocket(fd[nbsock], FIONBIO, (u_long *) &on) == SOCKET_ERROR) {
			netsyslog(LOG_ERR, "ioctlsocket(FIONBIO) fails: %m");
		        return; /* CMC here - changed exit to return */
		}
#endif /* SYS_WINNT */
		nbsock++;
	}
#ifdef _WIN32
	freeaddrinfo_win(ressave);
#else
	freeaddrinfo(ressave);
#endif
}

/*
 * sendpkt - send a packet to the specified destination
 */
static void
sendpkt(
	struct sockaddr_storage *dest,
	struct pkt *pkt,
	int len
	)
{
	int i;
	int cc;
	SOCKET sock = INVALID_SOCKET;

#ifdef SYS_WINNT
	DWORD err;
#endif /* SYS_WINNT */

	/* Find a local family compatible socket to send ntp packet to ntp server */
	for(i = 0; (i < MAX_AF); i++) {
		if(dest->ss_family == fd_family[i]) {
			sock = fd[i];
		break;
		}
	}

	if ( sock == INVALID_SOCKET ) {
		netsyslog(LOG_ERR, "cannot find family compatible socket to send ntp packet");
		        return; /* CMC here - changed exit to return */
		/*NOTREACHED*/
	}

	cc = sendto(sock, (char *)pkt, len, 0, (struct sockaddr *)dest,
			SOCKLEN(dest));

	if (cc == SOCKET_ERROR) {
#ifndef SYS_WINNT
		if (errno != EWOULDBLOCK && errno != ENOBUFS)
#else
		err = WSAGetLastError();
		if (err != WSAEWOULDBLOCK && err != WSAENOBUFS)
#endif /* SYS_WINNT */
			netsyslog(LOG_ERR, "sendto(%s): %m", stohost(dest));
	}
}


/*
 * input_handler - receive packets asynchronously
 */
void
input_handler(void)
{
	register int n;
	register struct recvbuf *rb;
	struct timeval tvzero;
	int fromlen;
	l_fp ts;
	int i;
#ifdef HAVE_POLL_H
	struct pollfd fds[MAX_AF];
#else
	fd_set fds;
#endif
	int fdc = 0;

	/*
	 * Do a poll to see if we have data
	 */
	for (;;) {
		tvzero.tv_sec = tvzero.tv_usec = 0;
#ifdef HAVE_POLL_H
		memcpy(fds, fdmask, sizeof(fdmask));
		n = poll(fds, (unsigned int)nbsock, tvzero.tv_sec * 1000);

		/*
		 * Determine which socket received data
		 */

		for(i=0; i < nbsock; i++) {
			if(fds[i].revents & POLLIN) {
				fdc = fd[i];
				break;
			}
		}

#else
		fds = fdmask;
		n = select((int) maxfd, &fds, (fd_set *)0, (fd_set *)0, &tvzero);

		/*
		 * Determine which socket received data
		 */

		for(i=0; i < nbsock; i++) {
			if(FD_ISSET(fd[i], &fds)) {
				 fdc = (int) fd[i];
				 break;
			}
		}

#endif

		/*
		 * If nothing to do, just return.  If an error occurred,
		 * complain and return.  If we've got some, freeze a
		 * timestamp.
		 */
		if (n == 0)
			return;
		else if (n == -1) {
			if (errno != EINTR)
				netsyslog(LOG_ERR,
#ifdef HAVE_POLL_H
					"poll(3) error: %m"
#else
					"select() error: %m"
#endif
					);
			return;
		}
		get_systime(&ts);

		/*
		 * Get a buffer and read the frame.  If we
		 * haven't got a buffer, or this is received
		 * on the wild card socket, just dump the packet.
		 */
		if (initializing || free_recvbuffs() == 0) {
			char buf[100];


#ifndef SYS_WINNT
			(void) read(fdc, buf, sizeof buf);
#else
			/* NT's _read does not operate on nonblocking sockets
			 * either recvfrom or ReadFile() has to be used here.
			 * ReadFile is used in [ntpd]ntp_intres() and ntpdc,
			 * just to be different use recvfrom() here
			 */
			recvfrom(fdc, buf, sizeof(buf), 0, (struct sockaddr *)0, NULL);
#endif /* SYS_WINNT */
			continue;
		}

		rb = get_free_recv_buffer();

		fromlen = sizeof(struct sockaddr_storage);
		rb->recv_length = recvfrom(fdc, (char *)&rb->recv_pkt,
		   sizeof(rb->recv_pkt), 0,
		   (struct sockaddr *)&rb->recv_srcadr, 
                   (socklen_t*) &fromlen);
		if (rb->recv_length == -1) {
			freerecvbuf(rb);
			continue;
		}

		/*
		 * Got one.  Mark how and when it got here,
		 * put it on the full list.
		 */
		rb->recv_time = ts;
		add_full_recv_buffer(rb);
	}
}


#if !defined SYS_WINNT && !defined SYS_CYGWIN32
/*
 * adj_systime - do a big long slew of the system time
 */
static int
l_adj_systime(
	l_fp *ts
	)
{
	struct timeval adjtv, oadjtv;
	int isneg = 0;
	l_fp offset;
#ifndef STEP_SLEW
	l_fp overshoot;
#endif

	/*
	 * Take the absolute value of the offset
	 */
	offset = *ts;
	if (L_ISNEG(&offset)) {
		isneg = 1;
		L_NEG(&offset);
	}

#ifndef STEP_SLEW
	/*
	 * Calculate the overshoot.  XXX N.B. This code *knows*
	 * ADJ_OVERSHOOT is 1/2.
	 */
	overshoot = offset;
	L_RSHIFTU(&overshoot);
	if (overshoot.l_ui != 0 || (overshoot.l_uf > ADJ_MAXOVERSHOOT)) {
		overshoot.l_ui = 0;
		overshoot.l_uf = ADJ_MAXOVERSHOOT;
	}
	L_ADD(&offset, &overshoot);
#endif
	TSTOTV(&offset, &adjtv);

	if (isneg) {
		adjtv.tv_sec = -adjtv.tv_sec;
		adjtv.tv_usec = -adjtv.tv_usec;
	}

	if (adjtv.tv_usec != 0 && !debug) {
		if (adjtime(&adjtv, &oadjtv) < 0) {
			msyslog(LOG_ERR, "Can't adjust the time of day: %m");
			return 0;
		}
	}
	return 1;
}
#endif /* SYS_WINNT */


/*
 * This fuction is not the same as lib/systime step_systime!!!
 */
static int
l_step_systime(
	l_fp *ts
	)
{
	double dtemp;

#ifdef SLEWALWAYS
#ifdef STEP_SLEW
	l_fp ftmp;
	int isneg;
	int n;

	if (debug) return 1;
	/*
	 * Take the absolute value of the offset
	 */
	ftmp = *ts;
	if (L_ISNEG(&ftmp)) {
		L_NEG(&ftmp);
		isneg = 1;
	} else
		isneg = 0;

	if (ftmp.l_ui >= 3) {		/* Step it and slew - we might win */
		LFPTOD(ts, dtemp);
		n = step_systime(dtemp);
		if (!n)
			return n;
		if (isneg)
			ts->l_ui = ~0;
		else
			ts->l_ui = ~0;
	}
	/*
	 * Just add adjustment into the current offset.  The update
	 * routine will take care of bringing the system clock into
	 * line.
	 */
#endif
	if (debug)
		return 1;
#ifdef FORCE_NTPDATE_STEP
	LFPTOD(ts, dtemp);
	return step_systime(dtemp);
#else
	l_adj_systime(ts);
	return 1;
#endif
#else /* SLEWALWAYS */
	if (debug)
		return 1;
	LFPTOD(ts, dtemp);
	return step_systime(dtemp);
#endif	/* SLEWALWAYS */
}


/* CMC took out */
#if 0  
/* XXX ELIMINATE printserver similar in ntptrace.c, ntpdate.c */
/*
 * printserver - print detail information for a server
 */
static void
printserver(
	register struct server *pp,
	FILE *fp
	)
{
	register int i;
	char junk[5];
	char *str;

	if (!debug) {
		(void) fprintf(fp, "server %s, stratum %d, offset %s, delay %s\n",
				   stoa(&pp->srcadr), pp->stratum,
				   lfptoa(&pp->offset, 6), fptoa((s_fp)pp->delay, 5));
		return;
	}

	(void) fprintf(fp, "server %s, port %d\n",
			   stoa(&pp->srcadr), ntohs(((struct sockaddr_in*)&(pp->srcadr))->sin_port));

	(void) fprintf(fp, "stratum %d, precision %d, leap %c%c, trust %03o\n",
			   pp->stratum, pp->precision,
			   pp->leap & 0x2 ? '1' : '0',
			   pp->leap & 0x1 ? '1' : '0',
			   pp->trust);

	if (pp->stratum == 1) {
		junk[4] = 0;
		memmove(junk, (char *)&pp->refid, 4);
		str = junk;
	} else {
		str = stoa(&pp->srcadr);
	}
	(void) fprintf(fp,
			   "refid [%s], delay %s, dispersion %s\n",
			   str, fptoa((s_fp)pp->delay, 5),
			   ufptoa(pp->dispersion, 5));

	(void) fprintf(fp, "transmitted %d, in filter %d\n",
			   pp->xmtcnt, pp->filter_nextpt);

	(void) fprintf(fp, "reference time:    %s\n",
			   prettydate(&pp->reftime));
	(void) fprintf(fp, "originate timestamp: %s\n",
			   prettydate(&pp->org));
	(void) fprintf(fp, "transmit timestamp:  %s\n",
			   prettydate(&pp->xmt));

	(void) fprintf(fp, "filter delay: ");
	for (i = 0; i < NTP_SHIFT; i++) {
		(void) fprintf(fp, " %-8.8s", fptoa(pp->filter_delay[i], 5));
		if (i == (NTP_SHIFT>>1)-1)
			(void) fprintf(fp, "\n        ");
	}
	(void) fprintf(fp, "\n");

	(void) fprintf(fp, "filter offset:");
	for (i = 0; i < PEER_SHIFT; i++) {
		(void) fprintf(fp, " %-8.8s", lfptoa(&pp->filter_offset[i], 6));
		if (i == (PEER_SHIFT>>1)-1)
			(void) fprintf(fp, "\n        ");
	}
	(void) fprintf(fp, "\n");

	(void) fprintf(fp, "delay %s, dispersion %s\n",
			   fptoa((s_fp)pp->delay, 5), ufptoa(pp->dispersion, 5));

	(void) fprintf(fp, "offset %s\n\n",
			   lfptoa(&pp->offset, 6));
}
#endif // 0 printserver

#if !defined(HAVE_VSPRINTF)
int
vsprintf(
	char *str,
	const char *fmt,
	va_list ap
	)
{
	FILE f;
	int len;

	f._flag = _IOWRT+_IOSTRG;
	f._ptr = str;
	f._cnt = 32767;
	len = _doprnt(fmt, ap, &f);
	*f._ptr = 0;
	return (len);
}
#endif

#if 0
/* override function in library since SA_RESTART makes ALL syscalls restart */
#ifdef SA_RESTART
void
signal_no_reset(
	int sig,
	void (*func)()
	)
{
	int n;
	struct sigaction vec;

	vec.sa_handler = func;
	sigemptyset(&vec.sa_mask);
	vec.sa_flags = 0;

	while (1)
	{
		n = sigaction(sig, &vec, NULL);
		if (n == -1 && errno == EINTR)
			continue;
		break;
	}
	if (n == -1)
	{
		perror("sigaction");
		        return 1; /* CMC here - changed exit to return */
	}
}
#endif
#endif

#ifdef HAVE_NETINFO
static ni_namelist *
getnetinfoservers(void)
{
	ni_status status;
	void *domain;
	ni_id confdir;
	ni_namelist *namelist = (ni_namelist*)malloc(sizeof(ni_namelist));

	/* Find a time server in NetInfo */
	if ((status = ni_open(NULL, ".", &domain)) != NI_OK) return NULL;

	while (status = ni_pathsearch(domain, &confdir, NETINFO_CONFIG_DIR) == NI_NODIR) {
		void *next_domain;
		if (ni_open(domain, "..", &next_domain) != NI_OK) break;
		ni_free(domain);
		domain = next_domain;
	}
	if (status != NI_OK) return NULL;

	NI_INIT(namelist);
	if (ni_lookupprop(domain, &confdir, "server", namelist) != NI_OK) {
		ni_namelist_free(namelist);
		free(namelist);
		return NULL;
	}

	return(namelist);
}
#endif

#ifdef SYS_WINNT
isc_boolean_t ntp_port_inuse(int af, u_short port)
{
	/*
	 * Check if NTP socket is already in use on this system
	 * This is only for Windows Systems, as they tend not to fail on the real bind() below
	 */
	
	SOCKET checksocket;
	struct sockaddr_in checkservice;
	checksocket = socket(af, SOCK_DGRAM, 0);
	if (checksocket == INVALID_SOCKET) {
		return (ISC_TRUE);
	}

	checkservice.sin_family = (short) AF_INET;
	checkservice.sin_addr.s_addr = INADDR_LOOPBACK;
	checkservice.sin_port = htons(port);

	if (bind(checksocket, (struct sockaddr *)&checkservice,
		sizeof(checkservice)) == SOCKET_ERROR) {
		if ( WSAGetLastError() == WSAEADDRINUSE ){
			closesocket(checksocket);
			return (ISC_TRUE);
		}
	}
	closesocket(checksocket);
	return (ISC_FALSE);
}
#endif

// CMC -- put the getaddrinfo(), freeaddrinfo(), and in6addr() here
#ifdef _WIN32

int getaddrinfo_win(const char *nodename, const char *servname,
	const struct addrinfo *hints, struct addrinfo **res)
{
	int rval;
	struct servent *sp;
	struct addrinfo *ai = NULL;
	int port;
	const char *proto = NULL;
	int family, socktype, flags, protocol;


	/*
	 * If no name is provide just return an error
	 */
	if (nodename == NULL && servname == NULL)
		return (EAI_NONAME);
	
	ai = calloc(sizeof(struct addrinfo), 1);
	if (ai == NULL)
		return (EAI_MEMORY);

	/*
	 * Copy default values from hints, if available
	 */
	if (hints != NULL) {
		ai->ai_flags = hints->ai_flags;
		ai->ai_family = hints->ai_family;
		ai->ai_socktype = hints->ai_socktype;
		ai->ai_protocol = hints->ai_protocol;

		family = hints->ai_family;
		socktype = hints->ai_socktype;
		protocol = hints->ai_protocol;
		flags = hints->ai_flags;

		switch (family) {
		case AF_UNSPEC:
			switch (hints->ai_socktype) {
			case SOCK_STREAM:
				proto = "tcp";
				break;
			case SOCK_DGRAM:
				proto = "udp";
				break;
			}
			break;
		case AF_INET:
		case AF_INET6:
			switch (hints->ai_socktype) {
			case 0:
				break;
			case SOCK_STREAM:
				proto = "tcp";
				break;
			case SOCK_DGRAM:
				proto = "udp";
				break;
			case SOCK_RAW:
				break;
			default:
				return (EAI_SOCKTYPE);
			}
			break;
#ifdef	AF_LOCAL
		case AF_LOCAL:
			switch (hints->ai_socktype) {
			case 0:
				break;
			case SOCK_STREAM:
				break;
			case SOCK_DGRAM:
				break;
			default:
				return (EAI_SOCKTYPE);
			}
			break;
#endif
		default:
			return (EAI_FAMILY);
		}
	} else {
		protocol = 0;
		family = 0;
		socktype = 0;
		flags = 0;
	}

	rval = do_nodename(nodename, ai, hints);
	if (rval != 0) {
		freeaddrinfo_win(ai);
		return (rval);
	}

	/*
	 * First, look up the service name (port) if it was
	 * requested.  If the socket type wasn't specified, then
	 * try and figure it out.
	 */
	if (servname != NULL) {
		char *e;

		port = strtol(servname, &e, 10);
		if (*e == '\0') {
			if (socktype == 0)
				return (EAI_SOCKTYPE);
			if (port < 0 || port > 65535)
				return (EAI_SERVICE);
			port = htons((unsigned short) port);
		} else {
			sp = getservbyname(servname, proto);
			if (sp == NULL)
				return (EAI_SERVICE);
			port = sp->s_port;
			if (socktype == 0) {
				if (strcmp(sp->s_proto, "tcp") == 0)
					socktype = SOCK_STREAM;
				else if (strcmp(sp->s_proto, "udp") == 0)
					socktype = SOCK_DGRAM;
			}
		}
	} else
		port = 0;

	/*
	 *
	 * Set up the port number
	 */
	if (ai->ai_family == AF_INET)
		((struct sockaddr_in *)ai->ai_addr)->sin_port = (unsigned short) port;
	else if (ai->ai_family == AF_INET6)
		((struct sockaddr_in6 *)ai->ai_addr)->sin6_port = (unsigned short) port;
	*res = ai;
	return (0);
}

void freeaddrinfo_win(struct addrinfo *ai)
{
	if (ai->ai_canonname != NULL)
	{
		free(ai->ai_canonname);
		ai->ai_canonname = NULL;
	}
	if (ai->ai_addr != NULL)
	{
		free(ai->ai_addr);
		ai->ai_addr = NULL;
	}
	free(ai);
	ai = NULL;
}

static int
do_nodename(
	const char *nodename,
	struct addrinfo *ai,
	const struct addrinfo *hints)
{
	struct hostent *hp = NULL;
	struct sockaddr_in *sockin;
	struct sockaddr_in6 *sockin6;
	int errval;

	ai->ai_addr = calloc(sizeof(struct sockaddr_storage), 1);
	if (ai->ai_addr == NULL)
		return (EAI_MEMORY);

	/*
	 * For an empty node name just use the wildcard.
	 * NOTE: We need to assume that the address family is
	 * set elsewhere so that we can set the appropriate wildcard
	 */
	if (nodename == NULL) {
		ai->ai_addrlen = sizeof(struct sockaddr_storage);
		if (ai->ai_family == AF_INET)
		{
			sockin = (struct sockaddr_in *)ai->ai_addr;
			sockin->sin_family = (short) ai->ai_family;
			sockin->sin_addr.s_addr = htonl(INADDR_ANY);
		}
		else
		{
			sockin6 = (struct sockaddr_in6 *)ai->ai_addr;
			sockin6->sin6_family = (short) ai->ai_family;
			/*
			 * we have already zeroed out the address
			 * so we don't actually need to do this
			 * This assignment is causing problems so
			 * we don't do what this would do.
			 sockin6->sin6_addr = in6addr_any;
			 */
		}
#ifdef HAVE_SA_LEN_IN_STRUCT_SOCKADDR
		ai->ai_addr->sa_len = SOCKLEN(ai->ai_addr);
#endif

		return (0);
	}

	/*
	 * See if we have an IPv6 address
	 */
	if(strchr(nodename, ':') != NULL) {
		if (inet_pton(AF_INET6, nodename,
		    &((struct sockaddr_in6 *)ai->ai_addr)->sin6_addr) == 1) {
			((struct sockaddr_in6 *)ai->ai_addr)->sin6_family = AF_INET6;
			ai->ai_family = AF_INET6;
			ai->ai_addrlen = sizeof(struct sockaddr_in6);
			return (0);
		}
	}

	/*
	 * See if we have an IPv4 address
	 */
	if (inet_pton(AF_INET, nodename,
	    &((struct sockaddr_in *)ai->ai_addr)->sin_addr) == 1) {
		((struct sockaddr *)ai->ai_addr)->sa_family = AF_INET;
		ai->ai_family = AF_INET;
		ai->ai_addrlen = sizeof(struct sockaddr_in);
		return (0);
	}

	/*
	 * If the numeric host flag is set, don't attempt resolution
	 */
	if (hints != NULL && (hints->ai_flags & AI_NUMERICHOST))
		return (EAI_NONAME);

	/*
	 * Look for a name
	 */

	errval = DNSlookup_name(nodename, AF_INET, &hp);

	if (hp == NULL) {
		if (errval == TRY_AGAIN || errval == EAI_AGAIN)
			return (EAI_AGAIN);
		else if (errval == EAI_NONAME) {
			if (inet_pton(AF_INET, nodename,
			    &((struct sockaddr_in *)ai->ai_addr)->sin_addr) == 1) {
				((struct sockaddr *)ai->ai_addr)->sa_family = AF_INET;
				ai->ai_family = AF_INET;
				ai->ai_addrlen = sizeof(struct sockaddr_in);
				return (0);
			}
			return (errval);
		}
		else
		{
			return (errval);
		}
	}
	ai->ai_family = hp->h_addrtype;
	ai->ai_addrlen = sizeof(struct sockaddr);
	sockin = (struct sockaddr_in *)ai->ai_addr;
	memcpy(&sockin->sin_addr, hp->h_addr, hp->h_length);
	ai->ai_addr->sa_family = hp->h_addrtype;
#ifdef HAVE_SA_LEN_IN_STRUCT_SOCKADDR
	ai->ai_addr->sa_len = sizeof(struct sockaddr);
#endif
	if (hints != NULL && hints->ai_flags & AI_CANONNAME) {
		ai->ai_canonname = malloc(strlen(hp->h_name) + 1);
		if (ai->ai_canonname == NULL)
			return (EAI_MEMORY);
		strcpy(ai->ai_canonname, hp->h_name);
	}
	return (0);
}

/*
 * Encapsulate gethostbyname to control the error code
 */
int
DNSlookup_name(
	const char *name,
	int ai_family,
	struct hostent **Addresses
)
{
	*Addresses = gethostbyname(name);
	return (h_errno);
}

int getnameinfo_win(const struct sockaddr *sa, u_int salen, char *host,
	size_t hostlen, char *serv, size_t servlen, int flags)
{
	struct hostent *hp;
	int namelen;

	if (sa->sa_family != AF_INET)
		return (EAI_FAMILY);
	hp = gethostbyaddr(
	    (const char *)&((const struct sockaddr_in *)sa)->sin_addr,
	    4, AF_INET);
	if (hp == NULL) {
		if (h_errno == TRY_AGAIN)
			return (EAI_AGAIN);
		else
			return (EAI_FAIL);
	}
	if (host != NULL && hostlen > 0) {
		/*
		 * Don't exceed buffer
		 */
		namelen = (int) min(strlen(hp->h_name), hostlen - 1);
		if (namelen > 0) {
			strncpy(host, hp->h_name, namelen);
			host[namelen] = '\0';
		}
	}
	return (0);
}

#endif   // WIN32 declarations to bypass need for ws2_32.dll (Winsock2)
