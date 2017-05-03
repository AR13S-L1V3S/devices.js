#include "includes.h"
#include <atlcomcli.h>
#include <MMDeviceAPI.h>

#include "Win32AudioMonitorTask.h"
#include "Win32DeviceMonitor.h"
#include "Win32DirectShowDeviceManager.h"

Win32AudioMonitorTask::Win32AudioMonitorTask(Win32DeviceJSImpl* impl)
	: AsyncProgressWorkerBase<AudioMonitorEvent>(impl->getOnDeviceChangeCallback()), impl(impl), targetCallback(nullptr)
{
	uv_mutex_init(&this->mutex);
	uv_mutex_init(&this->callbackMutex);
}
Win32AudioMonitorTask::~Win32AudioMonitorTask()
{
	if (targetCallback != nullptr) {
		delete this->targetCallback;
		this->targetCallback = nullptr;
	}

	uv_mutex_destroy(&this->callbackMutex);
	uv_mutex_destroy(&this->mutex);
}

void Win32AudioMonitorTask::HandleOKCallback() { }
void Win32AudioMonitorTask::HandleProgressCallback(const AudioMonitorEvent* data, size_t size)
{
	ScopedLock lock(this->callbackMutex);

	Nan::HandleScope scope;

	Nan::Callback* cb = this->targetCallback != nullptr ? this->targetCallback : this->callback;
	nbind::cbFunction fn(**cb);

	DeviceChangeEventArgs* event = nullptr;
	switch (data->type)
	{
	case StateChanged:
		if (data->state == DEVICE_STATE_ACTIVE)
			event = new DeviceChangeEventArgs(data->device, L"active");
		else if (data->state == DEVICE_STATE_DISABLED || data->state == DEVICE_STATE_UNPLUGGED || data->state == DEVICE_STATE_NOTPRESENT)
			event = new DeviceChangeEventArgs(data->device, L"disabled");
		break;

	case Added:
		event = new DeviceChangeEventArgs(data->device, L"added");
		break;

	case Removed:
		event = new DeviceChangeEventArgs(data->device, L"removed");
		break;

	case DefaultDeviceChanged:
		event = new DeviceChangeEventArgs(data->device, L"changed");
		break;
	}

	fn.call<void>(event);

	delete event;
}

void Win32AudioMonitorTask::WorkComplete()
{
	ScopedLock lock(this->callbackMutex);
	AsyncProgressWorkerBase<AudioMonitorEvent>::WorkComplete();
}
void Win32AudioMonitorTask::Execute(const ExecutionProgress& progress)
{
	Win32DeviceMonitor* helper = new Win32DeviceMonitor(this);
	CComPtr<IMMDeviceEnumerator> deviceEnumerator;

	HRESULT hr = CoCreateInstance(CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, IID_IMMDeviceEnumerator, reinterpret_cast<void**>(&deviceEnumerator));
	CERRORPR(hr, , "CoCreateInstance(CLSID_MMDeviceEnumerator, ...) failed with HRESULT (0x%08X)", hr);

	deviceEnumerator->RegisterEndpointNotificationCallback(helper);

	while (true) {
		while (this->isEventQueueEmpty() == false) {
			AudioMonitorEvent event = this->pullEvent();
			progress.Send(&event, 1);
		}

		Sleep(this->checkFrequencyMillis);
	}

	deviceEnumerator->UnregisterEndpointNotificationCallback(helper);
	delete helper;
}

void Win32AudioMonitorTask::pushEvent(AudioMonitorEvent& event)
{
	ScopedLock lock(this->mutex);
	this->eventQueue.push(event);
}
AudioMonitorEvent Win32AudioMonitorTask::pullEvent()
{
	ScopedLock lock(this->mutex);
	AudioMonitorEvent event = this->eventQueue.front();
	this->eventQueue.pop();

	return event;
}
bool Win32AudioMonitorTask::isEventQueueEmpty()
{
	ScopedLock lock(this->mutex);
	return this->eventQueue.empty();
}

void Win32AudioMonitorTask::swapCallback(nbind::cbFunction& callback)
{
	ScopedLock lock(this->callbackMutex);

	if (this->targetCallback != nullptr)
		delete this->targetCallback;

	this->targetCallback = new Nan::Callback(callback.getJsFunction());
}
