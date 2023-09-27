// graphics.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <trigger.h>
#include <Windows.h>

Trigger* trigger = nullptr;


void timerfunc(uintptr_t data) {
	Timer* timer = Timer::GetExpiredTimer();
	if (timer != nullptr) {
		printf("时间:%lf,timer %d,timeout:%f,elapsed:%f,count:%lu\n", Timer::GetGlobalTime(), data,timer->timeout(), timer->elapsed(),timer->count());
	}
	trigger->SetEvent(1,0,123);
}
void trigger_actionfunc() {
	printf("触发了触发器事件!\n");
}
int main()
{
	trigger = new Trigger();
	trigger->RegisterEvent(1, 0, 100);
	trigger->AddAction(trigger_actionfunc);

	//trigger->RemoveEvent(1);
	Timer* timer_list[1];
	printf("开始\n");
	for (int i = 0; i <sizeof(timer_list)/4;i++) {
		timer_list[i] = new Timer();
		timer_list[i]->start(0.5, true, timerfunc, i+1);
	}
	Sleep(3000);
	timer_list[0]->start(0.02, true, timerfunc, 2);
	/*
	Sleep(5000);
	printf("暂停\n");
	for (int i = 0; i < sizeof(timer_list)/4; i++) {
		timer_list[i]->pause();
	}
	Sleep(5000);
	printf("恢复\n");
	for (int i = 0; i < sizeof(timer_list) / 4; i++) {
		timer_list[i]->resume();
	}
	*/
	Sleep(2000);
	printf("删除\n");
	for (int i = 0; i < sizeof(timer_list) / 4; i++) {
		delete timer_list[i];
	}
	Sleep(1000);
	delete trigger;
	trigger = nullptr;
    std::cout << "Hello World!\n";
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
