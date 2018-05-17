/usr/local/mysql/bin/mysqldump -d -h db-private \
-u root -pPWD \
sensor \
qcn_align \
qcn_constant \
qcn_continual_filename \
qcn_country_latlng \
qcn_finalstats \
qcn_geo_ipaddr \
qcn_host_ipaddr \
qcn_kml_region \
qcn_level \
qcn_post \
qcn_quake \
qcn_ramp_coordinator \
qcn_ramp_participant \
qcn_recalcresult \
qcn_reds \
qcn_sensor \
qcn_showhostlocation \
qcn_stats \
qcn_trigger \
qcn_trigger_followup \
qcn_variety \
  > qcn_schema_nodata.sql


