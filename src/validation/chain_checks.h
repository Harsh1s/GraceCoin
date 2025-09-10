#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace gracecoin::validation {
struct BlockHeaderView { std::string previous_hash; std::string merkle_root; std::uint64_t timestamp; std::uint32_t nonce; };
bool has_parent_link(const BlockHeaderView& child, const std::string& parent_hash);
bool timestamp_is_monotonic(std::uint64_t previous_timestamp, std::uint64_t next_timestamp);
