#pragma once

#include <unordered_map>
#include <memory>
#include "wayItem.h"

using Way = std::unordered_map<size_t, wayItem>;
using Res = std::pair<bool, std::pair < size_t, bool > >;