# zen-ids
Introspective intrusion detection system for PHP applications.

This repository contains the dynamically linked ZenIDS extension for the reference implementation of the PHP interpreter. This extension requires a slightly modified version of PHP, which can be found in the `interp-opt` branch of the [zen-ids-php](https://github.com/uci-plrg/zen-ids-php) repository.

### Build

1. Dependencies:
  * `interp-opt` branch of the [zen-ids-php](https://github.com/uci-plrg/zen-ids-php) repository
  * `interp-opt` branch of this repository
2. Environment:
  * Set `$PHP_HOME` to the top-level directory of the local [zen-ids-php](https://github.com/uci-plrg/zen-ids-php) clone
3. Build:
  * `cd opmon && $PHP_HOME/scripts/phpize && ./current-config && make -j7 install`
    * Use option `current-config -d` for a debug build

### Tools

1. Application profiles can be built with the [zen-ids-profile](https://github.com/uci-plrg/zen-ids-profile) Java tools. 
  * In the paper, these are referred to as *trusted profiles*.
 
### Experiments

See the [experiment documentation](https://github.com/uci-plrg/zen-ids/blob/interp-opt/EXPERIMENTS.md).
