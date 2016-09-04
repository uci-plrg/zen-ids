# Instructions for a False Positive/Negative Experiment

This document begins with a high-level discussion of various factors that affect the quality and relevance of the experimental results. Following this, we provide step-by-step instructions for conducting the kind of record/replay experiment that we reported in the Zen IDS paper.

## Discussion

#### Choosing a Representative Scenario

The accuracy of Zen IDS depends on several factors of the deployment and usage scenario. For example, Zen IDS will probably perform well for an application with a large user base and high traffic, regardless of all other factors--but accuracy will decrease as the user base and traffic volume are reduced. For this reason, there is no universal accuracy benchmark for Zen IDS. It can only be measured for a specific deployment scenario, which must be carefully selected to represent whatever real-world conditions are of interest.

For this reason, we recommend recording live HTTP traffic to a real installation having significant traffic volume. In our experiments we recorded traffic with a modified version of [gor](https://github.com/buger/gor) that can replay traffic synchronously, which (a) preserves request sequence, and (b) minimizes the overall replay duration (though this may not work for applications that are sensitive to specific request timing). If live traffic is not available, it is also possible to simulate traffic using any means of generating HTTP requests, though the relevance of the results will depend on how realistic the request generator is.

#### Repeatability

The record/replay approach makes the experiment repeatable if the application only consumes data from local configuration and user input. For applications that consume external data, for example from Twitter feeds, the external source may or may not affect the accuracy of Zen IDS. If the experiment is intended to be precisely repeatable, it will usually be possible to record external inputs along with the HTTP request inputs. This requires extending the PHP interpreter to record network communication initiated by the application (contact us for suggestions about how to do this).

#### Avoid Spamming

Many PHP applications generate communications with users, such as an automated email sent when a new user registers or forgets their password. The record/replay approach will continue sending these emails on every replay, which may irritate your users and possibly get your IP address blacklisted at various important Internet gateways. We recommend scrubbing the replay data to redirect these automatic communications somewhere within your network, so that you can confirm the application is working correctly, and avoid spamming people.

## How to Conduct a Record/Replay Experiment

This section describes how to conduct the kind of record/replay experiment reported in the Zen IDS paper.

#### Record Live HTTP Traffic

Set up an HTTP recorder on all instances of Apache that comprise the live webserver. It is essential to include all nodes because otherwise the replay will be missing requests, which is likely to cause errors in the application and invalidate the results. For applications that do not maintain state on behalf of the user (such as a session), it may be possible to randomly sample traffic.

#### Building the *Trusted Profile*

The general procedure for building the *trusted profile* is to repeat the following sequence:

1. Execute some HTTP requests (e.g. from a crawler, or an HTTP replay).
  * Zen IDS generates a composite profile of the request executions.
  * Use utility `select-run -w 1` to find the newly generated profile.
    * The numeric argument indicates the index of the session, in reverse chronological order, where a session is delimited by Apache restart.
2. Merge the new profile together with previously generated profiles (if any).
  * We refer to the merged profile as a *dataset*.
  * Use utility `w-merge -o $dataset <profile1> <profile2> ...` 
    * It also works with just one input profile (i.e., unity merge).
    * The `$dataset` is a directory. It will be created if it does not exist (though its parent must exist).
3. Deploy the *trusted profile* to all Apache instances.
  * Copy the file `cfg.set` from `$dataset` to `$ZEN_IDS_DATASETS`, renaming it to the application name as specified in `opmon.site.roots` (see the [Deployment Instructions](https://github.com/uci-plrg/zen-ids/blob/interp-opt/README.md).
  * Note that Zen IDS generates only deltas with the deployed *trusted profile*, so the resulting new profile will only merge successfully with the `$dataset` from which the *trusted profile* was deployed.

The *trusted profile* for the test phase of the experiment can be any result of step 3, though normally the last *trusted profile* would be used since it will be the most complete.

#### Generating HTTP Requests

In our experiments, we generated the *trusted profile* using a combination of crawler requests and replay requests.

##### Crawling

For applications that are open to public crawlers, such as those sent by search engines, it is important to include crawl requests in the *trusted profile* to avoid false positives in the experiment. The behavior of real crawlers is erratic and cannot be reliably sampled from recorded HTTP traffic. For example, we observed the same crawler to start with a shallow investigation of our site, and then return several weeks later to make a thorough crawl of all links. We have no idea why this happens, but we know that the second crawl will generate a flood of false positives unless the *trusted profile* includes a thorough crawl of the site, including links the XML documents such as sitemaps. Our [crawl script for DokuWiki] gives an example of basic crawling that is sufficient for record/replay experiments.


#### Test for False Positives/Negatives

