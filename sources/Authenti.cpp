#include "../includes/Server.hpp"

void Server::client_authen(int fd, std::string cmd){
	Client *cli = GetClient(fd);
	std::vector<std::string> command = split_cmd(cmd);
	if(command.size() >= 2)
		cmd = cmd.substr(4);
	else
	{
		_sendResponse(ERR_NOTENOUGHPARAM(std::string("*")), fd);
		return ;
	}
	size_t position = cmd.find_first_not_of(" \t\n\r\f\v");
	size_t posend = cmd.find_last_not_of(" \t\n\r\f\v");
	cmd = cmd.substr(position, posend - position + 1);
	if(!cmd.empty() && cmd[0] == ':')
		cmd.erase(cmd.begin());
	if(cmd.empty())
	{
		_sendResponse(ERR_NOTENOUGHPARAM(std::string("*")), fd);
		return ;
	}
	if(!cli->getRegistered())
	{
		if(cmd == password)
			cli->setRegistered(true);
		else
			_sendResponse(ERR_INCORPASS(std::string("*")), fd);
	}
	else
         _sendResponse(ERR_ALREADYREGISTERED(GetClient(fd)->GetNickName()), fd);
}

bool Server::is_validNickname(std::string& nickname)
{
	if(!nickname.empty() && (nickname[0] == '&' || nickname[0] == '#' || nickname[0] == ':'))
		return false;
	for(size_t i = 1; i < nickname.size(); i++)
	{
		if(!std::isalnum(nickname[i]) && nickname[i] != '_')
			return false;
	}
	return true;
}


bool Server::nickNameInUse(std::string& nickname)
{
	for (size_t i = 0; i < this->clients.size(); i++)
	{
		if (this->clients[i].GetNickName() == nickname)
			return true;
	}
	return false;
}

void Server::set_nickname(std::string cmd, int fd) {
    cmd = cmd.substr(4);
    size_t pos = cmd.find_first_not_of("\t\v ");
	size_t posend = cmd.find_last_not_of("\t\v ");
    if (pos == std::string::npos || cmd.empty()) {
        _sendResponse(ERR_NOTENOUGHPARAM(std::string("*")), fd);
        return;
    }
    cmd = cmd.substr(pos,posend - pos + 1);
    if (cmd[0] == ':') {
        cmd = cmd.substr(1);
        size_t new_pos = cmd.find_first_not_of(" \t\v\r\n");
        if (new_pos != std::string::npos)
            cmd = cmd.substr(new_pos);
    }
	if (cmd.empty()) {
		_sendResponse(ERR_NOTENOUGHPARAM(std::string("*")), fd);
		return;
	}
    Client* cli = GetClient(fd);
    if (!is_validNickname(cmd)) {
		
        _sendResponse(ERR_ERRONEUSNICK(cmd), fd);
        return;
    }
    if (nickNameInUse(cmd) && cli->GetNickName() != cmd) {
        _sendResponse(ERR_NICKINUSE(cmd), fd);
        return;
    }
    std::string oldnick = cli->GetNickName();
    cli->SetNickname(cmd);
    for (size_t i = 0; i < channels.size(); i++) {
        Client* cl = channels[i].GetClientInChannel(oldnick);
        if (cl) 
			cl->SetNickname(cmd);
    }
    if (!oldnick.empty() && oldnick != cmd) {
        _sendResponse(RPL_NICKCHANGE(oldnick, cmd), fd);
		for(size_t i = 0; i < channels.size(); i++){
			channels[i].sendTo_all(RPL_NICKCHANGE(oldnick, cmd),fd);
		}
    }
    if (cli->getRegistered() && !cli->GetUserName().empty() && !cli->GetLogedIn()) {
        cli->setLogedin(true);
        _sendResponse(RPL_CONNECTED(cli->GetNickName()), fd);
    }
}

void	Server::set_username(std::string& cmd, int fd)
{
	std::vector<std::string> splited_cmd = split_cmd(cmd);
	std::string realName;
	Client *cli = GetClient(fd); 
	if((cli && splited_cmd.size() < 5))
		{_sendResponse(ERR_NOTENOUGHPARAM(cli->GetNickName()), fd); return; }
	if(!cli  || !cli->getRegistered())
		{_sendResponse(ERR_NOTREGISTERED(std::string("*")), fd); return;}
	else if (cli && !cli->GetUserName().empty())
		{_sendResponse(ERR_ALREADYREGISTERED(cli->GetNickName()), fd); return;}
	if(splited_cmd.size() == 5){
	if(splited_cmd[2] != "0" || splited_cmd[3] != "*" || splited_cmd[4][0] != ':'){
		_sendResponse(ERR_NOTENOUGHPARAM(cli->GetNickName()), fd);
			return;}
	}else
	if(splited_cmd[2] != "0" || splited_cmd[3] != "*" || splited_cmd[4] != ":")
	{
		_sendResponse(ERR_NOTENOUGHPARAM(cli->GetNickName()), fd);
		 return; 
	}
	if(splited_cmd.size() == 5)
		realName = splited_cmd[4].substr(1);
	else
		realName = splited_cmd[5];
	if(!is_validNickname(splited_cmd[1]) || realName.empty())
	{
		_sendResponse(ERR_ERRONEUSUSERNAME(cli->GetNickName()), fd);
        return;
	}
	cli->SetUsername(splited_cmd[1]);
	if(cli && cli->getRegistered() && !cli->GetUserName().empty() && !cli->GetNickName().empty() && cli->GetNickName() != "*"  && !cli->GetLogedIn())
	{
		cli->setLogedin(true);
		_sendResponse(RPL_CONNECTED(cli->GetNickName()), fd);
	}
}