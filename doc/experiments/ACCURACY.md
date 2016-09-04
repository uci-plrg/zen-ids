# Instructions for a False Positive/Negative Experiment

This document begins with a high-level discussion of various factors that affect the quality and relevance of the experimental results. Following this, we provide step-by-step instructions for conducting the kind of record/replay experiment that we reported in the paper.

## Discussion

#### Choosing a Representative Scenario

The accuracy of Zen IDS depends on several factors of the deployment and usage scenario. For example, Zen IDS will probably perform well for an application with a large user base and high traffic, regardless of all other factors--but accuracy will decrease as the user base and traffic volume are reduced. For this reason, there is no universal accuracy benchmark for Zen IDS. It can only be measured for a specific deployment scenario, which must be carefully selected to represent whatever real-world conditions are of interest.

For this reason, we recommend recording live HTTP traffic to a real installation having significant traffic volume. In our experiments we recorded traffic with a modified version of [gor](https://github.com/buger/gor) that can replay traffic synchronously, which (a) preserves request sequence, and (b) minimizes the overall replay duration (though this may not work for applications that are sensitive to specific request timing). If live traffic is not available, it is also possible to simulate traffic using any means of generating HTTP requests, though the relevance of the results will depend on how realistic the request generator is.

#### Repeatability

The record/replay approach makes the experiment repeatable if the application only consumes data from local configuration and user input. For applications that consume external data, for example from Twitter feeds, the external source may or may not affect the accuracy of Zen IDS. If the experiment is intended to be precisely repeatable, it will usually be possible to record external inputs along with the HTTP request inputs. This requires extending the PHP interpreter to record network communication initiated by the application (contact us for suggestions about how to do this).

#### Avoid Spamming

Many PHP applications generate communications with users, such as an automated email sent when a new user registers or forgets their password. The record/replay approach will continue sending these emails on every replay, which may irritate your users and possibly get your IP address blacklisted at various important Internet gateways. We recommend scrubbing the replay data to redirect these automatic communications somewhere within your network, so that you can confirm the application is working correctly, and avoid spamming people.

## How to Conduct a Record/Replay Experiment
