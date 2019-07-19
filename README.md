# 2D
## Brief Introduction
Multi class scheduler (MCS) running on top of a Traffic Generator for network experiments.

The MCS is a general job scheduling framework which can be used by different resources (e.g., compute, network) to realise different scheduling policies (e.g., FIFO).

This guide focuses on providing scheduling support for a network Traffic Generator.

### Multi Class Scheduler (MCS)
Each class in the **MCS** consists of a *k-FIFO scheduler* with its own *FIFO queue*.
It is completely characterized by three variables: 1) *Job size threshold*, 2) *Rate*, and *multiplexing level k*.
Class variables are read from a configuration file.

Each job generated is mapped to one of the classes
based on the job size and the size threshold of that class.
The scheduling policy for each class is k-FIFO where k is the maxmium multiplexing level (mpl).
Different classes can have different mpls.
The rate of each class defines its maxmium resource capacity and is enforced differently for different resources (e.g., rate-limiters for network flows).

It exposes a simple interface (request_handler, response_handler, processing_function) that can be used by different job generation applications.

The request_handler is invoked whenever the job spawns while the response_handler is called whenever the job is completed.
The processing_function specifies what **MCS** should do when a job is ready to be scheduled (e.g., send it to a server).

The **MCS** can support a variety of different scheduling policies e.g., FIFO, **[2D](https://dl.acm.org/citation.cfm?id=3281429)**, Processor Sharing (PS) for different job types as long as the interface is supported by the job generator.


### Support for Traffic Generator
For details of the Traffic Generator see **https://github.com/HKUST-SING/TrafficGenerator**

The Traffic Generator implements a **server-client** traffic model.
The **client** generates requests over TCP, destined to the server.
The **server** listens for incoming requests, and replies with a *TCP flow* of the requested size.

The **client** instantiates the **MCS** and uses the interface to interact with it. The *request_handler* is used whenever a flow is generated while the *response_handler* is invoked upon completion of a request.
The run_request method is set as the processing_function of the **MCS** during initialization.

The **MCS** has a light-weight sequence-manager which can optionally interact with a global **sequencer** to ensure ordering of flows across different client machines.
The sequence-manager obtains sequence numbers for requests from the sequencer over TCP and listens for FIN broadcasts over UDP from other clients to know of a requests turn. 
When the requests turn comes up, it is sent to the server using the processing_function.

Class rates are enforced at the **server** using Linux HTB.
**MCS** annotates each scheduled request with a ToS bit, unique to each class, which is used by the HTB at the server link.

## Build
The setup.py script in the main directory is responsible for building executables and setting up the configuration file to be used by the **client** and **MCS**. Additionally, it sets up class rates at the **server**.
This script takes as input a configuration file named setup_config.tr.
The following describes how the setup_config.tr needs to be modified before the script can run:

### Configuration File

This script specifies information for the **client** (e.g., servers, workload).
It also specifies sequencers, class thresholds, tos mapping and mpl to be used by the **MCS**. 

The format is a sequence of key and value, one key per line.

* **workload:** see ./conf for available workloads
```
workload VL2_CDF.txt
```
* **link_rate:** specified in Mbps
```
link_rate 10000
```
* **serverIP:** IP address of server. Note that it is assumed the server is reachable from the machine calling this script
```
serverIP 10.1.1.7
```
* **serverPort:** port on which server will listen
```
serverPort 5001
```
* **serverInterface:** device name corrosponding to the serverIP. This is used for setting up rates
```
serverInterface enp6s0f2
```
* **sequencerIP:** IP address of sequencer. Note that it is assumed the sequencer is reachable from the machine calling this script
```
sequencerIP 10.1.1.7
```
* **sequencerPort:** port on which sequencer will listen
```
sequencerPort 6001
```
* **thresholds:** comma seperated list of class thresholds. For a list of thresholds for different workloads for 2D's policy, see ./setup/class_description.tr
```
thresholds 3400,16176,545316,5159030,129372452,129372452
```
* **ratios:** comma seperated list of class ratios. For a list of ratios for different workloads for 2D's policy, see ./setup/class_description.tr
```
ratios 0.71,0.097,0.1,0.045,0.028,0.020
```

After the setup_config.tr file has been correctly modified, run ```python setup.py```.
You can see the executables in the ./bin folder and the client configuration file in the ./conf folder.

## Single Client-Single Server experiment

The run_one_to_one.py script starts a client-server traffic generator.
The first argument is the total number of flows and second argument is the offered load (in percentage).
The default is 100000 and 80.0 respectivelty

For instance, to start a client generating 5000 flows at 50.0% load, run the following:

```
python run_one_to_one.py -n 5000 -p 50.0
```
## Different Scheduling Policies
We describe how different policies can be realised by changing the client configuration file (see ./conf/client_text).
The important fields are the *class* fields.
The format of the class field is: classID threshold ToS mpl use_seq(1/0)

**FIFO**. Use a single class, with any threshold, and 1 mpl and enable sequencer. No rate limit is needed at server: 
```
class 0 1000 4 1 1
```

**PS**. Use a single class, with any threshold, and a large mpl (e.g., 10000). Disable sequencer. No rate limit is needed at server:
```
class 0 1000 4 10000 0
```

**[2D](https://dl.acm.org/citation.cfm?id=3281429)**. See ./setup/class_description.tr for thresholds for different workloads.
For example for VL2 workload:
```
class 0 3400 4 1 1
class 1 16176 32 1 1
class 2 545316 40 1 1
class 3 5159030 56 1 1
class 4 129372452 72 1 1
class 5 129372452 128 1 1
```

**[FIFO-LM](https://dl.acm.org/citation.cfm?id=2626322)**. Use two classes (short and long). First class's threshold is p90 of the workload, mpl is 1.
Second class has large mpl (e.g., 10000). Disable sequencer for second class.
For example, for running VL2 workload, the config file should contain:
```
class 0 400000 4 1 1
class 1 129372452 32 10000 0
```

##Output
A successful run of **client** creates a file with flow completion time results.

In files with flow completion times, each line gives flow size (in bytes), flow completion time (in microseconds), DSCP value, desired sending rate (in Mbps) and actual per-flow goodput (in Mbps). 

##Contact
For questions, please contact Abdullah Bin Faisal (https://www.eecs.tufts.edu/~abdullah/).