// This header should be used only in files contains inside platforms folder !!!
#pragma once

#include <string>
#include <utility>

namespace Term
{

namespace Private
{

std::pair<bool, std::string> getenv(const std::string& env);

}

}  // namespace Term
