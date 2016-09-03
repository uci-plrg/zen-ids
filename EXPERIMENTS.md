
# Security Experiments with Zen IDS 

This document gives an overview of the workflow for the two kinds of security experiments reported in the Zen IDS paper:
false positive/negative and exploit detection. It also gives an overview of performance benchmarking.
Due to privacy concerns, we are unable to provide the HTTP data used in our published experiments.

### Overview: False Positive/Negative Experiment

1. Install the PHP application in the Apache instance where Zen IDS is installed.
  * Manually verify that the application works normally with Zen IDS running.
2. If the application has an authentication scheme:
  * Instrument the login mechanism with the Zen IDS `set_user_level($level)` function.
  * Verify that the Apache error log contains entries recognizing user login and logout, including cookie authentication.
3. ...  

For complete instructions, see the 
[False Positive/Negative Experiment](https://github.com/uci-plrg/zen-ids/blob/interp-opt/doc/experiments/ACCURACY.md) page.

### Overview: Exploit Experiment

For complete instructions, see the 
[Exploit Experiment](https://github.com/uci-plrg/zen-ids/blob/interp-opt/doc/experiments/EXPLOIT.md) page.

### Overview: Performance Benchmark

For complete instructions, see the 
[Performance Benchmark](https://github.com/uci-plrg/zen-ids/blob/interp-opt/doc/experiments/PERFORMANCE.md) page.
