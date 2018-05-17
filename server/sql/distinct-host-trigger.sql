select id,p_ncpus,os_name,p_vendor,p_model,m_nbytes,os_name,os_version from host where id in (select distinct hostid  from qcn_trigger where sw_version>3.76);
