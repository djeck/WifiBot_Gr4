#include "wifibotClient.h"



RobotLibrary::forward(){
	
	WifibotClient::SendCommand(125,125,flags)
	
	}
	
RobotLibrary::backward(){
	
	WifibotClient::SendCommand(-125,-125,flags)
	
	}
	
RobotLibrary::left(){
	
	WifibotClient::SendCommand(-125,125,flags)
	
	}
	
RobotLibrary::right(){
	
	WifibotClient::SendCommand(125,-125,flags)
	
	}
	
RobotLibrary::stop(){
	
	WifibotClient::SendCommand(0,0,flags)
	
	}
	