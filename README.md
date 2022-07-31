# sensor-client

<!-- highlevel: https://ibb.co/ZMX8cxZ -->
<!-- sequence: https://ibb.co/hWDTRtx -->
## Record
- Software: Sensor-Client
- Purpose: provide a system for sensing forest temperature data and processing it.
- Document authors:
	- Ibrahim Mohammed: Junior Developer
- Date: 31/7/2022
## Glossary
- Sensor: A C++ process that observes temperature readings and publishes it to all subscribed clients.
- Client: A C++ process that is interested in the data published from Sensor.
- Subscriber: A subscribed client is a client that establishes connection with a Sensor to listend its published data. 
## Overview
       Sensor-client is a solution made to monitor temperature in forests, this solution is one of the contributions to deal with global warming. The system consists of 2 modules; **Sensor** and **Client**. Sensor module is responsible for sensing temperature in the forest and publishes readings to subscribed clients. Client is responsible for processing the data, aggregating it, and reporting the output. 

## Viewpoints
### High level Component Diagram

<a href="https://imgbb.com/"><img src="https://i.ibb.co/k5cZpK3/sensor-client-2-drawio.png" alt="sensor-client-2-drawio" border="0"></a>

 1. Sensor acts as server that listens for incoming connections (by default: on port 12345) 
 2. Clients can connect on that port to establish connections on which sensor readings will be sent on.
### Time Sequence diagram
<a href="https://imgbb.com/"><img src="https://i.ibb.co/XsSGZvf/Sensor-client-drawio-1.png" alt="Sensor-client-drawio-1" border="0"></a>

 1. Sensor has 2 working threads; one for data production (i.e., producer thread), and one for listening for incoming connections from new clients (i.e., listener thread)
 2. listener thread will accept connections from clients on a specific port and add them to subscrier list.
 3. prodcuer thread has 2 responsibilities:
    1. remove a client from subscriber list if it is inactive
    2. send readings data to all active subscribers
 4. Client can initiate a connection with Sensor, and listen for incoming readings for further processing.


## Implementation choices
   1. Sensor and clients code is written in **C++**
      - pros: 
         - C++ is very fast, which will be very uesful for clients because they have processing responsibilities. Sensors have more IO responsibilities, So they may also be implemented in other languages(Nodejs for example). But then, the used networking libraries shall be investigiated for compatibility.
      - cons
        - OS compatibility issues: Since C++ is compiled, the compiler makes binaries for a specific OS, putting restrictions on the environment to deploy the system on. a few related problems emerged during containerization too.
   2. Direct publish-subscribe communication model
      - pros:
        - It allows multiple clients to connect to sensor (more than 60k)
      - cons:
        - clients dependency on sensors: If sensors are down, clients will be down.
   3. TCP communication protocol
      - pros:
        - Guarantees delivery of data, no packet loss.
        - Guarantees ordering of data.
 
 ## Alternative
 1. Using Kafka for communication
    - pros: 
      - Loose coupling between sensor and clients
      - Provides data persistence, so clients won't miss out data, and they will all have same readings at all times.
      - Gurantees delivery and ordering of data.
      - Opens opportunities for different kinds of easy integrations (see [Kafka connectors](https://docs.confluent.io/home/connect/self-managed/kafka_connectors.html))
      - Opens opportunity for rich out-of-th-box processing components (see [ksqldb](https://ksqldb.io/), [kafka streams](https://kafka.apache.org/documentation/streams/))
    - cons:
      - more services to be manages and maintained
      - needs experience for managing lifecycle of system and data