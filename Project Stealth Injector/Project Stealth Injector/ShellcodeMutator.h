#pragma once

#include <Windows.h>
#include <cstdint>
#include <iostream>
#include <string>
#include<vector>

class ShellcodeMutator
{
public:
	ShellcodeMutator(std::vector<uint8_t>& input_shellcode) : shellcode(input_shellcode) {}


	void add_nops(int nops_count, const std::string& key)
	{
		uint8_t nops = 0x90 ^ key[0];
		for (int i = 0; i < nops_count; i++)
		{
			shellcode.insert(shellcode.begin(), nops); 
		}
	}

	std::vector<uint8_t> get_shellcode() const { return shellcode; }

private:
	std::vector<uint8_t> shellcode;
};
