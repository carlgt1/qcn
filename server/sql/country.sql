-- MySQL dump 10.13  Distrib 6.0.6-alpha, for unknown-linux-gnu (x86_64)
--
-- Host: db-private    Database: continual_download
-- ------------------------------------------------------
-- Server version	6.0.6-alpha-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `country`
--

DROP TABLE IF EXISTS `country`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `country` (
  `id` varchar(4) NOT NULL DEFAULT '',
  `name` varchar(64) DEFAULT NULL,
  `population` int(11) DEFAULT NULL,
  UNIQUE KEY `country_id_u` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `country`
--

LOCK TABLES `country` WRITE;
/*!40000 ALTER TABLE `country` DISABLE KEYS */;
INSERT INTO `country` VALUES ('AD','Andorra',69865),('AE','United Arab Emirates',2523915),('AF','Afghanistan',28513677),('AG','Antigua and Barbuda',68320),('AI','Anguilla',13008),('AL','Albania',3544808),('AM','Armenia',2991360),('AN','Netherlands Antilles',218126),('AO','Angola',10978552),('AQ','Antarctica',NULL),('AR','Argentina',39144753),('AS','American Samoa',57902),('AT','Austria',8174762),('AU','Australia',19913144),('AW','Aruba',71218),('AZ','Azerbaijan',7868385),('BA','Bosnia and Herzegovina',4007608),('BB','Barbados',278289),('BD','Bangladesh',141340476),('BE','Belgium',10348276),('BF','Burkina Faso',13574820),('BG','Bulgaria',7517973),('BH','Bahrain',677886),('BI','Burundi',6231221),('BJ','Benin',7250033),('BM','Bermuda',64935),('BN','Brunei Darussalam',365251),('BO','Bolivia',8724156),('BR','Brazil',184101109),('BS','Bahamas',299697),('BT','Bhutan',2185569),('BU','Burma (no longer exists)',NULL),('BV','Bouvet Island',NULL),('BW','Botswana',1561973),('BY','Belarus',10310520),('BZ','Belize',272945),('CA','Canada',32507874),('CC','Cocos (Keeling) Islands',NULL),('CF','Central African Republic',3742482),('CG','Congo',61315970),('CH','Switzerland',7450867),('CI','Cote D\'ivoire (Ivory Coast)',17327724),('CK','Cook Islands',21200),('CL','Chile',15823957),('CM','Cameroon',16063678),('CN','China',1298847624),('CO','Colombia',42310775),('CR','Costa Rica',3956507),('CS','Czechoslovakia (no longer exists)',NULL),('CU','Cuba',11308764),('CV','Cape Verde',415294),('CX','Christmas Island',NULL),('CY','Cyprus',775927),('CZ','Czech Republic',10246178),('DD','German Democratic Republic (no longer exists)',NULL),('DE','Germany',82424609),('DJ','Djibouti',466900),('DK','Denmark',5413392),('DM','Dominica',69278),('DO','Dominican Republic',8833634),('DZ','Algeria',32129324),('EC','Ecuador',13212742),('EE','Estonia',1341664),('EG','Egypt',76117421),('EH','Western Sahara',267405),('ER','Eritrea',4447307),('ES','Spain',40280780),('ET','Ethiopia',67851281),('FI','Finland',5214512),('FJ','Fiji',880874),('FK','Falkland Islands (Malvinas)',2967),('FM','Micronesia',108155),('FO','Faroe Islands',46662),('FR','France',60424213),('FX','France, Metropolitan',NULL),('GA','Gabon',1355246),('GB','United Kingdom (Great Britain)',60270708),('GD','Grenada',89357),('GE','Georgia',4693892),('GF','French Guiana',191309),('GH','Ghana',20757032),('GI','Gibraltar',27833),('GL','Greenland',56384),('GM','Gambia',1546848),('GN','Guinea',9246462),('GP','Guadeloupe',444515),('GQ','Equatorial Guinea',523051),('GR','Greece',10647529),('GS','South Georgia and the South Sandwich Islands',NULL),('GT','Guatemala',14280596),('GU','Guam',166090),('GW','Guinea-Bissau',1388363),('GY','Guyana',705803),('HK','Hong Kong',6855125),('HM','Heard and McDonald Islands',NULL),('HN','Honduras',6823568),('HR','Croatia',4496869),('HT','Haiti',7656166),('HU','Hungary',10032375),('ID','Indonesia',238452952),('IE','Ireland',3969558),('IL','Israel',6199008),('IM','Isle of Man',76315),('IN','India',1065070607),('IO','British Indian Ocean Territory',NULL),('IQ','Iraq',25374691),('IR','Iran',67503205),('IS','Iceland',293966),('IT','Italy',58057477),('JM','Jamaica',2713130),('JO','Jordan',5611202),('JP','Japan',127333002),('KE','Kenya',32021856),('KG','Kyrgyzstan',5081429),('KH','Cambodia',13363421),('KI','Kiribati',100798),('KM','Comoros',651901),('KN','St. Kitts and Nevis',38836),('KP','Korea, Democratic People\'s Republic of',22697553),('KR','Korea, Republic of',48598175),('KW','Kuwait',2257549),('KY','Cayman Islands',43103),('KZ','Kazakhstan',15143704),('LA','Lao People\'s Democratic Republic',6068117),('LB','Lebanon',3777218),('LC','Saint Lucia',164213),('LI','Liechtenstein',33436),('LK','Sri Lanka',19905165),('LR','Liberia',3390635),('LS','Lesotho',1865040),('LT','Lithuania',3607899),('LU','Luxembourg',462690),('LV','Latvia',2306306),('LY','Libyan Arab Jamahiriya',5631585),('MA','Morocco',32209101),('MC','Monaco',32270),('MD','Moldova, Republic of',4446455),('MG','Madagascar',17501871),('MH','Marshall Islands',57738),('ML','Mali',11956788),('MM','Myanmar',42720196),('MN','Mongolia',2751314),('MO','Macau',445286),('MP','Northern Mariana Islands',78252),('MQ','Martinique',429510),('MR','Mauritania',2998563),('MS','Monserrat',9245),('MT','Malta',396851),('MU','Mauritius',1220481),('MV','Maldives',339330),('MW','Malawi',11906855),('MX','Mexico',104959594),('MY','Malaysia',23522482),('MZ','Mozambique',18811731),('NA','Namibia',1954033),('NC','New Caledonia',213679),('NE','Niger',11360538),('NF','Norfolk Island',NULL),('NG','Nigeria',137253133),('NI','Nicaragua',5359759),('NL','Netherlands',16318199),('NO','Norway',4574560),('NP','Nepal',27070666),('NR','Nauru',12809),('NT','Neutral Zone (no longer exists)',NULL),('NU','Niue',NULL),('NZ','New Zealand',3993817),('OM','Oman',2903165),('PA','Panama',3000463),('PE','Peru',27544305),('PF','French Polynesia',266339),('PG','Papua New Guinea',5420280),('PH','Philippines',86241697),('PK','Pakistan',159196336),('PL','Poland',38626349),('PM','St. Pierre and Miquelon',6995),('PN','Pitcairn',NULL),('PR','Puerto Rico',3897960),('PT','Portugal',10524145),('PW','Palau',20016),('PY','Paraguay',6191368),('QA','Qatar',840290),('RE','Reunion',766153),('RO','Romania',22355551),('RU','Russian Federation',143782338),('RW','Rwanda',7954013),('SA','Saudi Arabia',25795938),('SB','Solomon Islands',523617),('SC','Seychelles',80832),('SD','Sudan',39148162),('SE','Sweden',8986400),('SG','Singapore',4353893),('SH','St. Helena',7415),('SI','Slovenia',2011473),('SJ','Svalbard and Jan Mayen Islands',NULL),('SK','Slovakia',5423567),('SL','Sierra Leone',5883889),('SM','San Marino',28503),('SN','Senegal',10852147),('SO','Somalia',8304601),('SR','Suriname',436935),('ST','Sao Tome and Principe',181565),('SU','Union of Soviet Socialist Republics (no longer exists)',NULL),('SV','El Salvador',6587541),('SY','Syrian Arab Republic',18016874),('SZ','Swaziland',1169241),('TC','Turks and Caicos Islands',19956),('TD','Chad',9538544),('TF','French Southern Territories',NULL),('TG','Togo',5556812),('TH','Thailand',64865523),('TJ','Tajikistan',7011556),('TK','Tokelau',NULL),('TM','Turkmenistan',4863169),('TN','Tunisia',9974722),('TO','Tonga',110237),('TP','East Timor',1019252),('TR','Turkey',68893918),('TT','Trinidad and Tobago',1096585),('TV','Tuvalu',11468),('TW','Taiwan',22749838),('TZ','Tanzania, United Republic of',36588225),('UA','Ukraine',47732079),('UG','Uganda',26404543),('UM','United States Minor Outlying Islands',NULL),('US','United States of America',293027571),('UY','Uruguay',3399237),('UZ','Uzbekistan',26410416),('VA','Vatican City State (Holy See)',NULL),('VC','St. Vincent and the Grenadines',117193),('VE','Venezuela',25017387),('VG','British Virgin Islands',22187),('VI','United States Virgin Islands',108775),('VN','Viet Nam',82689518),('VU','Vanuatu',202609),('WF','Wallis and Futuna Islands',15880),('WS','Samoa',177714),('YD','Democratic Yemen (no longer exists)',NULL),('YE','Yemen',20024867),('YT','Mayotte',186026),('ZA','South Africa',42718530),('ZM','Zambia',10462436),('ZR','Zaire',NULL),('ZW','Zimbabwe',12671860),('ZZ','Unknown or unspecified country',NULL);
/*!40000 ALTER TABLE `country` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2009-08-17 10:14:49
