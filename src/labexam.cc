#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"

using namespace ns3;

class MyApp : public Application
{
public:

  MyApp ();
  virtual ~MyApp();

  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);
  void ChangeRate(DataRate newrate);

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};

MyApp::MyApp ()
  : m_socket (0),
    m_peer (),
    m_packetSize (0),
    m_nPackets (0),
    m_dataRate (0),
    m_sendEvent (),
    m_running (false),
    m_packetsSent (0)
{
}

MyApp::~MyApp()
{
  m_socket = 0;
}

void
MyApp::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_nPackets = nPackets;
  m_dataRate = dataRate;
}

void
MyApp::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}

void
MyApp::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void
MyApp::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  m_socket->Send (packet);

  if (++m_packetsSent < m_nPackets)
    {
      ScheduleTx ();
    }
}

void
MyApp::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyApp::SendPacket, this);
    }
}

void
MyApp::ChangeRate(DataRate newrate)
{
   m_dataRate = newrate;
   return;
}

static void
CwndChange (uint32_t oldCwnd, uint32_t newCwnd)
{
  std::cout << Simulator::Now ().GetSeconds () << "\t" << newCwnd <<"\n";
}

void
IncRate (Ptr<MyApp> app, DataRate rate)
{
	app->ChangeRate(rate);
    return;
}


NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int main(int argc, char* argv[]) {
	CommandLine cmd;
		cmd.Parse(argc, argv);

		NodeContainer nodes;
		nodes.Create(12);

		InternetStackHelper internet;
		internet.Install(nodes);

		PointToPointHelper pointToPoint;
		pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
		pointToPoint.SetChannelAttribute("Delay", StringValue("5ms"));

		NetDeviceContainer c1n1 = pointToPoint.Install(nodes.Get(0), nodes.Get(2));
		NetDeviceContainer c2n1 = pointToPoint.Install(nodes.Get(0), nodes.Get(4));
		NetDeviceContainer c3n1 = pointToPoint.Install(nodes.Get(0), nodes.Get(6));
		NetDeviceContainer c4n1 = pointToPoint.Install(nodes.Get(0), nodes.Get(8));
		NetDeviceContainer c5n1 = pointToPoint.Install(nodes.Get(0), nodes.Get(10));
		NetDeviceContainer s1n2 = pointToPoint.Install(nodes.Get(1), nodes.Get(3));
		NetDeviceContainer s2n2 = pointToPoint.Install(nodes.Get(1), nodes.Get(5));
		NetDeviceContainer s3n2 = pointToPoint.Install(nodes.Get(1), nodes.Get(7));
		NetDeviceContainer s4n2 = pointToPoint.Install(nodes.Get(1), nodes.Get(9));
		NetDeviceContainer s5n2 = pointToPoint.Install(nodes.Get(1), nodes.Get(11));
		NetDeviceContainer n1n2 = pointToPoint.Install(nodes.Get(0), nodes.Get(1));

		Ipv4AddressHelper ipv4;
		ipv4.SetBase ("20.2.100.1", "255.255.255.255");
		Ipv4InterfaceContainer c1 = ipv4.Assign (c1n1);
		
		ipv4.SetBase ("20.2.101.1", "255.255.255.255");
		Ipv4InterfaceContainer c2 = ipv4.Assign (c2n1);
		
		ipv4.SetBase ("20.2.102.1", "255.255.255.255");
		Ipv4InterfaceContainer c3 = ipv4.Assign (c3n1);

		ipv4.SetBase ("20.2.103.1", "255.255.255.255");
		Ipv4InterfaceContainer s1 = ipv4.Assign (s1n2);
		
		ipv4.SetBase ("20.2.104.1", "255.255.255.255");
		Ipv4InterfaceContainer s2 = ipv4.Assign (s2n2);
		
		ipv4.SetBase ("20.2.105.1", "255.255.255.255");
		Ipv4InterfaceContainer s3 = ipv4.Assign (s3n2);
		
		ipv4.SetBase ("20.2.106.1", "255.255.255.255");
		Ipv4InterfaceContainer s4 = ipv4.Assign (s4n2);
		
		ipv4.SetBase ("20.2.107.1", "255.255.255.255");
		Ipv4InterfaceContainer n = ipv4.Assign (n1n2);
		
		ipv4.SetBase ("20.2.108.1", "255.255.255.255");
		Ipv4InterfaceContainer c4 = ipv4.Assign (c4n1);
		
		ipv4.SetBase ("20.2.109.1", "255.255.255.255");
		Ipv4InterfaceContainer c5 = ipv4.Assign (c5n1);
		
		ipv4.SetBase ("20.2.110.1", "255.255.255.255");
		Ipv4InterfaceContainer s5 = ipv4.Assign (s5n2);

		Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
		
		uint16_t sinkPort = 8000;
		Address sinkAddress(InetSocketAddress(c2.GetAddress(0), sinkPort));//+
		PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
		ApplicationContainer sinkApps = packetSinkHelper.Install(nodes.Get(2));//+
		sinkApps.Start(Seconds(0.));
		sinkApps.Stop(Seconds(100.));

		Ptr<Socket> ns3TcpSocket = Socket::CreateSocket(nodes.Get(3), TcpSocketFactory::GetTypeId());//*

		ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeCallback (&CwndChange));

		// Create TCP application at n0
		Ptr<MyApp> app = CreateObject<MyApp> ();
		app->Setup (ns3TcpSocket, sinkAddress, 1040, 100000, DataRate ("5Mbps"));
		nodes.Get (3)->AddApplication (app);//*
		app->SetStartTime (Seconds (1.));
		app->SetStopTime (Seconds (100.));

		
		

		Simulator::Stop (Seconds(10.0));
		Simulator::Run();
		Simulator::Destroy();
}