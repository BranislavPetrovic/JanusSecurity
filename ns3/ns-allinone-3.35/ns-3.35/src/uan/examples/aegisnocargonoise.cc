/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Hossam Khader <hossamkhader@gmail.com>
 */

#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/node-container.h"
#include "ns3/mobility-helper.h"
#include "ns3/mobility-model.h"
#include "ns3/basic-energy-source-helper.h"
#include "ns3/energy-source-container.h"
#include "ns3/uan-helper.h"
#include "ns3/uan-channel.h"
#include "ns3/acoustic-modem-energy-model-helper.h"
#include "ns3/packet-socket-helper.h"
#include "ns3/packet-socket-address.h"
#include "ns3/node.h"
#include "ns3/uan-prop-model-thorp.h"
#include "ns3/uan-prop-model-ideal.h"

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <bitset>

using namespace std;
using namespace ns3;

/**
 *
 * This example shows the usage of raw packets transfer data.
 * Two nodes are sending their remaining energy percentage (1 byte)
 * to a gateway node, that prints the received data.
 * The transmissions are scheduled at random times to avoid collisions
 *
 */

NS_LOG_COMPONENT_DEFINE("UanRawExample");

class UanExperiment {
public:
    UanExperiment();

    /**
     * Set the UAN nodes position
     */
    void SetupPositions();

    /**
     * Set the UAN nodes energy
     */
    void SetupEnergy();

    /**
     * Set the UAN nodes communication channels
     */
    void SetupCommunications();

    /**
     * Set the UAN nodes communication channels
     */
    void SetupApplications();
    
    void SendPacket1To1();
    
    void SendPacket2To1();
    
    void SendPacket3To1();
    
    void SendPacket4To1();
    
    void SendPacket5To1();
    
    void SendPacket1To0();
    
    void SendPacket2To0();
    
    void SendPacket3To0();
    
    void SendPacket4To0();
    
    void SendPacket5To0();

    /**
     * Send a packet from one of the nodes
     * \param node The sending node
     * \param pkt The packet
     * \param dst the destination
     */
    void SendSinglePacket(Ptr<Node> node, Ptr<Packet> pkt, Mac8Address dst);

    /**
     * Print the received packet
     * \param socket The receiving socket
     */
    void PrintReceivedPacket(Ptr<Socket> socket);

    /**
     * Prepare the experiment
     */
    void Prepare();

    /**
     * Teardown the experiment
     */
    void Teardown();

public:
    NodeContainer m_nodes; //!< UAN nodes
    std::map<Ptr<Node>, Ptr<Socket> > m_sockets; //!< send and receive sockets
    double timestamp;
    double oneWay;
    double rtt;
    ulong protI = 0;
    int pktNum;
    Ptr<UniformRandomVariable> uniformRandomVariable = CreateObject<UniformRandomVariable> ();
    int numFails;
    double totRTT = 0;
    float noiseBench = .1;
};

UanExperiment::UanExperiment() {
}

void
UanExperiment::SetupPositions() {
    MobilityHelper mobilityHelper;
    mobilityHelper.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobilityHelper.Install(m_nodes);
    m_nodes.Get(0)->GetObject<MobilityModel> ()->SetPosition(Vector(0, 0, 0));
    m_nodes.Get(1)->GetObject<MobilityModel> ()->SetPosition(Vector(10000, 0, 0));
}

void
UanExperiment::SetupCommunications() {
    Ptr<UanChannel> channel = CreateObject<UanChannel> ();
    //Ptr<UanPropModel> prop = CreateObject<UanPropModelIdeal>();
    Ptr<UanPropModel> prop = CreateObject<UanPropModelThorp>();
    channel->SetPropagationModel(prop);
    UanHelper uanHelper;
    NetDeviceContainer netDeviceContainer = uanHelper.Install(m_nodes, channel);
}

void
UanExperiment::PrintReceivedPacket(Ptr<Socket> socket) {
    Address srcAddress;
    //double distance = m_nodes.Get (1)->GetObject<MobilityModel> ()->GetPosition ().x;
    double avgRTT;
    while (socket->GetRxAvailable() > 0) {
        Ptr<Packet> packet1 = socket->RecvFrom(srcAddress);
        PacketSocketAddress packetSocketAddress5 = PacketSocketAddress::ConvertFrom(srcAddress);
        srcAddress = packetSocketAddress5.GetPhysicalAddress();
        
        //uint32_t k;
        int l;
        double a, b, c, d;
        string strpkt1, strpkt2, strpkt3, strpkt4, strpkt5;
        ifstream in1("/home/ttm4128/aegisnocargopkt1.txt", ios_base::in);
        ifstream in2("/home/ttm4128/aegisnocargopkt2.txt", ios_base::in);
        ifstream in3("/home/ttm4128/aegisnocargopkt3.txt", ios_base::in);
        ifstream in4("/home/ttm4128/aegisnocargopkt4.txt", ios_base::in);
        ifstream in5("/home/ttm4128/aegisnocargopkt5.txt", ios_base::in);
        
        //NS_LOG_UNCOND(pktNum);
        if (pktNum == 1) {
            in1 >> strpkt1;
            //NS_LOG_UNCOND(strpkt1);
            l = strpkt1.length();
            a = (double) Simulator::Now().GetMilliSeconds();
            c = ((double) l / 80.0) * 1000; // janus RX
            d = ((double) l / 80.0) * 1000 + 0.0368927; // janus TX + generation-encryption

            timestamp = a + c + d;

            Simulator::Schedule(Seconds(0), &UanExperiment::SendPacket2To1, this);
        } else if (pktNum == 2) {
            in2 >> strpkt1;
            //NS_LOG_UNCOND(strpkt1);
            l = strpkt1.length();
            a = (double) Simulator::Now().GetMilliSeconds();
            c = ((double) l / 80.0) * 1000; // janus RX
            d = ((double) l / 80.0) * 1000; // janus TX + generation-encryption

            timestamp = a + c + d;

            Simulator::Schedule(Seconds(0), &UanExperiment::SendPacket3To1, this);
        } else if (pktNum == 3) {
            in3 >> strpkt1;
            l = strpkt1.length();
            a = (double) Simulator::Now().GetMilliSeconds();
            c = 5 * ((double) l / 80.0) * 1000; // janus RX
            d = 5 * ((double) l / 80.0) * 1000; // janus TX + generation-encryption

            timestamp = a + c + d;

            Simulator::Schedule(Seconds(0), &UanExperiment::SendPacket4To1, this);
        } else if (pktNum == 4) {
            in4 >> strpkt1;
            l = strpkt1.length();
            a = (double) Simulator::Now().GetMilliSeconds();
            c = 5 * ((double) l / 80.0) * 1000; // janus RX
            d = 5 * ((double) l / 80.0) * 1000; // janus TX + generation-encryption

            timestamp = a + c + d;
            oneWay = timestamp - oneWay;

            Simulator::Schedule(Seconds(0), &UanExperiment::SendPacket5To1, this);
        } else if (pktNum == 5) {
            in1 >> strpkt1;
            l = strpkt1.length();
            a = (double) Simulator::Now().GetMilliSeconds();
            b = 0.0373279; // decryption-verification
            c = 5 * ((double) l / 80.0) * 1000; // janus RX
            d = 5 * ((double) l / 80.0) * 1000 + 0.0368927; // janus TX + generation-encryption

            timestamp = a + b + c + d;

            Simulator::Schedule(Seconds(0), &UanExperiment::SendPacket1To0, this);
        } else if (pktNum == 6) {
            in2 >> strpkt1;
            l = strpkt1.length();
            a = (double) Simulator::Now().GetMilliSeconds();
            c = 5 * ((double) l / 80.0) * 1000; // janus RX
            d = 5 * ((double) l / 80.0) * 1000; // janus TX + generation-encryption

            timestamp = a + c + d;

            Simulator::Schedule(Seconds(0), &UanExperiment::SendPacket2To0, this); 
        } else if (pktNum == 7) {
            in3 >> strpkt1;
            l = strpkt1.length();
            a = (double) Simulator::Now().GetMilliSeconds();
            c = 5 * ((double) l / 80.0) * 1000; // janus RX
            d = 5 * ((double) l / 80.0) * 1000; // janus TX + generation-encryption

            timestamp = a + c + d;

            Simulator::Schedule(Seconds(0), &UanExperiment::SendPacket3To0, this); 
        } else if (pktNum == 8) {
            in3 >> strpkt1;
            l = strpkt1.length();
            a = (double) Simulator::Now().GetMilliSeconds();
            c = 5 * ((double) l / 80.0) * 1000; // janus RX
            d = 5 * ((double) l / 80.0) * 1000; // janus TX + generation-encryption

            timestamp = a + c + d;

            Simulator::Schedule(Seconds(0), &UanExperiment::SendPacket4To0, this);
        } else if (pktNum == 9) {
            in3 >> strpkt1;
            l = strpkt1.length();
            a = (double) Simulator::Now().GetMilliSeconds();
            c = 5 * ((double) l / 80.0) * 1000; // janus RX
            d = 5 * ((double) l / 80.0) * 1000; // janus TX + generation-encryption

            timestamp = a + c + d;

            Simulator::Schedule(Seconds(0), &UanExperiment::SendPacket5To0, this);
        } else if (pktNum == 10) {
            in4 >> strpkt1;
            l = strpkt1.length();
            a = (double) Simulator::Now().GetMilliSeconds();
            b = 0.0373279; // decryption-verification
            c = 5 * ((double) l / 80.0) * 1000; // janus RX
            d = 5 * ((double) l / 80.0) * 1000 + 0.0368927; // janus TX + generation-encryption

            timestamp = a + b + c + d;
            rtt = timestamp - rtt + (double)numFails * 420000.0;
            totRTT += rtt;
            //NS_LOG_UNCOND(to_string(distance) << ", " << to_string(rtt));
            
            if (noiseBench < 1) {
                if (protI % 200 == 0) {
                    avgRTT = totRTT / 100.0;
                    NS_LOG_UNCOND(1.0 - noiseBench << ", " << avgRTT);
                    totRTT = 0;
                    noiseBench += .1;
                }
            }
            
            //distance += 10;
            //m_nodes.Get(1)->GetObject<MobilityModel> ()->SetPosition(Vector(distance, 0, 0));

//            if (protI == 1) {
//                k = timestamp;
//            }
            Simulator::Schedule(MilliSeconds(0), &UanExperiment::SendPacket1To1, this); 
        } 
            
        in1.close();
        in2.close();
        in3.close();
        in4.close();
        in5.close();
        
        while (l % 8 != 0) {
            l++;
        }

        uint8_t* janusDecArr1 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
        string binPacket1, binPacket2, binPacket3, binPacket4;        

        packet1->CopyData(janusDecArr1, l / 8);
        
        for (int i = 0; i < l / 8; i++) {
            binPacket1 += bitset<8>(*(janusDecArr1 + i)).to_string();
        }
//        NS_LOG_UNCOND(binPacket1);
        free(janusDecArr1);
    }
}

void
UanExperiment::SetupApplications() {
    NodeContainer::Iterator node = m_nodes.Begin();
    PacketSocketHelper packetSocketHelper;
    while (node != m_nodes.End()) {
        packetSocketHelper.Install(*node);
        PacketSocketAddress socketAddress;
        socketAddress.SetSingleDevice((*node)->GetDevice(0)->GetIfIndex());
        socketAddress.SetProtocol(0);
        m_sockets[*node] = Socket::CreateSocket(*node, TypeId::LookupByName("ns3::PacketSocketFactory"));
        m_sockets[*node]->Bind();
        m_sockets[*node]->Connect(socketAddress);
        m_sockets[*node]->SetRecvCallback(MakeCallback(&UanExperiment::PrintReceivedPacket, this));
        node++;
    }
}

int chartobin(char c) {
    if (c == '1')
        return 1;
    else
        return 0;
}

void UanExperiment::SendPacket1To1() {
    NodeContainer::Iterator node = m_nodes.Begin();
    node++;
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node = m_nodes.Begin();
    protI++;
    pktNum = 1;
    numFails = 0;
    double noise = uniformRandomVariable->GetValue (0, 1);
    
    string strpkt1;
    ifstream in1("/home/ttm4128/aegisnocargopkt1.txt", ios_base::in);
    in1 >> strpkt1;
    int l = strpkt1.length();
    
    int i, j, k, b, p, janusIndex = -1;
    char newArr8[l / 8];
    uint8_t* janusDecArr1 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
    j = -1;

    for (i = 0; i < l; i++) {
        j++;
        newArr8[j] = strpkt1[i];
        if ((i + 1) % 8 == 0) {
            janusIndex++;
            j = -1;
            b = 0;
            for (k = 7; k > -1; k--) {
                p = (int) pow(2, 7 - k);
                b += chartobin(newArr8[k]) * p;
            }
            // NS_LOG_UNCOND("b = " << b);
            *(janusDecArr1 + janusIndex) = (uint8_t) b;
        }
    }

    in1.close();
    Ptr<Packet> pkt1 = Create<Packet> (janusDecArr1, l / 8);
    delete janusDecArr1;
    
    while (noise >= noiseBench) {
        //NS_LOG_UNCOND("noise = " << noise);
        numFails++;
        protI++;
        noise = uniformRandomVariable->GetValue (0, 1);
    } 

    //NS_LOG_UNCOND("numFails = " << numFails);
    rtt = (double) (Simulator::Now().GetMilliSeconds());
    timestamp = oneWay = (double) Simulator::Now().GetMilliSeconds() + 0.0368927 + ((double) l / 80.0) * 1000; // Now + generation-encryption + Janus encoding
    Simulator::Schedule(Seconds(0), &UanExperiment::SendSinglePacket, this, *node, pkt1, dst);
    
    //NS_LOG_UNCOND("Node 0 | sent 1 packet to 1, timestamp: " << to_string(timestamp));
}

void UanExperiment::SendPacket2To1() {
    NodeContainer::Iterator node = m_nodes.Begin();
    node++;
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node = m_nodes.Begin();
    pktNum = 2;
    
    string strpkt;
    ifstream in1("/home/ttm4128/aegisnocargopkt2.txt", ios_base::in);
    in1 >> strpkt;
    int l = strpkt.length();
    
    int i, j, k, b, p, janusIndex = -1;
    char newArr8[l / 8];
    uint8_t* janusDecArr1 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
    j = -1;

    for (i = 0; i < l; i++) {
        j++;
        newArr8[j] = strpkt[i];
        if ((i + 1) % 8 == 0) {
            janusIndex++;
            j = -1;
            b = 0;
            for (k = 7; k > -1; k--) {
                p = (int) pow(2, 7 - k);
                b += chartobin(newArr8[k]) * p;
            }
            // NS_LOG_UNCOND("b = " << b);
            *(janusDecArr1 + janusIndex) = (uint8_t) b;
        }
    }

    in1.close();
    Ptr<Packet> pkt1 = Create<Packet> (janusDecArr1, l / 8);
    delete janusDecArr1;
    
    Simulator::Schedule(Seconds(1), &UanExperiment::SendSinglePacket, this, *node, pkt1, dst);
    timestamp += ((double)l / 80.0) * 1000; // Now + Janus encoding
    //NS_LOG_UNCOND("Node 0 | sent 2 packets to 1, timestamp: " << to_string(timestamp));
}

void UanExperiment::SendPacket3To1() {
    NodeContainer::Iterator node = m_nodes.Begin();
    node++;
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node = m_nodes.Begin();
    pktNum = 3;
    
    string strpkt;
    ifstream in("/home/ttm4128/aegisnocargopkt3.txt", ios_base::in);
    in >> strpkt;
    int l = strpkt.length();
    
    int i, j, k, b, p, janusIndex = -1;
    char newArr8[l / 8];
    uint8_t* janusDecArr = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
    j = -1;

    for (i = 0; i < l; i++) {
        j++;
        newArr8[j] = strpkt[i];
        if ((i + 1) % 8 == 0) {
            janusIndex++;
            j = -1;
            b = 0;
            for (k = 7; k > -1; k--) {
                p = (int) pow(2, 7 - k);
                b += chartobin(newArr8[k]) * p;
            }
            // NS_LOG_UNCOND("b = " << b);
            *(janusDecArr + janusIndex) = (uint8_t) b;
        }
    }

    in.close();
    Ptr<Packet> pkt1 = Create<Packet> (janusDecArr, l / 8);
    delete janusDecArr;
    
    Simulator::Schedule(Seconds(0), &UanExperiment::SendSinglePacket, this, *node, pkt1, dst);
    timestamp += ((double)l / 80.0) * 1000; // Now + Janus encoding
    //NS_LOG_UNCOND("Node 0 | sent 3 packets to 1, timestamp: " << to_string(timestamp));
}

void UanExperiment::SendPacket4To1() {
    NodeContainer::Iterator node = m_nodes.Begin();
    node++;
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node = m_nodes.Begin();
    pktNum = 4;
    
    string strpkt;
    ifstream in("/home/ttm4128/aegisnocargopkt4.txt", ios_base::in);
    in >> strpkt;
    int l = strpkt.length();
    
    int i, j, k, b, p, janusIndex = -1;
    char newArr8[l / 8];
    uint8_t* janusDecArr = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
    j = -1;

    for (i = 0; i < l; i++) {
        j++;
        newArr8[j] = strpkt[i];
        if ((i + 1) % 8 == 0) {
            janusIndex++;
            j = -1;
            b = 0;
            for (k = 7; k > -1; k--) {
                p = (int) pow(2, 7 - k);
                b += chartobin(newArr8[k]) * p;
            }
            // NS_LOG_UNCOND("b = " << b);
            *(janusDecArr + janusIndex) = (uint8_t) b;
        }
    }

    in.close();
    Ptr<Packet> pkt1 = Create<Packet> (janusDecArr, l / 8);
    delete janusDecArr;
    
    Simulator::Schedule(Seconds(0), &UanExperiment::SendSinglePacket, this, *node, pkt1, dst);
    timestamp += ((double)l / 80.0) * 1000; // Now + Janus encoding
    //NS_LOG_UNCOND("Node 0 | sent 4 packets to 1, timestamp: " << to_string(timestamp));
}

void UanExperiment::SendPacket5To1() {
    NodeContainer::Iterator node = m_nodes.Begin();
    node++;
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node = m_nodes.Begin();
    pktNum = 5;
    
    string strpkt;
    ifstream in("/home/ttm4128/aegisnocargopkt5.txt", ios_base::in);
    in >> strpkt;
    int l = strpkt.length();
    
    int i, j, k, b, p, janusIndex = -1;
    char newArr8[l / 8];
    uint8_t* janusDecArr = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
    j = -1;

    for (i = 0; i < l; i++) {
        j++;
        newArr8[j] = strpkt[i];
        if ((i + 1) % 8 == 0) {
            janusIndex++;
            j = -1;
            b = 0;
            for (k = 7; k > -1; k--) {
                p = (int) pow(2, 7 - k);
                b += chartobin(newArr8[k]) * p;
            }
            // NS_LOG_UNCOND("b = " << b);
            *(janusDecArr + janusIndex) = (uint8_t) b;
        }
    }

    in.close();
    Ptr<Packet> pkt1 = Create<Packet> (janusDecArr, l / 8);
    delete janusDecArr;
    
    Simulator::Schedule(Seconds(0), &UanExperiment::SendSinglePacket, this, *node, pkt1, dst);
    timestamp += ((double)l / 80.0) * 1000; // Now + Janus encoding
    //NS_LOG_UNCOND("Node 0 | sent 5 packets to 1, timestamp: " << to_string(timestamp));
}

void UanExperiment::SendPacket1To0() {  
    NodeContainer::Iterator node = m_nodes.Begin();
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node++;

    string strpkt1;
    ifstream in1("/home/ttm4128/aegisnocargopkt1.txt", ios_base::in);
    in1 >> strpkt1;
    int l = strpkt1.length();
    pktNum = 6;
    
    int i, j, k, b, p, janusIndex = -1;
    char newArr8[l / 8];
    uint8_t* janusDecArr1 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
    j = -1;

    for (i = 0; i < l; i++) {
        j++;
        newArr8[j] = strpkt1[i];
        if ((i + 1) % 8 == 0) {
            janusIndex++;
            j = -1;
            b = 0;
            for (k = 7; k > -1; k--) {
                p = (int) pow(2, 7 - k);
                b += chartobin(newArr8[k]) * p;
            }
            // NS_LOG_UNCOND("b = " << b);
            *(janusDecArr1 + janusIndex) = (uint8_t) b;
        }
    }

    in1.close();
    Ptr<Packet> pkt1 = Create<Packet> (janusDecArr1, l / 8);
    delete janusDecArr1;
    
    Simulator::Schedule(Seconds(0), &UanExperiment::SendSinglePacket, this, *node, pkt1, dst);
    timestamp += 0.0368927 + ((double)l / 80.0) * 1000; // Now + generation-encryption + Janus encoding
    //NS_LOG_UNCOND("Node 1 | sent 1 packet to 0, timestamp: " << to_string(timestamp));
}

void UanExperiment::SendPacket2To0() {  
    NodeContainer::Iterator node = m_nodes.Begin();
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node++;

    string strpkt1;
    ifstream in1("/home/ttm4128/aegisnocargopkt2.txt", ios_base::in);
    in1 >> strpkt1;
    int l = strpkt1.length();
    pktNum = 7;
    
    int i, j, k, b, p, janusIndex = -1;
    char newArr8[l / 8];
    uint8_t* janusDecArr1 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
    j = -1;

    for (i = 0; i < l; i++) {
        j++;
        newArr8[j] = strpkt1[i];
        if ((i + 1) % 8 == 0) {
            janusIndex++;
            j = -1;
            b = 0;
            for (k = 7; k > -1; k--) {
                p = (int) pow(2, 7 - k);
                b += chartobin(newArr8[k]) * p;
            }
            // NS_LOG_UNCOND("b = " << b);
            *(janusDecArr1 + janusIndex) = (uint8_t) b;
        }
    }

    in1.close();
    Ptr<Packet> pkt1 = Create<Packet> (janusDecArr1, l / 8);
    delete janusDecArr1;
    
    Simulator::Schedule(Seconds(0), &UanExperiment::SendSinglePacket, this, *node, pkt1, dst);
    timestamp += ((double)l / 80.0) * 1000; // Janus encoding
    //NS_LOG_UNCOND("Node 1 | sent 2 packets to 0, timestamp: " << to_string(timestamp));
}

void UanExperiment::SendPacket3To0() {  
    NodeContainer::Iterator node = m_nodes.Begin();
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node++;

    string strpkt1;
    ifstream in1("/home/ttm4128/aegisnocargopkt3.txt", ios_base::in);
    in1 >> strpkt1;
    int l = strpkt1.length();
    pktNum = 8;
    
    int i, j, k, b, p, janusIndex = -1;
    char newArr8[l / 8];
    uint8_t* janusDecArr1 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
    j = -1;

    for (i = 0; i < l; i++) {
        j++;
        newArr8[j] = strpkt1[i];
        if ((i + 1) % 8 == 0) {
            janusIndex++;
            j = -1;
            b = 0;
            for (k = 7; k > -1; k--) {
                p = (int) pow(2, 7 - k);
                b += chartobin(newArr8[k]) * p;
            }
            // NS_LOG_UNCOND("b = " << b);
            *(janusDecArr1 + janusIndex) = (uint8_t) b;
        }
    }

    in1.close();
    Ptr<Packet> pkt1 = Create<Packet> (janusDecArr1, l / 8);
    delete janusDecArr1;
    
    Simulator::Schedule(Seconds(0), &UanExperiment::SendSinglePacket, this, *node, pkt1, dst);
    timestamp += ((double)l / 80.0) * 1000; // Janus encoding
    //NS_LOG_UNCOND("Node 1 | sent 3 packets to 0, timestamp: " << to_string(timestamp));
}

void UanExperiment::SendPacket4To0() {  
    NodeContainer::Iterator node = m_nodes.Begin();
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node++;

    string strpkt1;
    ifstream in1("/home/ttm4128/aegisnocargopkt4.txt", ios_base::in);
    in1 >> strpkt1;
    int l = strpkt1.length();
    pktNum = 9;
    
    int i, j, k, b, p, janusIndex = -1;
    char newArr8[l / 8];
    uint8_t* janusDecArr1 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
    j = -1;

    for (i = 0; i < l; i++) {
        j++;
        newArr8[j] = strpkt1[i];
        if ((i + 1) % 8 == 0) {
            janusIndex++;
            j = -1;
            b = 0;
            for (k = 7; k > -1; k--) {
                p = (int) pow(2, 7 - k);
                b += chartobin(newArr8[k]) * p;
            }
            // NS_LOG_UNCOND("b = " << b);
            *(janusDecArr1 + janusIndex) = (uint8_t) b;
        }
    }

    in1.close();
    Ptr<Packet> pkt1 = Create<Packet> (janusDecArr1, l / 8);
    delete janusDecArr1;
    
    Simulator::Schedule(Seconds(0), &UanExperiment::SendSinglePacket, this, *node, pkt1, dst);
    timestamp += ((double)l / 80.0) * 1000; // Janus encoding
    //NS_LOG_UNCOND("Node 1 | sent 4 packets to 0, timestamp: " << to_string(timestamp));
}

void UanExperiment::SendPacket5To0() {  
    NodeContainer::Iterator node = m_nodes.Begin();
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node++;

    string strpkt1;
    ifstream in1("/home/ttm4128/aegisnocargopkt5.txt", ios_base::in);
    in1 >> strpkt1;
    int l = strpkt1.length();
    pktNum = 10;
    
    int i, j, k, b, p, janusIndex = -1;
    char newArr8[l / 8];
    uint8_t* janusDecArr1 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
    j = -1;

    for (i = 0; i < l; i++) {
        j++;
        newArr8[j] = strpkt1[i];
        if ((i + 1) % 8 == 0) {
            janusIndex++;
            j = -1;
            b = 0;
            for (k = 7; k > -1; k--) {
                p = (int) pow(2, 7 - k);
                b += chartobin(newArr8[k]) * p;
            }
            // NS_LOG_UNCOND("b = " << b);
            *(janusDecArr1 + janusIndex) = (uint8_t) b;
        }
    }

    in1.close();
    Ptr<Packet> pkt1 = Create<Packet> (janusDecArr1, l / 8);
    delete janusDecArr1;
    
    Simulator::Schedule(Seconds(0), &UanExperiment::SendSinglePacket, this, *node, pkt1, dst);
    timestamp += ((double)l / 80.0) * 1000; // Janus encoding
    //NS_LOG_UNCOND("Node 1 | sent 5 packets to 0, timestamp: " << to_string(timestamp));
}

void
UanExperiment::SendSinglePacket(Ptr<Node> node, Ptr<Packet> pkt, Mac8Address dst) {
    //NS_LOG_UNCOND ( Simulator::Now ().GetHours () << "h" << " packet sent to " << dst );
    PacketSocketAddress socketAddress;
    socketAddress.SetSingleDevice(node->GetDevice(0)->GetIfIndex());
    socketAddress.SetPhysicalAddress(dst);
    socketAddress.SetProtocol(0);
    m_sockets[node]->SendTo(pkt, 0, socketAddress);
}

void
UanExperiment::Prepare() {
    m_nodes.Create(2);
    SetupPositions();
    SetupCommunications();
    SetupApplications();
    SendPacket1To1();
}

void
UanExperiment::Teardown() {
    std::map<Ptr<Node>, Ptr<Socket> >::iterator socket;

    for (socket = m_sockets.begin(); socket != m_sockets.end(); socket++) {
        socket->second->Close();
    }
}

int
main(int argc, char *argv[]) {
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    UanExperiment experiment;
    experiment.Prepare();

    Simulator::Stop(Hours(48));
    Simulator::Run();
    Simulator::Destroy();

    experiment.Teardown();

    //double avg = UanExperiment::totRTT / UanExperiment::protI;
    //NS_LOG_UNCOND("Average RTT: " << avg);
    return 0;
}
