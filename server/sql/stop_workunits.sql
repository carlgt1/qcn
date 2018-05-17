DELETE FROM result
   WHERE server_state=2 AND hostid=0;

DELETE FROM workunit
   WHERE id NOT IN (SELECT DISTINCT workunitid FROM result);

