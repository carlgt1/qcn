/*** sactosac.c  - examines header of a data file written by the SAC
 *                 (Seismic Analysis Code) and switches it to the other
 *                 machine type: little-endian to big-endian
 *                 Based on saclinux2sun by G. A. Ichinose (1996)
 *                 G. Helffrich/U. Bristol/Feb. 11, 1998
 *                    bug in native conversion fixed Feb. 28, 2000
 *                    formatting bug fixed,
 *                    -m option added 4 Apr. 2007
 *
 *   usage: sactosac [-fv] [files] ...
 *      -f - force conversion even if format is native for machine on which
 *           program is running.  Otherwise only converts files that are in
 *           a different format than desired for current machine.
 *      -m - force conversion to MacSAC (big-endian, Sun) byte-order.
 *      -v - verbose - describe what program is doing and what it is converting.
 ***/
#include <stdio.h>
#include <errno.h>
#include "sac.h"
#define NUMHEADBYTES 632 /* floats and longs are 440 rest are characters */
#define VERSION 6

extern int errno;
#if !defined(linux) && !defined(BSD4d2) && !defined(__APPLE__)
extern char * sys_errlist[];
char *strerror(n)
   int n;
{
   return sys_errlist[n];
}
#endif

void usage(prog)
   char *prog;
{

	fprintf(stderr,
	   "Usage: %s [-vfm] [sacfile(s)]\n"
	   "  -f - force conversion even if format is native for machine on\n"
	   "       which program is running.  Otherwise only converts files\n"
	   "       that are in a different format than desired for current\n"
	   "       machine.\n"
	   "  -m - force conversion to MacSAC (big-endian, Sun) byte-order.\n"
           "  -v - verbose - describe what program is doing and what it is\n"
	   "       converting.\n"
	, prog);
	exit(-1);
}

long long_swap(cbuf)
   char cbuf[];
{
        union {
                char cval[4];
                long lval;
        } l_union;
        l_union.cval[0] = cbuf[3];
        l_union.cval[1] = cbuf[2];
        l_union.cval[2] = cbuf[1];
        l_union.cval[3] = cbuf[0];
        return(l_union.lval);
}

float float_swap(cbuf)
   char cbuf[];
{
        union {
                char cval[4];
                float fval;
        } f_union;
 
        f_union.cval[0] = cbuf[3];
        f_union.cval[1] = cbuf[2];
        f_union.cval[2] = cbuf[1];
        f_union.cval[3] = cbuf[0];
        return(f_union.fval);
}

int main(ac,av)
   int ac;
   char **av;
{
	struct sac_header sp;
	float *data, fbuf;
	long lbuf;
	int i, j, cvt, end;
	FILE *fp;
	float float_swap();
	long  long_swap();
	long nvh_native, nvh_other, nvh_be, npts;
	long *ptr;
	struct {int verbose, force, mac;} opt;
	union { short i; char c[2]; } me;

	opt.verbose = 0; opt.force = 0; opt.mac = 0;
	
	nvh_native = VERSION; nvh_other = long_swap(&nvh_native);
	me.i = 1; end = (me.c[0] == '\0');

	if (ac == 1) usage(av[0]);
	
	for( i = 1;  i < ac; i++) {
		if (*av[i] == '-') {
		/* Option, parse */
		   for (j=1;j<strlen(av[i]);j++)
		   switch (av[i][j]) {
		   case 'v':
		      opt.verbose = 1;
		      printf("%s: On a %s-endian machine.\n",
			 av[0], end ? "big":"little");
		      break;
		   case 'f':
		      opt.force = 1;
		      break;
		   case 'm':
		      opt.mac = 1; opt.force = 0;
		      break;
		   default:
		      fprintf(stderr, "%s: Unknown option \"-%c\" ignored.\n",
			 av[0],av[i][j]);
		   }
		   continue;
		}
		if ( (fp = fopen(av[i], "r+")) == NULL) {
			fprintf(stderr, "%s Error opening file %s\n", av[0], av[i]);
			continue;
		}


	/** read in header **/
		fread((char *)&sp, 440*sizeof(char), 1, fp);
		fread((char *)&sp.kstnm,(NUMHEADBYTES-440)*sizeof(char),1,fp);

	/* Determine whether native format or not native */

		if (sp.internal4 == nvh_native) { /* NATIVE format */
		   nvh_be = end;
		   cvt = (!nvh_be & opt.mac) | opt.force;
		   if (opt.verbose) {
		      printf("%s appears to be a format native to this machine%s.\n",av[i],
		      cvt ? ", converting anyway":"");
		   }
		   if (!cvt) continue;
		   npts = sp.npts;
		} else if (sp.internal4 == nvh_other) { /* FOREIGN format */
		   nvh_be = !end;
		   cvt = !opt.mac | (opt.mac & !nvh_be);
		   if (opt.verbose)
		      printf("%s appears to be a non-native format to this machine, %s.\n",av[i],
		      cvt ? "converting anyway":"leaving big-endian/MacSAC");
		   if (!cvt) continue;
		   npts = 0;
		} else {
		   fprintf(stderr,"%s: %s doesn't seem to be any kind of known SAC file, skipping.\n",av[0],av[i]);
		   continue;
		}
		if (opt.verbose && (sp.internal4 == nvh_native)) {
		   printf("%-4.4s GCARC %g B %g E %g", sp.kstnm,
			sp.delta, sp.b, sp.e);
		   printf(" %d %d %02d:%02d:%06.3f   NPTS %d\n",
			sp.nzyear, sp.nzjday, sp.nzhour, sp.nzmin,
			sp.nzsec+sp.nzmsec/1000.0, sp.npts);
		}
		for (ptr=(long *)&sp,j=0;j<440/4;ptr++,j++)
		   *ptr = long_swap(ptr);

		if (npts == 0) npts = sp.npts; /* Converted, now proper num */
		if (opt.verbose && (sp.internal4 == nvh_native)) {
		   printf("%-4.4s GCARC %g B %g E %g", sp.kstnm, sp.delta, sp.b, sp.e);
		   printf(" %d %d %02d:%02d:%06.3f   NPTS %d\n",
			sp.nzyear, sp.nzjday, sp.nzhour, sp.nzmin,
			sp.nzsec+sp.nzmsec/1000.0, sp.npts);
		}
		
	/** read the data **/
		if (NULL == (data = (float *)malloc(npts*sizeof(float)))) {
		   fprintf(stderr,"%s: Can't get memory for %s data, skipping.\n",
		      av[0],av[i]);
		   fclose(fp);
		   continue;
		}
	        fread(data, sizeof(float), npts, fp);
		for ( j=0; j<npts; j++) {
			data[j] = (float) float_swap(&data[j]);
		}

        /** write the header **/
		fseek(fp,0L,0);
		if (1 != fwrite(&sp, sizeof(struct sac_header), 1, fp)) {
		   fprintf(stderr,"%s: Error writing %s, %s.\n",av[0],av[i],
		      sys_errlist[errno]);
		} else if (npts != fwrite(&data[0], sizeof(float), npts, fp)) {
		   fprintf(stderr,"%s: Error writing %s\n",av[0],av[i]);
		}
		fclose(fp);
		free(data);
	}
	return 0;
}
