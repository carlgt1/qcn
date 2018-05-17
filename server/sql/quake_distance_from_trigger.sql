select hostid,result_name, from_unixtime(time_trigger) trig_time_utc, 
   round(latitude,2) lat, round(longitude,2) lon, 
      round(lat_lon_distance_m(latitude,longitude, 37.936, -77.933)/1000.0, 1) distance_km, 
   description sensor_type 
 from qcn_trigger, qcn_sensor 
   where qcn_quakeid=45915 and qcn_sensorid=qcn_sensor.id order by time_trigger;

