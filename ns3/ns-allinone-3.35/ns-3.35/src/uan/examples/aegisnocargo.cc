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

    /**
     * Send a packet from all the nodes
     */
    void SendPackets();
    
    void SendPacketTo1();
    
    void SendPacketTo0();
    
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

private:
    NodeContainer m_nodes; //!< UAN nodes
    std::map<Ptr<Node>, Ptr<Socket> > m_sockets; //!< send and receive sockets
    double timestamp;
    double oneWay;
    double rtt;
    ulong protI = 0;
    int pktNum;
};

UanExperiment::UanExperiment() {
}

void
UanExperiment::SetupPositions() {
    MobilityHelper mobilityHelper;
    mobilityHelper.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobilityHelper.Install(m_nodes);
    m_nodes.Get(0)->GetObject<MobilityModel> ()->SetPosition(Vector(0, 0, 0));
    m_nodes.Get(1)->GetObject<MobilityModel> ()->SetPosition(Vector(1, 0, 0));
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
    double distance = m_nodes.Get (1)->GetObject<MobilityModel> ()->GetPosition ().x;
    while (socket->GetRxAvailable() > 0) {
        Ptr<Packet> packet1 = socket->RecvFrom(srcAddress);
//                PacketSocketAddress packetSocketAddress1 = PacketSocketAddress::ConvertFrom(srcAddress);
//        srcAddress = packetSocketAddress1.GetPhysicalAddress();
//
//        Ptr<Packet> packet2 = socket->RecvFrom(srcAddress);
//                PacketSocketAddress packetSocketAddress2 = PacketSocketAddress::ConvertFrom(srcAddress);
//        srcAddress = packetSocketAddress2.GetPhysicalAddress();
//
//        Ptr<Packet> packet3 = socket->RecvFrom(srcAddress);
//                PacketSocketAddress packetSocketAddress3 = PacketSocketAddress::ConvertFrom(srcAddress);
//        srcAddress = packetSocketAddress3.GetPhysicalAddress();
//
//        Ptr<Packet> packet4 = socket->RecvFrom(srcAddress);
//                PacketSocketAddress packetSocketAddress4 = PacketSocketAddress::ConvertFrom(srcAddress);
//        srcAddress = packetSocketAddress4.GetPhysicalAddress();
//
//        Ptr<Packet> packet5 = socket->RecvFrom(srcAddress);
        PacketSocketAddress packetSocketAddress5 = PacketSocketAddress::ConvertFrom(srcAddress);
        srcAddress = packetSocketAddress5.GetPhysicalAddress();
        /*uint8_t* timestampArr = (uint8_t*)malloc(4 * sizeof(uint8_t));
        uint32_t timestampDec;*/
        
        uint32_t k;
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
            //b = 0.0161610; // decryption-verification
            c = ((double) l / 80.0) * 1000; // janus RX
            d = ((double) l / 80.0) * 1000 + 0.0368927; // janus TX + generation-encryption

            timestamp = a + c + d;

            Simulator::Schedule(Seconds(0), &UanExperiment::SendPacket2To1, this);
        } else if (pktNum == 2) {
            in2 >> strpkt1;
            //NS_LOG_UNCOND(strpkt1);
            l = strpkt1.length();
            a = (double) Simulator::Now().GetMilliSeconds();
            //b = 0.0161610; // decryption-verification
            c = ((double) l / 80.0) * 1000; // janus RX
            d = ((double) l / 80.0) * 1000; // janus TX + generation-encryption

            timestamp = a + c + d;

            Simulator::Schedule(Seconds(0), &UanExperiment::SendPacket3To1, this);
        } else if (pktNum == 3) {
            in3 >> strpkt1;
            l = strpkt1.length();
            a = (double) Simulator::Now().GetMilliSeconds();
            //b = 0.0161610; // decryption-verification
            c = 5 * ((double) l / 80.0) * 1000; // janus RX
            d = 5 * ((double) l / 80.0) * 1000; // janus TX + generation-encryption

            timestamp = a + c + d;

            Simulator::Schedule(Seconds(0), &UanExperiment::SendPacket4To1, this);
        } else if (pktNum == 4) {
            in4 >> strpkt1;
            l = strpkt1.length();
            a = (double) Simulator::Now().GetMilliSeconds();
            //b = 0.0373279; // decryption-verification
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
            //b = 0.0161610; // decryption-verification
            c = 5 * ((double) l / 80.0) * 1000; // janus RX
            d = 5 * ((double) l / 80.0) * 1000; // janus TX + generation-encryption

            timestamp = a + c + d;

            Simulator::Schedule(Seconds(0), &UanExperiment::SendPacket2To0, this); 
        } else if (pktNum == 7) {
            in3 >> strpkt1;
            l = strpkt1.length();
            a = (double) Simulator::Now().GetMilliSeconds();
            //b = 0.0161610; // decryption-verification
            c = 5 * ((double) l / 80.0) * 1000; // janus RX
            d = 5 * ((double) l / 80.0) * 1000; // janus TX + generation-encryption

            timestamp = a + c + d;

            Simulator::Schedule(Seconds(0), &UanExperiment::SendPacket3To0, this); 
        } else if (pktNum == 8) {
            in3 >> strpkt1;
            l = strpkt1.length();
            a = (double) Simulator::Now().GetMilliSeconds();
            //b = 0.0161610; // decryption-verification
            c = 5 * ((double) l / 80.0) * 1000; // janus RX
            d = 5 * ((double) l / 80.0) * 1000; // janus TX + generation-encryption

            timestamp = a + c + d;

            Simulator::Schedule(Seconds(0), &UanExperiment::SendPacket4To0, this);
        } else if (pktNum == 9) {
            in3 >> strpkt1;
            l = strpkt1.length();
            a = (double) Simulator::Now().GetMilliSeconds();
            //b = 0.0161610; // decryption-verification
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
            rtt = timestamp - rtt;
            NS_LOG_UNCOND(to_string(distance) << ", " << to_string(rtt));
            distance += 10;
            m_nodes.Get(1)->GetObject<MobilityModel> ()->SetPosition(Vector(distance, 0, 0));

            if (protI == 1) {
                k = timestamp;
            }
            Simulator::Schedule(MilliSeconds(420000 - k), &UanExperiment::SendPacket1To1, this); 
        } 
            
        in1.close();
        in2.close();
        in3.close();
        in4.close();
        in5.close();
        
        //cout << "print function" << endl;

//        NS_LOG_UNCOND("a = " << a);
//        NS_LOG_UNCOND("b = " << b);
//        NS_LOG_UNCOND("c = " << c);
//        NS_LOG_UNCOND(to_string(timestamp));

        while (l % 8 != 0) {
            l++;
        }

        uint8_t* janusDecArr1 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
//        uint8_t* janusDecArr3 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
//        uint8_t* janusDecArr4 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
//        uint8_t* janusDecArr5 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
        string binPacket1, binPacket2, binPacket3, binPacket4;
        

        /*  
        packet->CopyData (timestampArr, 4);

          timestampDec = *(timestampArr);

          for (int i = 1; i < 4; i++)
          {
              timestampDec |= *(timestampArr + i) << 8*i;
              // NS_LOG_UNCOND ("dec interval: " << +timestampDec);
          }
         */

        packet1->CopyData(janusDecArr1, l / 8);
        //NS_LOG_UNCOND("lollel");
//        packet2->CopyData(janusDecArr2, l / 8);
//        packet3->CopyData(janusDecArr3, l / 8);
//        packet4->CopyData(janusDecArr4, l / 8);
//        packet5->CopyData(janusDecArr5, l / 8);
        
        for (int i = 0; i < l / 8; i++) {
            binPacket1 += bitset<8>(*(janusDecArr1 + i)).to_string();
        }
//        NS_LOG_UNCOND(binPacket1);
        free(janusDecArr1);
//        
//        //in1("/home/ttm4128/ccmnocargopkt2.txt", ios_base::in);
//        in1 >> strpkt1;
//        uint8_t* janusDecArr2 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
        //packet1->CopyData(janusDecArr2, l / 8);
//        
//        for (int i = 0; i < l / 8; i++) {
//            binPacket2 += bitset<8>(*(janusDecArr2 + i)).to_string();
//        }
//        
//        packet1->CopyData(janusDecArr3, l / 8);
//        
//        for (int i = 0; i < l / 8; i++) {
//            binPacket3 += bitset<8>(*(janusDecArr3 + i)).to_string();
//        }
//        
//        packet1->CopyData(janusDecArr4, l / 8);
//        
//        for (int i = 0; i < l / 8; i++) {
//            binPacket4 += bitset<8>(*(janusDecArr4 + i)).to_string();
//        }
//
//        packet1->CopyData(janusDecArr5, l / 8);
//        
//        for (int i = 0; i < l / 8; i++) {
//            binPacket5 += bitset<8>(*(janusDecArr5 + i)).to_string();
//        }


//        for (int i = 0; i < l / 8; i++) {
//            
//            binPacket2 += bitset<8>(*(janusDecArr2 + i)).to_string();
//            binPacket3 += bitset<8>(*(janusDecArr3 + i)).to_string();
//            binPacket4 += bitset<8>(*(janusDecArr4 + i)).to_string();
//            binPacket5 += bitset<8>(*(janusDecArr5 + i)).to_string();
//        }
        
//        if (Mac8Address::IsMatchingType(srcAddress)) {
//            NS_LOG_UNCOND("Received data from " << Mac8Address::ConvertFrom(srcAddress) << ":\n" <<
//                    binPacket1 << "\n" << binPacket2 << "\n" << binPacket3 <<
//                    "\n" << binPacket4 << "\n" << binPacket5 << "\n" <<
//                    "timestamp: " << to_string(timestamp));
//            NS_LOG_UNCOND("Received data from " << Mac8Address::ConvertFrom(srcAddress) << ":\n" <<
//                    strpkt1 << "\n" << "timestamp: " << to_string(timestamp));
//            
//            if(pktNum % 4 == 0) {
//                NS_LOG_UNCOND("\n");
//            }
//            if(Mac8Address::ConvertFrom(srcAddress) == 0) {
//                SendPacketTo0();
//            }
//            else {
//                if(protI == 1) {
//                    k = timestamp;
//                }                    
//                Simulator::Schedule(MilliSeconds(420000 - k), &UanExperiment::SendPacketTo1, this);
//            }


            // NS_LOG_UNCOND ( "Time: " << Simulator::Now ().GetHours () << " h" << " | Node: " <<
            //               Mac8Address::ConvertFrom (srcAddress) << " | Timestamp: " << +timestampDec << " ms");
        //}
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

void
UanExperiment::SendPackets() {
    Ptr<UniformRandomVariable> uniformRandomVariable = CreateObject<UniformRandomVariable> ();
    double time = uniformRandomVariable->GetValue(0, 10);

    NodeContainer::Iterator node = m_nodes.Begin();
    NodeContainer::Iterator node1 = m_nodes.Begin();
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node++;
    Mac8Address dst1 = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    //while (node != m_nodes.End ())
    //  {
    string packet;
    ifstream in("/home/ttm4128/teglasyiffrc5.txt", ios_base::in);
    in >> packet;

    // NS_LOG_UNCOND(packet);

    int l = packet.length();
    //cout << l << endl;
    double numL = l;

    while (l % 8 != 0) {
        l++;
    }

    int i, j, k, b, p, janusIndex = -1;
    char arr8[l / 8];
    uint8_t* janusDecArr = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
    //uint8_t janusDecArr[8];
    j = -1;

    for (i = 0; i < l; i++) {
        j++;
        arr8[j] = packet[i];
        if ((i + 1) % 8 == 0) {
            janusIndex++;
            j = -1;
            b = 0;
            for (k = 7; k > -1; k--) {
                p = (int) pow(2, 7 - k);
                b += chartobin(arr8[k]) * p;
            }
            // NS_LOG_UNCOND("b = " << b);
            *(janusDecArr + janusIndex) = (uint8_t) b;
        }
    }

    in.close();

    /*NS_LOG_UNCOND("janusDecArr: ");
    
    for (int i = 0; i < 8; i++)
        NS_LOG_UNCOND(+(*(janusDecArr + i)));*/

    timestamp = (double)Simulator::Now().GetMilliSeconds();
    // NS_LOG_UNCOND ("ms: " << +timestamp);
    /**(timestampArr + 0) = (uint8_t)(timestamp & 0x000000ff);
     *(timestampArr + 1) = (uint8_t)((timestamp >> 8) & 0x000000ff);
    // NS_LOG_UNCOND ("ms: " << +(*(timestampArr + 1)));
     *(timestampArr + 2) = (uint8_t)((timestamp >> 16) & 0x000000ff);
     *(timestampArr + 3) = (uint8_t)((timestamp >> 24) & 0x000000ff);*/

    //  for (int i = 0; i < 4; i++)
    //    NS_LOG_UNCOND ("msArr: " << +i << ' ' << +(*(timestampArr + i)) << ' ');

    Ptr<Packet> pkt = Create<Packet> (janusDecArr, l / 8);
    //sleep(2);
    //    SendSinglePacket(*node, pkt, dst);

    Simulator::Schedule(MilliSeconds(timestamp), &UanExperiment::SendSinglePacket, this, *node, pkt, dst);
    cout << "Sent first packet, timestamp: " << timestamp << endl;

    //double distance = m_nodes.Get (1)->GetObject<MobilityModel> ()->GetPosition ().x;
    //double delay = distance / 1480;
    /*timestamp = (uint32_t)(Simulator::Now().GetMilliSeconds() + delay * 1000);
    // NS_LOG_UNCOND ("ms: " << +timestamp);
     *(timestampArr + 0) = (uint8_t)(timestamp & 0x000000ff);
     *(timestampArr + 1) = (uint8_t)((timestamp >> 8) & 0x000000ff);
    // NS_LOG_UNCOND ("ms: " << +(*(timestampArr + 1)));
     *(timestampArr + 2) = (uint8_t)((timestamp >> 16) & 0x000000ff);
     *(timestampArr + 3) = (uint8_t)((timestamp >> 24) & 0x000000ff);
     */
    //pkt = Create<Packet> (janusDecArr, 8);

    //sleep(2);
    //SendSinglePacket(*node1, pkt, dst1);
    //time = uniformRandomVariable->GetValue (0, 10);  

    //ofstream out("/media/sf_o365_Branislav_-_Master_Project_-_General/Data/RTTTeglasyRC5.csv", ios_base::out);  
    double distance = m_nodes.Get(1)->GetObject<MobilityModel> ()->GetPosition().x;
    double delay = 2 * (distance / 1500 + numL / 80 + .05);
//    double plotDist = distance;
//    double plotDelay = delay;
//
//    while(plotDist <= 10000)
//    {
//        plotDelay = 2 * (plotDist / 1500 + numL / 80 + .05);
//
//        out << plotDist << ", " << plotDelay << "\n";
//
//        plotDist += 10;
//    }
//
//    out.close();

    //    delay = (distance / 1500) + (numL / 80) + .05;

    //printf("%5.3f", (double)(64 / 80));
    time += 10;

    in >> packet;

    l = packet.length();
    janusIndex = -1;
    char newArr8[l / 8];
    janusDecArr = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
    //uint8_t janusDecArr[8];
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

    pkt = Create<Packet> (janusDecArr, l / 8);

//    sleep(5);
    Simulator::Schedule(MilliSeconds(timestamp+7000), &UanExperiment::SendSinglePacket, this, *node1, pkt, dst1);
    //timestamp = (uint32_t)Simulator::Now().GetMilliSeconds();
    cout << "Sent second packet, timestamp: " << timestamp+7000 << endl;

    NS_LOG_UNCOND("Round-trip delay: " << delay << " s");

    /*
    double time = uniformRandomVariable->GetValue (0, 60);
    Simulator::Schedule (Seconds (time), &UanExperiment::SendSinglePacket, this, *node, pkt, dst);

    time = uniformRandomVariable->GetValue (0, 60);
    Simulator::Schedule (Seconds (time), &UanExperiment::SendSinglePacket, this, *node1, pkt, dst1);*/
    //node++;
    //}
//    Simulator::Schedule(Hours(2), &UanExperiment::SendPackets, this);
    Simulator::Schedule(Minutes(7), &UanExperiment::SendPackets, this);
}

void UanExperiment::SendPacketTo1() {  
    NodeContainer::Iterator node = m_nodes.Begin();
    node++;
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node = m_nodes.Begin();
    protI++;
    
    string strpkt1, strpkt2, strpkt3, strpkt4, strpkt5;
    ifstream in1("/home/ttm4128/ccmnocargopkt1.txt", ios_base::in);
    in1 >> strpkt1;
    int l = strpkt1.length();
    //in >> pkt1;
    //l = pkt1.length();
    
    int i, j, k, b, p, janusIndex = -1;
    char newArr8[l / 8];
    uint8_t* janusDecArr1 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
    uint8_t* janusDecArr2 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
    uint8_t* janusDecArr3 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
    uint8_t* janusDecArr4 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
    uint8_t* janusDecArr5 = (uint8_t*) malloc(l / 8 * sizeof (uint8_t));
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
    
    ifstream in2("/home/ttm4128/ccmnocargopkt2.txt", ios_base::in);
    in2 >> strpkt2;
    janusIndex = -1;
    j = -1;

    for (i = 0; i < l; i++) {
        j++;
        newArr8[j] = strpkt2[i];
        if ((i + 1) % 8 == 0) {
            janusIndex++;
            j = -1;
            b = 0;
            for (k = 7; k > -1; k--) {
                p = (int) pow(2, 7 - k);
                b += chartobin(newArr8[k]) * p;
            }
            // NS_LOG_UNCOND("b = " << b);
            *(janusDecArr2 + janusIndex) = (uint8_t) b;
        }
    }
    
    ifstream in3("/home/ttm4128/ccmnocargopkt3.txt", ios_base::in);
    in3 >> strpkt3;
    janusIndex = -1;
    j = -1;

    for (i = 0; i < l; i++) {
        j++;
        newArr8[j] = strpkt3[i];
        if ((i + 1) % 8 == 0) {
            janusIndex++;
            j = -1;
            b = 0;
            for (k = 7; k > -1; k--) {
                p = (int) pow(2, 7 - k);
                b += chartobin(newArr8[k]) * p;
            }
            // NS_LOG_UNCOND("b = " << b);
            *(janusDecArr3 + janusIndex) = (uint8_t) b;
        }
    }
    
    ifstream in4("/home/ttm4128/ccmnocargopkt4.txt", ios_base::in);
    in4 >> strpkt4;
    janusIndex = -1;
    j = -1;
    
    for (i = 0; i < l; i++) {
        j++;
        newArr8[j] = strpkt4[i];
        if ((i + 1) % 8 == 0) {
            janusIndex++;
            j = -1;
            b = 0;
            for (k = 7; k > -1; k--) {
                p = (int) pow(2, 7 - k);
                b += chartobin(newArr8[k]) * p;
            }
            // NS_LOG_UNCOND("b = " << b);
            *(janusDecArr4 + janusIndex) = (uint8_t) b;
        }
    }

    ifstream in5("/home/ttm4128/ccmnocargopkt5.txt", ios_base::in);
    in5 >> strpkt5;
    janusIndex = -1;
    j = -1;
    
    for (i = 0; i < l; i++) {
        j++;
        newArr8[j] = strpkt5[i];
        if ((i + 1) % 8 == 0) {
            janusIndex++;
            j = -1;
            b = 0;
            for (k = 7; k > -1; k--) {
                p = (int) pow(2, 7 - k);
                b += chartobin(newArr8[k]) * p;
            }
            // NS_LOG_UNCOND("b = " << b);
            *(janusDecArr5 + janusIndex) = (uint8_t) b;
        }
    }

    in1.close();
    in2.close();
    in3.close();
    in4.close();
    in5.close();

    Ptr<Packet> pkt1 = Create<Packet> (janusDecArr1, l / 8);
    Ptr<Packet> pkt2 = Create<Packet> (janusDecArr2, l / 8);
    Ptr<Packet> pkt3 = Create<Packet> (janusDecArr3, l / 8);
    Ptr<Packet> pkt4 = Create<Packet> (janusDecArr4, l / 8);
    Ptr<Packet> pkt5 = Create<Packet> (janusDecArr5, l / 8);
    
    delete janusDecArr1;
    delete janusDecArr2;
    delete janusDecArr3;
    delete janusDecArr4;
    delete janusDecArr5;

    Simulator::Schedule(Seconds(0), &UanExperiment::SendSinglePacket, this, *node, pkt1, dst);
    timestamp = (double)Simulator::Now().GetMilliSeconds() + 0.0160203 + ((double)l / 80) * 1000; // Now + generation-encryption + Janus encoding
    NS_LOG_UNCOND("Node 0 | sent 1 packet to 1, timestamp: " << to_string(timestamp));

    Simulator::Schedule(Seconds(1), &UanExperiment::SendSinglePacket, this, *node, pkt2, dst);
    timestamp += 2 * ((double)l / 80) * 1000; // Now + Janus encoding
    NS_LOG_UNCOND("Node 0 | sent 2 packets to 1, timestamp: " << to_string(timestamp));

//    Simulator::Schedule(Seconds(0), &UanExperiment::SendSinglePacket, this, *node, pkt3, dst);
//    timestamp = (double)Simulator::Now().GetMilliSeconds() + 3 * ((double)l / 80) * 1000; // Now + Janus encoding
//    NS_LOG_UNCOND("Node 0 | sent 3 packets to 1, timestamp: " << to_string(timestamp));
//
//    Simulator::Schedule(Seconds(0), &UanExperiment::SendSinglePacket, this, *node, pkt4, dst);
//    timestamp = (double)Simulator::Now().GetMilliSeconds() + 4 * ((double)l / 80) * 1000; // Now + Janus encoding
//    NS_LOG_UNCOND("Node 0 | sent 4 packets to 1, timestamp: " << to_string(timestamp));
//
//    Simulator::Schedule(Seconds(0), &UanExperiment::SendSinglePacket, this, *node, pkt5, dst);
//    timestamp = (double)Simulator::Now().GetMilliSeconds() + 5 * ((double)l / 80) * 1000; // Now + generation-encryption + Janus encoding
//    NS_LOG_UNCOND("Node 0 | sent 5 packets to 1, timestamp: " << to_string(timestamp));
    //SendPacketTo0();

    //Simulator::Schedule(Minutes(2), &UanExperiment::SendPacketTo1, this);
}

void UanExperiment::SendPacket1To1() {
    NodeContainer::Iterator node = m_nodes.Begin();
    node++;
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node = m_nodes.Begin();
    protI++;
    pktNum = 1;
    
    string strpkt1;
    ifstream in1("/home/ttm4128/aegisnocargopkt1.txt", ios_base::in);
    in1 >> strpkt1;
    int l = strpkt1.length();
    //in >> pkt1;
    //l = pkt1.length();
    
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
    rtt = (double)(Simulator::Now().GetMilliSeconds());
    timestamp = oneWay = (double)Simulator::Now().GetMilliSeconds() + 0.0368927 + ((double)l / 80.0) * 1000; // Now + generation-encryption + Janus encoding
    //NS_LOG_UNCOND("Node 0 | sent 1 packet to 1, timestamp: " << to_string(timestamp));
}

void UanExperiment::SendPacket2To1() {
    NodeContainer::Iterator node = m_nodes.Begin();
    node++;
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node = m_nodes.Begin();
    //protI++;
    pktNum = 2;
    
    string strpkt;
    ifstream in1("/home/ttm4128/aegisnocargopkt2.txt", ios_base::in);
    in1 >> strpkt;
    int l = strpkt.length();
    //in >> pkt1;
    //l = pkt1.length();
    
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
    timestamp += ((double)l / 80.0) * 1000; // Now + generation-encryption + Janus encoding
    //NS_LOG_UNCOND("Node 0 | sent 2 packets to 1, timestamp: " << to_string(timestamp));
}

void UanExperiment::SendPacket3To1() {
    NodeContainer::Iterator node = m_nodes.Begin();
    node++;
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node = m_nodes.Begin();
    //protI++;
    pktNum = 3;
    
    string strpkt;
    ifstream in("/home/ttm4128/aegisnocargopkt3.txt", ios_base::in);
    in >> strpkt;
    int l = strpkt.length();
    //in >> pkt1;
    //l = pkt1.length();
    
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
    timestamp += ((double)l / 80.0) * 1000; // Now + generation-encryption + Janus encoding
    //NS_LOG_UNCOND("Node 0 | sent 3 packets to 1, timestamp: " << to_string(timestamp));
}

void UanExperiment::SendPacket4To1() {
    NodeContainer::Iterator node = m_nodes.Begin();
    node++;
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node = m_nodes.Begin();
    //protI++;
    pktNum = 4;
    
    string strpkt;
    ifstream in("/home/ttm4128/aegisnocargopkt4.txt", ios_base::in);
    in >> strpkt;
    int l = strpkt.length();
    //in >> pkt1;
    //l = pkt1.length();
    
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
    timestamp += ((double)l / 80.0) * 1000; // Now + generation-encryption + Janus encoding
    //NS_LOG_UNCOND("Node 0 | sent 4 packets to 1, timestamp: " << to_string(timestamp));
}

void UanExperiment::SendPacket5To1() {
    NodeContainer::Iterator node = m_nodes.Begin();
    node++;
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node = m_nodes.Begin();
    //protI++;
    pktNum = 5;
    
    string strpkt;
    ifstream in("/home/ttm4128/aegisnocargopkt5.txt", ios_base::in);
    in >> strpkt;
    int l = strpkt.length();
    //in >> pkt1;
    //l = pkt1.length();
    
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
    timestamp += ((double)l / 80.0) * 1000; // Now + generation-encryption + Janus encoding
    //NS_LOG_UNCOND("Node 0 | sent 5 packets to 1, timestamp: " << to_string(timestamp));
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
    timestamp += 0.002678 + (double)l / 80; // Now + RC5 encryption + Janus encoding
    NS_LOG_UNCOND("Sent packet to 0, timestamp: " << to_string(timestamp));

    //Simulator::Schedule(Hours(2), &UanExperiment::SendPacketTo0, this);
}

void UanExperiment::SendPacket1To0() {  
    NodeContainer::Iterator node = m_nodes.Begin();
    Mac8Address dst = Mac8Address::ConvertFrom((*node)->GetDevice(0)->GetAddress());
    node++;

    string strpkt1;
    ifstream in1("/home/ttm4128/aegisnocargopkt1.txt", ios_base::in);
    in1 >> strpkt1;
    int l = strpkt1.length();
    //in >> pkt1;
    //l = pkt1.length();
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
    //in >> pkt1;
    //l = pkt1.length();
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
    //in >> pkt1;
    //l = pkt1.length();
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
    //in >> pkt1;
    //l = pkt1.length();
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
    //in >> pkt1;
    //l = pkt1.length();
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
    //SendPackets();
    //SendPacketTo1();
//    SendPacketTo0();
    SendPacket1To1();
//    SendPacket2To1();
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

    Simulator::Stop(Days(6));
    Simulator::Run();
    Simulator::Destroy();

    experiment.Teardown();

    return 0;
}
