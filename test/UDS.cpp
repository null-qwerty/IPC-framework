/*
 * @Author: Longhao.Chen <Longhao.Chen@outlook.com>
 * @Date: 2023-10-13 22:05:39
 * @LastEditors: Longhao.Chen <Longhao.Chen@outlook.com>
 * @FilePath: /IPC-framework/test/UDS.cpp
 * @Description: 测试 UDS 目录下的文件
 * Copyright (c) 2023 by Longhao.Chen, All Rights Reserved. 
 */
#include <gtest/gtest.h>
#include <string>
#include <stdlib.h>
#include <iostream>

#include <UDS.hpp>
#include <Message.hpp>
#include <ERROR.hpp>

// 测试 UDS 中的 Server 类的创建
TEST(UDS, UDSServerCreate)
{
	try {
		Transceiver::UDS::Server a("Testabc");
		Transceiver::UDS::Server *b =
			new Transceiver::UDS::Server("Testcde");
		EXPECT_TRUE(a.getName() == std::string("Testabc"));
		EXPECT_TRUE(b->getName() == std::string("Testcde"));
		delete b;
		b = new Transceiver::UDS::Server("Testcde");
		delete b;
	} catch (ERROR e) {
		std::cout << e;
	}
}

TEST(UDS, UDSClientCreate)
{
	Transceiver::UDS::Client c("abc");
	Transceiver::UDS::Client *d = new Transceiver::UDS::Client("cde");
	EXPECT_TRUE(c.getDest() == std::string("abc"));
	EXPECT_TRUE(d->getDest() == std::string("cde"));
	delete d;
}

TEST(UDS, UDSServerClient)
{
	Message::Message msg(16);
	// 这里我们分成2个进程来测试通讯功能
	if (fork() == 0) {
		Transceiver::UDS::Client c("Testaaa");
		char *a = msg.setBuf();
		for (int i = 0; i < 16; ++i) {
			a[i] = (char)i;
		}
		sleep(1);
		c.send(msg);
		exit(0);
	} else {
		Transceiver::UDS::Server s("Testaaa");
		s.receive(msg);
		bool testres = true;
		for (int i = 0; i < 16; ++i) {
			if (msg.getBuf()[i] != (char)i)
				testres = false;
		}
		wait(0);
		EXPECT_TRUE(testres);
	}
}

TEST(UDS, UDSTransceiver)
{
	int pid = fork();
	try {
		// 仍然是2个进程通讯
		if (pid == 0) {
			Transceiver::UDS::Transceiver t("Test0s", "Test1s");
			Message::Message msg(16);
			t.receive(msg);
			t.send(msg);
			SUCCEED();
		} else {
			Transceiver::UDS::Transceiver t("Test1s", "Test0s");
			Message::Message msg(16), msg1(16);
			for (int i = 0; i < 16; ++i) {
				msg.setBuf()[i] = (char)i;
			}
			// 防止子进程的服务器未生成导致抛出错误
			sleep(1);
			t.send(msg);
			t.receive(msg1);
			bool testres = true;
			for (int i = 0; i < 16; ++i) {
				if (msg1.getBuf()[i] != (char)i)
					testres = false;
			}
			EXPECT_TRUE(testres);
		}
	} catch (ERROR e) {
		std::cout << "PID = " << pid << std::endl;
		std::cout << e;
		FAIL();
	}
	if (pid == 0)
		exit(0);
	else
		wait(0);
}