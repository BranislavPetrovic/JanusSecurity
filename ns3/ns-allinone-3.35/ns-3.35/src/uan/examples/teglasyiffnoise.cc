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
    
    void SendPacketTo1();
    
    void SendPacketTo0();

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

private:
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
        string strpkt1;
        ifstream in1("/home/ttm4128/teglasyiffrc5.txt", ios_base::in);
        
        if(Mac8Address::ConvertFrom(srcAddress) == 0) {
            in1 >> strpkt1;
            l = strpkt1.length();
            a = (double) Simulator::Now().GetMilliSeconds();
            b = 0.0026934; // decryption-verification
            c = ((double) l / 80.0) * 1000; // janus RX
            d = ((double) l / 80.0) * 1000 + 0.002678; // janus TX + generation-encryption

            timestamp = a + b + c + d;
            oneWay = timestamp - oneWay;
            
            Simulator::Schedule(Seconds(0), &UanExperiment::SendPacketTo0, this);
        } else {
            in1 >> strpkt1;
            l = strpkt1.length();
            a = (double) Simulator::Now().GetMilliSeconds();
            b = 0.0026934; // decryption-verification
            c = ((double) l / 80.0) * 1000; // janus RX
            d = ((double) l / 80.0) * 1000 + 0.002678; // janus TX + generation-encryption

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
            
            Simulator::Schedule(MilliSeconds(0), &UanExperiment::SendPacketTo1, this);
        }
                    
        in1.close();

        while (l % 8 != 0) {
            l++;
        }

        uint8_t* janusDecArr1 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
        string binPacket1;

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

void UanExperiment::SendPacketTo1() {  
    NodeContainer::Iterator node = m_nodes.Begin();
    node++;
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node = m_nodes.Begin();
    protI++;
    numFails = 0;
    double noise = uniformRandomVariable->GetValue (0, 1);
        
    string packet;
    ifstream in("/home/ttm4128/teglasyiffrc5.txt", ios_base::in);
    in >> packet;
    int l = packet.length();
    double numL = l;

    while (l % 8 != 0) {
        l++;
    }
    
    int i, j, k, b, p, janusIndex = -1;
    char newArr8[l / 8];
    uint8_t* janusDecArr = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
    j = -1;

    for (i = 0; i < l; i++) {
        j++;
        newArr8[j] = packet[i];
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

    Ptr<Packet> pkt = Create<Packet> (janusDecArr, l / 8);

    while (noise >= noiseBench) {
        //NS_LOG_UNCOND("noise = " << noise);
        numFails++;
        protI++;
        noise = uniformRandomVariable->GetValue (0, 1);
    } 

    Simulator::Schedule(Seconds(0), &UanExperiment::SendSinglePacket, this, *node, pkt, dst);
    rtt = (double)(Simulator::Now().GetMilliSeconds());
    timestamp = oneWay = (double)(Simulator::Now().GetMilliSeconds()) + 0.002678 + (numL / 80.0) * 1000; // Now + enc + Janus encoding
    //NS_LOG_UNCOND("Sent packet to 1, timestamp: " << to_string(timestamp) << " ms, distance: " << m_nodes.Get(1)->GetObject<MobilityModel> ()->GetPosition().x << " m");
}

void UanExperiment::SendPacketTo0() {  
    NodeContainer::Iterator node = m_nodes.Begin();
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node++;
    
    string packet;
    ifstream in("/home/ttm4128/teglasyiffrc5.txt", ios_base::in);
    in >> packet;
    int l = packet.length();
    in >> packet;
    l = packet.length();
    
    int i, j, k, b, p, janusIndex = -1;
    char newArr8[l / 8];
    uint8_t* janusDecArr = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
    j = -1;

    for (i = 0; i < l; i++) {
        j++;
        newArr8[j] = packet[i];
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

    Ptr<Packet> pkt = Create<Packet> (janusDecArr, l / 8);
    //cout << "timestamp at sendto0: " << timestamp << endl;
    Simulator::Schedule(Seconds(0), &UanExperiment::SendSinglePacket, this, *node, pkt, dst);
    timestamp += 0.002678 + (double)l / 80; // Now + gen-enc + Janus encoding
    //NS_LOG_UNCOND("Sent packet to 0, timestamp: " << to_string(timestamp));
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
    SendPacketTo1();
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

    Simulator::Stop(Days(2));
    Simulator::Run();
    Simulator::Destroy();

    experiment.Teardown();

    return 0;
}
