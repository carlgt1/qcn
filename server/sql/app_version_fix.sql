

use sensor;

update app_version set xml_doc=replace(xml_doc, '</version_name>', '</version_num>');

use continual;

update app_version set xml_doc=replace(xml_doc, '</version_name>', '</version_num>');


