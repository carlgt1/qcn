// triggers within a minute of the quake and received in that window too
select count(distinct hostid) from sensor.qcn_trigger where time_trigger between 1408875635 and 1408875700 and time_received between 1408875635 and 1408875700 and latitude between 37 and 39 and longitude between -123 and -121 and varietyid=0 and qcn_sensorid between 104 and 106;

/*  all online usb sensor computers */

select count(distinct hostid) from sensor.qcn_trigger where time_trigger between 1408874000 and 1408876600 and latitude between 37 and 39 and longitude between -123 and -121 and varietyid=1 and qcn_sensorid between 104 and 106;

select count(distinct hostid) from continual.qcn_trigger where time_trigger between 140887400 and 1408876600 and latitude between 37 and 39 and longitude between -123 and -121 and varietyid=1 and qcn_sensorid between 104 and 106;



/* detected triggers usb sensors */

select count(distinct hostid) from sensor.qcn_trigger where time_trigger between 1408875600 and 1408875720 and latitude between 37 and 39 and longitude between -123 and -121 and varietyid=0 and qcn_sensorid between 104 and 106;
select count(distinct hostid) from continual.qcn_trigger where time_trigger between 1408875600 and 1408875720 and latitude between 37 and 39 and longitude between -123 and -121 and varietyid=0 and qcn_sensorid between 104 and 106;



select min(latitude), max(latitude), min(longitude), max(longitude) from sensor.qcn_trigger where time_trigger between 1408875600 and 1408875720 and latitude between 36 and 39 and longitude between -123 and -121 and varietyid=0 and qcn_sensorid>99;

