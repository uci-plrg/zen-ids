

-- Evolution Table

DROP TABLE IF EXISTS opmon_evolution;

CREATE TABLE opmon_evolution (
  table_name VARCHAR(24),
  column_name VARCHAR(24),
  id BIGINT(20),
  PRIMARY KEY (table_name, column_name, id)
);


-- Evolution Triggers

DROP TRIGGER IF EXISTS evo_update_wp_commentmeta;

DELIMITER $$

CREATE TRIGGER evo_update_wp_commentmeta AFTER UPDATE ON wp_commentmeta
  FOR EACH ROW
  BEGIN
    IF NEW.comment_id != OLD.comment_id THEN
      REPLACE INTO opmon_evolution VALUES ('wp_commentmeta', 'comment_id', NEW.meta_id);
    END IF;
    IF NEW.meta_key != OLD.meta_key THEN
      REPLACE INTO opmon_evolution VALUES ('wp_commentmeta', 'meta_key', NEW.meta_id);
    END IF;
    IF NEW.meta_value != OLD.meta_value THEN
      REPLACE INTO opmon_evolution VALUES ('wp_commentmeta', 'meta_value', NEW.meta_id);
    END IF;
  END $$

DROP TRIGGER IF EXISTS evo_insert_wp_commentmeta;

CREATE TRIGGER evo_insert_wp_commentmeta AFTER INSERT ON wp_commentmeta
  FOR EACH ROW
  BEGIN
    REPLACE INTO opmon_evolution VALUES ('wp_commentmeta', 'comment_id', NEW.meta_id);
    REPLACE INTO opmon_evolution VALUES ('wp_commentmeta', 'meta_key', NEW.meta_id);
    REPLACE INTO opmon_evolution VALUES ('wp_commentmeta', 'meta_value', NEW.meta_id);
  END $$

DELIMITER ;
