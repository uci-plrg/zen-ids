# Zen IDS Configuration

#### opmon.ini

This file is deployed to `$PHP_HOME/conf.d/ext/opmon.ini` and will be loaded by the PHP interpreter when the webserver starts. The various modes of the Zen IDS extension are specified by the numeric entries in this file. Each mode is documented with the set of usable values.

* `opmon_verbose`: Controls which messages are printed to the Apache error log. Levels `Message` and `Detail` are extremely verbose and may fill the log. These should only be used for development.
* `opmon_cfi_mode`: Specifies the security mode of Zen IDS. The `Training` mode is used for generating a *trusted profile* (see [Experiment Documentation](https://github.com/uci-plrg/zen-ids/blob/interp-opt/EXPERIMENTS.md)) and never raises an alert. The remaining 4 modes monitor the PHP application according to the deployed *trusted profile*, and additionally expand the *trusted profile* according to the following expansion policies:
  * `DB CFI (2):`: Monitors the database for changes made by privileged users and expands the *trusted profile* to incorporate application features that are enabled by those changes.
  * `DGC CFI (3)`: Expands the *trusted profile* to include dynamically generated PHP files if they conform to the code generators observed during training. 
    * Warning: this mode has significant performance overhead and should not be used for processor-intensive workloads.
  * `File CFI (4)`: Monitors the filesystem for changes made by privileged users and expands the *trusted profile* to incorporate application features that are enabled by those changes.
  * `Rigid CFI (5)`: Never expands the *trusted profile*.
* `opmon_request_id_synch`: Specifies whether the request ID is maintained in a database or only in the filesystem. The database setting should only be used for applications that use a database.
* `opmon_request_edge_enabled`: Enables the `request-edge.run` profile, which records all inter-procedural edges for every request.
  * This file may become very large for long replays.
  * We recommend disabling this feature while performance benchmarking.
* `opmon_opcode_dump_enabled`: Enables a dump of all opcodes in each compiled function and script body of the PHP application. This is useful for debugging and should be disabled otherwise.
* `opmon_cfi_bailout`: For high-risk deployments where the *trusted profile* is reliably complete, this setting will terminate processing of the HTTP request when Zen IDS raises an intrusion alert.
