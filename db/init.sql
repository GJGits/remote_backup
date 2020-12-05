/* Non tutti i database dovrebbero essere uguali. Per ognuno viene segnalata l'istanza opportuna che dovrebbe avere un db.
   Tuttavia per questioni di semplicita' in fase di sviluppo tutti i db avranno le stesse tabelle
 */

 CREATE DATABASE IF NOT EXISTS db_utenti;
 USE db_utenti;
 
/* db1*/
 CREATE TABLE IF NOT EXISTS UsersDB(
	username VARCHAR(50) NOT NULL,
	db INT UNSIGNED NOT NULL,
	primary key (username, db));

/* db1*/
 CREATE TABLE IF NOT EXISTS db_status(
	db INT PRIMARY KEY,
	usc INT
);

/* db2, db3...*/
 CREATE TABLE IF NOT EXISTS users(
	username VARCHAR(50) PRIMARY KEY,
	hashed_password VARCHAR(128) NOT NULL,
	salt INT UNSIGNED NOT NULL,
	device_1_MAC VARCHAR(50) DEFAULT '',
	device_2_MAC VARCHAR(50) DEFAULT '',
	device_3_MAC VARCHAR(50) DEFAULT '',
	device_4_MAC VARCHAR(50) DEFAULT '',
	device_5_MAC VARCHAR(50) DEFAULT '',
	device_6_MAC VARCHAR(50) DEFAULT '',
	device_7_MAC VARCHAR(50) DEFAULT '',
	device_8_MAC VARCHAR(50) DEFAULT ''
	);

/* db2, db3...*/
 CREATE TABLE IF NOT EXISTS chunks(
     c_username varchar(100) NOT NULL, 
     c_id integer NOT NULL, 
     c_path varchar(512) NOT NULL, 
     c_lastmod integer NOT NULL, 
     num_chunks integer, 
     device_id integer, 
     primary key (c_username, c_path, c_id));

INSERT INTO db_status(db, usc) VALUES (1,0), (2,0), (3,0);
