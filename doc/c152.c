#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <mysql.h>
#include <time.h>

#define SIZE 256
#define PI 3.14159265
#define def_host_name  NULL /* host to connect to (default = localhost) */
#define def_user_name  NULL /* user name (default = your login name) */
#define def_password   NULL /* password (default = none) */
#define def_db_name    NULL /* database to use (default = none) */

MYSQL  *conn;        /* pointer to connection handler */

char datetime[25], eqmag[14];
double lat1, long1;

main()
{
FILE *fp;
char ch, s[20], t[20], year[6], month[4], date[4], hour[4], minute[4], seconds[7], datetime2[25];
char *p, *q, *dt, *yr, *mn, *dte, *hr, *min, *sec, *eqm;
long year2, month2, date2, hour2, minute2; /* variable declaration for coverting strings to long integers, etc */
double seconds2;

p = s;
q = t;
dt = datetime;
yr = year;
mn = month;
dte = date;
hr = hour;
min = minute;
sec = seconds;
eqm = eqmag;

int i = 1;

fp = fopen ("catsearch_04-01_05-12.20255", "r");
if (fp == NULL)
	printf("File could not be opened");

while (1)                /* This loop is for basically to read the catalog which is having field of fixed lenth and to call other subfucnctions       */ 
{ 	ch = fgetc(fp);
	if (ch == EOF)
		break;

if (i < 23 )
{	 *dt = ch;
		dt++;
		if (i < 5 )
		{ *yr = ch;
			yr++;
		}
		else if (i < 6 )
		{ }
		else if (i < 8)
		{ *mn = ch;
			mn++;
		}
		else if (i < 9 )
		{ }
		else if (i < 11)
		{ *dte = ch;
			dte++;
		}
		else if (i < 12 )
		{ }
		else if (i < 14)
		{ *hr = ch;
			hr++;
		}
		else if (i < 15 )
		{ }
		else if (i < 17)
		{ *min = ch;
			min++;
		}
		else if (i < 18 )
		{ }
		else if (i < 23)
		{ *sec = ch;
			sec++;
		}
		else 
		{ }
}

else if (i < 32)
	{ *p = ch;
		p++;
	} 
else if ( i < 42)
	{ *q = ch;
		q++;
	} 
else if (i < 49)
	{}
else if (i < 60)
	{ *eqm = ch;
		eqm++; 
	}
else 
	{}

if (i == 60)
	{ *p = '\0';
	*q = '\0';
	*dt = '\0';
		*yr = '\0';
		*mn = '\0';
		*dte = '\0';
		*hr = '\0';
		*min = '\0';
		*sec = '\0';
		*eqm = '\0';
	lat1 = atof ( s );
	long1 = atof ( t );

		year2 = atol (year);
		month2 = atol (month);
		date2 = atol (date);
		hour2 = atol (hour);
		minute2 = atol (minute);
		seconds2 = atof (seconds);
/*Sign Conventions: Entered Latitude value should be +ve if it is towards North and -ve if it is towards South*/
/*Similarly Entered Longitude value should be +ve if it is towards East and -ve if it is towards west */
	
	printf("%s %s %s \t\t %lf %lf  \t\t", datetime, s, t, lat1, long1);
		/* printf("%s-%s-%s \t %s:%s:%s\n", year, month, date, hour, minute, seconds); */
		printf("%ld-%ld-%ld \t %ld:%ld:%lf\n", year2, month2, date2, hour2, minute2, seconds2);
	DeltaDT(year2, month2, date2, hour2, minute2, seconds2, datetime2);    /*********************** Subfunction to add x sec to the erthquake time ********************/
		printf("\t\t\tDate time2 = %s\n", datetime2);	
		/*  Calling Subfunction to Execute MySql Query */
	LapInReg (lat1, long1, datetime, datetime2);  /*      ##############################  Passing latitude, longitude, eq. time amd  eq. time + x sec to a subfunction to perform mysql query  #########  */
	}
i = i + 1;

if (ch == '\n')
{
i = 1;
p = s;
q = t;
dt = datetime;
yr = year;
mn = month;
dte = date;
hr = hour;
min = minute;
sec = seconds;
eqm = eqmag;
}
}

fclose(fp);
}


/* 	************	Subfunction to  Execute MySQL Query *************		*/


LapInReg(double lat11, double long11, char *DT, char *DT2)
{
/*********String Concatenations******************/

char s1[] = "Select id, result_name, latitude, longitude, from_unixtime(time_trigger) from snapshorts.qcn_trigger where latitude between", s2[] = "and", s[500], s3[] = "and longitude between", s4[] = "and", datetime[25], datetime2[25], *dt, *dt2;   

dt = datetime;
dt2 = datetime2;

while ( *DT != '\0')
	{ *dt = *DT;
		dt++;
		 DT++;
	}
*dt = '\0';
								
while ( *DT2 != '\0')
	{ *dt2 = *DT2;
		dt2++;
		 DT2++;
	}
*dt2 = '\0';
									/* 	--------> Note : Define size of s[] based on the mysql statement      */ 
MYSQL_RES *res_set;

/*********For Boundry Determination******************/

double Latitude, Longitude, RBoundary, LBoundary, TBoundary, BBoundary;

	Latitude = lat11;
	Longitude = long11;

Boundary(Latitude, Longitude, &RBoundary, &LBoundary, &TBoundary, &BBoundary); /*********Subfunction For Boundry Determination******************/

	/* lat1 = BBoundary;   /*Bottom Boundary*/
	/* lat2 = TBoundary;   /*Top Boundary*/
	/*long1 = LBoundary;  /* Left Boundary */
	/*long2 = RBoundary;  /* Right Bou  */

sprintf(s, "%s %lf %s %lf %s %lf %s %lf and time_trigger between unix_timestamp('%s') and unix_timestamp('%s')", s1, BBoundary, s2, TBoundary, s3, LBoundary, s4, RBoundary, datetime, datetime2);


printf("\n MySQL Query \n%s\n\n", s);

/************ Connecting with MySQL***************/

  conn = mysql_init (NULL);
	if (conn == NULL)
	{ 
	fprintf (stderr, "mysql_init() failed (probably out of memory)\n");
	}	
	else
	printf("\nMySQL server is connection is established\n");
 	
if(mysql_real_connect (
        conn,          /* pointer to connection handler */
        def_host_name, /* host to connect to */
        def_user_name, /* user name */
        def_password,  /* password */
        def_db_name,   /* database to use */
        0,             /* port (use default) */
        NULL,          /* socket (use default) */
        0)            /* flags (none) */
 	== NULL)
	{
	fprintf (stderr, "mysql_real_connect() failed: \nError %u (%s)\n",
				mysql_errno (conn), mysql_error (conn));
	}
	else
	printf("Longin to MySQL server is successful\n\n\n");

/***************************** Place to Play with MySQL *****  MySQL Executable Statements*********************/

	/* ----  Querry---------*/
		
	if (mysql_query(conn, s) != 0)
		 printf("Query Failed");
	else
		{
		res_set = mysql_store_result (conn);   /* generate result set */
		if (res_set == NULL)
		  	printf("Query Failed");
		else
		{	process_result_set1 (conn, res_set);
			mysql_free_result (res_set);
		}
		}

/*************************** End of Place to Work with MySQL **************************/
  mysql_close (conn);
 
}


/*	************ 	Sub-Function to show MySQL Statements  **************** 	*/

process_result_set1(MYSQL *conn, MYSQL_RES *res_set)

{
MYSQL_ROW row;
unsigned int i;
FILE *ftp;  
double d, lat2, long2;
double DistanceBetwnPoints(double, double, double, double);

ftp = fopen("Output/result3.out", "a"); 


while ((row=mysql_fetch_row(res_set)) != NULL)
{
	for (i = 0; i < mysql_num_fields (res_set); i++)
	{ if (i>0)
		fputc('\t',stdout);  
			fputc('\t',ftp);  
	printf("%s", row[i] != NULL ? row[i] : "NULL");
		fprintf(ftp, "%s", row[i] != NULL ? row[i] : "NULL");
	}
	fputc ('\n', stdout);  
			lat2 = atof (row[2]);
			long2 = atof (row[3]);
			d = DistanceBetwnPoints(lat1, long1, lat2, long2);  /*   ##############Above two lines assignments Depends upon the sequence of fields requested  ####################  */
			fprintf(ftp, "\t%s\t%lf\t%lf\t%s\t%lf", eqmag, lat1, long1, datetime, d);		
		fputc ('\n', ftp);  
}
fclose(ftp);
	if(mysql_errno (conn) != 0)
	printf("mysql_fetch_row() failed");
	else
	printf("%lu rows returned \n", (unsigned long) mysql_num_rows (res_set));
}


/*************  Sub-routines to det Boundaries of Region of Interest ********************/

Boundary(double Latitude, double Longitude, double *RBoundary, double *LBoundary, double *TBoundary, double *BBoundary)
{double LongDifference(double, double), LatDifference(double, double);
double LongDiff, LatDiff;

LongDiff = LongDifference(Latitude, Longitude);   

LatDiff = LatDifference(Latitude, Longitude);   

*RBoundary = Longitude + LongDiff;

*LBoundary = Longitude - LongDiff;
 
*TBoundary = Latitude + LatDiff;

*BBoundary = Latitude - LatDiff;

/* printf(" \n From Fuction Boundary \n LongDiff = %f  LatDiff = %f\n\n", LongDiff, LatDiff); */
}


/**********Function for obtaining Longitude of a Point B moved from a Point A laterally towards East side (i.e. latitude is constant) */

double LongDifference(double latitude, double longitude)
{
double lat1, lat1b, lat2, long1, long2, c, var1, DiffLong, d, R;

/*Enter Latitude value +ve if it is towards North, if towards South, enter its value with -ve*/
lat1 = latitude;
lat2 = lat1;

lat1b = 90 - lat1;

/* Enter Longitude value +ve if it is towards East and if it is towards west enter its value -ve */
long1 = longitude;
d = 100;
R = 6378;

c = cos (d / R);

var1 = (c - cos (lat1b*PI/180) * cos(lat1b*PI/180)) / sin(lat1b*PI/180) / sin(lat1b*PI/180);

DiffLong = acos(var1) * 180 / PI;    /* It represents difference in Longitudes */

long2 = DiffLong + long1;

/* printf(" \n Long1 =  %f,  Lat1 =  %f, \n Difference in Longitudes = %f, long2 = %f  \n", long1, lat1, DiffLong, long2); */

return(DiffLong);
}


/******************Function for obtaining Latitude of a Point B moved from a Point A vertically (downwards) towards South side (i.e. longitude is constant) */

double LatDifference(double latitude, double longitude)
{
double lat1, lat1b, lat2, lat2b, long1, long2, c, var1, Error, DiffLat, d, R, x1, x2, x3, x4;

/*Enter Latitude value +ve if it is towards North, if towards South, enter its value with -ve*/
lat1 = latitude;

lat1b = 90 - lat1;

/* Enter Longitude value +ve if it is towards East and if it is towards west enter its value -ve */
long1 = longitude;
long2 = long1;

d = 100;  /* the differece between two points is 100 kms */
R = 6378;  /* Radius of Earth */

c = cos (d / R);

x1 = sin (lat1b*PI/180);
x2 = cos (lat1b*PI/180);

lat2b = lat1b;

x3 = sin (lat2b*PI/180);
x4 = cos (lat2b*PI/180);
var1 = x1 * x3 + x2 * x4;
Error = fabs((c-var1)/c*100);

while (Error >= 0.002)

{
lat2b = lat2b + 0.1;
x3 = sin (lat2b*PI/180);
x4 = cos (lat2b*PI/180);
var1 = x1 * x3 + x2 * x4;
Error = fabs((c - var1)/c*100);  
}

lat2 = 90 - lat2b;  /* +value represents towards North and -ve values represents latitude is towards South) */

DiffLat = fabs(lat2b - lat1b); 

/* printf("\n Long1 = %f,  Lat1 =  %f, \n Difference in Latitudes = %f, lat2 = %f  \n", long1, lat1, DiffLat, lat2); */

return(DiffLat);
}


/* *************	Subfunction to Add Delta DT to the EQ time 	******	*/

DeltaDT(long yr, long mon, long jday, long hr, long mn, double sec, char *datetime)
{
long yc, gyr, gmon, gmondays;
double check, gsec;
long nleap;
double gday, ghr, gmin;
struct tm *gtime;
time_t time_sec;  /* data type is long int */
char buffer[SIZE], *buf;
buf = buffer;

gyr = yr - 1970;
yc = (yr - 1) - 1968; /* To adjust leaf years in between currenct year and 1970. 1 we r neglecting current year. We r actually substracting 1972 from yr as it is leap year */
nleap = (long)(yc/4);  /* to count presence of leap year 1972 */

/* printf("nleap = %ld\n", nleap); */
/*printf("gyr = %ld, gmonthDays= %ld gday= %ld ghr= %ld  gmin = %ld\n, sec= %lf\n", yr, mon, jday, hr, mn, sec); */

gmon = mon-1;
if (gmon == 1)
	gmondays = 31;
else if(gmon == 2)
	gmondays = 31+28;  /* Leap year effect is considered the after else if */
else if(gmon == 3)
	gmondays = 31+28+31;
else if(gmon == 4)
	gmondays = 31+28+31+30;
else if(gmon == 5)
	gmondays = 31+28+31+30+31;
else if(gmon == 6)
	gmondays = 31+28+31+30+31+30;
else if(gmon == 7)
	gmondays = 31+28+31+30+31+30+31;
else if(gmon == 8)
	gmondays = 31+28+31+30+31+30+31+31;
else if(gmon == 9)
	gmondays = 31+28+31+30+31+30+31+31+30;
else if(gmon == 10)
	gmondays = 31+28+31+30+31+30+31+31+30+31;
else if(gmon == 11)
	gmondays = 31+28+31+30+31+30+31+31+30+31+30;
else
	{gmondays = 0;} /* This case is for month of January where gmon becomes 0  */
if (gmon >= 2)
	{check = (yr - 1968)%4;
/*	printf("check=%f\n",check); */
	if (check == 0)
		gmondays = gmondays + 1;
	}

gday = gyr*365.0 + nleap + gmondays + jday - 1;
ghr = gday*24.0 + hr;
gmin = ghr*60.0 + mn;
gsec = gmin*60.0 + sec;

/* printf("gyr = %ld, gmonthDays= %d gday= %lf ghr= %lf  gmin = %lf\n", gyr, gmondays, gday, ghr, gmin); */
printf("Unix Time (in sec since 1970) = %lf\t", gsec);

time_sec = gsec + 30;   /* ######################## ADDED 30 Sec TO THE TIME ############# Any further changes to addition of time should follow this step here ################################## */
/* ****************	As time_sec var data type is long int, it cannot hold decimal part of sec ********************	*/
gtime = gmtime(&time_sec);  /* ***********gmtime is fuction defined in time.h to convert given sec to date time based on GMT. *********/
				/************* gtime is 'struct tm' structure variable . struct tm defined in time.h 	************************************/
strftime (buffer, SIZE, "%Y-%m-%d %H:%M:%S\n", gtime);  /***** &&&&&& We r loosing seconds after decimal (milly sec) with gtime conversion ############## MAY RESULT IN ONE SECOND LESS THAN EXPECTED ####################################***/
/* fputs (buffer, stdout); */

while (*buf != '\0')
	{*datetime = *buf;
		buf++;
		datetime++;
	}
*datetime = '\0';
}


/*Program for obtaining distance between two Points on Earth */

double DistanceBetwnPoints(double lat1, double long1, double lat2, double long2)
{
double lat1b, lat2b, c, var1, Error, DiffLat, d, R, x1, x2, x3, x4;

/*Enter Latitude value +ve if it is towards North, if towards South, enter its value with -ve*/
/* Enter Longitude value +ve if it is towards East and if it is towards west enter its value -ve */

lat1b = 90 - lat1;
lat2b = 90 - lat2;

R = 6378;  /* Radius of Earth */

c = cos (d / R);

x1 = sin (lat1b*PI/180);
x2 = cos (lat1b*PI/180);
x3 = sin (lat2b*PI/180);
x4 = cos (lat2b*PI/180);

c = x1 * x3 * cos ((long1 - long2)*PI/180) + x2 * x4;

d = R * acos(c);

/* printf("Distance = %f \n", d);	*/

return(d);
}
