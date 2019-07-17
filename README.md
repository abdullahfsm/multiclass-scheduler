# 2D
## Brief Introduction
Multi class scheduler (MSC) running on top of a Traffic Generator for network experiments.

The MSC is a general job scheduling framework and can be used by different job types (e.g., compute, network) and different scheduling policies (e.g., FIFO).

This guide focuses on providing scheduling support for a network Traffic Generator.

### Multi Class Scheduler (MCS)
Each class in the **MSC** consists a *k-FIFO scheduler* with its own *FIFO queue*.
It is completely characterized by three variables: 1) *Job size threshold*, 2) *Rate*, and *multiplexing level k*
Class variables are read from a configuration file.

Each job generated is mapped to one of the classes
based on the requested size and the size threshold of that class.

The scheduling policy for each class is k-FIFO where k is the maxmium multiplexing level. Different classes can have different multiplexing levels. The rate of each class defines its maxmium resource capacity and is enforced differently for different resources.

It exposes a simple interface (request_handler, response_handler) that can be used by different job generation applications. 

The **MCS** can support a variety of different scheduling policies e.g., FIFO, **([2D](https://dl.acm.org/citation.cfm?id=3281429))**, Processor Sharing (PS) for different job types as long as the interface is supported by the job generator.


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

The Workload, link rate, server IP+port and sequencer IP+port need to be set in the setup.py file.

After the setup.py file has been modified, run ```python setup.py```.
You can see the executables in the ./bin folder and the configuration file in the ./conf folder. 


## Single Client-Single Server experiment

The run_one_to_one.py scipts starts a client server traffic generator.
The first argument is the total number of flows and second argument is the offered load (in percentage).
The default is 100000 and 80.0 respectivelty

For instance, to start a client that will generate 5000 flows at 50.0% load, run the following:

```
python run_one_to_one.py -n 5000 -p 50.0
```

## Client Configuration File
The client configuration file specifies the list of servers, the request size distribution, the Differentiated Services Code Point (DSCP) value distribution, the sending rate distribution and the request fanout distribution (only for **incast-client**). We provide several client configuration files as examples in ./conf directory.  

The format is a sequence of key and value(s), one key per line. The permitted keys are:

* **server:** IP address and TCP port of a server.
```
server 192.168.1.51 5001
```

* **req_size_dist:** request size distribution file path and name.
```
req_size_dist conf/DCTCP_CDF.txt
```
There must be one request size distribution file, present at the given path, 
which specifies the CDF of the request size distribution. See "DCTCP_CDF.txt" in ./conf directory 
for an example with proper formatting.

* **dscp:** DSCP value and weight. The DSCP value and weight are both integers. Note that DSCP value should be smaller than 64.
```
dscp 0 25
dscp 1 25
dscp 2 25
dscp 3 25
```
For each request, the client chooses a DSCP value with a probability proportional to the weight. The traffic of the request and the corresponding response(s) will be marked with this DSCP value. If the user does not specify the DSCP distribution, the DSCP value is always 0 for all requests. **This feature can be used to create multiple classes of traffic (e.g., section 5.1.2 of [MQ-ECN](https://www.usenix.org/system/files/conference/nsdi16/nsdi16-paper-bai.pdf) paper).**  

* **rate:** sending rate and weight. The sending rate (in Mbps) and weight are both integers.
```
rate 0Mbps 10
rate 500Mbps 30
rate 800Mbps 60
```
For each request, the client chooses a rate with a probability proportional to the weight. To enforce the sending rate, the sender will add some delay at the application layer. *Note that 0Mbps indicates no rate limiting.* If the user specifies very low sending rates, the client may achieve a much lower average RX throughput in practice, which is undesirable. If the user does not specify the sending rate distribution, the sender will not rate-limit the traffic. **We suggest the user simply disabling this feature except for some special scenarios.**   

* **fanout:** fanout value and weight. Note that only **incast-client** need this key. The fanout and weight are both 
integers.
```
fanout 1 50
fanout 2 30
fanout 8 20
```
For each request, the client chooses a fanout with a probability proportional to the weight. For example, with the above configuration, half the requests have fanout 1, and 20% have fanout 8. If the user does not specify the fanout distribution, the fanout size is always 1 for all requests.

##Output
A successful run of **client** creates a file with flow completion time results. A successful run of **incast-client** creates two files with flow completion time results and request completion time results, respectively. You can directly use ./bin/result.py to parse these files. 

In files with flow completion times, each line gives flow size (in bytes), flow completion time (in microseconds), DSCP value, desired sending rate (in Mbps) and actual per-flow goodput (in Mbps). 

In files with request completion times, each line gives request size (in bytes), request completion time (in microseconds), DSCP value, desired sending rate (in Mbps), actual per-request goodput (in Mbps) and request fanout size.

##Contact
For questions, please contact Wei Bai (http://sing.cse.ust.hk/~wei/).

##Publications
- Paper on Traffic Generator: please use the citation below as the reference to Traffic Generator
```
Enabling ECN in Multi-Service Multi-Queue Data Centers
Wei Bai, Li Chen, Kai Chen, Haitao Wu
USENIX NSDI 2016

@inproceedings {194968,
    author = {Wei Bai and Li Chen and Kai Chen and Haitao Wu},
    title = {Enabling ECN in Multi-Service Multi-Queue Data Centers},
    booktitle = {13th USENIX Symposium on Networked Systems Design and Implementation (NSDI 16)},
    year = {2016},
    month = Mar,
    isbn = {978-1-931971-29-4},
    address = {Santa Clara, CA},
    pages = {537--549},
    url = {https://www.usenix.org/conference/nsdi16/technical-sessions/presentation/bai},
    publisher = {USENIX Association},
}
```

- Papers that use Traffic Generator:
```
ClickNP: Highly Flexible and High-performance Network Processing with Reconfigurable Hardware
Bojie Li, Kun Tan, Larry Luo, Renqian Luo, Yanqing Peng, Ningyi Xu, Yongqiang Xiong, Peng Cheng, Enhong Chen
ACM SIGCOMM 2016

Fast and Cautious: Leveraging Multi-path Diversity for Transport Loss Recovery in Data Centers
Guo Chen, Yuanwei Lu, Yuan Meng, Bojie Li, Kun Tan, Dan Pei, Peng Cheng, Larry Luo, Yongqiang Xiong, Xiaoliang Wang, Youjian Zhao
USENIX ATC 2016
```






