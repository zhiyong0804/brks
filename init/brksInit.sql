create database dongnaobike;

use dongnaobike; 

create table accountrecords ( 
	id  INT(16)   AUTO_INCREMENT primary key, 
	mobile varchar(16) not null, 
	stmp timestamp not null default '1970-01-01 08:00:01', 
	type  tinyint(1)  not null default 0, 
	money int(11) not null default 0 
)DEFAULT CHARSET=utf8;

create table bikeinfo ( 
	id int(11) AUTO_INCREMENT primary key, 
	status tinyint(1) not null  default 0, 
	trouble int(11) not null default 0, 
	tmsg  varchar(16), 
	latitude double(10,6) not null default 0.000000, 
	longitude  double(10,6) not null default 0.000000, 
	DEVNO INT(16) UNIQUE DEFAULT NULL, 
	type int(11) not null default 0, 
	mobile  varchar(16) default null, 
	sstmp timestamp not null DEFAULT CURRENT_TIMESTAMP 
)DEFAULT CHARSET=utf8;

create table travelinfo ( 
	id int(16)  AUTO_INCREMENT primary key, 
	mobile varchar(16)  not null UNIQUE, 
	type tinyint(1)  not null default 0, 
	mileage double default null, 
	discharge double default null, 
	calorie double default null, 
	stmp bigint(20) not null default 0, 
	duration int(4) not null default 0, 
	amount int(1) not null default 0 
)DEFAULT CHARSET=utf8;

create table userinfo ( 
	id int(16)  AUTO_INCREMENT primary key, 
	mobile varchar(16)  not null UNIQUE default 1300000000, 
	username varchar(128) not null default 0, 
	verify int(4) not null default 0, 
	registertm timestamp  not null  DEFAULT CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP, 
	MONEY INT(4) not null default 0 
)DEFAULT CHARSET=utf8;
