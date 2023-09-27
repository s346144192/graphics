#include <trigger.h>
#include <windows.h>

//#pragma comment(lib,"Winmm.lib")


void* SyncUtils::_hinstance = nullptr;
uint32_t SyncUtils::_cls_atom=0;
void* SyncUtils::_handle=nullptr;
uint32_t SyncUtils::_maxsyncid = SyncUtils::SYNC_ID_MIN;

bool SyncUtils::Initialize(void* hinstance) {
	if (!_cls_atom) {
		WNDCLASSEX  winclass;
		winclass.cbSize = sizeof(WNDCLASSEX);
		winclass.style = CS_VREDRAW | CS_HREDRAW;
		winclass.lpfnWndProc = (WNDPROC)Relay;
		winclass.cbClsExtra = 0;
		winclass.cbWndExtra = 0;
		winclass.hInstance = (HINSTANCE)hinstance;
		winclass.hIcon = nullptr;
		winclass.hCursor = nullptr;
		winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		winclass.lpszMenuName = NULL;
		winclass.lpszClassName = TEXT("Winclass_SyncUtils");
		winclass.hIconSm = nullptr;
		_cls_atom =RegisterClassEx(&winclass);
	}
	if (_cls_atom!=0 && !_handle) {
		if (!hinstance) {
			hinstance = GetModuleHandle(NULL);
		}
		_hinstance = hinstance;
		_handle = (void*)CreateWindow("Winclass_SyncUtils",nullptr,NULL,0,0,0,0, HWND_MESSAGE,0, (HINSTANCE)_hinstance,0);
	}
	return _cls_atom && _handle;
}
bool SyncUtils::Release() {
	bool result = true;
	if (_cls_atom) {
		result = UnregisterClass(TEXT("Winclass_SyncUtils"), (HINSTANCE)_hinstance);
		_cls_atom = 0;
	}
	if (_handle) {
		result &= DestroyWindow((HWND)_handle);
		_handle = nullptr;
	}
	return result;
}
uint32_t SyncUtils::NewSyncId() {
	if (_maxsyncid < SYNC_ID_MAX) {
		return ++_maxsyncid;
	}
	return SYNC_ID_MIN;
}
int SyncUtils::Send( SyncFunc callback, SyncID id, uintptr_t data) {
	SyncUtils::Initialize(NULL);
	if ((!_handle || !callback) || (id < SYNC_ID_MIN & id > SYNC_ID_MAX)) {
		return 0;
	}
	return ::SendMessage((HWND)_handle, id, (WPARAM)callback,data);
}
int SyncUtils::Post(SyncFunc callback, SyncID id, uintptr_t data){
	SyncUtils::Initialize(NULL);
	if ((!_handle || !callback) || (id < SYNC_ID_MIN & id > SYNC_ID_MAX)) {
		return 0;
	}
	return ::PostMessage((HWND)_handle, id, (WPARAM)callback, data);
}
int __stdcall SyncUtils::Relay(void* handle, SyncID id, SyncFunc callback, uintptr_t data) {
	if (id>= SYNC_ID_MIN & id<= SYNC_ID_MAX) {
		if (callback) {
			callback(id,data);
		}
		return TRUE;
	}
	return DefWindowProc((HWND)handle,id, (WPARAM)callback,data);
}

SyncID Timer::_syncid=0;
int64_t Timer::_frequency =0;
int64_t Timer::_start=0;
std::thread* Timer::_thread=nullptr;
std::mutex Timer::_mutex;
std::mutex Timer::_sync_mutex;
void* Timer::_event=nullptr;
std::atomic<bool> Timer::_exit=false;
std::vector<Timer*> Timer::_task_list;
std::vector<Timer*> Timer::_sync_list;
Timer* Timer::_expred=nullptr;

Timer::Timer() {
	_time=0.00;
	_timeout=0.0f;
	_elapsed=0.0f;
	_extra = 0.0f;
	_periodic = false;
	_pause=false;
	_func=nullptr;
	_param =0;
}
Timer::~Timer() {
	stop();
	_time = 0.00;
	_timeout = 0.0f;
	_elapsed = 0.0f;
	_extra = 0.0f;
	_periodic = false;
	_pause = false;
	_func = nullptr;
	_param = 0;
}

void Timer::Initialize() {
	if (!_syncid) {
		_syncid = SyncUtils::NewSyncId();
	}
	if (!_event) {
		_event = (void*)CreateEvent(NULL,FALSE,FALSE,NULL);
	}
	 if (!_frequency) {
		 QueryPerformanceFrequency((LARGE_INTEGER*)&_frequency);
		 QueryPerformanceCounter((LARGE_INTEGER*)&_start);
	 }
	 if (!_thread) {
		 _exit = false;
		 _thread = new std::thread(ThreadFunc);
	 }
}
void Timer::Release() {

	if(_thread!=nullptr)
	{
		_exit = true;
		SetEvent((HANDLE)_event);
		_thread->join();
		delete _thread;
		_thread = nullptr;
	}
	if (_event) {
		CloseHandle((HANDLE)_event);
		_event = nullptr;
	} 
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_task_list.clear();
	}
	{
		std::lock_guard<std::mutex> lock(_sync_mutex);
		_sync_list.clear();
	}
}
void Timer::ThreadFunc() {
	while (!_exit) {
		float min_time_remaining = 60.0;
		bool reqsync = false;
		{
			std::lock_guard<std::mutex> lock(_mutex);
			std::unique_lock<std::mutex> sync_lock(_sync_mutex,std::defer_lock);
			LARGE_INTEGER current = {0};
			QueryPerformanceCounter(&current);
			double time = (double)current.QuadPart / (double)_frequency;
			Timer* timer=nullptr;
			for (auto it = _task_list.begin(); it != _task_list.end(); ) {
				timer = *it;
				if (!timer) {
					it = _task_list.erase(it);
					continue;
				}
				if (!timer->_pause && !timer->_syncing) {
					if (timer->_resume) {
						timer->_resume = false;
						timer->_time = time - timer->_elapsed;
					}
					timer->_elapsed = time - timer->_time + timer->_extra;
					if (timer->_elapsed >= timer->_timeout) {

						timer->_count++;

						timer->_extra = timer->_elapsed - timer->_timeout;
						if (timer->_extra > timer->_timeout) {
							timer->_extra = timer->_timeout;
						}
						else if(timer->_extra < 0) {
							timer->_extra = 0;
						}
						timer->_time = time;
						timer->_elapsed = timer->_timeout;

						sync_lock.lock();
						_sync_list.push_back(timer);
						timer->_syncing = true;
						sync_lock.unlock();

						reqsync = true;

						if (timer->_periodic) {
							if (timer->_timeout < min_time_remaining) {
								min_time_remaining = timer->_timeout- timer->_extra;
							}
						}
						else {
							it = _task_list.erase(it);
							continue;
						}
					}
					else {
						if (timer->_timeout - timer->_elapsed < min_time_remaining) {
							min_time_remaining = timer->_timeout - timer->_elapsed;
						}
					}
				}
				++it;
			}
		}
		if (reqsync) {
			SyncUtils::Send(SyncFunc, _syncid, 0);
		}
		DWORD sleep_time = (DWORD)(min_time_remaining * 1000);
		WaitForSingleObject((HANDLE)_event, sleep_time);
	}
}
void Timer::SyncFunc(SyncID /*syncid*/, uintptr_t /*data*/) {
	std::unique_lock<std::mutex> lock(_sync_mutex);
	_expred = nullptr;
	for (size_t i = 0; i < _sync_list.size(); i++) {
		_expred = _sync_list[i];
		if (_expred != nullptr) {
			lock.unlock();
			if (_expred->_func != nullptr) {
				_expred->_func(_expred->_param);
			}
			_expred->_syncing = false;
			lock.lock();
		}
	}
	_expred = nullptr;
	_sync_list.clear();
}
void Timer::RemoveTask(Timer* timer) {
	std::lock_guard<std::mutex> lock(_mutex);
	for (auto it = _task_list.begin(); it != _task_list.end(); ++it) {
		if (timer == (*it)) {
			_task_list.erase(it);
			break;
		}
	}
}
void Timer::RemoveSyncTask(Timer* timer) {
	std::lock_guard<std::mutex> lock(_sync_mutex);
	for (auto it = _sync_list.begin(); it != _sync_list.end(); ++it) {
		if (timer == (*it)) {
			timer->_syncing = false;
			_sync_list.erase(it);
			break;
		}
	}
}
Timer* Timer::GetExpiredTimer() {
	return _expred;
}
double Timer::GetGlobalTime() {
	if (!_frequency) {
		QueryPerformanceFrequency((LARGE_INTEGER*)&_frequency);
		QueryPerformanceCounter((LARGE_INTEGER*)&_start);
	}
	LARGE_INTEGER current;
	QueryPerformanceCounter(&current);
	return (double)current.QuadPart / (double)_frequency;
}


void Timer::start(float timeout, bool periodic, TimerFunc func, uintptr_t param) {
	Initialize();
	stop();
	{
		std::lock_guard<std::mutex> lock(_mutex);
		LARGE_INTEGER current;
		QueryPerformanceCounter(&current);
		_time = (double)current.QuadPart / (double)_frequency;
		_timeout = timeout;
		_elapsed = 0.0f;
		_extra = 0.0f;
		_periodic = periodic;
		_func = func;
		_param = param;
		_count = 0;
		_pause = false;
		_resume = false;
		_task_list.push_back(this);
		SetEvent((HANDLE)_event);
	}
 }
void Timer::stop() {
	RemoveTask(this);
	RemoveSyncTask(this);
}
void Timer::pause() {
	std::lock_guard<std::mutex> lock(_mutex);
	if (!_pause) {
		LARGE_INTEGER current;
		QueryPerformanceCounter(&current);
		double time = (double)current.QuadPart / (double)_frequency;
		_elapsed = time - _time;
		_pause = true;
		_resume = false;
	}
}
void Timer::resume() {
	std::lock_guard<std::mutex> lock(_mutex);
	if (_pause) {
		_pause = false;
		_resume = true;
		SetEvent((HANDLE)_event);
	}
}
float Timer::timeout() {
	std::lock_guard<std::mutex> lock(_mutex);
	return _timeout;
}
float Timer::elapsed() {
	std::lock_guard<std::mutex> lock(_mutex);
	if (_pause || _syncing) {
		return _elapsed;
	}
	LARGE_INTEGER current;
	QueryPerformanceCounter(&current);
	double time1 = (double)current.QuadPart / (double)_frequency;
	double time2 = _time;
	float elapsed0 = time1 - time2;
	if (elapsed0>_timeout) {
		return _timeout;
	}
	else {
		return elapsed0;
	}
}
float Timer::remaining() {
	return timeout() - elapsed();
}
uint32_t Timer::count() {
	std::lock_guard<std::mutex> lock(_mutex);
	return _count;
}


SyncID Trigger::_syncid=0;
TriggerEvent  Trigger::_event = { 0 };
Trigger*  Trigger::_trigger=nullptr;
std::queue<TriggerEvent> Trigger::_sync_events;
std::mutex Trigger::_sync_mutex;
std::unordered_map<Trigger*, bool> Trigger::_trigger_map;

void Trigger::Initialize() {
	if (_syncid == 0) {
		_syncid = SyncUtils::NewSyncId();
		_event = { 0,0,0 };
		_trigger = nullptr;
	}
}
void Trigger::Release() {

}
void Trigger::SyncFunc(SyncID /*syncid*/, uintptr_t /*data*/) {
	std::unique_lock<std::mutex> lock(_sync_mutex);
	TriggerEvent e = {0,0,0};
	while (!_sync_events.empty()) {
		e = _sync_events.front();
		_sync_events.pop();
		lock.unlock();
		ReplyEvent(e.eventid,e.target,e.data);
		lock.lock();
	}
}

bool Trigger::SetEvent(uint32_t eventid, uintptr_t target, uintptr_t data) {
	Initialize(); 
	{
		std::lock_guard<std::mutex> lock(_sync_mutex);
		_sync_events.push({ eventid,target,data });
	}
	SyncUtils::Send(SyncFunc, _syncid,0);
	return true;
}
uint32_t Trigger::ReplyEvent(uint32_t eventid, uintptr_t target, uintptr_t data) {
	uint32_t count = 0;
	Trigger* trigger = nullptr;
	_event = { eventid,target,data };
	for (auto& k_trig: _trigger_map) {
		trigger = k_trig.first;
		if (trigger) {
			if (trigger->CheckEvent(eventid, target)) {
				trigger->ConditionalExecute();
				count++;
			}
		}
	}
	_event = { 0,0,0 };
	return count;
}

Trigger::Trigger() {
	_condition_func = nullptr;
	_action_func = nullptr;
	_trigger_map[this] = false;
}
Trigger::~Trigger() {
	_trigger_map.erase(this);
}

bool Trigger::RegisterEvent(uint32_t eventid, uintptr_t target, uintptr_t param) {
	if (!eventid) {
		return false;
	}
	_events[eventid][target] = param;
	return true;
}
bool Trigger::RemoveEvent(uint32_t eventid) {
	const auto& it = _events.find(eventid);
	if (it != _events.end()) {
		auto target_map = &((*it).second);
		target_map->clear();
		_events.erase(it);
		return true;
	}
	return false;
}
bool Trigger::RemoveEventTarget(uint32_t eventid, uintptr_t target) {
	const auto& it = _events.find(eventid);
	if (it != _events.end()) {
		auto target_map = &((*it).second);
		target_map->erase(target);
		return true;
	}
	return false;
}
void Trigger::ClearEvents() {
	for (auto target_map:_events) {
		target_map.second.clear();
	}
	_events.clear();
}
bool Trigger::CheckEvent(uint32_t eventid, uintptr_t target) {
	const auto& it = _events.find(eventid);
	if (it != _events.end()) {
		auto target_map = &((*it).second);
		return (target_map->find(0) != target_map->end() || target_map->find(target) != target_map->end());
	}
	return false;
}
bool Trigger::IsExistEvent(uint32_t eventid){
	return _events.find(eventid) != _events.end();
}
bool Trigger::IsEventExistTarget(uint32_t eventid, uintptr_t target) {
	const auto& it = _events.find(eventid);
	if (it != _events.end()) {
		auto target_map = &((*it).second);
		return (target_map->find(target) != target_map->end());
	}
	return false;
}
Trigger::TargetMap* Trigger::GetEventTargets(uint32_t eventid) {
	const auto& it = _events.find(eventid);
	if (it != _events.end()) {
		return &((*it).second);
	}
	return nullptr;
}
uintptr_t Trigger::GetEventParam(uint32_t eventid, uintptr_t target) {
	const auto& it = _events.find(eventid);
	if (it != _events.end()) {
		auto target_map = &((*it).second);
		const auto& item = target_map->find(target);
		if (item != target_map->end()) {
			return item->second;
		}
	}
	return 0;
}
void Trigger::AddCondition(TriggerCondition conditionfunc) {
	_condition_func = conditionfunc;
}
void Trigger::RemoveCondition() {
	_condition_func = nullptr;
}

void Trigger::AddAction(TriggerAction actionfunc) {
	_action_func = actionfunc;
}
void Trigger::RemoveAction() {
	_action_func = nullptr;
}

bool Trigger::Evaluate() {
	if (!_condition_func)
		return true;
	_trigger = this;
	bool result= _condition_func();
	_trigger = nullptr;
	return result;
}
void Trigger::Execute() {
	if (!_action_func)
		return;
	_trigger = this;
	_action_func();
	_trigger = nullptr;
}
bool Trigger::ConditionalExecute() {
	if (Evaluate()) {
		Execute();
		return true;
	}
	return false;
}

void Trigger::SetEnable(bool enable) {
	_trigger_map[this] = enable;
}
bool Trigger::IsEnable() {
	return _trigger_map[this];
}

Trigger* Trigger::GetTriggeringTrigger() {
	return _trigger;
}
TriggerEvent& Trigger::GetTriggeringEvent() {
	return _event;
}

