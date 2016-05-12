

-- Evolution Table

DROP TABLE IF EXISTS opmon_evolution;

CREATE TABLE opmon_evolution (
  id BIGINT(20) KEY AUTO_INCREMENT,
  table_name VARCHAR(24),
  column_name VARCHAR(24),
  table_key BIGINT(20),
  UNIQUE KEY (table_name, column_name, table_key)
);
