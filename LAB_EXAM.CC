#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

int main(int argc, char *argv[])
{
    LogComponentEnable ("PacketSink", LOG_LEVEL_ALL);
	
	Config::SetDefault("ns3::OnOffApplication::PacketSize", UintegerValue(250));
    	Config::SetDefault("ns3::OnOffApplication::DataRate", StringValue("10Mbps"));
	
	CommandLine cmd;
	cmd.Parse (argc, argv);
	Time::SetResolution(Time::NS);
    
    	NodeContainer nodes0;
	nodes0.Create(7);
	
	NodeContainer nodes1;
	nodes1.Add(nodes0.Get(4));
	nodes1.Add(nodes0.Get(0));
	
	NodeContainer nodes2;
	nodes2.Add(nodes0.Get(0));
	nodes2.Add(nodes0.Get(3));
	
	NodeContainer nodes3;
	nodes3.Add(nodes0.Get(2));
	nodes3.Add(nodes0.Get(1));
	
	NodeContainer nodes4;
	nodes4.Add(nodes0.Get(1));
	nodes4.Add(nodes0.Get(5));
	
	NodeContainer nodes5;
	nodes5.Add(nodes0.Get(2));
	nodes5.Add(nodes0.Get(6));
	
	NodeContainer intnodes;
	intnodes.Add(nodes0.Get(0));
	intnodes.Add(nodes0.Get(1));
PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
	pointToPoint.SetChannelAttribute("Delay", StringValue("3ms"));
	
    NetDeviceContainer devices1;
	devices1 = pointToPoint.Install(nodes1);
    
    NetDeviceContainer devices2;
	devices2 = pointToPoint.Install(nodes2);

	NetDeviceContainer devices3;
	devices3 = pointToPoint.Install(nodes3);
	
	NetDeviceContainer devices4;
	devices4 = pointToPoint.Install(nodes4);
	
	NetDeviceContainer devices5;
	devices5= pointToPoint.Install(nodes5);
	
	NetDeviceContainer intdevices;
	intdevices = pointToPoint.Install(intnodes);
	
	InternetStackHelper stack;
	stack.Install(nodes0);
	
    Ipv4AddressHelper address1;
    address1.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces1 = address1.Assign (devices1);
        
    Ipv4AddressHelper address2;
    address2.SetBase ("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces2 = address2.Assign (devices2);
    
    Ipv4AddressHelper address3;
    address3.SetBase ("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces3 = address3.Assign (devices3);
    
    Ipv4AddressHelper address4;
    address4.SetBase ("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces4 = address4.Assign (devices4);
 Ipv4AddressHelper address5;
    address5.SetBase ("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces5 = address5.Assign (intdevices);
    
    Ipv4AddressHelper address6;
    address5.SetBase ("10.1.6.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces6 = address6.Assign (devices5);
    
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    
    uint16_t port = 9;
    OnOffHelper onoff("ns3::TcpSocketFactory", Address(InetSocketAddress(interfaces6.GetAddress(1), port)));
    onoff.SetConstantRate(DataRate("10Mb/s"));
    ApplicationContainer apps = onoff.Install(nodes0.Get(0));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(10.0));

    // Create a packet sink to receive these packets
    PacketSinkHelper sink("ns3::TcpSocketFactory", Address(InetSocketAddress(Ipv4Address::GetAny(), port)));
    apps = sink.Install(nodes0.Get(6));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(10.0));
    
    Simulator::Run();
    Simulator::Destroy();
	return 0;
}