select id,hostid,from_unixtime(time_trigger),runtime_cpu,runtime_clock from continual.qcn_trigger where runtime_cpu/runtime_clock > .50;
