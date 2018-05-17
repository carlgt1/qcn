alter table sensor.qcn_trigger 
  add column mxy1p float 
    after magnitude,
  add column mz1p float 
    after mxy1p,
  add column mxy1a float
    after mz1p,
  add column mz1a float 
    after mxy1a,
  add column mxy2a float 
    after mz1a,
  add column mz2a float 
    after mxy2a,
  add column mxy4a float 
    after mz2a,
  add column mz4a float
    after mxy4a
;

alter table continual.qcn_trigger 
  add column mxy1p float 
    after magnitude,
  add column mz1p float 
    after mxy1p,
  add column mxy1a float
    after mz1p,
  add column mz1a float 
    after mxy1a,
  add column mxy2a float 
    after mz1a,
  add column mz2a float 
    after mxy2a,
  add column mxy4a float 
    after mz2a,
  add column mz4a float
    after mxy4a
;

alter table sensor_archive.qcn_trigger 
  add column mxy1p float 
    after magnitude,
  add column mz1p float 
    after mxy1p,
  add column mxy1a float
    after mz1p,
  add column mz1a float 
    after mxy1a,
  add column mxy2a float 
    after mz1a,
  add column mz2a float 
    after mxy2a,
  add column mxy4a float 
    after mz2a,
  add column mz4a float
    after mxy4a
;

alter table continual_archive.qcn_trigger 
  add column mxy1p float 
    after magnitude,
  add column mz1p float 
    after mxy1p,
  add column mxy1a float
    after mz1p,
  add column mz1a float 
    after mxy1a,
  add column mxy2a float 
    after mz1a,
  add column mz2a float 
    after mxy2a,
  add column mxy4a float 
    after mz2a,
  add column mz4a float
    after mxy4a
;


