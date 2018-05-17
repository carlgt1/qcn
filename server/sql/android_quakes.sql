select t.id, qcn_sensorid, from_unixtime(time_trigger), from_unixtime(time_received), qcn_quakeid, q.description from sensor_archive.qcn_trigger t,sensor.qcn_quake q where qcn_sensorid=7 and qcn_quakeid>0 and qcn_quakeid=q.id;

select t.id, qcn_sensorid, from_unixtime(time_trigger), from_unixtime(time_received), qcn_quakeid, q.description from sensor.qcn_trigger t,sensor.qcn_quake q where qcn_sensorid=7 and qcn_quakeid>0 and qcn_quakeid=q.id;


