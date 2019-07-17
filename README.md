# 2D
## Brief Introduction
Multi class scheduler (MCS) running on top of a Traffic Generator for network experiments.

The MCS is a general job scheduling framework and can be used by different job types (e.g., compute, network) and different scheduling policies (e.g., FIFO).

This guide focuses on providing scheduling support for a network Traffic Generator.

### Multi Class Scheduler (MCS)
Each class in the **MCS** consists a *k-FIFO scheduler* with its own *FIFO queue*.
It is completely characterized by three variables: 1) *Job size threshold*, 2) *Rate*, and *multiplexing level k*.
Class variables are read from a configuration file.

Each job generated is mapped to one of the classes
based on the job size and the size threshold of that class.

The scheduling policy for each class is k-FIFO where k is the maxmium multiplexing level. Different classes can have different multiplexing levels. The rate of each class defines its maxmium resource capacity and is enforced differently for different resources.

It exposes a simple interface (request_handler, response_handler) that can be used by different job generation applications. 

The **MCS** can support a variety of different scheduling policies e.g., FIFO, **[2D](https://dl.acm.org/citation.cfm?id=3281429)**, Processor Sharing (PS) for different job types as long as the interface is supported by the job generator.


### Support for Traffic Generator
For details of the Traffic Generator see **https://github.com/HKUST-SING/TrafficGenerator**

The Traffic Generator implements a **server-client** traffic model.
The **server** listens for incoming requests, and replies with a *TCP flow* of the requested size.
The **client** generates requests over TCP using a client configuration file.

The **client** instantiates the **MCS** and uses the interface to interact with it. The *request_handler* is used whenever a flow is generated while the *response_handler* is invoked upon completion of a request.

The **MCS** optionally interacts with a **sequencer** to ensure global ordering across differnet client machines. To get a sequence number it uses persistant TCP connections and listens for FIN broadcasts from other clients to know of a requests turn. 

Class rates are enforced at the **server** using Linux HTB.
**MCS** annotates each scheduled request with a ToS bit, unique to each class, which is used by the HTB at the server link.

## Build
The setup.py script in the main directory is responsible for building executables and setting up the configuration file to be used by the **client** and **MCS**. Additionally, it sets up class rates at the **server**.

The workload, link rate, server IP+port and sequencer IP+port need to be set in the setup.py file.
To run 2D's policy, corrosponding to the selected workload, the right thresholds and rates needs to be specified in the setup.py file. For a list of threshold and rates for different workload, see ./setup/class_description.tr

After the setup.py file has been modified, run ```python setup.py```.
You can see the executables in the ./bin folder and the configuration file in the ./conf folder. 


## Single Client-Single Server experiment

The run_one_to_one.py scipts starts a client server traffic generator.
The first argument is the total number of flows and second argument is the offered load (in percentage).
The default is 100000 and 80.0 respectivelty

For instance, to start a client generating 5000 flows at 50.0% load, run the following:

```
python run_one_to_one.py -n 5000 -p 50.0
```

## Configuration File
The client configuration file (see conf/client_config.txt) specifies information for the **client** (e.g., servers, workload).
It is also specifies list of sequencers, class thresholds, tos mapping and multiplexing level to be used by the **MCS**.

The format is a sequence of key and value(s), one key per line.

* **sequencer:** IP address and TCP port of sequencer.
```
sequencer 192.168.1.51 5001
```

* **threshold:** class threshold along with its unique ToS and Multiplexing level.
```
threshold 3400 tos 4 mpl 1
```

* **use_seq:** To use (1) or not use (0) the sequencer.
```
use_seq 0
```
There must be one threshold entry and they need to be specified in sorted order (w.r.t. threshold).

For example this is permissible:
threshold 3400 tos 4 mpl 1
threshold 16176 tos 32 mpl 1

While this will lead to faulty behaviour:
threshold 16176 tos 32 mpl 1
threshold 3400 tos 4 mpl 1

##Output
A successful run of **client** creates a file with flow completion time results.

In files with flow completion times, each line gives flow size (in bytes), flow completion time (in microseconds), DSCP value, desired sending rate (in Mbps) and actual per-flow goodput (in Mbps). 

##Contact
For questions, please contact Abdullah Bin Faisal (https://www.eecs.tufts.edu/~abdullah/).


