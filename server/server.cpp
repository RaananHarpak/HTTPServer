//Server
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include <chrono>
#include <sstream> 
using namespace std;

#define TIME_PORT 27015

void main()
{
	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Server: Error at WSAStartup()\n";
		return;
	}
	SOCKET m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_socket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}
	sockaddr_in serverService;
	serverService.sin_family = AF_INET;
	serverService.sin_addr.s_addr = INADDR_ANY;	//inet_addr("127.0.0.1");
	serverService.sin_port = htons(TIME_PORT);
	if (SOCKET_ERROR == bind(m_socket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(m_socket);
		WSACleanup();
		return;
	}
	sockaddr client_addr;
	int client_addr_len = sizeof(client_addr);
	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[2];
	int res;
	cout << "Time Server: Wait for clients' requests.\n";
	while (true)
	{
		bytesRecv = recvfrom(m_socket, recvBuff, 2, 0, &client_addr, &client_addr_len);
		recvBuff[1] = '\0';
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}
		recvBuff[bytesRecv] = '\0'; 
		cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";
		/*
		MENU:
		1.GetTime
		2.GetTimeWithoutDate
		3.GetTimeSinceEpoch
		4.GetClientToServerDelayEstimation
		5.MeasureRTT
		6.GetTimeWithoutDateOrSeconds
		7.GetYear
		8.GetMonthAndDay
		9.GetSecondsSinceBeginingOfMonth
		10.Getweek_year
		11.GetDaylightSavings
		12.GetTimeWithoutDateInCity
		13.MeasureTimeLap
		14.Exit";
		*/
		time_t timer;
		time(&timer);
		string DateTime = ctime(&timer);
		int daysCounter = stoi(DateTime.substr(8, 2));
		int hoursCounter = stoi(DateTime.substr(11, 2));
		int minutesCounter = stoi(DateTime.substr(14, 2));
		int secondsCounter = stoi(DateTime.substr(17, 2));
		struct tm* local;
		char week_year[3];
		local = localtime(&timer);

		string result2;
		static struct tm last_time_enter;
		struct tm* curr_time_enter = localtime(&timer);
		static bool if_time_started = false;
		int city, min_passed, sec_passed;
		time_t localTime;
		localTime = time(NULL);
		struct tm* local_time_struct, *city_time = gmtime(&localTime);
		stringstream result_city;
		char month_year[3];
		char day_month[3];
		float avg=0, avg_result = 0;
	
		switch (recvBuff[0])
		{
		case 'A':
			strcpy(sendBuff, ctime(&timer));
			sendBuff[strlen(sendBuff)] = '\0';
			break;

		case 'B':
			strcpy(sendBuff, DateTime.substr(11, 8).c_str());
			sendBuff[strlen(sendBuff)] = '\0';
			break;

		case 'C':
			strcpy(sendBuff, (to_string(time(nullptr))).c_str());
			sendBuff[strlen(sendBuff)] = '\0';
			break;

		case 'D':
			strcpy(sendBuff, (to_string(GetTickCount())).c_str());
			sendBuff[strlen(sendBuff)] = '\0';
			break;

		case 'E':
			strcpy(sendBuff, ctime(&timer));
			sendBuff[strlen(sendBuff)] = '\0';
			break;
		case 'F':
			strcpy(sendBuff, DateTime.substr(11, 5).c_str());
			sendBuff[strlen(sendBuff)] = '\0';
			break;

		case 'G':
			strcpy(sendBuff, DateTime.substr(20, 4).c_str());
			sendBuff[strlen(sendBuff)] = '\0';
			break;

		case 'H':
			strcpy(sendBuff, DateTime.substr(4, 6).c_str());
			sendBuff[strlen(sendBuff)] = '\0';
			break;

		case 'I':
			res = hoursCounter * 3600 + minutesCounter * 60 + secondsCounter + daysCounter * 86400;
			strcpy(sendBuff, (to_string(res)).c_str());
			sendBuff[strlen(sendBuff)] = '\0';
			break;

		case 'J':
			strcpy(sendBuff, to_string(strftime(week_year, 3, "%U", local)).c_str());
			sendBuff[strlen(sendBuff)] = '\0';
			break;

		case 'K':
			local_time_struct = localtime(&timer);
			strftime(month_year, 3, "%m", local_time_struct);
			strftime(day_month, 3, "%d", local_time_struct);
			if ((stoi(month_year) > 3 && stoi(month_year) < 10) || (stoi(month_year) == 3 && stoi(day_month) > 24) || (stoi(month_year) == 10 && stoi(day_month) < 30))
				result2 = "1";
			else
				result2 = "0";
			strcpy(sendBuff, result2.c_str());
			sendBuff[strlen(sendBuff)] = '\0';
				break;
			
		case 'M'://case 13
				if (!if_time_started || curr_time_enter->tm_year > last_time_enter.tm_year || curr_time_enter->tm_mon > last_time_enter.tm_mon|| curr_time_enter->tm_mday > last_time_enter.tm_mday || curr_time_enter->tm_hour > last_time_enter.tm_hour ||curr_time_enter->tm_min > last_time_enter.tm_min + 3 || (curr_time_enter->tm_min == last_time_enter.tm_min + 3 && curr_time_enter->tm_sec > last_time_enter.tm_sec))
				{
					last_time_enter = *curr_time_enter;
					if_time_started = true;
					result2 =  "time started";
				}
				else if(if_time_started)
				{
					min_passed = (curr_time_enter->tm_min * 60 + curr_time_enter->tm_sec - (last_time_enter.tm_min * 60 + last_time_enter.tm_sec))/60;
					sec_passed = (curr_time_enter->tm_min * 60 + curr_time_enter->tm_sec - (last_time_enter.tm_min * 60 + last_time_enter.tm_sec))%60;
					result2 = to_string(min_passed) + ":" + to_string(sec_passed);
					if_time_started = false;
				}
				strcpy(sendBuff,result2.c_str());
				sendBuff[strlen(sendBuff)] = '\0';
				break;
			
		case 'N'://DohaTIME
				city = 3;
				result_city << (city_time->tm_hour += city) % 24 << ":" << city_time->tm_min << ":" << city_time->tm_sec;
				strcpy(sendBuff, (result_city.str()).c_str());
				sendBuff[strlen(sendBuff)] = '\0';
				break;
			
		case 'O'://PragueTIME
				city = 1;
				result_city << (city_time->tm_hour += city) % 24 << ":" << city_time->tm_min << ":" << city_time->tm_sec;
				strcpy(sendBuff, (result_city.str()).c_str());
				sendBuff[strlen(sendBuff)] = '\0';
				break;
			
		case 'P'://New YorkTIME
				city = -5;
				result_city << (city_time->tm_hour += city) % 24 << ":" << city_time->tm_min << ":" << city_time->tm_sec;
				strcpy(sendBuff, (result_city.str()).c_str());
				sendBuff[strlen(sendBuff)] = '\0';
				break;
			
		case 'Q'://BerlinTIME
				city = 1;
				result_city << (city_time->tm_hour += city) % 24 << ":" << city_time->tm_min << ":" << city_time->tm_sec;
				strcpy(sendBuff, (result_city.str()).c_str());
				sendBuff[strlen(sendBuff)] = '\0';
				break;
			
		case 'R'://UTCTIME
				city = 0;
				result_city << (city_time->tm_hour += city) % 24 << ":" << city_time->tm_min << ":" << city_time->tm_sec;
				strcpy(sendBuff, (result_city.str()).c_str());
				sendBuff[strlen(sendBuff)] = '\0';
				break;
			default:
				break;
		}
  
		bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&client_addr, client_addr_len);
		if (SOCKET_ERROR == bytesSent)
			{
				cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
				closesocket(m_socket);
				WSACleanup();
				return;
			}
		cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
	}

	// Closing connections and Winsock.
	cout << "Time Server: Closing Connection.\n";
	closesocket(m_socket);
	WSACleanup();
}