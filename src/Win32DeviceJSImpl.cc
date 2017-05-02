#include "includes.h"
#include <dshow.h>
#include <initguid.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <Devicetopology.h>

#pragma comment(lib, "strmiids")

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

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
	uv_mutex_t mutex, callbackMutex;
	Win32DeviceJSImpl* impl;
	std::queue<AudioMonitorEvent> eventQueue;
};
class Win32AudioMonitorHelper : public IMMNotificationClient
{
private:
	std::unordered_map<std::wstring, device_t*> devices;

public:
	Win32AudioMonitorHelper(Win32AudioMonitorTask* task)
		: task(task) {}
	Win32AudioMonitorHelper(Win32AudioMonitorHelper&) = delete;
	virtual ~Win32AudioMonitorHelper(void) {}

private:
	static std::wstring getFriendlyName(const std::wstring& deviceId)
	{
		CComPtr<IMMDeviceEnumerator> deviceEnumerator;
		HRESULT hr = CoCreateInstance(CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, IID_IMMDeviceEnumerator, reinterpret_cast<void**>(&deviceEnumerator));
		COM_ERRORPR(hr, L"", "CoCreateInstance(CLSID_MMDeviceEnumerator, ...) failed with HRESULT (0x%08X)", hr);

		CComPtr<IMMDevice> device;
		hr = deviceEnumerator->GetDevice(deviceId.c_str(), &device);
		COM_ERRORPR(hr, L"", "IMMDeviceEnumerator::GetDevice(...) failed with HRESULT (0x%08X)", hr);

		CComPtr<IPropertyStore> properties;
		hr = device->OpenPropertyStore(STGM_READ, &properties);
		COM_ERRORPR(hr, L"", "IMMDevice::OpenPropertyStore(...) failed with HRESULT (0x%08X)", hr);

		PROPERTYKEY key;
		key.pid = 14;
		key.fmtid = { 0xa45c254e, 0xdf1c, 0x4efd,{ 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0 } };

		PROPVARIANT variant;
		hr = properties->GetValue(key, &variant);
		COM_ERRORPR(hr, L"", "IPropertyStore::GetValue(...) failed with HRESULT (0x%08X)", hr);

		return variant.pwszVal;
	}

private:
	STDMETHOD(QueryInterface)(const IID& riid, void** ppvObject) override
	{
		if (riid == IID_IUnknown || riid == __uuidof(IMMNotificationClient))
		{
			*ppvObject = static_cast<IMMNotificationClient*>(this);
			return S_OK;
		}

		*ppvObject = nullptr;
		return E_NOINTERFACE;
	}
	STDMETHOD_(ULONG, AddRef)() override
	{
		return 1;
	}
	STDMETHOD_(ULONG, Release)() override
	{
		return 1;
	}
	STDMETHOD(OnDeviceStateChanged)(LPCWSTR pwstrDeviceId, DWORD dwNewState) override
	{
		std::wstring deviceId = pwstrDeviceId;
		if (this->devices.find(deviceId) == this->devices.end()) {
			std::wstring label = Win32AudioMonitorHelper::getFriendlyName(deviceId);
			devices[deviceId] = new device_t(deviceId.c_str(), label.c_str());
		}

		AudioMonitorEvent event(devices[deviceId], StateChanged, dwNewState);
		this->task->pushEvent(event);
		return S_OK;
	}
	STDMETHOD(OnDeviceAdded)(LPCWSTR pwstrDeviceId) override
	{
		std::wstring deviceId = pwstrDeviceId;
		if (this->devices.find(deviceId) == this->devices.end()) {
			std::wstring label = Win32AudioMonitorHelper::getFriendlyName(deviceId);
			devices[deviceId] = new device_t(deviceId.c_str(), label.c_str());
		}

		AudioMonitorEvent event(devices[deviceId], Added);
		this->task->pushEvent(event);
		return S_OK;
	}
	STDMETHOD(OnDeviceRemoved)(LPCWSTR pwstrDeviceId) override
	{
		std::wstring deviceId = pwstrDeviceId;
		if (this->devices.find(deviceId) == this->devices.end()) {
			std::wstring label = Win32AudioMonitorHelper::getFriendlyName(deviceId);
			devices[deviceId] = new device_t(deviceId.c_str(), label.c_str());
		}

		AudioMonitorEvent event(devices[deviceId], Removed);
		this->task->pushEvent(event);
		return S_OK;
	}
	STDMETHOD(OnDefaultDeviceChanged)(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId) override
	{
		if ((role != eConsole && role != eCommunications) || (flow != eRender && flow != eCapture) || pwstrDefaultDeviceId == NULL)
			return S_OK;

		std::wstring deviceId = pwstrDefaultDeviceId;
		if (this->devices.find(deviceId) == this->devices.end()) {
			std::wstring label = Win32AudioMonitorHelper::getFriendlyName(deviceId);
			devices[deviceId] = new device_t(deviceId.c_str(), label.c_str());
		}

		AudioMonitorEvent event(devices[deviceId], DefaultDeviceChanged);
		this->task->pushEvent(event);
		return S_OK;
	}
	STDMETHOD(OnPropertyValueChanged)(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) override
	{
		return S_OK;
	}

private:
	Win32AudioMonitorTask* task;
};

Win32AudioMonitorTask::Win32AudioMonitorTask(Win32DeviceJSImpl* impl)
	: AsyncProgressWorkerBase<AudioMonitorEvent>(impl->getOnDeviceChangeCallback()), impl(impl)
{
	uv_mutex_init(&this->mutex);
	uv_mutex_init(&this->callbackMutex);
}
Win32AudioMonitorTask::~Win32AudioMonitorTask()
{
	uv_mutex_destroy(&this->callbackMutex);
	uv_mutex_destroy(&this->mutex);
}
void Win32AudioMonitorTask::HandleOKCallback() { }
void Win32AudioMonitorTask::HandleProgressCallback(const AudioMonitorEvent* data, size_t size)
{
	ScopedLock lock(this->callbackMutex);

	Nan::HandleScope scope;
	nbind::cbFunction fn(**this->callback);
	
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
	Win32AudioMonitorHelper* helper = new Win32AudioMonitorHelper(this);
	CComPtr<IMMDeviceEnumerator> deviceEnumerator;

	HRESULT hr = CoCreateInstance(CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, IID_IMMDeviceEnumerator, reinterpret_cast<void**>(&deviceEnumerator));
	COM_ERRORPR(hr,, "CoCreateInstance(CLSID_MMDeviceEnumerator, ...) failed with HRESULT (0x%08X)", hr);
	
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

	delete this->callback;
	this->callback = new Nan::Callback(callback.getJsFunction());
}

Win32DeviceJSImpl* Win32DeviceJSImpl::instance = nullptr;
Win32DeviceJSImpl* Win32DeviceJSImpl::getInstance(void)
{
	if (Win32DeviceJSImpl::instance == nullptr)
		Win32DeviceJSImpl::instance = new Win32DeviceJSImpl;

	return Win32DeviceJSImpl::instance;
}

Win32DeviceJSImpl::Win32DeviceJSImpl(void)
	: ondevicechange(nullptr), audioMonitorTask(nullptr)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
}

Win32DeviceJSImpl::~Win32DeviceJSImpl(void)
{
	CoUninitialize();
}
bool Win32DeviceJSImpl::enumerateDevices(nbind::cbFunction& callback)
{
	std::vector<device_t*> result;
	HRESULT hr = S_OK;

	CComPtr<IMMDeviceEnumerator> deviceEnumerator;
	hr = CoCreateInstance(CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, IID_IMMDeviceEnumerator, reinterpret_cast<void**>(&deviceEnumerator));
	COM_ERRORPR(hr, false, "CoCreateInstance(CLSID_MMDeviceEnumerator, ...) failed with HRESULT (0x%08X)", hr);

	CComPtr<IMMDeviceCollection> collection;
	hr = deviceEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &collection);
	COM_ERRORPR(hr, false, "EnumAudioEndpoints(...) failed with HRESULT (0x%08X)", hr);

	UINT activeDeviceCount = 0;
	collection->GetCount(&activeDeviceCount);

	if (activeDeviceCount == 0) {
		callback.call<void>(result);
		return true;
	}
	
	PROPERTYKEY key;
	key.pid = 14;
	key.fmtid = { 0xa45c254e, 0xdf1c, 0x4efd,{ 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0 } };
	for (UINT i = 0; i < activeDeviceCount; ++i) {
		CComPtr<IMMDevice> device;
		hr = collection->Item(i, &device);
		if (FAILED(hr))
			continue;

		wchar_t* deviceId = nullptr;
		device->GetId(&deviceId);

		CComPtr<IPropertyStore> properties;
		hr = device->OpenPropertyStore(STGM_READ, &properties);
		if (FAILED(hr))
			continue;
		
		PROPVARIANT variant;
		hr = properties->GetValue(key, &variant);
		if (FAILED(hr))
			continue;

		result.push_back(new device_t(deviceId, variant.pwszVal));
	}

	CComPtr<ICreateDevEnum> createDeviceEnum;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&createDeviceEnum));
	COM_ERRORPR(hr, false, "CoCreateInstance(CLSID_SystemDeviceEnum, ...) failed with HRESULT (0x%08X)", hr);

	CComPtr<IEnumMoniker> enumMoniker;
	hr = createDeviceEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0);
	COM_ERRORPR(hr, false, "CreateClassEnumerator(CLSID_VideoInputDeviceCategory, ...) failed with HRESULT (0x%08X)", hr);

	IMoniker* moniker = nullptr;
	while (enumMoniker->Next(1, &moniker, nullptr) == S_OK) {
		IPropertyBag* propertyBag;
		hr = moniker->BindToStorage(nullptr, nullptr, IID_PPV_ARGS(&propertyBag));
		if (FAILED(hr)) {
			moniker->Release();
			continue;
		}

		VARIANT variant;
		VariantInit(&variant);

		hr = propertyBag->Read(L"Description", &variant, 0);
		if (FAILED(hr))
			hr = propertyBag->Read(L"FriendlyName", &variant, 0);

		if (FAILED(hr)) {
			moniker->Release();
			continue;
		}

		std::wstring label = variant.bstrVal;
		VariantClear(&variant);

		hr = propertyBag->Read(L"DevicePath", &variant, nullptr);
		if (FAILED(hr)) {
			moniker->Release();
			continue;
		}

		std::wstring deviceId = variant.bstrVal;
		VariantClear(&variant);

		propertyBag->Release();
		moniker->Release();

		result.push_back(new device_t(deviceId.c_str(), label.c_str()));
	}

	callback.call<void>(result);
	return true;
}

void Win32DeviceJSImpl::setOnDeviceChangeCallback(nbind::cbFunction& callback)
{
	if (this->ondevicechange != nullptr) {
		delete this->ondevicechange;
	}

	this->ondevicechange = new Nan::Callback(callback.getJsFunction());

	if (this->audioMonitorTask == nullptr) {
		this->audioMonitorTask = new Win32AudioMonitorTask(this);
		Nan::AsyncQueueWorker(this->audioMonitorTask);
	} else {
		this->audioMonitorTask->swapCallback(callback);
	}
}

void Win32DeviceJSImpl::release()
{
	if (this->ondevicechange != nullptr)
		delete this->ondevicechange;
}

Nan::Callback* Win32DeviceJSImpl::getOnDeviceChangeCallback()
{
	return this->ondevicechange;
}
