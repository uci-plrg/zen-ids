

-- Evolution Triggers

DROP TRIGGER IF EXISTS evo_update_wp_commentmeta;
DROP TRIGGER IF EXISTS evo_insert_wp_commentmeta;
DROP TRIGGER IF EXISTS evo_update_wp_comments;
DROP TRIGGER IF EXISTS evo_insert_wp_comments;
DROP TRIGGER IF EXISTS evo_update_wp_links;
DROP TRIGGER IF EXISTS evo_insert_wp_links;
DROP TRIGGER IF EXISTS evo_update_wp_options;
DROP TRIGGER IF EXISTS evo_insert_wp_options;
DROP TRIGGER IF EXISTS evo_update_wp_postmeta;
DROP TRIGGER IF EXISTS evo_insert_wp_postmeta;
DROP TRIGGER IF EXISTS evo_update_wp_posts;
DROP TRIGGER IF EXISTS evo_insert_wp_posts;
DROP TRIGGER IF EXISTS evo_update_wp_term_taxonomy;
DROP TRIGGER IF EXISTS evo_insert_wp_term_taxonomy;
DROP TRIGGER IF EXISTS evo_update_wp_terms;
DROP TRIGGER IF EXISTS evo_insert_wp_terms;
DROP TRIGGER IF EXISTS evo_update_wp_usermeta;
DROP TRIGGER IF EXISTS evo_insert_wp_usermeta;
DROP TRIGGER IF EXISTS evo_update_wp_users;
DROP TRIGGER IF EXISTS evo_insert_wp_users;

DELIMITER $$
CREATE TRIGGER evo_insert_wp_commentmeta AFTER INSERT ON wp_commentmeta
FOR EACH ROW
BEGIN
  REPLACE INTO opmon_evolution_staging VALUES ('wp_commentmeta', 'comment_id', NEW.meta_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_commentmeta', 'meta_key', NEW.meta_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_commentmeta', 'meta_value', NEW.meta_id, connection_id());
END $$

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

CREATE TRIGGER evo_insert_wp_comments AFTER INSERT ON wp_comments
FOR EACH ROW
BEGIN
  REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_post_ID', NEW.comment_ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_author', NEW.comment_ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_author_email', NEW.comment_ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_author_url', NEW.comment_ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_author_IP', NEW.comment_ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_date', NEW.comment_ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_date_gmt', NEW.comment_ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_content', NEW.comment_ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_karma', NEW.comment_ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_approved', NEW.comment_ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_agent', NEW.comment_ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_type', NEW.comment_ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_parent', NEW.comment_ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'user_id', NEW.comment_ID, connection_id());
END $$

CREATE TRIGGER evo_update_wp_comments AFTER UPDATE ON wp_comments
FOR EACH ROW
BEGIN
  IF NEW.comment_post_ID != OLD.comment_post_ID THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_post_ID', NEW.comment_ID, connection_id());
  END IF;
  IF NEW.comment_author != OLD.comment_author THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_author', NEW.comment_ID, connection_id());
  END IF;
  IF NEW.comment_author_email != OLD.comment_author_email THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_author_email', NEW.comment_ID, connection_id());
  END IF;
  IF NEW.comment_author_url != OLD.comment_author_url THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_author_url', NEW.comment_ID, connection_id());
  END IF;
  IF NEW.comment_author_IP != OLD.comment_author_IP THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_author_IP', NEW.comment_ID, connection_id());
  END IF;
  IF NEW.comment_date != OLD.comment_date THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_date', NEW.comment_ID, connection_id());
  END IF;
  IF NEW.comment_date_gmt != OLD.comment_date_gmt THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_date_gmt', NEW.comment_ID, connection_id());
  END IF;
  IF NEW.comment_content != OLD.comment_content THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_content', NEW.comment_ID, connection_id());
  END IF;
  IF NEW.comment_karma != OLD.comment_karma THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_karma', NEW.comment_ID, connection_id());
  END IF;
  IF NEW.comment_approved != OLD.comment_approved THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_approved', NEW.comment_ID, connection_id());
  END IF;
  IF NEW.comment_agent != OLD.comment_agent THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_agent', NEW.comment_ID, connection_id());
  END IF;
  IF NEW.comment_type != OLD.comment_type THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_type', NEW.comment_ID, connection_id());
  END IF;
  IF NEW.comment_parent != OLD.comment_parent THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'comment_parent', NEW.comment_ID, connection_id());
  END IF;
  IF NEW.user_id != OLD.user_id THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_comments', 'user_id', NEW.comment_ID, connection_id());
  END IF;
END $$

CREATE TRIGGER evo_insert_wp_links AFTER INSERT ON wp_links
FOR EACH ROW
BEGIN
  REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_url', NEW.link_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_name', NEW.link_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_image', NEW.link_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_target', NEW.link_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_description', NEW.link_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_visible', NEW.link_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_owner', NEW.link_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_rating', NEW.link_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_updated', NEW.link_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_rel', NEW.link_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_notes', NEW.link_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_rss', NEW.link_id, connection_id());
END $$

CREATE TRIGGER evo_update_wp_links AFTER UPDATE ON wp_links
FOR EACH ROW
BEGIN
  IF NEW.link_url != OLD.link_url THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_url', NEW.link_id, connection_id());
  END IF;
  IF NEW.link_name != OLD.link_name THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_name', NEW.link_id, connection_id());
  END IF;
  IF NEW.link_image != OLD.link_image THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_image', NEW.link_id, connection_id());
  END IF;
  IF NEW.link_target != OLD.link_target THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_target', NEW.link_id, connection_id());
  END IF;
  IF NEW.link_description != OLD.link_description THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_description', NEW.link_id, connection_id());
  END IF;
  IF NEW.link_visible != OLD.link_visible THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_visible', NEW.link_id, connection_id());
  END IF;
  IF NEW.link_owner != OLD.link_owner THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_owner', NEW.link_id, connection_id());
  END IF;
  IF NEW.link_rating != OLD.link_rating THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_rating', NEW.link_id, connection_id());
  END IF;
  IF NEW.link_updated != OLD.link_updated THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_updated', NEW.link_id, connection_id());
  END IF;
  IF NEW.link_rel != OLD.link_rel THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_rel', NEW.link_id, connection_id());
  END IF;
  IF NEW.link_notes != OLD.link_notes THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_notes', NEW.link_id, connection_id());
  END IF;
  IF NEW.link_rss != OLD.link_rss THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_links', 'link_rss', NEW.link_id, connection_id());
  END IF;
END $$

CREATE TRIGGER evo_insert_wp_options AFTER INSERT ON wp_options
FOR EACH ROW
BEGIN
  DECLARE option_hash BIGINT(20);
  SET option_hash = CONV(CONVERT(MD5(NEW.option_name), BINARY(8)), 16, 10);
  REPLACE INTO opmon_evolution_staging VALUES ('wp_options', 'option_name', option_hash, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_options', 'option_value', option_hash, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_options', 'autoload', option_hash, connection_id());
END $$

CREATE TRIGGER evo_update_wp_options AFTER UPDATE ON wp_options
FOR EACH ROW
BEGIN
  DECLARE option_hash BIGINT(20);
  SET option_hash = CONV(CONVERT(MD5(NEW.option_name), BINARY(8)), 16, 10);
  IF NEW.option_name != OLD.option_name THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_options', 'option_hash', option_hash, connection_id());
  END IF;
  IF NEW.option_value != OLD.option_value THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_options', 'option_value', option_hash, connection_id());
  END IF;
  IF NEW.autoload != OLD.autoload THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_options', 'autoload', option_hash, connection_id());
  END IF;
END $$

CREATE TRIGGER evo_insert_wp_postmeta AFTER INSERT ON wp_postmeta
FOR EACH ROW
BEGIN
  DECLARE meta_hash BIGINT(20);
  SET meta_hash = CONV(CONVERT(MD5(NEW.meta_key), BINARY(8)), 16, 10) ^
                  CONV(CONVERT(MD5(NEW.post_id), BINARY(8)), 16, 10);
  REPLACE INTO opmon_evolution_staging VALUES ('wp_postmeta', 'post_id', meta_hash, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_postmeta', 'meta_key', meta_hash, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_postmeta', 'meta_value', meta_hash, connection_id());
END $$

CREATE TRIGGER evo_update_wp_postmeta AFTER UPDATE ON wp_postmeta
FOR EACH ROW
BEGIN
  DECLARE meta_hash BIGINT(20);
  SET meta_hash = CONV(CONVERT(MD5(NEW.meta_key), BINARY(8)), 16, 10) ^
                  CONV(CONVERT(MD5(NEW.post_id), BINARY(8)), 16, 10);
  IF NEW.post_id != OLD.post_id THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_postmeta', 'post_id', meta_hash, connection_id());
  END IF;
  IF NEW.meta_key != OLD.meta_key THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_postmeta', 'meta_key', meta_hash, connection_id());
  END IF;
  IF NEW.meta_value != OLD.meta_value THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_postmeta', 'meta_value', meta_hash, connection_id());
  END IF;
END $$

CREATE TRIGGER evo_insert_wp_posts AFTER INSERT ON wp_posts
FOR EACH ROW
BEGIN
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_author', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_date', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_date_gmt', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_content', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_title', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_excerpt', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_status', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'comment_status', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'ping_status', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_password', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_name', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'to_ping', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'pinged', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_modified', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_modified_gmt', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_content_filtered', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_parent', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'guid', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'menu_order', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_type', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_mime_type', NEW.ID, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'comment_count', NEW.ID, connection_id());
END $$

CREATE TRIGGER evo_update_wp_posts AFTER UPDATE ON wp_posts
FOR EACH ROW
BEGIN
  IF NEW.post_author != OLD.post_author THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_author', NEW.ID, connection_id());
  END IF;
  IF NEW.post_date != OLD.post_date THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_date', NEW.ID, connection_id());
  END IF;
  IF NEW.post_date_gmt != OLD.post_date_gmt THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_date_gmt', NEW.ID, connection_id());
  END IF;
  IF NEW.post_content != OLD.post_content THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_content', NEW.ID, connection_id());
  END IF;
  IF NEW.post_title != OLD.post_title THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_title', NEW.ID, connection_id());
  END IF;
  IF NEW.post_excerpt != OLD.post_excerpt THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_excerpt', NEW.ID, connection_id());
  END IF;
  IF NEW.post_status != OLD.post_status THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_status', NEW.ID, connection_id());
  END IF;
  IF NEW.comment_status != OLD.comment_status THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'comment_status', NEW.ID, connection_id());
  END IF;
  IF NEW.ping_status != OLD.ping_status THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'ping_status', NEW.ID, connection_id());
  END IF;
  IF NEW.post_password != OLD.post_password THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_password', NEW.ID, connection_id());
  END IF;
  IF NEW.post_name != OLD.post_name THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_name', NEW.ID, connection_id());
  END IF;
  IF NEW.to_ping != OLD.to_ping THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'to_ping', NEW.ID, connection_id());
  END IF;
  IF NEW.pinged != OLD.pinged THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'pinged', NEW.ID, connection_id());
  END IF;
  IF NEW.post_modified != OLD.post_modified THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_modified', NEW.ID, connection_id());
  END IF;
  IF NEW.post_modified_gmt != OLD.post_modified_gmt THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_modified_gmt', NEW.ID, connection_id());
  END IF;
  IF NEW.post_content_filtered != OLD.post_content_filtered THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_content_filtered', NEW.ID, connection_id());
  END IF;
  IF NEW.post_parent != OLD.post_parent THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_parent', NEW.ID, connection_id());
  END IF;
  IF NEW.guid != OLD.guid THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'guid', NEW.ID, connection_id());
  END IF;
  IF NEW.menu_order != OLD.menu_order THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'menu_order', NEW.ID, connection_id());
  END IF;
  IF NEW.post_type != OLD.post_type THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_type', NEW.ID, connection_id());
  END IF;
  IF NEW.post_mime_type != OLD.post_mime_type THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'post_mime_type', NEW.ID, connection_id());
  END IF;
  IF NEW.comment_count != OLD.comment_count THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_posts', 'comment_count', NEW.ID, connection_id());
  END IF;
END $$

CREATE TRIGGER evo_insert_wp_term_taxonomy AFTER INSERT ON wp_term_taxonomy
FOR EACH ROW
BEGIN
  REPLACE INTO opmon_evolution_staging VALUES ('wp_term_taxonomy', 'term_id', NEW.term_taxonomy_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_term_taxonomy', 'taxonomy', NEW.term_taxonomy_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_term_taxonomy', 'description', NEW.term_taxonomy_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_term_taxonomy', 'parent', NEW.term_taxonomy_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_term_taxonomy', 'count', NEW.term_taxonomy_id, connection_id());
END $$

CREATE TRIGGER evo_update_wp_term_taxonomy AFTER UPDATE ON wp_term_taxonomy
FOR EACH ROW
BEGIN
  IF NEW.term_id != OLD.term_id THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_term_taxonomy', 'term_id', NEW.term_taxonomy_id, connection_id());
  END IF;
  IF NEW.taxonomy != OLD.taxonomy THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_term_taxonomy', 'taxonomy', NEW.term_taxonomy_id, connection_id());
  END IF;
  IF NEW.description != OLD.description THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_term_taxonomy', 'description', NEW.term_taxonomy_id, connection_id());
  END IF;
  IF NEW.parent != OLD.parent THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_term_taxonomy', 'parent', NEW.term_taxonomy_id, connection_id());
  END IF;
  IF NEW.count != OLD.count THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_term_taxonomy', 'count', NEW.term_taxonomy_id, connection_id());
  END IF;
END $$

CREATE TRIGGER evo_insert_wp_terms AFTER INSERT ON wp_terms
FOR EACH ROW
BEGIN
  REPLACE INTO opmon_evolution_staging VALUES ('wp_terms', 'name', NEW.term_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_terms', 'slug', NEW.term_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_terms', 'term_group', NEW.term_id, connection_id());
END $$

CREATE TRIGGER evo_update_wp_terms AFTER UPDATE ON wp_terms
FOR EACH ROW
BEGIN
  IF NEW.name != OLD.name THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_terms', 'name', NEW.term_id, connection_id());
  END IF;
  IF NEW.slug != OLD.slug THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_terms', 'slug', NEW.term_id, connection_id());
  END IF;
  IF NEW.term_group != OLD.term_group THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_terms', 'term_group', NEW.term_id, connection_id());
  END IF;
END $$

CREATE TRIGGER evo_insert_wp_usermeta AFTER INSERT ON wp_usermeta
FOR EACH ROW
BEGIN
  REPLACE INTO opmon_evolution_staging VALUES ('wp_usermeta', 'user_id', NEW.umeta_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_usermeta', 'meta_key', NEW.umeta_id, connection_id());
  REPLACE INTO opmon_evolution_staging VALUES ('wp_usermeta', 'meta_value', NEW.umeta_id, connection_id());
END $$

CREATE TRIGGER evo_update_wp_usermeta AFTER UPDATE ON wp_usermeta
FOR EACH ROW
BEGIN
  IF NEW.user_id != OLD.user_id THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_usermeta', 'user_id', NEW.umeta_id, connection_id());
  END IF;
  IF NEW.meta_key != OLD.meta_key THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_usermeta', 'meta_key', NEW.umeta_id, connection_id());
  END IF;
  IF NEW.meta_value != OLD.meta_value THEN
    REPLACE INTO opmon_evolution_staging VALUES ('wp_usermeta', 'meta_value', NEW.umeta_id, connection_id());
  END IF;
END $$

CREATE TRIGGER evo_insert_wp_users AFTER INSERT ON wp_users
FOR EACH ROW
BEGIN
  IF @is_admin = TRUE THEN
    INSERT IGNORE INTO opmon_evolution (table_name, column_name, table_key) VALUES ('wp_users', 'user_login', NEW.ID);
    INSERT IGNORE INTO opmon_evolution (table_name, column_name, table_key) VALUES ('wp_users', 'user_pass', NEW.ID);
    INSERT IGNORE INTO opmon_evolution (table_name, column_name, table_key) VALUES ('wp_users', 'user_nicename', NEW.ID);
    INSERT IGNORE INTO opmon_evolution (table_name, column_name, table_key) VALUES ('wp_users', 'user_email', NEW.ID);
    INSERT IGNORE INTO opmon_evolution (table_name, column_name, table_key) VALUES ('wp_users', 'user_url', NEW.ID);
    INSERT IGNORE INTO opmon_evolution (table_name, column_name, table_key) VALUES ('wp_users', 'user_registered', NEW.ID);
    INSERT IGNORE INTO opmon_evolution (table_name, column_name, table_key) VALUES ('wp_users', 'user_activation_key', NEW.ID);
    INSERT IGNORE INTO opmon_evolution (table_name, column_name, table_key) VALUES ('wp_users', 'user_status', NEW.ID);
    INSERT IGNORE INTO opmon_evolution (table_name, column_name, table_key) VALUES ('wp_users', 'display_name', NEW.ID);
  END IF;
END $$

CREATE TRIGGER evo_update_wp_users AFTER UPDATE ON wp_users
FOR EACH ROW
BEGIN
  IF @is_admin = TRUE THEN
    IF NEW.user_login != OLD.user_login THEN
      INSERT IGNORE INTO opmon_evolution (table_name, column_name, table_key) VALUES ('wp_users', 'user_login', NEW.ID);
    END IF;
    IF NEW.user_pass != OLD.user_pass THEN
      INSERT IGNORE INTO opmon_evolution (table_name, column_name, table_key) VALUES ('wp_users', 'user_pass', NEW.ID);
    END IF;
    IF NEW.user_nicename != OLD.user_nicename THEN
      INSERT IGNORE INTO opmon_evolution (table_name, column_name, table_key) VALUES ('wp_users', 'user_nicename', NEW.ID);
    END IF;
    IF NEW.user_email != OLD.user_email THEN
      INSERT IGNORE INTO opmon_evolution (table_name, column_name, table_key) VALUES ('wp_users', 'user_email', NEW.ID);
    END IF;
    IF NEW.user_url != OLD.user_url THEN
      INSERT IGNORE INTO opmon_evolution (table_name, column_name, table_key) VALUES ('wp_users', 'user_url', NEW.ID);
    END IF;
    IF NEW.user_registered != OLD.user_registered THEN
      INSERT IGNORE INTO opmon_evolution (table_name, column_name, table_key) VALUES ('wp_users', 'user_registered', NEW.ID);
    END IF;
    IF NEW.user_activation_key != OLD.user_activation_key THEN
      INSERT IGNORE INTO opmon_evolution (table_name, column_name, table_key) VALUES ('wp_users', 'user_activation_key', NEW.ID);
    END IF;
    IF NEW.user_status != OLD.user_status THEN
      INSERT IGNORE INTO opmon_evolution (table_name, column_name, table_key) VALUES ('wp_users', 'user_status', NEW.ID);
    END IF;
    IF NEW.display_name != OLD.display_name THEN
      INSERT IGNORE INTO opmon_evolution (table_name, column_name, table_key) VALUES ('wp_users', 'display_name', NEW.ID);
    END IF;
  END IF;
END $$

DELIMITER ;
