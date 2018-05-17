select yearweek(from_unixtime(time_trigger)) yw, count(*) ctr from sensor.qcn_trigger group by yw;

select EXTRACT(YEAR_MONTH FROM from_unixtime(time_trigger)) ym, count(*) ctr from continual.qcn_trigger group by ym;


