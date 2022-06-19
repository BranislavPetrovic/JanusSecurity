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

NS_LOG_COMPONENT_DEFINE ("UanRawExample");


class UanExperiment
{
public:
  UanExperiment ();

  /**
   * Set the UAN nodes position
   */
  void SetupPositions ();

  /**
   * Set the UAN nodes energy
   */
  void SetupEnergy ();

  /**
   * Set the UAN nodes communication channels
   */
  void SetupCommunications ();

  /**
   * Set the UAN nodes communication channels
   */
  void SetupApplications ();

  /**
   * Send a packet from all the nodes
   */
  void SendPackets ();

  /**
   * Send a packet from one of the nodes
   * \param node The sending node
   * \param pkt The packet
   * \param dst the destination
   */
  void SendSinglePacket (Ptr<Node> node, Ptr<Packet> pkt, Mac8Address dst);

  /**
   * Print the received packet
   * \param socket The receiving socket
   */
  void PrintReceivedPacket (Ptr<Socket> socket);

  /**
   * Prepare the experiment
   */
  void Prepare ();

  /**
   * Teardown the experiment
   */
  void Teardown ();

private:
  NodeContainer m_nodes; //!< UAN nodes
  std::map<Ptr<Node>, Ptr<Socket> > m_sockets; //!< send and receive sockets
};


UanExperiment::UanExperiment ()
{
}

void
UanExperiment::SetupPositions ()
{
  MobilityHelper mobilityHelper;
  mobilityHelper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityHelper.Install (m_nodes);
  m_nodes.Get (0)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
  m_nodes.Get (1)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 0, 0));
}

void
UanExperiment::SetupCommunications ()
{
  Ptr<UanChannel> channel = CreateObject<UanChannel> ();
  UanHelper uanHelper;
  NetDeviceContainer netDeviceContainer = uanHelper.Install (m_nodes, channel);
}

void
UanExperiment::PrintReceivedPacket (Ptr<Socket> socket)
{
  Address srcAddress;
  while (socket->GetRxAvailable () > 0)
    {
      Ptr<Packet> packet = socket->RecvFrom (srcAddress);
      PacketSocketAddress packetSocketAddress = PacketSocketAddress::ConvertFrom (srcAddress);
      srcAddress = packetSocketAddress.GetPhysicalAddress ();
      /*uint8_t* timestampArr = (uint8_t*)malloc(4 * sizeof(uint8_t));
      uint32_t timestampDec;*/

        string receivedFile;
        ifstream in("/tmp/januspktbin.txt", ios_base::in);
        in >> receivedFile;
        in.close();

        //NS_LOG_UNCOND(packet);

        int l = receivedFile.length();

        while(l % 8 != 0)
        {
            l++;
        }

      uint8_t* janusDecArr = (uint8_t*)malloc(l / 8 * sizeof(uint8_t));
      string binPacket;

    /*  
    packet->CopyData (timestampArr, 4);

      timestampDec = *(timestampArr);

      for (int i = 1; i < 4; i++)
      {
          timestampDec |= *(timestampArr + i) << 8*i;
          // NS_LOG_UNCOND ("dec interval: " << +timestampDec);
      }
      */

      packet->CopyData(janusDecArr, l / 8);

      for (int i = 0; i < l / 8; i++)
      {
          binPacket += bitset<8>(*(janusDecArr + i)).to_string();
      }

      if(Mac8Address::IsMatchingType (srcAddress))
        {
            NS_LOG_UNCOND("Received packet: " << binPacket);
    

         // NS_LOG_UNCOND ( "Time: " << Simulator::Now ().GetHours () << " h" << " | Node: " <<
           //               Mac8Address::ConvertFrom (srcAddress) << " | Timestamp: " << +timestampDec << " ms");
        }
    }
}

void
UanExperiment::SetupApplications ()
{
  NodeContainer::Iterator node = m_nodes.Begin ();
  PacketSocketHelper packetSocketHelper;
  while (node != m_nodes.End ())
    {
      packetSocketHelper.Install (*node);
      PacketSocketAddress socketAddress;
      socketAddress.SetSingleDevice ((*node)->GetDevice (0)->GetIfIndex ());
      socketAddress.SetProtocol (0);
      m_sockets[*node] = Socket::CreateSocket (*node, TypeId::LookupByName ("ns3::PacketSocketFactory"));
      m_sockets[*node]->Bind ();
      m_sockets[*node]->Connect (socketAddress);
      m_sockets[*node]->SetRecvCallback (MakeCallback (&UanExperiment::PrintReceivedPacket, this));
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
UanExperiment::SendPackets ()
{
  Ptr<UniformRandomVariable> uniformRandomVariable = CreateObject<UniformRandomVariable> ();
double time = uniformRandomVariable->GetValue (0, 10);

  NodeContainer::Iterator node = m_nodes.Begin();
  NodeContainer::Iterator node1 = m_nodes.Begin();
  Mac8Address dst = Mac8Address::ConvertFrom ((*node)->GetDevice (0)->GetAddress ());
  node++;
  Mac8Address dst1 = Mac8Address::ConvertFrom ((*node)->GetDevice (0)->GetAddress ());
  //while (node != m_nodes.End ())
  //  {
    string packet;
    ifstream in("/tmp/januspktbin.txt", ios_base::in);
    in >> packet;

    // NS_LOG_UNCOND(packet);

    int l = packet.length();
    double numL = l;
    
    while(l % 8 != 0)
    {
        l++;
    }

    int i, j, k, b, p, janusIndex = -1;
    char arr8[l / 8];
    uint8_t* janusDecArr = (uint8_t*)malloc(l / 8 * sizeof(uint8_t));
    //uint8_t janusDecArr[8];
    j = -1;

    for (i = 0; i < l; i++) 
    {
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
        *(janusDecArr + janusIndex) = (uint8_t)b;
        }
    }

    in.close();

    /*NS_LOG_UNCOND("janusDecArr: ");
    
    for (int i = 0; i < 8; i++)
        NS_LOG_UNCOND(+(*(janusDecArr + i)));*/
    
    // uint32_t timestamp = (uint32_t)Simulator::Now().GetMilliSeconds();
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

    Simulator::Schedule (Seconds (time), &UanExperiment::SendSinglePacket, this, *node, pkt, dst);

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

    ofstream out("/media/sf_JanusCode/distdelay.csv", ios_base::out);  
    double distance = m_nodes.Get (1)->GetObject<MobilityModel> ()->GetPosition ().x;
    double delay = distance / 1500 + numL / 80 + .05;
    double plotDist = distance;
    double plotDelay = delay;

    while(plotDist < 10000)
    {
        plotDelay = plotDist / 1500 + numL / 80 + .05;
        
        out << plotDist << ", " << plotDelay << "\n";
        
        plotDist += 10;
    }
    
    out.close();

    delay = (distance / 1500) + (numL / 80) + .05;
    NS_LOG_UNCOND("Delay: " << delay << " s");
    //printf("%5.3f", (double)(64 / 80));
    time += 10;  

    in >> packet;

    l = packet.length();
    janusIndex = -1;
    char newArr8[l / 8];
    janusDecArr = (uint8_t*)malloc(l / 8 * sizeof(uint8_t));
    //uint8_t janusDecArr[8];
    j = -1;

    for (i = 0; i < l; i++) 
    {
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
        *(janusDecArr + janusIndex) = (uint8_t)b;
        }
    }

    in.close();

    pkt = Create<Packet> (janusDecArr, l / 8);

    Simulator::Schedule (Seconds (time), &UanExperiment::SendSinglePacket, this, *node1, pkt, dst1);

    /*
    double time = uniformRandomVariable->GetValue (0, 60);
    Simulator::Schedule (Seconds (time), &UanExperiment::SendSinglePacket, this, *node, pkt, dst);

    time = uniformRandomVariable->GetValue (0, 60);
    Simulator::Schedule (Seconds (time), &UanExperiment::SendSinglePacket, this, *node1, pkt, dst1);*/
      //node++;
    //}
  Simulator::Schedule (Hours (2), &UanExperiment::SendPackets, this);
}

void
UanExperiment::SendSinglePacket (Ptr<Node> node, Ptr<Packet> pkt, Mac8Address dst)
{
  //NS_LOG_UNCOND ( Simulator::Now ().GetHours () << "h" << " packet sent to " << dst );
  PacketSocketAddress socketAddress;
  socketAddress.SetSingleDevice (node->GetDevice (0)->GetIfIndex ());
  socketAddress.SetPhysicalAddress (dst);
  socketAddress.SetProtocol (0);
  m_sockets[node]->SendTo (pkt, 0, socketAddress);
}

void
UanExperiment::Prepare ()
{
  m_nodes.Create (2);
  SetupPositions ();
  SetupCommunications ();
  SetupApplications ();
  SendPackets ();
}

void
UanExperiment::Teardown ()
{
  std::map<Ptr<Node>, Ptr<Socket> >::iterator socket;

  for (socket = m_sockets.begin (); socket != m_sockets.end (); socket++)
    {
      socket->second->Close ();
    }
}

int
main (int argc, char *argv[])
{
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);

  UanExperiment experiment;
  experiment.Prepare ();

  Simulator::Stop (Days (6));
  Simulator::Run ();
  Simulator::Destroy ();

  experiment.Teardown ();

  return 0;
}
