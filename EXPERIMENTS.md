
# Security Experiments with ZenIDS 

This document gives an overview of the workflow for the two kinds of security experiments reported in the ZenIDS paper:
false positive/negative and exploit detection. It also gives an overview of performance benchmarking.
Due to privacy concerns, we are unable to provide the HTTP data used in our published experiments.

### Overview: False Positive/Negative Experiment

1. Install the PHP application in a live Apache instance and record some HTTP traffic.
  * We recommend HTTP recorder [gor](https://github.com/buger/gor)
2. Install the same PHP application in the Apache instance where ZenIDS is installed.
  * Manually verify that the application works normally with ZenIDS running.
2. If the application has an authentication scheme:
  * Instrument the login mechanism with the ZenIDS `set_user_level($level)` function.
  * Verify that the Apache error log contains entries recognizing user login and logout, including cookie authentication.
4. Crawl the application with `wget`
4. Create and deploy the initial *trusted profile* 
5. Replay a prefix of the HTTP traffic recorded in step 1
6. Update and deploy the *trusted profile*
7. Replay the remainder of HTTP traffic
  * Evaluate the accuracy by manually counting false positives and negatives among the reported alerts

For complete instructions, see the 
[False Positive/Negative Experiment](https://github.com/uci-plrg/zen-ids/blob/interp-opt/doc/experiments/ACCURACY.md) page.

### Overview: Exploit Experiment

1. Pick your favorite exploit
  * We recommend [exploit-db.com](http://exploit-db.com)
  * ZenIDS can only detect exploits that manipulate control flow at the script level. This excludes:
    * Data-only attacks such as conventional XSS and SQLi
    * Attacks on the native code of the interpreter
2. Install the vulnerable PHP application in the Apache instance where ZenIDS is installed.
3. Verify the exploit by running the POC.
4. Create a *trusted profile* for the application
  * Crawl it with `wget`
  * Access the application in a browser, using a variety of features in ways that appear normal
  * Create and deploy the resulting profile
5. Test the vulnerability scenario:
  * Continue to use and crawl the application, counting the number of false alerts (if any)
  * Run the POC and verify the expected alert

For complete instructions, see the 
[Exploit Experiment](https://github.com/uci-plrg/zen-ids/blob/interp-opt/doc/experiments/EXPLOIT.md) page.

### Overview: Performance Benchmark

1. Choose an FP/FN experiment as the basis for the benchmark. 
2. Build the ZenIDS fork of PHP without the ZenIDS extension or any of its hooks
2. Replay some of the basis HTTP traffic
  * Select a segment of traffic that takes at least 2 minutes to replay--otherwise the standard deviation will outweigh the ZenIDS overhead
3. Rebuild PHP with the ZenIDS extension
4. Deploy the *trusted profile* from the basis experiment 
5. Replay the same segment of HTTP traffic (step 3) and compare the running time

For complete instructions, see the 
[Performance Benchmark](https://github.com/uci-plrg/zen-ids/blob/interp-opt/doc/experiments/PERFORMANCE.md) page.
