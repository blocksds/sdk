---
title: Wi-Fi
weight: 1
---

## 1. Introduction

The Nintendo DS and DSi support Wi-Fi communications.

- Nintendo DS:

  - Mostly compatible with IEEE 802.11b standards (the DS only supports transfer
    rates of 1 Mbps and 2 Mbps transfer rates, not 5.5 Mbps or 11 Mbps).
  - Local multiplayer without an Access Point with up to 16 players. This is
    sometimes called Ni-Fi.
  - Supports connecting to Access Points without encryption (open) or with WEP
    encryption.

- Nintendo DSi:

  - Compatible with the IEEE 802.11b/g standards.
  - Supports connecting to open APs, as well as APs using WEP, WPA or WPA2
    encryption.
  - It can run in DS-compatible mode (and lose access to the new features added
    in DSi).

{{< callout type="warning" >}}
Open networks and networks using WEP encryption are unsafe. Be careful when
creating an access point compatible with the Nintendo DS. You can limit the
number of devices that can connect to your AP simultaneously, enable MAC
filtering, or use a router that doesn't have access to the rest of your home
network and that you only use for the Nintendo DS.
{{< /callout >}}

Wi-Fi support in BlocksDS is provided through the library DSWiFi. You can check
its documentation [here](https://blocksds.skylyrac.net/dswifi/). This chapter of
the tutorial won't get into details about how to use the library, check the
tutorials in the library documentation instead. This chapter will focus on
understanding how to use them at a very high level to create an application, as
well as some other libraries you can use with BlocksDS (Mbed TLS and libcurl).

## 2. Differences between local and online communications

You may think that once you have an application that supports local multiplayer
it's pretty easy to add online multiplayer, but there are big differences
between both kinds of communications that will force you to write your code in
very different ways.

Local communications:

- The natural way to organize players is by using "rooms". Any console that acts
  as a multiplayer host becomes an Access Point, and any console that acts as
  client sees all APs available nearby. Hosts transmit a game ID defined by the
  developer, and clients can look for APs with that game ID. Clients can try to
  connect to any of the APs. As soon as the connection is complete, you can
  start sending raw data between the host and the client.

  All of this is handled on the ARM7, the ARM9 only has to tell the ARM7 some
  detais like the maximum number of players or whether new players are allowed
  or not (you will probably want to prevent new connections when the game has
  started!).

- Clients are identified by their association ID (AID). The host has AID 0,
  clients have AIDs 1 to 15.

- Latency is very low. Clients can ask the Wi-Fi driver to setup a packet to be
  sent as soon as the host sends a packet. When the host sends a packet, all
  clients respond with their own packets, which is a very fast process. This can
  happen multiple times per frame in theory.

  Hosts can also send packets to the clients without triggering automatic
  replies, and clients can send packets to the host without waiting to receive a
  packet first.

- Clients can only communicate with the host, they can't communicate directly
  between each other.

- Packets are received as a single unit. The game may have to split data into
  multiple packets manually, but the packets always arrive the same way they
  were sent.

Online communications:

- There is no automatic way to create "rooms" with DSWiFi. You need to learn how
  to create a server and client in your game. This is done using standard C
  functions, so there are plenty of tutorials and documentation of how to do
  this, but it's more work than local multiplayer in general.

- All consoles have their own IP. You will need to add code to support IPv4 and
  IPv6 if you want your code to be future-proof.

- Latency is high. It depends on your local network, as well as the Internet
  servers between you and the other consoles.

- All consoles can communicate with all other consoles in the game, not just
  with the host.

- Packets may arrive divided in multiple chunks. You need to buffer data as you
  receive it and handle it when you have all the data you need.

In practice, when you're creating the local multiplayer mode of a game, the host
console can assume that it will get information from all the other players
reliably, fast, and frequently. When creating the online multiplayer mode of a
game the host needs to be ready to deal with clients that have a slow
connection, maybe even a connection that drops packets frequently. This may not
matter for a game like Chess, but it's a big problem for a car racing game.

## 3. Local communications

There is a page about how to implement local communications in your games in the
documentation of [DSWiFi](https://blocksds.skylyrac.net/dswifi/). This tutorial
won't repeat the same information.

The basic idea of a multiplayer game is that a host follows some steps:

- Wait for clients to connect.
- Block new connections and send a signal to the clients to start the game.
- In the game loop, receive data from clients and send data to clients.
- Disconnect.

Clients follow other steps:

- Look for hosts of the same game (checking the Nintendo metadata in the access
  point struct, which contains a game ID) and list them on the screen.
- Let the player select a host and connect to it.
- Wait for the game to start.
- In the game loop, receive data from the host and send data back to it.
- Disconnect.

Note that hosts can kick clients out of the game at any point after they have
connected to the host!

This example shows you how to do all of this:
[`examples/dswifi/local_multiplayer`](https://github.com/blocksds/sdk/tree/master/examples/dswifi/local_multiplayer)

This other example shows how to send different types of packets between the host
and the clients:
[`examples/dswifi/multiplayer_packets`](https://github.com/blocksds/sdk/tree/master/examples/dswifi/multiplayer_packets)

## 4. Basic online communications

Online communications are more complicated. The first step is connecting to an
Access Point, like a router. The DS can store the settings of up to 3 networks
in the firmware, and the DSi can store the settings of 3 more networks. In
general, your game will simply try to connect to the networks stored in the
firmware, but you can also give the option to the player to connect to any AP
that the DS can see. Check the following example to see how to do that:

- [`examples/dswifi/list_access_points`](https://github.com/blocksds/sdk/tree/master/examples/dswifi/list_access_points)

You will need to allow the player to type a password if the AP is using WEP or
WPA2 encryption (WPA2 is only supported in DSi, not DS). Once the user has
provided the password (or not, if the network is open) you can ask DSWiFi to
connect to that AP, and DSWiFi will automatically try to connect to it and to
get an IP address. As soon as it gets an IPv4 address DSWiFi will consider the
connection a success, but it will also try to get an IPv6 address in the
background (IPv6 isn't supported everywhere, so it's better to not expect an
address just in case). Check the following two examples:

- [`examples/dswifi/connection_info`](https://github.com/blocksds/sdk/tree/master/examples/dswifi/connection_info)
- [`examples/dswifi/full_ap_demo`](https://github.com/blocksds/sdk/tree/master/examples/dswifi/full_ap_demo)

Once the connection is ready you can start trying to connect to other devices
and communicate with them. This example shows you how to connect to a web server
and download the contents of a website:

- [`examples/dswifi/get_website`](https://github.com/blocksds/sdk/tree/master/examples/dswifi/get_website)

The code used to communicate with other devices is just standard C code, so you
should be able to find books and tutorials of how to communicate between
different devices using C sockets.

## 5. Encrypted online communcations

The previous section has talked about how to communicate without encryption (the
only encryption happens between the DS and the AP, but not between the AP and
the remote device). If you want to encrypt the data you're sending you will need
to use other libraries.

BlocksDS has pre-packaged versions of [Mbed TLS](https://github.com/Mbed-TLS/mbedtls)
and [libcurl](https://curl.se/libcurl) that you can install with:

```sh
wf-pacman -Sy blocksds-mbedtls blocksds-libcurl
```

There are two examples of how to use the libraries to download a website using
TLS encryption:

- [`examples/dswifi/get_website_ssl`](https://github.com/blocksds/sdk/tree/master/examples/dswifi/get_website_ssl)
- [`examples/dswifi/get_website_libcurl`](https://github.com/blocksds/sdk/tree/master/examples/dswifi/get_website_libcurl)

There are plenty of tutorials and documentation on how to use the libraries
online.
