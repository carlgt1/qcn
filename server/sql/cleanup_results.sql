delete from sensor.result where server_state=2;
delete from continual.result where server_state=2;
delete from sensor.workunit where id not in (select distinct workunitid from sensor.result);
delete from continual.workunit where id not in (select distinct workunitid from continual.result);
