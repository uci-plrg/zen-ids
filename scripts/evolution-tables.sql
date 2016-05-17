

-- Evolution Table

DROP TABLE IF EXISTS opmon_request_sequence;
DROP TABLE IF EXISTS opmon_evolution;

CREATE TABLE opmon_request_sequence (
  request_id BIGINT(20)
);

INSERT INTO opmon_request_sequence VALUES (0);

CREATE TABLE opmon_evolution (
  request_id BIGINT(20),
  table_name VARCHAR(24),
  column_name VARCHAR(24),
  table_key BIGINT(20),
  UNIQUE KEY (table_name, column_name, table_key)
);
