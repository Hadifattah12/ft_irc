#include "../includes/Server.hpp"

std::string Server::tTopic() {
    std::time_t current = std::time(NULL);
    std::stringstream res;
    res << current;
    return res.str();
}

std::string Server::extractTopic(const std::string &cmd, std::vector<std::string> scmd) {
    size_t pos = cmd.find(":");
    if (pos != std::string::npos)
        return cmd.substr(pos + 1);
    else if (scmd.size() > 2)
        return scmd[2];
    return "";
}

void Server::Topic(std::string &cmd, int &fd) {
    std::vector<std::string> scmd = split_cmd(cmd);
    if (scmd.size() < 2) {
        senderror(461, GetClient(fd)->GetNickName(), fd, " :Not enough parameters\r\n");
        return;
    }
	std::string nmch;
	if(scmd[1][0] == '#')
		nmch = scmd[1].substr(1);
	else{
		senderror(403, "#" + nmch, fd, " :No such channel\r\n");
        return;
	}
    Channel *channel = GetChannel(nmch);

    if (!channel) {
        senderror(403, "#" + nmch, fd, " :No such channel\r\n");
        return;
    }

    if (!channel->get_client(fd) && !channel->get_admin(fd)) {
        senderror(442, "#" + nmch, fd, " :You're not on that channel\r\n");
        return;
    }

    if (scmd.size() == 2) {
 		if (GetChannel(nmch)->GetTopicName() == "")
 		{_sendResponse(": 331 " + GetClient(fd)->GetNickName() + " " + "#" + nmch + " :No topic is set\r\n", fd);return;}
         else {
            _sendResponse(": 332 " + GetClient(fd)->GetNickName() + " #" + nmch + " :" + channel->GetTopicName() + "\r\n", fd);
            _sendResponse(": 333 " + GetClient(fd)->GetNickName() + " #" + nmch + " " + channel->GetTopicSetter() + " " + channel->GetTime() + "\r\n", fd);
        }
        return;
    }

    std::string topicContent = extractTopic(cmd, scmd);
    if (topicContent.empty()) {
        channel->SetTopicName("");
        senderror(331, "#" + nmch, fd, " :No topic is set\r\n");
        return;
    }

    bool is_admin = channel->get_admin(fd);
    if (channel->Gettopic_restriction() && !is_admin) {
        senderror(482, "#" + nmch, fd, " :You're Not a channel operator\r\n");
        return;
    }

    channel->SetTopicName(topicContent);
    channel->SetTopicSetter(GetClient(fd)->GetNickName());
    channel->SetTime(tTopic());
    std::string response = ":" + GetClient(fd)->GetNickName() + "!" + GetClient(fd)->GetUserName() + "@localhost TOPIC #" + nmch + " :" + topicContent + "\r\n";
    channel->sendTo_all(response);
}