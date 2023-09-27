#pragma once
#include <stdint.h>
#include <vector>
#include <queue>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>

typedef uint32_t SyncID;
typedef void(*SyncFunc)(SyncID id,uintptr_t data);

class SyncUtils {
private:
	static void* _hinstance;
	static uint32_t _cls_atom;
	static void* _handle;
	static SyncID _maxsyncid;
public:
	enum {
		SYNC_ID_MIN = 0x1000,
		SYNC_ID_MAX = 0x7FFF
	};
	static bool Initialize(void* hinstance);
	static bool Release();
	static SyncID NewSyncId();
	static int Send(SyncFunc callback, SyncID id, uintptr_t data);
	static int Post(SyncFunc callback, SyncID id, uintptr_t data);
	static int __stdcall Relay(void* handle, SyncID id, SyncFunc callback, uintptr_t data);
};

typedef void(*TimerFunc)(uintptr_t param);

class Timer{
private:
	static SyncID _syncid;
	static int64_t _frequency;
	static int64_t _start;
	static std::thread* _thread;
	static std::mutex _mutex;
	static std::mutex _sync_mutex;
	static void* _event;
	static std::atomic<bool> _exit;
	static std::vector<Timer*> _task_list;
	static std::vector<Timer*> _sync_list;
	static Timer* _expred;

	double _time;
	float _timeout;
	float _elapsed;
	float _extra;
	bool _periodic,_pause,_resume,_syncing;
	TimerFunc _func;
	uintptr_t _param;
	uint32_t _count;

	static void Initialize();
	static void Release();
	static void ThreadFunc();
	static void SyncFunc(SyncID /*syncid*/, uintptr_t /*data*/);
	static void RemoveTask(Timer* timer);
	static void RemoveSyncTask(Timer* timer);
public:
	static Timer* GetExpiredTimer();
	static double GetGlobalTime();

	Timer();
	~Timer();
	Timer(const Timer& timer) = delete;
	Timer(Timer&& timer) = delete;
	void start(float timeout,bool periodic, TimerFunc func, uintptr_t param);
	void stop();
	void pause();
	void resume();
	float timeout();
	float elapsed();
	float remaining();
	uint32_t count();
};


struct TriggerEvent {
	uint32_t eventid;
	uintptr_t target;
	uintptr_t data;
};

typedef bool(*TriggerCondition)();
typedef void(*TriggerAction)();

class Trigger {
private:
	typedef std::unordered_map<uintptr_t, uintptr_t> TargetMap;
	typedef std::unordered_map<int, TargetMap> EventMap;

	static SyncID _syncid;
	static TriggerEvent _event;
	static Trigger* _trigger;
	static std::queue<TriggerEvent> _sync_events;
	static std::mutex _sync_mutex;
	static std::unordered_map<Trigger*, bool> _trigger_map;

	// (eventid,(target,data))
	EventMap _events;
	TriggerCondition _condition_func;
	TriggerAction _action_func;

	static void Initialize();
	static void Release();
	static void SyncFunc(SyncID /*syncid*/, uintptr_t /*data*/);
public:
	static bool SetEvent(uint32_t eventid, uintptr_t target, uintptr_t data);
	static uint32_t ReplyEvent(uint32_t eventid, uintptr_t target, uintptr_t data);

	Trigger();
	~Trigger();
	// target==0 则触发任何eventid的事件
	bool RegisterEvent(uint32_t eventid,uintptr_t target, uintptr_t param);
	bool RemoveEvent(uint32_t eventid);
	bool RemoveEventTarget(uint32_t eventid, uintptr_t target);
	void ClearEvents();

	bool CheckEvent(uint32_t eventid, uintptr_t target);

	bool IsExistEvent(uint32_t eventid);
	bool IsEventExistTarget(uint32_t eventid, uintptr_t target);
	//不安全指针。
	TargetMap* GetEventTargets(uint32_t eventid);
	uintptr_t GetEventParam(uint32_t eventid, uintptr_t target);

	void AddCondition(TriggerCondition conditionfunc);
	void RemoveCondition();
	void AddAction(TriggerAction actionfunc);
	void RemoveAction();

	bool Evaluate();
	void Execute();
	bool ConditionalExecute();

	void SetEnable(bool enable);
	bool IsEnable();

	Trigger* GetTriggeringTrigger();
	TriggerEvent& GetTriggeringEvent();
};


