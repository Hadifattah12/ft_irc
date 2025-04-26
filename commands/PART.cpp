#include "../includes/Server.hpp"

std::string SplitCmdPR(std::string &cmd, std::vector<std::string> tmp)
{
	std::string reason;
	if(tmp.size() > 2){
		size_t pos = cmd.find(':');
		if(pos != std::string::npos)
			reason = cmd.substr(pos);
		else
			reason = tmp[2];
	}
	return reason;
}

int Server::SplitCmdPart(std::string cmd, std::vector<std::string> tmp, std::string &reason, int fd,std::string &channell)
{
	reason = SplitCmdPR(cmd, tmp);
	if(tmp.size() < 2) {
		tmp.clear(); 
		return 0;
	}
	channell = tmp[1];
	tmp.clear();
	if (reason[0] == ':') 
		reason.erase(reason.begin());
	if (channell[0] == '#')
		channell.erase(channell.begin());
	else{
		senderror(403, GetClient(fd)->GetNickName(), channell, GetClient(fd)->GetFd(), " :No such channel\r\n"); 
		return 0;
		}
	return 1;
}

void Server::PART(std::string cmd, int fd)
{
	std::vector<std::string> tmp = split_cmd(cmd);
	bool flag = 0;
	std::string reason,channell;
	if (!SplitCmdPart(cmd, tmp, reason, fd,channell)){
			senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n"); 
			return;
		}
		for (size_t j = 0; j < this->channels.size(); j++){
			if (this->channels[j].GetName() == channell){
				flag = 1;
				if (!channels[j].get_client(fd) && !channels[j].get_admin(fd))
					{senderror(442, GetClient(fd)->GetNickName(), "#" + channell, GetClient(fd)->GetFd(), " :You're not on that channel\r\n"); continue;}
					std::stringstream ss;
					ss << ":" << GetClient(fd)->GetNickName() << "!~" << GetClient(fd)->GetUserName() << "@" << "localhost" << " PART #" << channell;
					if (!reason.empty())
						ss << " :" << reason << "\r\n";
					else ss << "\r\n";
					channels[j].sendTo_all(ss.str());
					if (channels[j].get_admin(channels[j].GetClientInChannel(GetClient(fd)->GetNickName())->GetFd())){
						channels[j].remove_admin(channels[j].GetClientInChannel(GetClient(fd)->GetNickName())->GetFd());
						if (channels[j].GetAdminCount() == 0)
						{
							bool promoted = false;
							for (int fd_candidate = 0; fd_candidate < 1024 && !promoted; ++fd_candidate)
							{
								Client* candidate = channels[j].get_client(fd_candidate);
								if (candidate)
								{
									std::string new_admin_nick = candidate->GetNickName();
									channels[j].change_clientToAdmin(new_admin_nick);
									std::string notice = ":" + new_admin_nick + "!~" + candidate->GetUserName() + "@localhost MODE #" + channels[j].GetName() + " +o " + new_admin_nick + "\r\n";
									channels[j].sendTo_all(notice);
									promoted = true;
								}
							}
						}
					}else
						channels[j].remove_client(channels[j].GetClientInChannel(GetClient(fd)->GetNickName())->GetFd());
					if (channels[j].GetClientsNumber() == 0)
						channels.erase(channels.begin() + j);
			}
		}
		if (!flag)
			senderror(403, GetClient(fd)->GetNickName(), "#" + channell , GetClient(fd)->GetFd(), " :No such channel\r\n");
	}