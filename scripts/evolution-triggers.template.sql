

-- Evolution Triggers

DROP TRIGGER IF EXISTS evo_insert_table;
DROP TRIGGER IF EXISTS evo_update_table;

DELIMITER $$
CREATE TRIGGER evo_insert_table AFTER INSERT ON table
FOR EACH ROW
BEGIN
  IF @is_admin = TRUE THEN
    SELECT request_id INTO @request_id FROM opmon_request_sequence;
    REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'table', 'field', NEW.field);
  END IF;
END $$

CREATE TRIGGER evo_update_table AFTER UPDATE ON table
FOR EACH ROW
BEGIN
  IF @is_admin = TRUE THEN
    SELECT request_id INTO @request_id FROM opmon_request_sequence;
    IF NEW.field != OLD.field THEN
      REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form', 'field', NEW.field);
    END IF;
  END IF;
END $$

DELIMITER ;
