// TCPClient.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <cstdlib>
#include <tcp/client/client.h>
#include "TCPClient.h"

using namespace std;

int main()
{
	cout << "TCPClient" << endl;
	anp::tcp::client c;
	c.set_on_receive([&](auto& data) {
		cout << "Data received: " << data << std::endl;
	});
	c.connect("127.0.0.1", 123);
	system("pause");
	return 0;
}
