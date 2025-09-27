#include "chain_checks.h"

#include <sstream>

namespace gracecoin::validation {
bool has_parent_link(const BlockHeaderView& child, const std::string& parent_hash) { return child.previous_hash == parent_hash; }
