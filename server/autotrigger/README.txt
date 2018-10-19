autotrigger.py -- this script is run on the web server in the BOINC/qcn/bin directory to periodically check for new quakes and
   see if any QCN hosts triggered in that time period (depends on the QCN-specific mysql functions in qcn/server/sql_functions).

   If any quakes matched, a "trickle-down" message is created (BOINC msg_to_host table) to request the SAC files to be uploaded
   from the particular host machine that matches the seismic event.


procupload.py -- this script is run on the upload server to process files returned from QCN host machines (either via the autotrigger
  script above or via a manual request from a project admin on the BOINC html/ops/trig.php page).

  The file is unzipped and matched in the database so that the upload information & download URL for the file can be put in the 
  qcn_trigger table for this event

