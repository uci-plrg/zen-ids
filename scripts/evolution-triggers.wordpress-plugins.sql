

-- Evolution Triggers

DROP TRIGGER IF EXISTS evo_insert_wp_rg_form;
DROP TRIGGER IF EXISTS evo_update_wp_rg_form;
DROP TRIGGER IF EXISTS evo_insert_wp_rg_form_meta;
DROP TRIGGER IF EXISTS evo_update_wp_rg_form_meta;
DROP TRIGGER IF EXISTS evo_insert_wp_rg_form_view;
DROP TRIGGER IF EXISTS evo_update_wp_rg_form_view;

DELIMITER $$
CREATE TRIGGER evo_insert_wp_rg_form AFTER INSERT ON wp_rg_form
FOR EACH ROW
BEGIN
  IF @is_admin = TRUE THEN
    SELECT request_id INTO @request_id FROM opmon_request_sequence;
    REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form', 'id', NEW.id);
    REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form', 'title', NEW.id);
    REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form', 'date_created', NEW.id);
    REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form', 'is_active', NEW.id);
    REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form', 'is_trash', NEW.id);
  END IF;
END $$

CREATE TRIGGER evo_update_wp_rg_form AFTER UPDATE ON wp_rg_form
FOR EACH ROW
BEGIN
  IF @is_admin = TRUE THEN
    SELECT request_id INTO @request_id FROM opmon_request_sequence;
    IF NEW.id != OLD.id THEN
      REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form', 'id', NEW.id);
    END IF;
    IF NEW.title != OLD.title THEN
      REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form', 'title', NEW.title);
    END IF;
    IF NEW.date_created != OLD.date_created THEN
      REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form', 'date_created', NEW.date_created);
    END IF;
    IF NEW.is_active != OLD.is_active THEN
      REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form', 'is_active', NEW.is_active);
    END IF;
    IF NEW.is_trash != OLD.is_trash THEN
      REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form', 'is_trash', NEW.is_trash);
    END IF;
  END IF;
END $$

DELIMITER $$
CREATE TRIGGER evo_insert_wp_rg_form_meta AFTER INSERT ON wp_rg_form_meta
FOR EACH ROW
BEGIN
  IF @is_admin = TRUE THEN
    SELECT request_id INTO @request_id FROM opmon_request_sequence;
    REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form_meta', 'form_id', NEW.form_id);
    REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form_meta', 'display_meta', NEW.display_meta);
    REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form_meta', 'entries_grid_meta', NEW.entries_grid_meta);
    REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form_meta', 'confirmations', NEW.confirmations);
    REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form_meta', 'notifications', NEW.notifications);
  END IF;
END $$

CREATE TRIGGER evo_update_wp_rg_form_meta AFTER UPDATE ON wp_rg_form_meta
FOR EACH ROW
BEGIN
  IF @is_admin = TRUE THEN
    SELECT request_id INTO @request_id FROM opmon_request_sequence;
    IF NEW.form_id != OLD.form_id THEN
      REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form_meta', 'form_id', NEW.form_id);
    END IF;
    IF NEW.display_meta != OLD.display_meta THEN
      REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form_meta', 'display_meta', NEW.display_meta);
    END IF;
    IF NEW.entries_grid_meta != OLD.entries_grid_meta THEN
      REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form_meta', 'entries_grid_meta', NEW.entries_grid_meta);
    END IF;
    IF NEW.confirmations != OLD.confirmations THEN
      REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form_meta', 'confirmations', NEW.confirmations);
    END IF;
    IF NEW.notifications != OLD.notifications THEN
      REPLACE INTO opmon_evolution (request_id, table_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form_meta', 'notifications', NEW.notifications);
    END IF;
  END IF;
END $$

DELIMITER $$
CREATE TRIGGER evo_insert_wp_rg_form_view AFTER INSERT ON wp_rg_form_view
FOR EACH ROW
BEGIN
  IF @is_admin = TRUE THEN
    SELECT request_id INTO @request_id FROM opmon_request_sequence;
    REPLACE INTO opmon_evolution (request_id, wp_rg_form_view_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form_view', 'id', NEW.id);
    REPLACE INTO opmon_evolution (request_id, wp_rg_form_view_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form_view', 'form_id', NEW.form_id);
    REPLACE INTO opmon_evolution (request_id, wp_rg_form_view_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form_view', 'date_created', NEW.date_created);
    REPLACE INTO opmon_evolution (request_id, wp_rg_form_view_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form_view', 'ip', NEW.ip);
    REPLACE INTO opmon_evolution (request_id, wp_rg_form_view_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form_view', 'count', NEW.count);
  END IF;
END $$

CREATE TRIGGER evo_update_wp_rg_form_view AFTER UPDATE ON wp_rg_form_view
FOR EACH ROW
BEGIN
  IF @is_admin = TRUE THEN
    SELECT request_id INTO @request_id FROM opmon_request_sequence;
    IF NEW.id != OLD.id THEN
      REPLACE INTO opmon_evolution (request_id, wp_rg_form_view_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form', 'id', NEW.id);
    END IF;
    IF NEW.form_id != OLD.form_id THEN
      REPLACE INTO opmon_evolution (request_id, wp_rg_form_view_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form', 'form_id', NEW.form_id);
    END IF;
    IF NEW.date_created != OLD.date_created THEN
      REPLACE INTO opmon_evolution (request_id, wp_rg_form_view_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form', 'date_created', NEW.date_created);
    END IF;
    IF NEW.ip != OLD.ip THEN
      REPLACE INTO opmon_evolution (request_id, wp_rg_form_view_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form', 'ip', NEW.ip);
    END IF;
    IF NEW.count != OLD.count THEN
      REPLACE INTO opmon_evolution (request_id, wp_rg_form_view_name, column_name, table_key)
        VALUES (@request_id, 'wp_rg_form', 'count', NEW.count);
    END IF;
  END IF;
END $$

DELIMITER ;
