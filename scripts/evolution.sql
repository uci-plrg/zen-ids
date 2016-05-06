

-- Evolution Table

DROP TABLE IF EXISTS opmon_evolution;
DROP TABLE IF EXISTS opmon_evolution_staging;

CREATE TABLE opmon_evolution (
  table_name VARCHAR(24),
  column_name VARCHAR(24),
  id BIGINT(20),
  PRIMARY KEY (table_name, column_name, id)
);

CREATE TABLE opmon_evolution_staging (
  table_name VARCHAR(24),
  column_name VARCHAR(24),
  id BIGINT(20),
  connection_id int,
  PRIMARY KEY (table_name, column_name, id, connection_id)
);


-- Evolution Procedures

DROP PROCEDURE IF EXISTS opmon_evolution_commit;
DROP PROCEDURE IF EXISTS opmon_evolution_discard;

DELIMITER $$

CREATE PROCEDURE opmon_evolution_commit()
BEGIN
  REPLACE INTO opmon_evolution (
    SELECT table_name, column_name, id
    FROM opmon_evolution_staging
    WHERE connection_id = connection_id());
  CALL opmon_evolution_discard();
END $$

CREATE PROCEDURE opmon_evolution_discard()
BEGIN
  DELETE FROM opmon_evolution_staging WHERE connection_id = connection_id();
END $$

DELIMITER ;


-- Evolution Triggers

DROP TRIGGER IF EXISTS evo_update_wp_commentmeta;
DROP TRIGGER IF EXISTS evo_insert_wp_commentmeta;

DELIMITER $$

CREATE TRIGGER evo_update_wp_commentmeta AFTER UPDATE ON wp_commentmeta
FOR EACH ROW
BEGIN
  IF NEW.comment_id != OLD.comment_id THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_commentmeta', 'comment_id', NEW.meta_id, connection_id());
  END IF;
  IF NEW.meta_key != OLD.meta_key THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_commentmeta', 'meta_key', NEW.meta_id, connection_id());
  END IF;
  IF NEW.meta_value != OLD.meta_value THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_commentmeta', 'meta_value', NEW.meta_id, connection_id());
  END IF;
END $$

CREATE TRIGGER evo_insert_wp_commentmeta AFTER INSERT ON wp_commentmeta
FOR EACH ROW
BEGIN
  REPLACE INTO opmon_evolution_staging VALUES ('wp_commentmeta', 'comment_id', NEW.meta_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_commentmeta', 'meta_key', NEW.meta_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_commentmeta', 'meta_value', NEW.meta_id, connection_id());
END $$

DELIMITER ;
