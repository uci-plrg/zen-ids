# Instructions for Performance Benchmarking

This document begins with a high-level discussion about benchmark experiments using ZenIDS, and concludes with detailed instructions for conducting experiments similar to those reported in the ZenIDS paper.

## Discussion

The performance of ZenIDS depends on the deployment and usage scenario. For example, we observe up to 1% difference in overhead between WordPress and typical usage of GitList. But for an unexpected usage scenarios such as the continual downloading of tarballs from GitList, the overhead of ZenIDS is of course well below 1%. To get accurate and useful results from a benchmark experiment, it is important to determine which deployment scenarios are most relevant, and to design the experiment to simulate those conditions in an effective way.

## How to Conduct a Performance Benchmark Experiment

If the goal of the benchmark is to measure the overhead in a live deployment, a simple approach is to record some HTTP traffic from the deployment and use it as input for the benchmark. Since the number of alerts will affect the performance, it is important to use a *trusted profile* similar to the one that will be used in the live scenario. It is also important that the application respond correctly to the replayed HTTP traffic; see the discussion on this topic in the [FP/FN Experiment documentation](https://github.com/uci-plrg/zen-ids/blob/interp-opt/doc/experiments/ACCURACY.md). 

The steps to measure the performance are as follows:

1. Configure the Apache instance with only one worker (see `/etc/apache2/mods-available/php7.conf`), to minimize atifacts of process scheduling and the arbitrary distribution of requests among worker processes.
2. Build a native version of the ZenIDS PHP interpreter:
  * Follow the [build instructions](https://github.com/uci-plrg/zen-ids-php/blob/interp-opt/README.md) for the instrumented interpreter, but use `./current-config-plain` instead of `./current-config`.
2. Replay the HTTP requests that have been selected as input for the experiment.
  * Reset the application state to the experiment's start state.
  * Evaluate a sample of application output to verify that it is working correctly.
  * We suggest using *nix utility `time` to measure the running time (use the absolute path to avoid accidentally running the bash built-in `time` command, which is totally different).
3. Rebuild the PHP interpreter with the ZenIDS extension.
4. Repeat step 2

In our experience, the standard deviation can be unusually high (compared to native executables, for example), so we recommend taking the geometric mean of at least 10 runs. 
