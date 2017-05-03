#ifndef _H_WIN32AUDIOMONITORTASK_
#define _H_WIN32AUDIOMONITORTASK_

enum MonitorEventType
{
	StateChanged,
	Added,
	Removed,
	DefaultDeviceChanged,
};

struct AudioMonitorEvent
{
public:
	AudioMonitorEvent(void) :
		device(nullptr), type(static_cast<MonitorEventType>(0)), state(-1)
	{
	}
	AudioMonitorEvent(device_t* d, MonitorEventType t, DWORD s = -1) :
		device(d), type(t), state(s) {}
	AudioMonitorEvent(const AudioMonitorEvent& e) :
		device(e.device), type(e.type), state(e.state) { }
	~AudioMonitorEvent(void) {}

public:
	device_t* device;
	MonitorEventType type;
	DWORD state;
};

class Win32AudioMonitorTask : public Nan::AsyncProgressWorkerBase<AudioMonitorEvent>
{
private:
	const std::size_t checkFrequencyMillis = 250;

public:
	Win32AudioMonitorTask(Win32DeviceJSImpl* impl);
	Win32AudioMonitorTask(Win32AudioMonitorTask&) = delete;
	~Win32AudioMonitorTask(void);

protected:
	void HandleOKCallback() override;
	void HandleProgressCallback(const AudioMonitorEvent* data, size_t size) override;
	void WorkComplete() override;

public:
	void Execute(const ExecutionProgress& progress) override;

public:
	void pushEvent(AudioMonitorEvent& event);
	void swapCallback(nbind::cbFunction& callback);

private:
	AudioMonitorEvent pullEvent(void);
	bool isEventQueueEmpty(void);

private:
	Nan::Callback* targetCallback;
	uv_mutex_t mutex, callbackMutex;
	Win32DeviceJSImpl* impl;
	std::queue<AudioMonitorEvent> eventQueue;
};
#endif
