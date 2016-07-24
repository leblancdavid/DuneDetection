#pragma once;

#include <string>

#define TEST_CASE_DIRECTORY GetDirectoryName(__FILE__)

std::string GetDirectoryName(std::string path);