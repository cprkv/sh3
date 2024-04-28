#pragma once
#include "core/common.hpp"

// TODO: this should go to core/system/utils?
std::optional<std::string> convertWideStringToMultiByte( std::wstring_view input );
