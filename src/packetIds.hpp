#pragma once
#include <cstdint>

// Current Version: 777 (26.3)
namespace flatland::packet {

namespace handshake {
constexpr int32_t intention = 0;
}

namespace login {
constexpr int32_t helloServerbound = 0;
constexpr int32_t loginAcknowledgedServerbound = 3;

constexpr int32_t loginFinishedClientbound = 2;
} // namespace login

namespace configuration {
constexpr int32_t clientInformationServerbound = 0;
constexpr int32_t finishConfigurationServerbound = 3;
constexpr int32_t selectKnownPacksServerbound = 7;

constexpr int32_t finishConfigurationClientbound = 3;
constexpr int32_t registryDataClientbound = 7;
constexpr int32_t updateTagsClientbound = 14;
constexpr int32_t selectKnownPacksClientbound = 15;
} // namespace configuration

namespace play {
constexpr int32_t chunkBatchFinishedClientbound = 11;
constexpr int32_t chunkBatchStartClientbound = 12;
constexpr int32_t gameEventClientbound = 39;
constexpr int32_t keepAliveClientbound = 45;
constexpr int32_t levelChunkWithLightClientbound = 46;
constexpr int32_t loginClientbound = 50;
constexpr int32_t playerPositionClientbound = 73;
constexpr int32_t setChunkCacheCenterClientbound = 96;
constexpr int32_t setDefaultSpawnPositionClientbound = 99;

constexpr int32_t keepAliveServerbound = 28;
} // namespace play

namespace status {
constexpr int32_t statusRequestServerbound = 0;
constexpr int32_t pingRequestServerbound = 1;

constexpr int32_t statusResponseClientbound = 0;
constexpr int32_t pongResponseClientbound = 1;
} // namespace status

} // namespace flatland::packet
