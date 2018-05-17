UPDATE continual.qcn_trigger SET flag=1 WHERE time_trigger < (SELECT value_int+1 FROM sensor.qcn_constant WHERE description='ArchiveTime');
INSERT INTO continual_archive.qcn_trigger SELECT * FROM continual.qcn_trigger WHERE flag=1;
DELETE FROM continual.qcn_trigger WHERE flag=1;
OPTIMIZE TABLE continual_archive.qcn_trigger;
OPTIMIZE TABLE continual.qcn_trigger;
UPDATE continual.result SET xml_doc_in=NULL, xml_doc_out=NULL where server_state>2;
UPDATE continual.workunit SET xml_doc=NULL,min_quorum=0,target_nresults=0 
   WHERE id IN (SELECT workunitid FROM continual.result WHERE server_state>2);
UPDATE continual.result r, continual.workunit w SET r.server_state=4 WHERE r.server_state=2
   AND r.workunitid=w.id AND w.xml_doc IS NULL;
OPTIMIZE TABLE continual.result;
OPTIMIZE TABLE continual.workunit;

UPDATE sensor.qcn_trigger SET flag=1 WHERE time_trigger < (SELECT value_int+1 FROM sensor.qcn_constant WHERE description='ArchiveTime');
INSERT INTO sensor_archive.qcn_trigger SELECT * FROM sensor.qcn_trigger WHERE flag=1;
DELETE FROM sensor.qcn_trigger WHERE flag=1;
OPTIMIZE TABLE sensor_archive.qcn_trigger;
OPTIMIZE TABLE sensor.qcn_trigger;
UPDATE sensor.result SET xml_doc_in=NULL, xml_doc_out=NULL where server_state>2;
UPDATE sensor.workunit SET xml_doc=NULL,min_quorum=0,target_nresults=0 
   WHERE id IN (SELECT workunitid FROM sensor.result WHERE server_state>2);
UPDATE sensor.result r, sensor.workunit w SET r.server_state=4 WHERE r.server_state=2
   AND r.workunitid=w.id AND w.xml_doc IS NULL;
OPTIMIZE TABLE sensor.result;
OPTIMIZE TABLE sensor.workunit;
