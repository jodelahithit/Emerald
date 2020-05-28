#pragma once

enum class ConnectionState {
	Pending,
	Connected,
	Disconnected,
};

class Client : public Singleton<Client> {
private:
	Client() { m_salt = Random::Int(0, Math::MAX_INT); }
	~Client() {}
	friend Singleton;

	ConnectionState m_connectionState = ConnectionState::Disconnected;
	Connection m_serverConnection;
	NetHost m_host;
	Window* m_window;
	bool m_initialized = false;
	Timer m_timer;

	uint32 m_salt;
	float m_totalFrameTime = 0;
	uint64 m_frameCount = 0;
	float m_lastFrameTime = 0;
	int m_fps = 0;

	bool m_running = true;

	AsyncQueue<function<void()>> m_queue;

	void HandleQueue();

public:
	HDRPipeline* pipeline = nullptr;

	float GetTotalFrameTime() { return m_totalFrameTime; }

	void Initialize();
	void Run();
	void Cleanup();
	void OnWindowClose();

	void OnResize(int width, int height);
	void Update(TimeStep time);
	void Render();

	void CapabilitiesCheck();

	void OnConnectionAccepted(const PacketConnectionAccepted& packet);
	void OnKick(const PacketKick& packet);
	void OnGameData(const PacketGameData& packet);
	void OnAddEntity(const PacketAddEntity& packet);
	void OnRemoveEntity(const PacketRemoveEntity& packet);
	void OnUpdateEntities(const PacketUpdateEntities& packet);
	void OnBlockUpdate(const PacketBlockUpdate& packet);
	void HandlePacket(const ReceivedPacket& packet);
	void OnHandshakeChallenge(const PacketHandshakeChallenge& packet);

	void QueueTask(function<void()> task) {
		m_queue.Add(task);
	}
	uint64_t GetFrameCount() { return m_frameCount; }

	uint GetWidth() { return m_window->GetWidth(); }
	uint GetHeight() { return m_window->GetHeight(); }
	float GetAspect() { return m_window->GetAspect(); }

	Window* GetWindow() { return m_window; }
};

static Client* GetClient() {
	return Client::GetInstance();
}

//static GraphicsPipeline* GetPipeline() { return Application::GetInstance()->GetPipeline(); }
//static GBuffer* GetGBuffer() { return Application::GetInstance()->GetPipeline()->GetGBuffer(); }