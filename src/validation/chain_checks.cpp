#include "chain_checks.h"

#include <sstream>

namespace gracecoin::validation {
bool has_parent_link(const BlockHeaderView& child, const std::string& parent_hash) { return child.previous_hash == parent_hash; }
bool timestamp_is_monotonic(std::uint64_t previous_timestamp, std::uint64_t next_timestamp) { return next_timestamp >= previous_timestamp; }
std::string summarize_header(const BlockHeaderView& header) { std::ostringstream out; out << header.previous_hash << ':' << header.merkle_root << ':' << header.timestamp << ':' << header.nonce; return out.str(); }
}  // namespace gracecoin::validation
