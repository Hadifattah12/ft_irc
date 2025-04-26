#include "../includes/Server.hpp"
#include <map>

void Server::QUIT(std::string cmd, int fd)
{
	std::string reason;
	cmd = cmd.substr(4);
	std::vector<std::string> command = split_cmd(cmd);
	std::string userexited = GetClient(fd)->GetNickName();
	if (!cmd.empty()) 
	{
		if (command.size() == 1 && command[0] == ":") 
			reason = ":";
		else if (command.size() == 1 && command[0][0] == ':') 
			reason = command[0].substr(1);
		else if (command.size() > 1 && command[0] == ":") 
		{
			for (size_t i = 1; i < command.size(); i++) 
				reason += (i == 1 ? "" : " ") + command[i];
		}
		else if (command.size() > 1 && command[0][0] == ':') 
		{
			reason = command[0].substr(1);
			for (size_t i = 1; i < command.size(); i++) 
				reason += " " + command[i];
		}
		else
			reason = command[0];
	}
	else
		reason = "quit";
	for (size_t i = 0; i < channels.size(); ++i)
	{
		bool was_client = channels[i].get_client(fd);
		bool was_admin = channels[i].get_admin(fd);

		if (was_client)
			channels[i].remove_client(fd);
		if (was_admin)
			channels[i].remove_admin(fd);

		if (channels[i].GetClientsNumber() == 0)
		{
			channels.erase(channels.begin() + i);
			--i;
			continue;
		}
		else
		{
			std::string rpl = ":" + GetClient(fd)->GetNickName() + "!~" + GetClient(fd)->GetUserName() + "@localhost QUIT :" + reason + "\r\n";
			channels[i].sendTo_all(rpl);
			if (channels[i].GetAdminCount() == 0)
			{
				bool promoted = false;
				for (int newAdmin = 0; newAdmin < 1024 && !promoted; ++newAdmin)
				{
					Client* candidate = channels[i].get_client(newAdmin);
					if (candidate)
					{
						std::string new_admin_nick = candidate->GetNickName();
						channels[i].change_clientToAdmin(new_admin_nick);
						std::string notice = ":" + userexited + "!~" + candidate->GetUserName() + "@localhost MODE #" + channels[i].GetName() + " +o " + new_admin_nick + "\r\n";
						channels[i].sendTo_all(notice);
						promoted = true;
					}
				}
			}
		}
	}

	std::cout << RED << "Client <" << fd << "> Disconnected" << WHI << std::endl;
	RmChannels(fd);
	RemoveClient(fd);
	RemoveFds(fd);
	close(fd);
}
