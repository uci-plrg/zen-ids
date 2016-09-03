# zen-ids
Introspective intrusion detection system for PHP applications.

This repository contains the dynamically linked ZenIDS extension for the reference implementation of the PHP interpreter. This extension requires a slightly modified version of PHP, which can be found in the `interp-opt` branch of the [zen-ids-php](https://github.com/uci-plrg/zen-ids-php) repository.

### Build

1. Dependencies:
  * `interp-opt` branch of the [zen-ids-php](https://github.com/uci-plrg/zen-ids-php) repository
  * `interp-opt` branch of this repository
2. Environment:
  * Set `$PHP_HOME` to the top-level directory of the local [zen-ids-php](https://github.com/uci-plrg/zen-ids-php) clone
  * Set `$ZEN_IDS_HOME` to the top-level directory of this repository
  * Set `$ZEN_IDS_DATASETS` to any directory where the profile data can be stored
    * This location should have plenty of disk space for large applications and/or extensive profiling and monitoring.
    * Specifying a fast disk, especially an SSD, will improve overall performance.
  * Set `$ZEN_IDS_EVOLUTION` to any directory where evolution metadata can be stored.
    * This location is less write-intensive than the dataset directory, but disk speed still may affect performance.
3. Build:
  * `cd opmon && $PHP_HOME/scripts/phpize && ./current-config && make -j && sudo make -j install`
    * Use option `current-config -d` for a debug build
4. Configure:
  * `cd $PHP_HOME && mkdir conf.d/ext && cp $ZEN_IDS_HOME/opmon.ini.default $PHP_HOME/conf.d/ext/opmon.ini`

### Tools

1. Application profiles can be built with the [zen-ids-profile](https://github.com/uci-plrg/zen-ids-profile) Java tools. 
  * In the paper, these are referred to as *trusted profiles*.
 
### Experiments

See the [experiment documentation](https://github.com/uci-plrg/zen-ids/blob/interp-opt/EXPERIMENTS.md).
