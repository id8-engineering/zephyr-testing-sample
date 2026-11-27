# Connecting native_sim and QEMU to a Local MQTT Broker

How to run Zephyr's
[MQTT publisher sample](https://docs.zephyrproject.org/latest/samples/net/mqtt_publisher/README.html)
on `native_sim` and `qemu_cortex_m3` against an MQTT broker on the development
computer. Tested on Ubuntu Linux.

## Setup

| Target           | Network setup                | Broker address seen by Zephyr |
|------------------|------------------------------|-------------------------------|
| `native_sim`     | Offloaded sockets (NSOS)     | `127.0.0.1`                   |
| `qemu_cortex_m3` | QEMU user networking (SLIRP) | `10.0.2.2`                    |

Neither setup needs root privileges or network configuration on the host.

## Prerequisites

Set up the workspace as described in the [README](../README.md).

Install the Mosquitto broker and command-line clients:

```shell
sudo apt install mosquitto mosquitto-clients
```

## 1. Start the broker

```shell
sudo systemctl start mosquitto
systemctl is-active mosquitto
```

Expected output:

```
active
```

The broker listens on `127.0.0.1:1883`.

## 2. Verify the broker

Terminal 1:

```shell
mosquitto_sub -h 127.0.0.1 -t 'zts/#' -v
```

Terminal 2:

```shell
mosquitto_pub -h 127.0.0.1 -t zts/host-test -m "hello from host"
```

Expected output in terminal 1:

```
zts/host-test hello from host
```

## 3. Subscribe to the sample topic

The sample publishes to the topic `sensors`. Keep this running in terminal 1
during steps 4 and 5:

```shell
mosquitto_sub -h 127.0.0.1 -t sensors -v
```

## 4. Publish from native_sim

Run from `~/zts-workspace` in terminal 2.

Build:

```shell
west build -p always -b native_sim -d build-mqtt-native zephyr/samples/net/mqtt_publisher -- \
  -DCONFIG_ETH_NATIVE_TAP=n \
  -DCONFIG_NET_DRIVERS=y \
  -DCONFIG_NET_SOCKETS_OFFLOAD=y \
  -DCONFIG_NET_NATIVE_OFFLOADED_SOCKETS=y \
  -DCONFIG_NET_CONFIG_PEER_IPV4_ADDR=\"127.0.0.1\"
```

Run:

```shell
west build -d build-mqtt-native -t run
```

Expected output:

```
[00:00:00.000,000] <inf> net_mqtt_publisher_sample: attempting to connect:
[00:00:00.000,002] <inf> net_mqtt: Connect completed
[00:00:00.000,003] <inf> net_mqtt_publisher_sample: MQTT client connected!
[00:00:00.000,003] <inf> net_mqtt_publisher_sample: try_to_connect: 0 <OK>
[00:00:00.510,001] <inf> net_mqtt_publisher_sample: mqtt_publish: 0 <OK>
[00:00:01.020,001] <inf> net_mqtt_publisher_sample: mqtt_publish: 0 <OK>
[00:00:01.020,002] <inf> net_mqtt_publisher_sample: PUBACK packet id: 46796
```

Expected output in terminal 1:

```
sensors DOORS:OPEN_QoS0
sensors DOORS:OPEN_QoS1
sensors DOORS:OPEN_QoS2
```

Press `Ctrl+C` to stop.

## 5. Publish from QEMU

Run from `~/zts-workspace` in terminal 2.

Build:

```shell
west build -p always -b qemu_cortex_m3 -d build-mqtt-qemu zephyr/samples/net/mqtt_publisher -- \
  -DCONFIG_NET_QEMU_USER=y \
  -DCONFIG_NET_CONFIG_MY_IPV4_ADDR=\"10.0.2.15\" \
  -DCONFIG_NET_CONFIG_MY_IPV4_GW=\"10.0.2.2\" \
  -DCONFIG_NET_CONFIG_PEER_IPV4_ADDR=\"10.0.2.2\"
```

Run:

```shell
west build -d build-mqtt-qemu -t run
```

Expected output:

```
[00:00:00.000,000] <inf> net_config: IPv4 address: 10.0.2.15
[00:00:00.000,000] <inf> net_mqtt_publisher_sample: attempting to connect:
[00:00:00.010,000] <inf> net_mqtt: Connect completed
[00:00:00.010,000] <inf> net_mqtt_publisher_sample: MQTT client connected!
[00:00:00.020,000] <inf> net_mqtt_publisher_sample: try_to_connect: 0 <OK>
[00:00:00.530,000] <inf> net_mqtt_publisher_sample: mqtt_publish: 0 <OK>
[00:00:01.040,000] <inf> net_mqtt_publisher_sample: mqtt_publish: 0 <OK>
[00:00:01.040,000] <inf> net_mqtt_publisher_sample: PUBACK packet id: 62521
```

Expected output in terminal 1:

```
sensors DOORS:OPEN_QoS0
sensors DOORS:OPEN_QoS1
sensors DOORS:OPEN_QoS2
```

Press `Ctrl+A`, release, then press `X` to exit QEMU.
