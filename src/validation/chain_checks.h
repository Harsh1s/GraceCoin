#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace gracecoin::validation {
struct BlockHeaderView { std::string previous_hash; std::string merkle_root; std::uint64_t timestamp; std::uint32_t nonce; };
