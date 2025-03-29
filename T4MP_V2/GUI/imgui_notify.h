#pragma once
#include <string>
#include <vector>

struct Notification {
	std::string username;
	float startTime;
	float duration;
	bool isDisconnect;
};

class NotificationSystem {
private:
	std::vector<Notification> activeNotifications;
public:
	void AddConnectionNotification(const char* username, float duration = 5.0f, bool disconnect = false);
	void Draw();
};