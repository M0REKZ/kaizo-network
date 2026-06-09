//From E-Client
#ifndef GAME_CLIENT_COMPONENTS_ENTITY_PERFORMANCE_STATISTICS_H
#define GAME_CLIENT_COMPONENTS_ENTITY_PERFORMANCE_STATISTICS_H

#include <game/client/component.h>

#include <cstdint>

class CPerformanceStatistics : public CComponent
{
	int m_LastGameTick = -1;
	int m_LastSnapshotTick = -1;
	int64_t m_LastTime = 0;
	float m_EffectiveTps = 0.0f;
	float m_SnapshotRate = 0.0f;
	int m_SnapshotCount = 0;

	int m_CurrentFPS = 0;
	int64_t m_LastFpsUpdateTime = 0;

public:
	void UpdateServerStats();

	void OnReset() override;

	int Sizeof() const override { return sizeof(*this); }
	void OnRender() override;
};

#endif // GAME_CLIENT_COMPONENTS_ENTITY_PERFORMANCE_STATISTICS_H
