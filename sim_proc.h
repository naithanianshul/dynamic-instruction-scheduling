#ifndef SIM_PROC_H
#define SIM_PROC_H

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

typedef unsigned long int uli;

typedef struct proc_params{
    uli rob_size;
    uli iq_size;
    uli width;
}proc_params;

// Put additional data structures here as per your requirement

class ROB
{
public:
    int size;
	int head, tail;

    vector <int> value;

    vector <int> dst;
    vector <int> rdy;
    vector <int> exe;
    vector <int> mis;
    vector <int> pc;

    void initialize(int table_size)
    {
        this->size = table_size;
        
        this->head = 0;
        this->tail = 0;

        for (int i = 0; i < size; i++)
        {
            dst.push_back(0);
            rdy.push_back(0);
            pc.push_back(0);
        }
    }

    bool isEmpty()
    {
        if (this->head == this->tail)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};

class RMT
{
public:
	int size;
    vector <int> valid;
    vector <int>  ROB_tag;

    void initialize(int table_size)
    {
        size = table_size;

        for (int i = 0; i < size; i++)
        {
            valid.push_back(0);
            ROB_tag.push_back(-99);
        }
    }
};

typedef struct Instruction
{
	int pc, type, dst, counter;
    
    int src1_reg, rs1_rob_name, rs1_rdy, rs1_rob_done;
    int src2_reg, rs2_rob_name, rs2_rdy, rs2_rob_done;

	int start_cycle_FE, total_cycles_FE;
    int start_cycle_DE, total_cycles_DE;
    int start_cycle_RN, total_cycles_RN;
    int start_cycle_RR, total_cycles_RR;
    int start_cycle_DI, total_cycles_DI;
	int start_cycle_IS, total_cycles_IS;
    int start_cycle_EX, total_cycles_EX;
    int start_cycle_WB, total_cycles_WB;
    int start_cycle_RT, total_cycles_RT;

	bool operator < (const Instruction &temp) const
	{
		bool result = (pc < temp.pc);
        return result;
	}
}Instruction;

class Stage
{
public:
	bool is_empty;
	vector <Instruction> instruc;
};

class IQ
{
public:
	int size;
	vector <Instruction> entries;
    vector <int> valid;
    vector <int> dst_tag;
    vector <int> rs1_rdy;
    vector <int> rs1_tag;
    vector <int> rs2_rdy;
    vector <int> rs2_tag;
};

class OutOfOrder
{
public:
    int cycle, PC, width;
    
    // To pass instructions across each stage 
    Stage FE, DE, RN, RR, DI, IS, EX, WB, RT;
    
    ROB rob;
    IQ iq;
    RMT rmt;

    OutOfOrder(proc_params params)
    {
        this->cycle = -1;
	    this->PC = 0;
        this->width = params.width;
        iq.size = params.iq_size;

        rmt.initialize(67);
        rob.initialize(params.rob_size);

        DE.is_empty = true;
        RN.is_empty = true;
        RR.is_empty = true;
        DI.is_empty = true;
        WB.is_empty = true;
        RT.is_empty = true;
    }

    bool Advance_Cycle(fstream& fin)
    {
        this->cycle++;

        if (DE.is_empty == true && RN.is_empty == true && RR.is_empty == true && DI.is_empty == true)
            if (iq.entries.size() == 0 && EX.instruc.size() == 0 && WB.instruc.size() == 0)
                if (rob.isEmpty() == true)
                    if (rob.pc[rob.tail] == 0)
                        if (fin.eof() == true)
                            return false;
        
        return true;
    }

    void Retire()
    {
        int c = 0;

        while ((c < this->width))
        {
            if (rob.isEmpty() == true && rob.head != rob.size-1 && (rob.pc[rob.head + 1] == 0))
            {
                break;
            }

            if (rob.rdy[rob.head] == 1)
            {
                for (int i = 0; i < rmt.size; i++)
                {
                    if (rmt.ROB_tag[i] == rob.head)
                    {
                        rmt.ROB_tag[i] = 0;
                        rmt.valid[i] = 0;
                    }
                }

                for (uli i = 0; i < RT.instruc.size(); i++)
                {
                    if (RT.instruc[i].pc == rob.pc[rob.head])
                    {
                        std::cout << RT.instruc[i].pc << " fu{" << RT.instruc[i].type << "} src{" << RT.instruc[i].src1_reg << "," << RT.instruc[i].src2_reg << "} ";
                        std::cout << "dst{" << rob.dst[rob.head] << "} FE{" << RT.instruc[i].start_cycle_FE << "," << RT.instruc[i].total_cycles_FE << "} ";
                        std::cout << "DE{" << RT.instruc[i].start_cycle_DE << "," << RT.instruc[i].total_cycles_DE << "} RN{" << RT.instruc[i].start_cycle_RN << "," << RT.instruc[i].total_cycles_RN << "} ";
                        std::cout << "RR{" << RT.instruc[i].start_cycle_RR << "," << RT.instruc[i].total_cycles_RR << "} DI{" << RT.instruc[i].start_cycle_DI << "," << RT.instruc[i].total_cycles_DI << "} ";
                        std::cout << "IS{" << RT.instruc[i].start_cycle_IS << "," << RT.instruc[i].total_cycles_IS << "} EX{" << RT.instruc[i].start_cycle_EX << "," << RT.instruc[i].total_cycles_EX << "} ";
                        
                        RT.instruc[i].total_cycles_RT = (this->cycle + 1) - RT.instruc[i].start_cycle_RT;
                        std::cout << "WB{" << RT.instruc[i].start_cycle_WB << "," << RT.instruc[i].total_cycles_WB << "} RT{" << RT.instruc[i].start_cycle_RT << "," << RT.instruc[i].total_cycles_RT << "} " << '\n';

                        RT.instruc.erase(RT.instruc.begin() + i);
                    }
                }

                rob.dst[rob.head] = 0;
                rob.rdy[rob.head] = 0;
                rob.pc[rob.head] = 0;

                for (uli i = 0; i < RR.instruc.size(); i++)
                {
                    if (RR.instruc[i].rs1_rob_name == rob.head)
                    {
                        RR.instruc[i].rs1_rdy = 1;
                    }

                    if (RR.instruc[i].rs2_rob_name == rob.head)
                    {
                        RR.instruc[i].rs2_rdy = 1;
                    }
                }
                
                (rob.head != (rob.size - 1)) ? rob.head++ : rob.head = 0;

                c++;
            }
            else
            {
                break;
            }
        }
    }

    void Writeback()
    {
        if (WB.instruc.size() > 0)
        {
            for (uli i = 0; i < WB.instruc.size(); i++)
            {
                rob.rdy[WB.instruc[i].dst] = 1;
                WB.instruc[i].total_cycles_WB = this->cycle + 1 - WB.instruc[i].start_cycle_WB; 
                WB.instruc[i].start_cycle_RT = this->cycle + 1;
                
                // transfer instructions to next stage
                RT.instruc.push_back(WB.instruc[i]);
            }
            
            // clear instructions from current stage
            WB.instruc.clear();
        }
    }

    void Execute()
    {
        if (EX.instruc.size() > 0)
        {
            for (uli i = 0; i < EX.instruc.size(); i++)
            {
                EX.instruc[i].counter--;
            }

            bool empty_counter = true;
            while (empty_counter == true)
            {
                empty_counter = false;

                for (uli i = 0; i < EX.instruc.size(); i++)
                {
                    if (EX.instruc[i].counter == 0)
                    {
                        EX.instruc[i].start_cycle_WB = this->cycle + 1;
                        EX.instruc[i].total_cycles_EX = EX.instruc[i].start_cycle_WB - EX.instruc[i].start_cycle_EX;

                        WB.instruc.push_back(EX.instruc[i]);

                        // Wake up instructions in RR
                        for (uli j = 0; j < RR.instruc.size(); j++)
                        {
                            if (EX.instruc[i].dst == RR.instruc[j].rs1_rob_name)
                            {
                                RR.instruc[j].rs1_rdy = 1;
                            }

                            if (EX.instruc[i].dst == RR.instruc[j].rs2_rob_name)
                            {
                                RR.instruc[j].rs2_rdy = 1;
                            }
                        }

                        // Wake up instructions in DI
                        for (uli j = 0; j < DI.instruc.size(); j++)
                        {
                            if (EX.instruc[i].dst == DI.instruc[j].rs1_rob_name)
                            {
                                DI.instruc[j].rs1_rdy = 1;
                            }

                            if (EX.instruc[i].dst == DI.instruc[j].rs2_rob_name)
                            {
                                DI.instruc[j].rs2_rdy = 1;
                            }
                        }

                        // Wake up instructions in IQ
                        for (uli j = 0; j < iq.entries.size(); j++)
                        {
                            if (EX.instruc[i].dst == iq.entries[j].rs1_rob_name)
                            {
                                iq.entries[j].rs1_rdy = 1;
                            }

                            if (EX.instruc[i].dst == iq.entries[j].rs2_rob_name)
                            {
                                iq.entries[j].rs2_rdy = 1;
                            }
                        }

                        EX.instruc.erase(EX.instruc.begin() + i);
                        empty_counter = true;
                        break;
                    }
                }
            }
        }
    }

    void Issue()
    {
        if (iq.entries.size() > 0)
        {
            sort(iq.entries.begin(), iq.entries.end());

            int c = 0;
            bool empty_counter = true;
            while (empty_counter == true)
            {
                empty_counter = false;
                for (uli i = 0; i < iq.entries.size(); i++)
                {
                    if (iq.entries[i].rs1_rdy == 1 && iq.entries[i].rs2_rdy == 1)
                    {
                        iq.entries[i].start_cycle_EX = this->cycle + 1;
                        iq.entries[i].total_cycles_IS = this->cycle + 1 - iq.entries[i].start_cycle_IS;
                        EX.instruc.push_back(iq.entries[i]);
                        iq.entries.erase(iq.entries.begin() + i);
                        c++;
                        empty_counter = true;
                        break;
                    }
                }
                if (c == this->width)
                    break;
            }
        }
    }

    void Dispatch()
    {
        if (DI.is_empty == false && (iq.size - iq.entries.size()) >= DI.instruc.size())
        {
            DI.is_empty = true;

            for (uli i = 0; i < DI.instruc.size(); i++)
            {
                DI.instruc[i].start_cycle_IS = this->cycle + 1;
                DI.instruc[i].total_cycles_DI = this->cycle + 1 - DI.instruc[i].start_cycle_DI;
                
                // transfer instructions to next stage
                iq.entries.push_back(DI.instruc[i]);
            }

            // clear instructions from current stage
            DI.instruc.clear();
        }
    }

    void RegRead()
    {
        if (RR.is_empty == false && DI.is_empty == true)
        {
            RR.is_empty = true;
            DI.is_empty = false;

            for (uli i = 0; i < RR.instruc.size(); i++)
            {
                RR.instruc[i].start_cycle_DI = this->cycle + 1;
                RR.instruc[i].total_cycles_RR = this->cycle + 1 - RR.instruc[i].start_cycle_RR;
                
                if (RR.instruc[i].rs1_rob_done == 1)
                {
                    if (rob.rdy[RR.instruc[i].rs1_rob_name] == 1)
                    {
                        RR.instruc[i].rs1_rdy = 1;
                    }
                }
                else
                {
                    RR.instruc[i].rs1_rdy = 1;
                }

                if (RR.instruc[i].rs2_rob_done == 1)
                {
                    if (rob.rdy[RR.instruc[i].rs2_rob_name] == 1)
                    {
                        RR.instruc[i].rs2_rdy = 1;
                    }
                }
                else
                {
                    RR.instruc[i].rs2_rdy = 1;
                }
            }

            // transfer instructions to next stage
            RR.instruc.swap(DI.instruc);
            // clear instructions from current stage
            RR.instruc.clear();
        }
    }

    void Rename()
    {
        if ((RN.is_empty == false) && (RR.is_empty))
        {
            uli ROBspace;
            if (rob.tail < rob.head)
            {
                ROBspace = rob.head - rob.tail;
            }
            else if (rob.head < rob.tail)
            {
                ROBspace = rob.size - (rob.tail - rob.head);
            }
            else
            {
                if (rob.tail < rob.size - 1)
                {
                    ROBspace = 0;
                    if (rob.dst[rob.tail + 1] == 0 && rob.pc[rob.tail + 1] == 0 && rob.rdy[rob.tail + 1] == 0)
                    {
                        ROBspace = rob.size;
                    }
                }
                else
                {
                    ROBspace = 0;
                    if (rob.dst[rob.tail - 1] == 0 && rob.pc[rob.tail - 1] == 0 && rob.rdy[rob.tail - 1] == 0)
                    {    
                        ROBspace = rob.size;
                    }
                }
            }

            if (ROBspace >= RN.instruc.size())
            {
                RN.is_empty = true;
                RR.is_empty = false;

                for (uli i = 0; i < RN.instruc.size(); i++)
                {
                    if ((RN.instruc[i].src2_reg != -1) && (rmt.valid[RN.instruc[i].src2_reg] == 1))
                    {
                        // Rename rs2_rob_name
                        RN.instruc[i].rs2_rob_name = rmt.ROB_tag[RN.instruc[i].rs2_rob_name];
                        // rs2_rob_done == 1 means rename rs2_rob_name is done
                        RN.instruc[i].rs2_rob_done = 1;
                    }

                    if ((RN.instruc[i].src1_reg != -1) && (rmt.valid[RN.instruc[i].src1_reg] == 1))
                    {
                        // Rename rs1_rob_name
                        RN.instruc[i].rs1_rob_name = rmt.ROB_tag[RN.instruc[i].rs1_rob_name];
                        // rs1_rob_done == 1 means rename rs1_rob_name is done
                        RN.instruc[i].rs1_rob_done = 1;
                    }

                    if (RN.instruc[i].dst != -1)
                    {
                        // Assign ROB tail to RMT table
                        rmt.ROB_tag[RN.instruc[i].dst] = rob.tail;
                        rmt.valid[RN.instruc[i].dst] = 1;
                    }

                    rob.rdy[rob.tail] = 0;

                    // Set the ROB tail to destination
                    rob.dst[rob.tail] = RN.instruc[i].dst;
                    // Set the ROB pc to instruction pc
                    rob.pc[rob.tail] = RN.instruc[i].pc;

                    RN.instruc[i].dst = rob.tail;

                    RN.instruc[i].start_cycle_RR = this->cycle + 1;
                    RN.instruc[i].total_cycles_RN = this->cycle + 1 - RN.instruc[i].start_cycle_RN;

                    // Increment the tail in ROB
                    (rob.tail != (rob.size - 1)) ? rob.tail++ : rob.tail = 0;
                }

                // transfer instructions to next stage
                RN.instruc.swap(RR.instruc);
                // clear instructions from current stage
                RN.instruc.clear();
            }
        }
    }

    void Decode()
    {
        if (DE.is_empty == false && RN.is_empty == true)
        {
            DE.is_empty = true;
            RN.is_empty = false;

            for (uli i = 0; i < DE.instruc.size(); i++)
            {
                DE.instruc[i].start_cycle_RN = this->cycle + 1;
                DE.instruc[i].total_cycles_DE = this->cycle + 1 - DE.instruc[i].start_cycle_DE;
            }
            
            // transfer instructions to next stage
            DE.instruc.swap(RN.instruc);
            // clear instructions from current stage
            DE.instruc.clear();
        }
    }

    void Fetch(fstream& fin)
    {
        Instruction instruction;
        string line;
        
        if (DE.is_empty == true)
        {
            while (getline(fin, line))
            {
                //cout << "Line - " << line << '\n';
                DE.is_empty = false;
                this->PC++;

                instruction.pc = this->PC - 1;              
                instruction.type = line[7] - '0';
                switch (instruction.type)
                {
                    case 0:
                        instruction.counter = 1;
                        break;
                    case 1:
                        instruction.counter = 2;
                        break;
                    case 2:
                        instruction.counter = 5;
                        break;
                    default:
                        break;
                }
                instruction.rs1_rob_done = 0;
                instruction.rs2_rob_done = 0;
                instruction.rs1_rdy = 0;
                instruction.rs2_rdy = 0;
                
                if (line[9] == '-')
                {
                    if (line[12] == '-')
                    {
                        instruction.dst = -1;
                        instruction.src1_reg = -1;
                        instruction.src2_reg = stoi(line.substr(15, 2).c_str());
                    }
                    else if (line[13] == ' ')
                    {
                        instruction.dst = -1;
                        instruction.src1_reg = line[12] - '0';
                        instruction.src2_reg = stoi(line.substr(14, 2).c_str());
                    }
                    else
                    {
                        instruction.dst = -1;
                        instruction.src1_reg = stoi(line.substr(12, 2).c_str());
                        instruction.src2_reg = stoi(line.substr(15, 2).c_str());
                    }
                }
                else if (line[10] == ' ')
                {
                    if (line[11] == '-')
                    {
                        instruction.dst = line[9] - '0';
                        instruction.src1_reg = -1;
                        instruction.src2_reg = stoi(line.substr(14, 2).c_str());
                    }
                    else if (line[12] == ' ')
                    {
                        instruction.dst = line[9] - '0';
                        instruction.src1_reg = line[11] - '0';
                        instruction.src2_reg = stoi(line.substr(13, 2).c_str());
                    }
                    else
                    {
                        instruction.dst = line[9] - '0';
                        instruction.src1_reg = stoi(line.substr(11, 2).c_str());
                        instruction.src2_reg = stoi(line.substr(14, 2).c_str());
                    }
                }
                else
                {
                    if (line[12] == '-')
                    {
                        instruction.dst = stoi(line.substr(9, 2).c_str());
                        instruction.src1_reg = -1;
                        instruction.src2_reg = stoi(line.substr(15, 2).c_str());
                    }
                    else if (line[13] == ' ')
                    {
                        instruction.dst = stoi(line.substr(9, 2).c_str());
                        instruction.src1_reg = line[12] - '0';
                        instruction.src2_reg = stoi(line.substr(14, 2).c_str());
                    }
                    else
                    {
                        instruction.dst = stoi(line.substr(9, 2).c_str());
                        instruction.src1_reg = stoi(line.substr(12, 2).c_str());
                        instruction.src2_reg = stoi(line.substr(15, 2).c_str());
                    }
                }

                instruction.rs1_rob_name = instruction.src1_reg;
                instruction.rs2_rob_name = instruction.src2_reg;

                instruction.start_cycle_FE = this->cycle;
                instruction.start_cycle_DE = this->cycle + 1;
                instruction.total_cycles_FE = 1;

                // transfer instructions to next stage
                DE.instruc.push_back(instruction);
                
                if (DE.instruc.size() == this->width)
                    break;
            }
        }
    }

    void printStats()
    {
        std::cout << "# === Processor Configuration ===" << '\n';
        std::cout << "# ROB_SIZE = " << rob.size << '\n';
        std::cout << "# IQ_SIZE  = " << iq.size << '\n';
        std::cout << "# WIDTH    = " << this->width << '\n';
        std::cout << "# === Simulation Results ========" << '\n';
        std::cout << "# Dynamic Instruction Count    = " << this->PC << '\n';
        std::cout << "# Cycles                       = " << this->cycle << '\n';
        std::cout << "# Instructions Per Cycle (IPC) = " << std::fixed << std::setprecision(2) << ((float)this->PC / (float)this->cycle) << '\n';
    }
    
};

#endif
