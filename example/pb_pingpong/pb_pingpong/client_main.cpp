#include <iostream>
#include "network.h"
#include "message.pb.h"
#include <log.h>

ffnet::NervureConfigure nc("../clnt_net_conf.ini");

void	sendPingMsg()
{
	boost::shared_ptr<PingPong::Ping> pMsg(new PingPong::Ping());
	pMsg->set_msg("ping from client");
	pMsg->set_id(1);
	
	String ip = nc.get<String>("tcp-client.target-svr-ip-addr");
    uint16_t port = nc.get<uint16_t>("tcp-client.target-svr-port");
	
	ffnet::EndpointPtr_t tpp(new ffnet::Endpoint(ffnet::tcp_v4, boost::asio::ip::address_v4::from_string(ip), port));
	ffnet::NetNervure::send(pMsg, tpp);
	
	std::cout<<"service running..."<<std::endl;
}

void	onRecvPong(boost::shared_ptr<PingPong::Pong>pPong, ffnet::EndpointPtr_t pEP)
{
	PingPong::Pong & msg = *(pPong.get());
	std::cout<<"got pong! "<<msg.msg()<<std::endl;
	sendPingMsg();
}

int main(int argc, char **argv) {
	
    initialize_log("clnt.log");
	ffnet::ProtoBufNervure pbn;
	
	String ip = nc.get<String>("tcp-client.target-svr-ip-addr");
    uint16_t port = nc.get<uint16_t>("tcp-client.target-svr-port");

	ffnet::EndpointPtr_t ep(new ffnet::Endpoint(ffnet::tcp_v4, boost::asio::ip::address_v4::from_string(ip), port));
	pbn.addTCPClient(ep);
	
	pbn.addNeedToRecvPkg<PingPong::Pong>(onRecvPong);
	sendPingMsg();
	pbn.run();
	
	
    return 0;
}