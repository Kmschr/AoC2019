/* ######################
 * #  INTCODE COMPUTER  #
 * ######################
 * Author: Kevin Schroeder
 */

#ifndef INTCODE_H
#define INTCODE_H

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

typedef long long INT64;
constexpr bool DEBUG = false;

class INTCODE {
  public:
	INTCODE(std::string filename) {
		std::ifstream in(filename);

		if (!in)
			throw std::runtime_error("INTCODE: Could not load program \"" + filename + "\"");

		int i=0;
		std::string line;

		while (std::getline(in, line, ',')) {
			std::stringstream conv(line);
			conv >> MEM[i++];
		}
	}

	int RUN(std::istream &IN, std::ostream &OUT) {
		while (PC >= 0) {
			// DECODE INSTRUCTION
			int OPCODE = MEM[PC] % 100;
			int MODES = MEM[PC] / 100;
			
			std::vector<INT64*> ARGS;

			// HALT
			if (OPCODE == 99) {
				DISPLAY_INSTRUCTION("HALT", MODES, ARGS);
				RUNNING = false;
				return 0;
			}

			// ADD/MUL
			if (OPCODE == 1 || OPCODE == 2) {
				INT64 S1 = MEM[PC+1]; // SOURCE 1
				INT64 S2 = MEM[PC+2]; // SOURCE 2
				INT64 D1 = MEM[PC+3]; // DEST
				ARGS.push_back(&S1);
				ARGS.push_back(&S2);
				ARGS.push_back(&D1);

				if (OPCODE == 1)
					DISPLAY_INSTRUCTION("ADD", MODES, ARGS);
				if (OPCODE == 2)
					DISPLAY_INSTRUCTION("MUL", MODES, ARGS);

				APPLY_MODES(ARGS, MODES, BASE, 2);

				// Perform ADD/MUL
				if (OPCODE == 1)
					MEM[D1] = S1 + S2;
				if (OPCODE == 2)
					MEM[D1] = S1 * S2;

				// Show pseudocode
				if (DEBUG) {
					if (OPCODE == 1)
						std::cout << "MEM[" << D1 << "] = " << S1 << " + " << S2 << '\n';
					if (OPCODE == 2)
						std::cout << "MEM[" << D1 << "] = " << S1 << " * " << S2 << '\n';
				}

				PC += 4;
			} 
			
			// INPUT
			if (OPCODE == 3) {

				if (INTERRUPT) {
					INTERRUPT = false;
					return 0;
				} else {
					INTERRUPT = true;
				}

				INT64 D1 = MEM[PC+1]; // DEST
				ARGS.push_back(&D1);

				DISPLAY_INSTRUCTION("IN", MODES, ARGS);
				if (DEBUG)
					std::cout << "MEM[" << D1 << "] = INPUT" << '\n';

				//std::cout << ">> ";
				IN >> MEM[D1];

				//std::cout << MEM[D1] << '\n';
				
				PC += 2;
				return 0;
			}

			// OUTPUT
			if (OPCODE == 4) {
				INT64 S1 = MEM[PC+1]; // SOURCE
				ARGS.push_back(&S1);

				DISPLAY_INSTRUCTION("OUT", MODES, ARGS);

				APPLY_MODES(ARGS, MODES, BASE);

				// print to stdout first, then ostringstream
				if (DEBUG)
					std::cout << '\n' << ">> " << S1 << '\n';

				if ((NUM_OUT+1) % 3 == 0)
					OUT << S1 << "\n";
				else
					OUT << S1 << ",";

				NUM_OUT++;
				PC += 2;
			}

			// JUMP
			if (OPCODE == 5 || OPCODE == 6) {
				INT64 C1 = MEM[PC+1]; // CONDITIONAL
				INT64 A1 = MEM[PC+2]; // JMP ADDRESS
				ARGS.push_back(&C1);
				ARGS.push_back(&A1);

				if (OPCODE == 5)
					DISPLAY_INSTRUCTION("JMPT", MODES, ARGS);
				if (OPCODE == 6)
					DISPLAY_INSTRUCTION("JMPF", MODES, ARGS);

				APPLY_MODES(ARGS, MODES, BASE);

				if ((OPCODE == 5 && C1 != 0) || (OPCODE == 6 && C1 == 0))
					PC = A1;
				else
					PC += 3;
				
				if (DEBUG) {
					if (OPCODE == 5)
						std::cout << "if (" << C1 << " != 0) PC = " << A1 << '\n';
					if (OPCODE == 6)
						std::cout << "if (" << C1 << " == 0) PC = " << A1 << '\n';
				}

			}

			// LESS/EQUAL
			if (OPCODE == 7 || OPCODE == 8) {
				INT64 S1 = MEM[PC+1]; // SOURCE 1
				INT64 S2 = MEM[PC+2]; // SOURCE 2
				INT64 D1 = MEM[PC+3]; // DEST
				ARGS.push_back(&S1);
				ARGS.push_back(&S2);
				ARGS.push_back(&D1);

				if (OPCODE == 7)
					DISPLAY_INSTRUCTION("LESS", MODES, ARGS);
				if (OPCODE == 8)
					DISPLAY_INSTRUCTION("EQL", MODES, ARGS);

				APPLY_MODES(ARGS, MODES, BASE, 2);

				if ((OPCODE == 7 && S1 < S2) || (OPCODE == 8 && S1 == S2))
					MEM[D1] = 1;
				else
					MEM[D1] = 0;

				if (DEBUG) {
					if (OPCODE == 7)
						std::cout << "if (" << S1 << " < " << S2 << ") MEM[" << D1 << "] = 1" << '\n';
					if (OPCODE == 8)
						std::cout << "if (" << S1 << " == " << S2 << ") MEM[" << D1 << "] = 1" << '\n';
				}

				PC += 4;
			}

			// BASE ADJUST
			if (OPCODE == 9) {
				INT64 S1 = MEM[PC+1]; // SOURCE
				ARGS.push_back(&S1);

				DISPLAY_INSTRUCTION("BASE", MODES, ARGS);

				APPLY_MODES(ARGS, MODES, BASE);

				BASE += S1;

				if (DEBUG)
					std::cout << "BASE += " << S1 << '\n';

				PC += 2;
			}

		}

		return 1;
	}

	void SET_MEM(INT64 INDEX, INT64 VALUE) {
		MEM[INDEX] = VALUE;
	}

	bool IS_RUNNING() {
		return RUNNING;
	}

  private:
	std::map<INT64, INT64> MEM; // Program Memory
	INT64 PC = 0; // Program Counter
	INT64 BASE = 0; // Base Address for Relative Mode
	INT64 NUM_OUT = 0;
	bool INTERRUPT = true;
	bool RUNNING = true;

	void DISPLAY_INSTRUCTION(std::string OPCODE, int MODES, const std::vector<INT64*> &ARGS) {
		if (!DEBUG)
			return;

		size_t NUM_ARGS = ARGS.size();
		constexpr size_t MAX_ARGS = 3;

		// Display Instruction
		std::cout << std::setw(5) << std::left << PC;
		std::cout << std::setw(6) << OPCODE;
		std::cout << std::setw(3) << std::right << std::setfill('0') << MODES << std::setfill(' ');
		std::cout << "  ";

		// Display Arguments
		size_t ARG_SPACE = 30 / MAX_ARGS;
		for (size_t i=0; i < NUM_ARGS; i++) {
			if (i != NUM_ARGS-1)
				std::cout << std::setw(ARG_SPACE) << std::left << std::to_string(*ARGS[i]) + ',';
			else
				std::cout << std::setw(ARG_SPACE) << std::left << std::to_string(*ARGS[i]);
		}

		// Fill empty argument space
		if (NUM_ARGS != MAX_ARGS)
			std::cout << std::setw(ARG_SPACE * (MAX_ARGS - NUM_ARGS)) << ' ';
	}

	void APPLY_MODES(std::vector<INT64*> &ARGS, int MODES, INT64 BASE, size_t DEST=50) {
		for (size_t i=0; i < ARGS.size(); ++i) {
			// Get last digit of modes
			int MODE = MODES % 10;

			// POSITION MODE
			if (MODE == 0)
				*ARGS[i] = *ARGS[i];

			// RELATIVE MODE
			if (MODE == 2)
				*ARGS[i] = BASE + *ARGS[i];

			// Only fetch from memory if not a destination address
			if (i != DEST && MODE != 1)
				*ARGS[i] = MEM[*ARGS[i]];

			// Remove last digit from modes
			MODES /= 10;
		}
	}
};

#endif