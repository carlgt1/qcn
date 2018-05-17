/// below here some common strings to use

#define QCNGUI_INIT_DIR        "init"
#define QCNGUI_BOINC_INIT_FILE "init_data.xml"
#define QCNGUI_APP_NAME        "QuakeCatcherNetwork.app"
#define QCNGUI_XML_PREFS_FILE  "qcnliveprefs.xml"
//#define QCNGUI_XML_GRAPHICS_PREFS_FILE  "graphicsprefs.xml"

// Win or Mac icon
#ifdef _WIN32
#define FILENAME_LOGO   "qcnwin.ico"
#else
#define FILENAME_LOGO   "qcnlogo.png"
#endif

#define FILENAME_SPLASH "splash.png"

#define SET_COMPANY "edu.iris"
#define SET_APP     "QCNLive"

#define DIR_MAKEQUAKE "../makequake"
#define PRINTER_PDF   "PDF File"


#define QT_WINDOW_SETTINGS_GEOMETRY "geometry"
#define QT_WINDOW_SETTINGS_STATE    "winstate"

#define MY_RECT_DEFAULT_POS_X    16
#define MY_RECT_DEFAULT_POS_Y    24
#define MY_RECT_DEFAULT_WIDTH   945
#define MY_RECT_DEFAULT_HEIGHT  650
 
#define XML_LATITUDE   "lat"
#define XML_LONGITUDE  "lng"
#define XML_X          "x"
#define XML_Y          "y"
#define XML_WIDTH      "w"
#define XML_HEIGHT     "h"
#define XML_STATION    "stn"
#define XML_ELEVATION  "elm"
#define XML_FLOOR      "elf"
#define XML_CONTINUAL  "cnt"
#define XML_SACFORMAT  "sac"
// #define XML_SENSOR	   "sns"
#define XML_AXIS_SINGLE  "asng"
#define XML_MAKEQUAKE_TIME "mqt"
#define XML_MAKEQUAKE_COUNTDOWN "mqc"
#define XML_VERTICAL_TIME "vtm"
#define XML_VERTICAL_TRIGGER "vtr"
#define XML_MAKEQUAKE_PRINTER "mqp"


// URL for the earthquake xml listing
#define QCNGUI_URL_QUAKE "http://quakecatcher.net/sensor/download/qcn-quake.xml"

#define QCNGUI_SHMEM       "quakewx"
#define QCNGUI_SHMEM_FILE  "boinc_quakewx_0"

// use the following list if curl failed so we have something to show!
#define QCNGUI_QUAKE_HISTORICAL "<quakes>\n \
<wequ001>  9.5|1960/05/22 19:11:14|  -38.24|-73.05 |   0.0|Chile, #1|http://earthquake.usgs.gov/regional/world/events/1960_05_22.php</wequ001>  \n \
<wequ002>  9.2|1964/03/28 03:36:14|  61.02 |-147.65|   0.0|Prince William Sound, Alaska, #2|http://earthquake.usgs.gov/regional/states/events/1964_03_28.php</wequ002> \n \
<wequ003>  9.1|2004/12/26 00:58:53|  3.30  |95.78  |   0.0|West Coast, Northern Sumatra, #3|http://earthquake.usgs.gov/eqcenter/eqinthenews/2004/usslav/</wequ003> \n \
<wequ004>  9.0|1952/11/04 16:58:26|  52.76 |160.06 |   0.0|Kamchatka, #4|http://earthquake.usgs.gov/regional/world/events/1952_11_04.php</wequ004> \n \
<wequ005>  9.0|2011/03/11 05:46:23|  38.322|142.369|  32.0|East Coast of Honshu Japan, #5|http://earthquake.usgs.gov/earthquakes/eqinthenews/2011/usc0001xgp/</wequ005> \n \
<wequ006>  8.8|2010/02/27 06:34:14| -35.846|-72.719|  35.0|Offshore Maule, Chile, #6|http://earthquake.usgs.gov/earthquakes/eqinthenews/2010/us2010tfan</wequ006> \n \
<wequ007>  8.8|1906/01/31 15:35:00|  1.0   |-81.5  |   0.0|Off the Coast of Ecuador, #7|http://earthquake.usgs.gov/regional/world/events/1906_01_31.php</wequ007> \n \
<wequ008>  8.7|1965/02/04 05:01:00|  51.21 |178.50 |   0.0|Rat Islands, Alaska, #8|http://earthquake.usgs.gov/regional/states/events/1965_02_04.php</wequ008> \n \
<wequ009>  8.6|2005/03/28 16:09:36|  2.08  |97.01  |   0.0|Northern Sumatra, Indonesia, #9|http://earthquake.usgs.gov/eqcenter/eqinthenews/2005/usweax/</wequ009> \n \
<wequ010>  8.6|1950/08/15 14:09:00|  28.5  |96.5   |   0.0|Assam, Tibet, #10|http://earthquake.usgs.gov/regional/world/events/1950_08_15.php</wequ010> \n \
<wequ011>  8.6|1957/03/09 14:22:31|  51.56 |-175.39|   0.0|Andreanof Islands, Alaska, #11|http://earthquake.usgs.gov/regional/states/events/1957_03_09.php</wequ011> \n \
<wequ012>  8.6|1938/02/01 19:04:00|  -5.05 |131.62 |   0.0|Banda Sea, Indonesia, #12|http://earthquake.usgs.gov/regional/world/events/1938_02_01.php</wequ012> \n \
<wequ013>  8.5|1923/02/03 00:00:00|  54.0  |161.0  |   0.0|Kamchatka, #13|http://earthquake.usgs.gov/regional/world/10_largest_world.php</wequ013> \n \
<wequ014>  8.5|1922/11/11 04:32:00|  -28.55|-70.5  |   0.0|Chile-Argentina Border, #14|http://earthquake.usgs.gov/regional/world/events/1922_11_11.php</wequ014> \n \
<wequ015>  8.5|1963/10/13 00:00:00|  44.9  |149.6  |   0.0|Kuril Islands, #15|http://earthquake.usgs.gov/regional/world/10_largest_world.php</wequ015> \n \
</quakes>\n"


#define QCNGUI_INIT_1 "<app_init_data>\n \
<app_version>%d</app_version>\n \
<app_name>qcn</app_name>\n \
<user_name></user_name>\n \
<team_name></team_name>\n \
<project_dir>init</project_dir>\n \
<boinc_dir>.</boinc_dir>\n \
<wu_name>%s</wu_name>\n \
<slot>0</slot>\n \
<wu_cpu_time>0.000000</wu_cpu_time>\n \
<user_total_credit>0.000000</user_total_credit>\n \
<user_expavg_credit>0.000000</user_expavg_credit>\n \
<host_total_credit>0.000000</host_total_credit>\n \
<host_expavg_credit>0.000000</host_expavg_credit>\n \
<resource_share_fraction>1.000000</resource_share_fraction>\n \
<checkpoint_period>60.000000</checkpoint_period>\n \
<fraction_done_update_period>1.000000</fraction_done_update_period>\n \
<fraction_done_start>0.000000</fraction_done_start>\n \
<fraction_done_end>1.000000</fraction_done_end>\n \
<rsc_fpops_est>1000000.000000</rsc_fpops_est>\n \
<rsc_fpops_bound>10000000000.000000</rsc_fpops_bound>\n \
<rsc_memory_bound>100000000.000000</rsc_memory_bound>\n \
<rsc_disk_bound>100000000.000000</rsc_disk_bound>\n \
<project_preferences>\n "

/*
 \
<color_scheme>Tahiti Sunset</color_scheme> \
<ssp>0</ssp> \
<lat>40.12950005266513</lat> \
<lng>-75.32405465841292</lng> \
<max_frames_sec>100</max_frames_sec> \
<qlatlng> \
<qlllat>37.427017</qlllat> \
<qlllng>-122.149630</qlllng> \
<qlllvv>3.000000</qlllvv> \
<qlllvt>1</qlllvt> \
<qllal>6</qllal> \
</qlatlng>"
*/

#define QCNGUI_INIT_2 "\n</project_preferences>\n \
<host_info>\n \
<domain_name>%s</domain_name>\n \
</host_info>\n \
</app_init_data>\n"


