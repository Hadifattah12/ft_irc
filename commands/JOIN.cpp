#include "../includes/Server.hpp"

int Server::SplitJoin(std::string cmd, int fd, std::string &channel, std::string &pass)
{
    std::istringstream iss(cmd);
    std::vector<std::string> tokens;
    std::string word;
    while (iss >> word)
        tokens.push_back(word);
    if (tokens.size() < 2) {
        return 0;
    }
    std::string Channels = tokens[1];
    if (tokens.size() > 2) {
        pass = tokens[2];
    }
    size_t pos = Channels.find(',');
    channel = (pos == std::string::npos) ? Channels : Channels.substr(0, pos);
    if (channel[0] != '#') {
        senderror(403, GetClient(fd)->GetNickName(), channel, GetClient(fd)->GetFd(), " :No such channel\r\n");
        return 2;
    }
    channel = channel.substr(1); 
    return 1;
}

int Server::SearchForClients(std::string nickname)
{
    int count = 0;
    for (size_t i = 0; i < this->channels.size(); i++) {
        if (this->channels[i].GetClientInChannel(nickname))
            count++;
    }
    return count;
}

bool IsInvited(Client *cli, std::string ChName, int flag){
	if(cli->GetInviteChannel(ChName)){
		if (flag == 1)
			cli->RmChannelInvite(ChName);
		return true;
	}
	return false;
}

void Server::ExistCh(std::string channel, int fd, std::string pass)
{
    Client *cli = GetClient(fd);
    if (SearchForClients(cli->GetNickName()) >= 10) {
        senderror(405, cli->GetNickName(), cli->GetFd(), " :You have joined too many channels\r\n");
        return;
    }
    for (size_t i = 0; i < channels.size(); i++) {
        if (channels[i].GetName() == channel) {
            if (channels[i].GetClientInChannel(cli->GetNickName()))
                return;

            if (!channels[i].GetPassword().empty() && channels[i].GetPassword() != pass) {
                senderror(475, cli->GetNickName(), "#" + channel, cli->GetFd(), " :Cannot join channel (+k) - bad key\r\n");
                return;
            }

            if (channels[i].GetInvitOnly() && !IsInvited(cli, channel, 1)) {
                senderror(473, cli->GetNickName(), "#" + channel, cli->GetFd(), " :Cannot join channel (+i)\r\n");
                return;
            }

            if (channels[i].GetLimit() && channels[i].GetClientsNumber() >= channels[i].GetLimit()) {
                senderror(471, cli->GetNickName(), "#" + channel, cli->GetFd(), " :Cannot join channel (+l)\r\n");
                return;
            }

            channels[i].add_client(*cli);
            _sendResponse(RPL_JOINMSG(cli->getHostname(), cli->getIpAdd(), channel) +
                          RPL_NAMREPLY(cli->GetNickName(), channel, channels[i].clientChannel_list()) +
                          RPL_ENDOFNAMES(cli->GetNickName(), channel), fd);
            channels[i].sendTo_all(RPL_JOINMSG(cli->getHostname(), cli->getIpAdd(), channel), fd);
            return;
        }
    }
    Channel newChannel;
    newChannel.SetName(channel);
    newChannel.add_admin(*cli);
    newChannel.set_createiontime();
    channels.push_back(newChannel);
    _sendResponse(RPL_JOINMSG(cli->getHostname(), cli->getIpAdd(), newChannel.GetName()) +
        RPL_NAMREPLY(cli->GetNickName(), newChannel.GetName(), newChannel.clientChannel_list()) +
            RPL_ENDOFNAMES(cli->GetNickName(), newChannel.GetName()), fd);
}

void Server::JOIN(std::string cmd, int fd)
{
    std::string channel, pass;
    int flag = SplitJoin(cmd, fd, channel, pass);
    if (flag == 0) {
        senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n");
        return;
    }
    else if(flag == 1)
        ExistCh(channel, fd, pass);
}
