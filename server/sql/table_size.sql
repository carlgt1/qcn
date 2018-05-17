SELECT table_name AS `Table`,  round(((data_length + index_length) / 1024 / 1024), 2) `Size in MB`  FROM information_schema.TABLES  WHERE table_schema = "sensor" order by table_name;

SELECT table_name AS `Table`,  round(((data_length + index_length) / 1024 / 1024), 2) `Size in MB`  FROM information_schema.TABLES  WHERE table_schema = "continual" order by table_name;


