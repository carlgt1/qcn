/usr/local/mysql/bin/mysqldump -h db-private \
-u root -pPWD \
sensor \
qcn_align \
qcn_constant \
qcn_country_latlng \
qcn_geo_ipaddr \
qcn_kml_region \
qcn_level \
qcn_post \
qcn_quake \
qcn_sensor \
qcn_variety \
  > qcn_lookup.sql

