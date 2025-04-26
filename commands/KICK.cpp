#include "../includes/Server.hpp"

std::string Server::SplitCmdKick(std::string cmd, std::vector<std::string> &tmp, std::string &user, int fd)
{
	std::string reason;
	std::string splited_channel;
	std::vector<std::string> command = split_cmd(cmd);
	std::string channels;
	if(command.size() < 3)
		return std::string("");
	size_t pos = cmd.find(":");
	if(pos != std::string::npos)
		reason = cmd.substr(pos + 1);
	else if(command.size()  > 3)
		reason = command[3];
	channels = command[1];
	user = command[2];
	tmp.clear();
	for (size_t i = 0; i < channels.size(); i++){
		if (channels[i] == ','){
				tmp.push_back(splited_channel); 
				splited_channel.clear();
			}else 
				splited_channel += channels[i];
	}
	tmp.push_back(splited_channel);
	for (size_t i = 0; i < tmp.size(); i++)
		{if (tmp[i].empty())tmp.erase(tmp.begin() + i--);}
	for (size_t i = 0; i < tmp.size(); i++){
			if (*(tmp[i].begin()) == '#')
				tmp[i].erase(tmp[i].begin());
			else
				{senderror(403, GetClient(fd)->GetNickName(), tmp[i], GetClient(fd)->GetFd(), " :No such channel\r\n"); tmp.erase(tmp.begin() + i--);}
		}
	return reason;
}

void Server::KICK(std::string cmd, int fd)
{
	std::vector<std::string> tmp;
	std::string user, reason;
	reason = SplitCmdKick(cmd, tmp, user, fd);

	if (user.empty()) {
		senderror(461, GetClient(fd)->GetNickName(), GetClient(fd)->GetFd(), " :Not enough parameters\r\n");
		return;
	}

	for (size_t i = 0; i < tmp.size(); ++i)
	{
		Channel *ch = GetChannel(tmp[i]);
		if (!ch) {
			senderror(403, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :No such channel\r\n");
			continue;
		}

		if (!ch->get_client(fd) && !ch->get_admin(fd)) {
			senderror(442, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :You're not on that channel\r\n");
			continue;
		}

		if (!ch->get_admin(fd)) {
			senderror(482, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :You're not channel operator\r\n");
			continue;
		}

		Client *target = ch->GetClientInChannel(user);
		if (!target) {
			senderror(441, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :They aren't on that channel\r\n");
			continue;
		}

		// Prevent kicking last admin
		if (ch->get_admin(target->GetFd()) && ch->GetAdminCount() == 1) {
			senderror(482, GetClient(fd)->GetNickName(), "#" + tmp[i], GetClient(fd)->GetFd(), " :Cannot remove last admin\r\n");
			continue;
		}

		std::stringstream ss;
		ss << ":" << GetClient(fd)->GetNickName() << "!~" << GetClient(fd)->GetUserName()
		   << "@localhost KICK #" << tmp[i] << " " << user;

		if (!reason.empty())
			ss << " :" << reason << "\r\n";
		else
			ss << "\r\n";

		ch->sendTo_all(ss.str());

		if (ch->get_admin(target->GetFd()))
			ch->remove_admin(target->GetFd());
		else
			ch->remove_client(target->GetFd());

		if (ch->GetClientsNumber() == 0)
			channels.erase(channels.begin() + i--);
	}
}