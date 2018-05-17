#took 8 seconds

create table qcn_stats select hostid,result_name,max(runtime_clock) runtime,max(time_received) time_rec from qcn_trigger group by hostid,result_name having max(runtime_clock)>0;

