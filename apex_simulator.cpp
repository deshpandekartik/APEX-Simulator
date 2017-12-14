#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <string>
using namespace std;

// ######################### classes definations #########################

struct display_class
{
	std::string fetch, decode, execute , mul1, mul2 , writeback , memory , div1, div2, div3, div4, status ;
	std::string fetch_status, decode_status, mul1_status,mul2_status, div1_status, div2_status, div3_status,div4_status, execute_status ;
	int fetch_pc_no, decode_pc_no, execute_pc_no, mul1_pc_no, mul2_pc_no, writeback_pc_no, memory_pc_no, div1_pc_no, div2_pc_no, div3_pc_no, div4_pc_no;

};


struct forwarding_bus
{
		int register_number, value;
		bool status;

		forwarding_bus() : status(true),register_number(999),value(999)
		{
		}
};

class Code_Memory
{
        public:
                int file_line_number,address, instruction_no;
		int cycles ;
		int alu_result;
		int mem_result;
                std::string instruction_string;

		// instruction variables
		std::string type,dst1,src1,src2, src3, pipelinestage , zeroflag;
		int src1_value, src2_value,src3_value ;
		bool src1_bus_status, src2_bus_status, src3_bus_status;

		Code_Memory();

                // Parameterized Constructor
                Code_Memory(int x, int y, string z, int a)
                {
                        file_line_number = x;
                        address = y;
                        instruction_string = z;
                        instruction_no = a;
                        cycles = 0;
                        zeroflag = "inactive";
                        mem_result = 0;
                        alu_result = 0;
                        src1_value = 0;
                        src2_value = 0;
                        src3_value = 0;
                        src1_bus_status = false;
                        src2_bus_status = false;
                        src3_bus_status = false;
                        pipelinestage = "Processing";
                }

		// instruction_parameters ( type,dst1,src1,src2)
		void instruction_parameters(std::string a, std::string b, std::string c, std::string d, std::string e)
		{
			type =  a;
			dst1 = b;
			src1 = c;
			src2 = d;
			src3 = e;
		}
};

class Register
{
        public:
                int value;
                bool status;
                Register(int x, bool y, int a, bool b)
                {
                        value = x ;
                        status = y ;
                }

                // check if register status is valid or not
                bool is_status_valid(Register temp)
                {
                        return temp.status;
                }

                // set register status
                void set_reg_status(Register temp, bool change_status)
                {
                        temp.status = change_status;
                }

                // set register value
                void set_reg_value(Register temp, int change_value)
                {
                        temp.value = change_value;
                }
};


class Data_Memory
{
	public: int data_mem_base_address;
		int data_mem_value;

		Data_Memory( int x, int y )
		{
			data_mem_base_address = x;
			data_mem_value = y;
		}
};

class execute_buffer_class
{
	public:
		int PC, cycles;
		std::string instruction_string, type;

		// constructor
		//execute_buffer_class();

		execute_buffer_class (int a, int b, std::string c, std::string d)
		{
					PC = a;
					cycles = b;
					instruction_string = c;
					type = d;
		}

		// set_execute_object_values ( PC,cycles ,instruction_string ,type)
		void set_execute_object_values(int a, int b, std::string c, std::string d)
		{
			PC = a;
			cycles = b;
			instruction_string = c;
			type = d;
		}
};

// ###############################################################


// ################### Global Declaration ########################

// initially address is 0
int linenumber = 0;
int MAX_CYCLES = 220;
int no_of_cycle = 0;
int stall_var = 0;
int mull_stall = 0;
std::string zero_flag = "active";
std::string check_zero_flag = "active";
bool jumping_flag = false;
bool jumping_flag_2 = false;
bool check_bz_status_mem = true;
bool check_bz_status_wb = true;
bool check_if_jumping = false;

// declaring globally , for other classes methods to access
Register *R[15];
// declaring globally , for other classes methods to access
Code_Memory *code_memory[94000];
// declaring globally , for other classes methods to access
Data_Memory *data_memory[94000];
//  declaring globally , for other classes methods to access
execute_buffer_class *execute_buffer_array[94000];
display_class *display_class_array[94000];


// fetch instruction buffer
std::string fetch_buffer = "";
//decode instruction buffer
std::string decode_rf_buffer = "";
// execute instruction buffer
std::string execute_buffer = "";
// execute instruction buffer
std::string memory_buffer = "";
// writeback instruction buffer
std::string writeback_buffer = "";

forwarding_bus int_fu_bus ,mul_fu_bus, div_fu_bus, z_fu_bus;

//function declarations
bool fetch();
bool decode_rf();
bool execute_stage();
bool memory_stage();
bool writeback_stage();
int get_source_value(Code_Memory temp, int station);
void set_destination_value(Code_Memory temp, int result);
void set_destination_reg_status(Code_Memory temp, bool regstatus);
bool check_register_status(Code_Memory temp);
bool check_other_stages();
bool check_stage_execution_status();
void display();
// ###############################################################

// ################################ FIVE STAGE PIPELINE FUNCS ###############################

int program_counter = 3996;
int address_val = 3996;

int fetch_PC = address_val, decode_PC = address_val,  execute_PC = address_val, memory_PC = address_val, writeback_PC = address_val;
bool fetch()
{


	if ( writeback_PC >= 4000 && writeback_buffer != "" )
	{
		Code_Memory temp = *(code_memory[writeback_PC]);
		temp.pipelinestage = "DONE";

			if (  temp.type == "HALT")
	        {
				fetch_PC = address_val, decode_PC = address_val,  execute_PC = address_val, memory_PC = address_val, writeback_PC = address_val;
				display_class_array[no_of_cycle]->fetch = "Empty";
				fetch_buffer = "";
				decode_rf_buffer = "";
				execute_buffer = "";
				memory_buffer = "";
				writeback_buffer = "";
				bool status = check_stage_execution_status();
				return status;
	        }

        if ( temp.type == "LOAD" )
		{
			set_destination_value(temp, temp.mem_result);
		}
		else
		{
            set_destination_value(temp, temp.alu_result);
		}

                // update instruction object with new entries
                *code_memory[writeback_PC] = temp;
	}


	if ( stall_var > 0  || check_other_stages() == false )
	{
		if ( fetch_buffer == "" )
		{
			display_class_array[no_of_cycle]->fetch = "Empty";
		}
		else
		{
			display_class_array[no_of_cycle]->fetch = fetch_buffer;
			display_class_array[no_of_cycle]->fetch_status = " Stalled";
		}
		display_class_array[no_of_cycle]->fetch_pc_no = fetch_PC;
		bool status = check_stage_execution_status();
		return status;
	}
	else
	{
		// increment PC to point to next instruction
		fetch_PC = fetch_PC + 4;


		if ( fetch_PC <=  address_val && stall_var < 1 && check_if_jumping == false )
		{
			fetch_buffer = code_memory[fetch_PC]->instruction_string;
			display_class_array[no_of_cycle]->fetch_pc_no = fetch_PC;
			display_class_array[no_of_cycle]->fetch = fetch_buffer;
			return true;
		}
		else
		{
				display_class_array[no_of_cycle]->fetch = "Empty";
				display_class_array[no_of_cycle]->fetch_pc_no = fetch_PC;
				fetch_buffer = "";
				bool status = check_stage_execution_status();
				return status;
			// finished parsing entire array of instructions
		}
	}
}

bool decode_rf()
{
	display_class_array[no_of_cycle]->decode = "Empty";


	if ( stall_var  > 0 )
	{
		if ( decode_rf_buffer != "" )
		{
			if ( stall_var > 0 )
			{
				display_class_array[no_of_cycle]->decode = decode_rf_buffer;
				display_class_array[no_of_cycle]->decode_status = " Stalled";
				display_class_array[no_of_cycle]->decode_pc_no = decode_PC;
			}
		}
		return true;
	}
	else
	{

	decode_rf_buffer = fetch_buffer;


        if ( decode_rf_buffer != "" && decode_PC <= address_val && stall_var < 1 && check_if_jumping == false )
        {
                // increment PC to point to next instruction
                decode_PC = fetch_PC;

                Code_Memory temp = *(code_memory[decode_PC]);

                display_class_array[no_of_cycle]->decode = decode_rf_buffer;
                display_class_array[no_of_cycle]->decode_pc_no = decode_PC;


                // Remove all , and replace them with a space
                std::replace( decode_rf_buffer.begin(), decode_rf_buffer.end(), ',', ' ');
                // Change multiple spaces with single space

                int array_size = 0;

                std::vector<std::string> split_instruction;
                std::istringstream iss(decode_rf_buffer);
                for(std::string s; iss >> s; array_size += 1)
                        split_instruction.push_back(s);

                // instruction_parameters ( type,dst1,src1,src2)

                if ( array_size == 1 )
                {
                        // single instruction , (  HALT )
                		if ( split_instruction[0] == "HALT" )
                		{
                				fetch_buffer = "";
                		}
                        temp.instruction_parameters(split_instruction[0],"NULL","NULL","NULL","NULL");
                }
                else if ( array_size == 2 )
                {
                        // Ex: BNZ BZ
                		if ( split_instruction[0] == "BZ" || split_instruction[0] == "BNZ" )
                		{
                			temp.instruction_parameters(split_instruction[0],"NULL", split_instruction[1] ,"NULL","NULL");
                		}
                		else
                		{
                			temp.instruction_parameters(split_instruction[0],split_instruction[1],"NULL","NULL","NULL");
                		}
                }
                else if ( array_size == 3 )
                {
                        // Ex: MOVC instruction
                		if ( split_instruction[0] == "JUMP" )
                	    {
                			temp.instruction_parameters(split_instruction[0],"NULL",split_instruction[1],split_instruction[2],"NULL");
                	    }
                		else
                		{
                			temp.instruction_parameters(split_instruction[0],split_instruction[1],split_instruction[2],"NULL","NULL");
                		}

                }
                else if ( array_size == 4 )
                {
                        // Ex: STORE, ADD, SUB, MUL, JAL
                		if (  split_instruction[0] == "STORE" )
                		{
                			temp.instruction_parameters(split_instruction[0],"NULL",split_instruction[2],split_instruction[3],split_instruction[1]);
                		}
                		else
                		{
                			temp.instruction_parameters(split_instruction[0],split_instruction[1],split_instruction[2],split_instruction[3],"NULL");
                		}
                }
                // update instruction object with new entries
                *code_memory[decode_PC] = temp;
	}

	return true;

	}
}


bool  same_stage_execute = true;
int execute_buffer_array_length = 0;
bool execute_stage()
{
	execute_buffer = decode_rf_buffer ;

	jumping_flag = false;
	display_class_array[no_of_cycle]->mul1 = "Empty";
	display_class_array[no_of_cycle]->mul2 = "Empty";
	display_class_array[no_of_cycle]->execute = "Empty";
	display_class_array[no_of_cycle]->div1 = "Empty";
	display_class_array[no_of_cycle]->div2 = "Empty";
	display_class_array[no_of_cycle]->div3 = "Empty";
	display_class_array[no_of_cycle]->div4 = "Empty";

	if ( execute_PC == decode_PC )
	{
			same_stage_execute = true;
	}
	else
	{
			same_stage_execute = false;
	}
	execute_PC = decode_PC;
	int mull_fu_flag=0, add_fu_flag = 0, div_fu_flag = 0;


	bool mul1_flag = false;
	bool mul2_flag = false;
	int largest_PC = 0;
	if ( decode_PC >= (program_counter + 4) )
	{
			// check if stall to be introduced
			if ( execute_buffer_array_length > 0 )
			{
					for ( int i =0; i < execute_buffer_array_length; i ++ )
					{
							execute_buffer_class object = *(execute_buffer_array[i]);

							if ( object.type == "DIV" || object.type == "HALT" )
							{
									div_fu_flag = div_fu_flag + 1;
							}
							else
							if ( object.type == "MUL")
							{
									mull_fu_flag = mull_fu_flag + 1;
							}
							else
							{
									add_fu_flag = add_fu_flag + 1;

							}
					}

						largest_PC = execute_buffer_array[0]->PC;

						for ( int c = 1; c < execute_buffer_array_length; c++)
						{
							if (execute_buffer_array[c]->PC > largest_PC)
							{
								largest_PC  = execute_buffer_array[c]->PC;
							}
						}
					//largest_PC = execute_buffer_array[execute_buffer_array_length - 1]->PC;
			}

			Code_Memory temp = *(code_memory[execute_PC]);

			if ( stall_var == 1 )
			{
				execute_buffer = "";
			}
			// changes here
			if ( largest_PC < execute_PC && decode_rf_buffer != "" &&  ( same_stage_execute == false || stall_var == 1) && jumping_flag_2 == false )
			{
				if ( check_register_status(temp) == false || ( ( temp.type == "DIV" || temp.type == "HALT" )  && div_fu_flag >= 4 ) || ( temp.type == "MUL" && mull_fu_flag >= 2) || ( temp.type != "MUL" && temp.type != "DIV" && add_fu_flag == 1))
				{
						stall_var = 1;
						execute_buffer = "";
						// cannot fetch this instruction // execute buffer is full
				}
				else
				{
						// set_execute_object_values ( PC,cycles ,instruction_string ,type)
						execute_buffer_array[execute_buffer_array_length] = new execute_buffer_class(execute_PC, 0, temp.instruction_string, temp.type);
						execute_buffer_array_length ++ ;
						stall_var = 0;
				}
			}
	}

	jumping_flag_2 = false;



	 // processing the instructions in the array
	if ( execute_buffer_array_length > 0 )
	{
			for ( int i=0; i < execute_buffer_array_length; i++ )
			{
					execute_buffer_class object = *(execute_buffer_array[i]);
					Code_Memory temp = *(code_memory[execute_buffer_array[i]->PC]);

					if ( object.type == "")
					{
						break ;
					}
					else
					if ( object.type == "DIV" || object.type == "HALT" )
					{
						// ###########################
						// DIV FUNCTIONAL UNIT
						// ###########################
						if ( object.cycles <= 4 )
						{

							object.cycles = object.cycles + 1;

							if ( object.type == "DIV" )
							{
									int result1 = get_source_value(temp , 1);
									int result2 = get_source_value(temp, 2);

									if ( result2 == 0 )
									{
										cout << "EXCEPTION ! Divide by zero error encountered , making divisor 1 /n" ;
										result2 = 1;
									}

									int result = result1 / result2;
									temp.alu_result = result;
									temp.pipelinestage = "EX";
									// interlocking , set dest register value to false
									set_destination_reg_status(temp, false);
							}
							else
							{
									fetch_buffer = "";
									decode_rf_buffer = "";
							}


							if ( object.cycles == 1 )
							{
								display_class_array[no_of_cycle]->div1 = object.instruction_string;
								display_class_array[no_of_cycle]->div1_pc_no = object.PC;
							}
							else
							if ( object.cycles == 2 )
							{
								display_class_array[no_of_cycle]->div2 = object.instruction_string;
								display_class_array[no_of_cycle]->div2_pc_no = object.PC;
							}
							else
							if ( object.cycles == 3 )
							{
								display_class_array[no_of_cycle]->div3 = object.instruction_string;
								display_class_array[no_of_cycle]->div3_pc_no = object.PC;
							}
							else
							if ( object.cycles == 4 )
							{
								display_class_array[no_of_cycle]->div4 = object.instruction_string;
								display_class_array[no_of_cycle]->div4_pc_no = object.PC;

							}

							*code_memory[execute_buffer_array[i]->PC] = temp;
						}
					}
					else
					if ( object.type == "MUL")
					{
							// ##########################
							// MUL Functional Unit
							// ##########################
							if ( object.cycles >= 1 && mul2_flag == false)
							{

									if ( object.cycles == 2 )
									{
										 	 // stalled
										  	  display_class_array[no_of_cycle]->mul2_status = " Stalled";
									}

									mul2_flag = true;
									// 2nd cycle
									object.cycles = 2;

									int result1 = get_source_value(temp,1);
									int result2 = get_source_value(temp,2);
									int result = result1 * result2;
									temp.alu_result = result;
									temp.pipelinestage = "EX";

									// interlocking , set dest register value to false
									set_destination_reg_status(temp, false);

									// mull has finished execution though both stages

									display_class_array[no_of_cycle]->mul2 = object.instruction_string;
									display_class_array[no_of_cycle]->mul2_pc_no = object.PC;
									*code_memory[execute_buffer_array[i]->PC] = temp;
							}
							else
							{
									if ( object.cycles == 1 )
									{
											// stalled
											display_class_array[no_of_cycle]->mul1_status = " Stalled";
									}

									mul1_flag = true;
									// 1st cycle

									// one cycle of MUL done
									object.cycles = 1;

									// interlocking , set dest register value to false
									set_destination_reg_status(temp, false);

									display_class_array[no_of_cycle]->mul1 = object.instruction_string;
									display_class_array[no_of_cycle]->mul1_pc_no = object.PC;
									*code_memory[execute_buffer_array[i]->PC] = temp;
							}
					}
					else
					{
							// ###############################
							// INT Functional Unit
							// ###############################
							Code_Memory temp = *(code_memory[execute_buffer_array[i]->PC]);
							display_class_array[no_of_cycle]->execute = temp.instruction_string;
							display_class_array[no_of_cycle]->execute_pc_no = object.PC;
							execute_buffer_class object = *(execute_buffer_array[i]);
							temp.pipelinestage = "EX";

							if ( temp.cycles > 0 )
							{
								display_class_array[no_of_cycle]->execute_status = " Stalled";
							}
							if ( temp.type == "MOVC" || temp.type == "MOV" )
							{
										int result = get_source_value(temp,1);
										temp.alu_result = result;
										// interlocking , set dest register value to false
										set_destination_reg_status(temp, false);
										temp.cycles = 1;
							}
							else if ( temp.type == "JUMP" )
							{
									int result1 = get_source_value(temp,1);
							        int result2 = get_source_value(temp,2);
							        int result = result1 + result2;


									if ( temp.cycles == 0)
									{
											temp.alu_result = result;
											jumping_flag = true;
											temp.cycles = 1;
											check_if_jumping = true;
											if ( execute_buffer_array_length > 1 )
											{
													jumping_flag_2 = true;
											}
									}
									else if ( temp.cycles == 1 )
									{
											check_if_jumping = false;
											temp.cycles = 2;
											jumping_flag_2 = false;
					        				fetch_PC = temp.alu_result - 4;
				                			decode_rf_buffer = "";
				                			fetch_buffer = "";
									}
							}
							else if ( temp.type == "JAL" )
							{

									int result1 = get_source_value(temp,1);
									int result2 = get_source_value(temp,2);
									int result = result1 + result2;

									set_destination_reg_status(temp, false);
									if ( temp.cycles == 0)
									{
											check_if_jumping = true;
											temp.mem_result = result;
											temp.alu_result = temp.address + 4;

											jumping_flag = true;
											temp.cycles = 1;
											if ( execute_buffer_array_length > 1 )
											{
													jumping_flag_2 = true;
											}
									}
									else if ( temp.cycles == 1 )
									{
											check_if_jumping = false;
											temp.cycles = 2;
											jumping_flag_2 = false;
					        				fetch_PC = temp.mem_result - 4;
				                			decode_rf_buffer = "";
				                			fetch_buffer = "";
									}

							}
							else if ( temp.type == "AND" )
							{
									int result1 = get_source_value(temp,1);
									int result2 = get_source_value(temp,2);
									int result = result1 & result2;

									temp.alu_result = result;
									temp.cycles = 1;
									// interlocking , set dest register value to false
									set_destination_reg_status(temp, false);

							}
							else if ( temp.type == "EX-OR")
							{
									int result1 = get_source_value(temp,1);
									int result2 = get_source_value(temp,2);
									int result = result1 ^ result2;
									temp.cycles = 1;

									temp.alu_result = result;
									// interlocking , set dest register value to false
									set_destination_reg_status(temp, false);
							}
							else if ( temp.type == "OR" )
							{
									int result1 = get_source_value(temp,1);
									int result2 = get_source_value(temp,2);
									int result = result1 | result2;

									temp.cycles = 1;
									temp.alu_result = result;
									set_destination_reg_status(temp, false);
							}
							else if ( temp.type == "BNZ" || temp.type == "BZ" )
							{
									int zero_flag_inst_address = temp.address ;
									int while_flag = 0;
									std::string zero_flag_temp;
									while ( while_flag == 0 )
									{
											zero_flag_inst_address = zero_flag_inst_address - 4;
											if ( zero_flag_inst_address <= address_val )
											{
													Code_Memory preceding = *(code_memory[zero_flag_inst_address]);
													if ( preceding.type == "ADD" || preceding.type == "SUB"
																			|| preceding.type == "MUL" || preceding.type == "DIV" )
													{
															if ( preceding.alu_result == 0 )
															{
																	zero_flag_temp = "active";
															}
															else
															{
																	zero_flag_temp = "inactive";
															}
															while_flag = 1;
													}
											}
											else
											{
													// no preceeding instruction can set BZ flag
													zero_flag_temp = "inactive";
											}
									}



								int result = get_source_value(temp,1);
								result = temp.address + result;
								temp.zeroflag = zero_flag_temp;
								check_zero_flag = zero_flag_temp;
								if ( temp.cycles == 0)
								{

										temp.alu_result = result;
										jumping_flag = true;
										temp.cycles = 1;
										if ( execute_buffer_array_length > 1 )
										{
												if ( zero_flag_temp == "inactive" && temp.type == "BNZ" )
							        			{
														check_if_jumping = true;
							        					jumping_flag_2 = true;
							        			}
							        			if ( zero_flag_temp == "active" && temp.type == "BZ" )
							        			{
							        					check_if_jumping = true;
							        					jumping_flag_2 = true;
							        			}
										}
								}
								else if ( temp.cycles == 1 )
								{
										if ( ( zero_flag_temp == "inactive" && temp.type == "BNZ") || (( check_zero_flag == "active" && temp.type == "BZ" )) )
										{
											check_if_jumping = false;
											temp.cycles = 2;
											jumping_flag_2 = false;
											fetch_PC = temp.alu_result - 4;
											decode_rf_buffer = "";
											fetch_buffer = "";
										}
								}
							}
							else if ( temp.type == "HALT" )
							{
									fetch_buffer = "";
							        decode_rf_buffer = "";
							}
							else if ( temp.type == "STORE" )
							{
									temp.cycles = 1;
									int result1 = get_source_value(temp,1);
							        int result2 = get_source_value(temp,2);
									int result = result1 + result2;
							        temp.alu_result = result;

							        // interlocking , set dest register value to false
							        //set_destination_reg_status(temp, false);
							}
							else if ( temp.type == "LOAD" )
							{
									temp.cycles = 1;
									int result1 = get_source_value(temp,1);
							        int result2 = get_source_value(temp,2);
							        int result = result1 + result2;
							        // now find value in mem[result]

							        temp.alu_result = result;
							        // interlocking , set dest register value to false
							        set_destination_reg_status(temp, false);
							}
							else if ( temp.type == "ADD" )
							{
									temp.cycles = 1;
									int result1 = get_source_value(temp,1);
							        int result2 = get_source_value(temp,2);
							        int result = result1 + result2;

							        temp.alu_result = result;


							        // interlocking , set dest register value to false
							        set_destination_reg_status(temp, false);

									if ( temp.alu_result == 0)
									{
											zero_flag = "active";
									}
									else
									{
											zero_flag = "inactive";
									}
							}
							else if ( temp.type == "SUB" )
							{
									temp.cycles = 1;
									int result1 = get_source_value(temp,1);
							        int result2 = get_source_value(temp,2);
							        int result = result1 - result2;
							        temp.alu_result = result;

							        // interlocking , set dest register value to false
							        set_destination_reg_status(temp, false);

							}
							// set_execute_object_values ( PC,cycles ,instruction_string ,type)

							// update instruction object with new entries
							*code_memory[execute_buffer_array[i]->PC] = temp;
							//*execute_buffer_array[i] = object;
					}
					*execute_buffer_array[i] = object ;
			}

	}


	// dataforwarding logic
	int_fu_bus.status = false;
	int_fu_bus.register_number = 999;
	int_fu_bus.value = 999;

	mul_fu_bus.status = false;
	mul_fu_bus.register_number = 999;
	mul_fu_bus.value = 999;

	div_fu_bus.status = false;
	div_fu_bus.register_number = 999;
	div_fu_bus.value = 999;

	z_fu_bus.status = false;
	z_fu_bus.register_number = 999;
	z_fu_bus.value = 999;

	if ( execute_buffer_array_length > 0 )
	{
			// array of finished jobs
			// set_execute_object_values ( PC,cycles ,instruction_string ,type)
			for ( int i=0; i < execute_buffer_array_length; i++ )
			{
				if ( execute_buffer_array[i]->type != "BZ" && execute_buffer_array[i]->type != "BNZ"
						&& execute_buffer_array[i]->type != "JUMP" && execute_buffer_array[i]->type != "HALT"
								&&  execute_buffer_array[i]->type != "STORE" && execute_buffer_array[i]->type != "LOAD" )
				{
						if ( ( execute_buffer_array[i]->type == "MUL" && execute_buffer_array[i]->cycles >= 2 ) ||
									( ( execute_buffer_array[i]->type == "DIV" )  && execute_buffer_array[i]->cycles >= 4 ) ||
										( execute_buffer_array[i]->type != "DIV" && execute_buffer_array[i]->type != "MUL" )  )
						{

							Code_Memory temp = *(code_memory[execute_buffer_array[i]->PC]);
							std::string source = temp.dst1;
							std::replace( source.begin(), source.end(), 'R', ' ');
							int register_number = atoi( source.c_str() );
							int result = temp.alu_result;
							if ( execute_buffer_array[i]->type == "DIV" )
							{
									// data forwarding logic
									div_fu_bus.status = true;
									div_fu_bus.register_number = register_number;
									div_fu_bus.value = result;
											z_fu_bus.status = true;
											z_fu_bus.value = result;
							}
							else if ( execute_buffer_array[i]->type == "MUL" )
							{
									// data forwarding logic
									mul_fu_bus.status = true;
									mul_fu_bus.register_number = register_number;
									mul_fu_bus.value = result;
											z_fu_bus.status = true;
											z_fu_bus.value = result;
							}
							else
							{
									// data forwarding logic
									int_fu_bus.status = true;
									int_fu_bus.register_number = register_number;
									int_fu_bus.value = result;

									if ( execute_buffer_array[i]->type == "ADD" || execute_buffer_array[i]->type == "SUB")
									{
													z_fu_bus.status = true;
													z_fu_bus.value = result;
									}
							}
					   }
				  }
			}
	}

	bool next_check = true;

	if ( execute_buffer_array_length > 0 )
	{
			// array of finished jobs
			// set_execute_object_values ( PC,cycles ,instruction_string ,type)
			for ( int i=0; i < execute_buffer_array_length; i++ )
			{
				if (( execute_buffer_array[i]->type == "DIV" || execute_buffer_array[i]->type == "HALT" ) && execute_buffer_array[i]->cycles >= 4 )
				{
						execute_buffer = execute_buffer_array[i]->instruction_string;
						execute_PC = execute_buffer_array[i]->PC;

						// array of executing jobs
						for ( int j = 0; j < execute_buffer_array_length; j++ )
						{
								// remove the processed element from the array
								if ( execute_buffer_array[i] == execute_buffer_array[j] )
								{
										int pos = j ;
										// execute_buffer_array_length -1
										for(int k=0; k < execute_buffer_array_length ; k++)
										{
												if( k >= pos)
												{
													execute_buffer_array[k] = execute_buffer_array[k+1];
												}
										}
								}
						}
						execute_buffer_array_length = execute_buffer_array_length - 1 ;
						next_check = false;
						break;
				}
				else
				{
						execute_buffer = "";
				}
			}
	}

	// MULTIPLICATION FU

	if ( next_check == true )
	{


	if ( execute_buffer_array_length > 0 )
	{
			// array of finished jobs
			// set_execute_object_values ( PC,cycles ,instruction_string ,type)
			for ( int i=0; i < execute_buffer_array_length; i++ )
			{
				if ( execute_buffer_array[i]->type == "MUL"  && execute_buffer_array[i]->cycles >= 2 )
				{
						execute_buffer = execute_buffer_array[i]->instruction_string;
						execute_PC = execute_buffer_array[i]->PC;

						// array of executing jobs
						for ( int j = 0; j < execute_buffer_array_length; j++ )
						{
								// remove the processed element from the array
								if ( execute_buffer_array[i] == execute_buffer_array[j] )
								{
										int pos = j ;
										// execute_buffer_array_length -1
										for(int k=0; k < execute_buffer_array_length ; k++)
										{
												if( k >= pos)
												{
													execute_buffer_array[k] = execute_buffer_array[k+1];
												}
										}
								}
						}
						execute_buffer_array_length = execute_buffer_array_length - 1 ;
						next_check = false;
						break;
				}
				else
				{
						execute_buffer = "";
				}
			}
	}
	}


	if ( next_check == true )
	{

	// INT FU
	if ( execute_buffer_array_length > 0 )
	{
			// array of finished jobs
			// set_execute_object_values ( PC,cycles ,instruction_string ,type)
			for ( int i=0; i < execute_buffer_array_length; i++ )
			{
				if ( execute_buffer_array[i]->type != "DIV" && execute_buffer_array[i]->type != "MUL" &&  execute_buffer_array[i]->type != "HALT" )
				{
						execute_buffer = execute_buffer_array[i]->instruction_string;
						execute_PC = execute_buffer_array[i]->PC;

						// array of executing jobs
						for ( int j = 0; j < execute_buffer_array_length; j++ )
						{
								// remove the processed element from the array
								if ( execute_buffer_array[i] == execute_buffer_array[j] )
								{
										int pos = j ;
										// execute_buffer_array_length -1
										for(int k=0; k < execute_buffer_array_length ; k++)
										{
												if( k >= pos)
												{
													execute_buffer_array[k] = execute_buffer_array[k+1];
												}
										}
								}
						}
						execute_buffer_array_length = execute_buffer_array_length - 1 ;
						break;
				}
				else
				{
						execute_buffer = "";
				}
			}
	}

	}


	for ( int i =0; i < execute_buffer_array_length; i ++ )
	{
			execute_buffer_class object = *(execute_buffer_array[i]);

			if ( object.type == "DIV" || object.type == "HALT" )
			{
					div_fu_flag = div_fu_flag + 1;
			}
			else
			if ( object.type == "MUL")
			{
					mull_fu_flag = mull_fu_flag + 1;
			}
			else
			{
					add_fu_flag = add_fu_flag + 1;

			}
	}

	return true;
}

bool memory_stage()
{
		check_bz_status_mem = true;
		display_class_array[no_of_cycle]->memory = "Empty";
		display_class_array[no_of_cycle]->memory_pc_no = execute_PC;
		memory_buffer = execute_buffer;

			if ( memory_buffer != "" && memory_PC <= address_val)
        	{


					memory_PC = execute_PC;

        	        Code_Memory temp = *(code_memory[memory_PC]);

        	        temp.cycles = 0;
        	        // only perform memory stage if instruction is load or store

        	        if ( temp.type == "LOAD" )
        	        {
        	                // store memory value of execute stage to mem_result
                	        temp.mem_result = data_memory[temp.alu_result]->data_mem_value;
                	}
                	else if ( temp.type == "STORE" )
                	{
                	        // store 1st operand value in memory[ 2nd operand + 3rd operand ]

	                        int result = get_source_value(temp , 3);

	                        data_memory[temp.alu_result]->data_mem_value = result;
	                }
                	else if ( temp.type == "HALT")
                	{

                			fetch_buffer = "";
                			decode_rf_buffer = "";
                			execute_buffer = "";
                	}
                	else if ( temp.type == "JUMP" || temp.type == "BZ" || temp.type == "BNZ" || temp.type == "JAL" )
                	{
                			if ( jumping_flag == true )
                			{
                					if ( temp.type == "BZ" || temp.type == "BNZ" )
                					{
                            				if ( temp.zeroflag == "inactive" &&  temp.type == "BNZ" )
                            				{
                            						check_if_jumping = false;
                            						fetch_PC = temp.alu_result - 4 ;
                            						fetch_buffer = "";
                            						decode_rf_buffer = "";
                            						execute_buffer = "";
                            				}
                            				if ( temp.zeroflag == "active" &&  temp.type == "BZ" )
                            				{
                            						check_if_jumping = false;
                            						fetch_PC = temp.alu_result - 4 ;
                            						fetch_buffer = "";
                            						decode_rf_buffer = "";
                            						execute_buffer = "";
                            				}
                					}
                					else
                					{
                							fetch_PC = temp.alu_result - 4 ;
                							check_if_jumping = false;
                							if ( temp.type == "JAL" )
                							{
                									fetch_PC = temp.mem_result - 4;
                							}
                							fetch_buffer = "";
                							decode_rf_buffer = "";
                							execute_buffer = "";
                					}
                			}
                	}

                	if ( temp.type == "ADD" || temp.type == "DIV" || temp.type == "MUL" || temp.type == "SUB" )
                	{
                			check_bz_status_mem = false;
                	}

        			if ( memory_buffer != "" )
        			{
        				display_class_array[no_of_cycle]->memory = memory_buffer;
        			}

        			temp.pipelinestage = "MEM";
        	        // update instruction object with new entries
	                *code_memory[memory_PC] = temp;

		}
		return true;
}

bool writeback_stage()
{
	check_bz_status_wb = true;
	// increment no of cycles
		no_of_cycle = no_of_cycle + 1;
		display_class_array[no_of_cycle]->writeback_pc_no = memory_PC;
		display_class_array[no_of_cycle]->writeback = "Empty";
       	writeback_buffer = memory_buffer;


       	if ( writeback_buffer != "" && writeback_PC <= address_val)
        {
                // Increment PC
                writeback_PC = memory_PC;

                if ( writeback_buffer != "" )
                {
                		display_class_array[no_of_cycle]->writeback = writeback_buffer;
                }

                Code_Memory temp = *(code_memory[writeback_PC]);

                display_class_array[no_of_cycle]->writeback = temp.instruction_string;
                if ( temp.type == "HALT" )
                {
                	fetch_buffer = "";
                	decode_rf_buffer = "";
                }
                if ( temp.type == "JUMP" )
                {
                	memory_buffer = "";
                }

                if ( temp.type == "ADD" || temp.type == "DIV" || temp.type == "MUL" || temp.type == "SUB" )
                {
                		check_bz_status_wb = false;
                }
                temp.pipelinestage = "WB";
                *code_memory[writeback_PC] = temp;
        }
       	return true;
}

//############################################################################
// Necessary functions #######################################################

int get_source_value(Code_Memory temp, int station)
{

		std::string source;
		if ( station == 0 )
		{
				source = temp.dst1;
		}
		else if ( station == 1 )
		{
				source = temp.src1;
		}
		else if ( station == 2 )
		{
				source = temp.src2;
		}
		else if ( station == 3 )
		{
				source = temp.src3;
		}

    	int decoded_sorce;
      	// check if soruce is a literal or Register
       	if (source.find("R") != std::string::npos)
       	{
           		// Register

             	// replace R with null
              	std::replace( source.begin(), source.end(), 'R', ' ');

              	// get value of this register
               	decoded_sorce = R[atoi( source.c_str() )]->value;
               	int register_number = atoi( source.c_str() );
               	if ( int_fu_bus.register_number == register_number || mul_fu_bus.register_number == register_number || div_fu_bus.register_number == register_number )
               	{
               			if ( int_fu_bus.register_number == register_number && int_fu_bus.status == true )
               			{
               					decoded_sorce = int_fu_bus.value;
               			}
               			if ( mul_fu_bus.register_number == register_number && mul_fu_bus.status == true)
               			{
               					decoded_sorce = mul_fu_bus.value;
               			}
               			if ( div_fu_bus.register_number == register_number && div_fu_bus.status == true)
               			{
               					decoded_sorce = div_fu_bus.value;
               			}
               	}
               	else
               	{
               		if ( station == 0 )
               		{
               			/*
               			if ( temp.dst1_bus_status == true )
               			{
               					decoded_sorce = temp.dst1_value;
               			}
               			*/
               		}
               		else
               		if ( station == 1 )
               		{
               			if ( temp.src1_bus_status == true )
               			{
               					decoded_sorce = temp.src1_value;
               			}
               		}
               		else if ( station == 2)
               		{
               			if ( temp.src2_bus_status == true )
               			{
               					decoded_sorce = temp.src2_value;
               			}
               		}
               		else if ( station == 3 )
               		{
               			if ( temp.src3_bus_status == true )
               			{
               					decoded_sorce = temp.src3_value;
               			}
               		}

               	}
       	}
       	else
    	{
       			// literal
            	// replace # with null
           		std::replace( source.begin(), source.end(), '#', ' ');

           		// get exact value of this literal
           		decoded_sorce = atoi( source.c_str() );
     	}
      	return decoded_sorce;
}



void set_destination_value(Code_Memory temp, int result)
{
	if (temp.type == "STORE" || temp.type == "BNZ" || temp.type == "BZ" || temp.type == "JUMP" || temp.type == "HALT")
	{
		// dont do anything in wb stage
		//set_destination_reg_status(temp, true);
	}
	else
        {

		std::string source = temp.dst1;
		// destination is a register
	   	std::replace( source.begin(), source.end(), 'R', ' ');
	   	int register_number = atoi( source.c_str() );
	    	R[register_number]->value = result;

		// set value as true, of dest register ( interlockinglogic )
             	set_destination_reg_status(temp, true);
	}

	//set psw flags
	if ( temp.type == "ADD" || temp.type == "SUB" || temp.type == "MUL" || temp.type == "AND" || temp.type == "OR" || temp.type == "XOR")
	{
			if ( result == 0)
			{
					zero_flag = "active";
			}
			else
			{
					zero_flag = "inactive";
			}
	}
}

void set_destination_reg_status(Code_Memory temp, bool regstatus)
{
	    std::string destination = temp.dst1;
     	std::replace( destination.begin(), destination.end(), 'R', ' ');
   	    int register_number = atoi( destination.c_str() );
      	R[register_number]->status = regstatus;
}


bool check_register_status(Code_Memory temp)
{
	if ( temp.type == "BNZ" || temp.type == "BZ")
	{
		int zero_flag_inst_address = temp.address ;
		int while_flag = 0;
		while ( while_flag == 0 )
		{
			zero_flag_inst_address = zero_flag_inst_address - 4;
			if ( zero_flag_inst_address <= address_val )
			{
					Code_Memory preceding = *(code_memory[zero_flag_inst_address]);
					if ( preceding.type == "ADD" || preceding.type == "SUB" || preceding.type == "MUL" || preceding.type == "DIV" )
					{
							while_flag = 1;
					}
			}
			else
			{
					// no preceeding instruction can set BZ flag
					return true;
			}
		}

		Code_Memory preceding = *(code_memory[zero_flag_inst_address]);
		if ( preceding.pipelinestage == "DONE" || preceding.pipelinestage == "MEM")
		{
				return true;
		}
		else if ( preceding.pipelinestage == "WB" )
		{
				return false;
		}
		else if ( preceding.pipelinestage == "EX" )
		{
			// check if stall to be introduced
			if ( execute_buffer_array_length > 0 )
			{
					for ( int i = 0; i < execute_buffer_array_length; i ++ )
					{

							execute_buffer_class object = *(execute_buffer_array[i]);

							if ( object.PC == zero_flag_inst_address )
							{
									if ( object.type == "DIV" && object.cycles >= 4 )
									{
											return true;
									}
									else
									if ( object.type == "MUL" && object.cycles >= 2  )
									{
											return true;
									}
									else
									if ( object.type == "ADD" || object.type == "SUB" )
									{
											return true;
									}
									else
									{
											return false;
									}
							}
					}
			}
		}
		else
		{
				return false;
		}
	}


	bool srcstatus1,srcstatus2,dststatus,srcstatus3;

	int array_size = 0;
	std::vector<std::string> split_instruction;
                std::istringstream iss(temp.instruction_string);
                for(std::string s; iss >> s; array_size += 1)
                        split_instruction.push_back(s);

	if ( temp.dst1 != "NULL" )
	{
			std::string destination12 = temp.dst1;
			if (destination12.find("R") != std::string::npos)
			{
				// destination register status

				std::replace( destination12.begin(), destination12.end(), 'R', ' ');
				int register_number = atoi( destination12.c_str() );
				dststatus = R[register_number]->status;

				if ( dststatus == false )
				{
					dststatus = false;
				}
			}
			else
			{
				dststatus = true;
			}
	}
	else
	{
		dststatus = true;
	}

	// source register status

	if ( temp.src1 != "NULL")
	{
		std::string instruction_register = temp.src1;
	    // check if soruce is a literal or Register
        if (instruction_register.find("R") != std::string::npos)
        {
        	std::replace( instruction_register.begin(), instruction_register.end(), 'R', ' ');
        	int register_number = atoi( instruction_register.c_str() );
        	srcstatus1 = R[register_number]->status;

        	if ( srcstatus1 == false )
        	{
        			if ( ( int_fu_bus.register_number == register_number && int_fu_bus.status == true ) ||
        					( mul_fu_bus.register_number == register_number && mul_fu_bus.status == true ) ||
							( div_fu_bus.register_number == register_number && div_fu_bus.status == true ))
        			{

							if ( int_fu_bus.register_number == register_number )
							{
									srcstatus1 = true;
									temp.src1_value = int_fu_bus.value;
									temp.src1_bus_status = true;
							}
							else
							if ( mul_fu_bus.register_number == register_number )
							{
									srcstatus1 = true;
									temp.src1_value = mul_fu_bus.value;
									temp.src1_bus_status = true;
							}
							else
							if ( div_fu_bus.register_number == register_number )
							{
									srcstatus1 = true;
									temp.src1_value = div_fu_bus.value;
									temp.src1_bus_status = true;
							}
        			}
        			else
        			{
        					if ( temp.src1_bus_status == true )
        					{
        							srcstatus1 = true;
        					}
        			}
        	}
        }
        else
        {
        		srcstatus1 = true;
        }
	}
	else
	{
		srcstatus1 = true;
	}

	if ( temp.src2 != "NULL" )
	{
		std::string instruction_register = temp.src2;
		if (instruction_register.find("R") != std::string::npos)
		{
                std::replace( instruction_register.begin(), instruction_register.end(), 'R', ' ');
                int register_number = atoi( instruction_register.c_str() );
                srcstatus2 = R[register_number]->status;

                if ( srcstatus2 == false )
        		{

    					if ( int_fu_bus.register_number == register_number )
    					{
    							srcstatus2 = true;
    							temp.src2_value = int_fu_bus.value;
    							temp.src2_bus_status = true;
    					}
    					else
    					if ( mul_fu_bus.register_number == register_number )
    					{
    							srcstatus2 = true;
    							temp.src2_value = mul_fu_bus.value;
    							temp.src2_bus_status = true;
    					}
    					else
    					if ( div_fu_bus.register_number == register_number )
    					{
    							srcstatus2 = true;
    							temp.src2_value = div_fu_bus.value;
    							temp.src2_bus_status = true;
    					}
    					else
    					if ( temp.src2_bus_status == true )
    					{
    						    srcstatus2 = true;
    					}
        		 }
        }
        else
        {
                srcstatus2 = true;
        }
	}
	else
	{
		srcstatus2 = true;
	}


	if ( temp.src3 != "NULL" )
	{
		std::string instruction_register = temp.src3;
		if (instruction_register.find("R") != std::string::npos)
		{
                std::replace( instruction_register.begin(), instruction_register.end(), 'R', ' ');
                int register_number = atoi( instruction_register.c_str() );
                srcstatus3 = R[register_number]->status;

                if ( srcstatus3 == false )
        		{


    					if ( int_fu_bus.register_number == register_number )
    					{
    							srcstatus3 = true;
    							temp.src3_value = int_fu_bus.value;
    							temp.src3_bus_status = true;
    					}
    					else
    					if ( mul_fu_bus.register_number == register_number )
    					{
    							srcstatus3 = true;
    							temp.src3_value = mul_fu_bus.value;
    							temp.src3_bus_status = true;
    					}
    					else
    					if ( div_fu_bus.register_number == register_number )
    					{
    							srcstatus3 = true;
    							temp.src3_value = div_fu_bus.value;
    							temp.src3_bus_status = true;
    					}
    					else
    					if ( temp.src3_bus_status == true )
    					{
    					    	srcstatus3 = true;
    					}
        		 }
        }
        else
        {
                srcstatus3 = true;
        }
	}
	else
	{
		srcstatus3 = true;
	}

	*code_memory[temp.address] = temp;

	if ( srcstatus2 == true && srcstatus1 == true && dststatus == true && srcstatus3 == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool check_other_stages()
{

		bool flag = true;

		if ( writeback_PC >= 4000 )
		{
				Code_Memory temp = *(code_memory[writeback_PC]);
				if ( temp.type == "HALT"  )
				{
						flag = false;
				}
		}

		if ( memory_PC >= 4000 )
		{
				Code_Memory temp = *(code_memory[memory_PC]);
				if ( temp.type == "HALT" )
				{
						flag = false;
				}
		}
		if ( execute_PC >= 4000 )
		{
				Code_Memory temp = *(code_memory[execute_PC]);
				if ( temp.type == "HALT" )
				{
						flag = false;
				}
		}
		if ( decode_PC >= 4000 )
		{
				Code_Memory temp = *(code_memory[decode_PC]);
				if (  temp.type == "HALT" )
				{
						flag = false;
				}
		}
		return flag;
}

bool check_stage_execution_status()
{
		if ( fetch_buffer == "" && decode_rf_buffer == "" && execute_buffer == "" && memory_buffer == "" && writeback_buffer == "" && fetch_PC >=  address_val && execute_buffer_array_length <= 0)
		{
				return false;
		}
		else
		{
				return true;
		}
}

void display()
{

		// display contents of each stage
		for ( int i = 1 ; i <= no_of_cycle; i ++  )
		{

			if ( display_class_array[i]->fetch == "Empty" && display_class_array[i]->decode == "Empty" && display_class_array[i]->execute == "Empty"
					&& display_class_array[i]->mul1 == "Empty" && display_class_array[i]->mul2 == "Empty" && display_class_array[i]->div1 == "Empty"
							&& display_class_array[i]->div2 == "Empty" && display_class_array[i]->div3 == "Empty"
									&& display_class_array[i]->div4 == "Empty" && display_class_array[i]->memory == "Empty"
											&& display_class_array[i]->writeback == "Empty" )
			{

				// do nothing
			}
			else
			{

				cout << "\nCycle " << i << "\n" ;
			if ( display_class_array[i]->fetch == "Empty" || display_class_array[i]->fetch == "" )
			{
					cout << "Fetch : " << display_class_array[i]->fetch << "\n" ;
			}
			else
			{
					cout << "Fetch : " << "(I" << code_memory[display_class_array[i]->fetch_pc_no]->instruction_no << ") " << display_class_array[i]->fetch << display_class_array[i]->fetch_status << "\n" ;
			}

			if ( display_class_array[i]->decode == "Empty" || display_class_array[i]->decode == "")
			{
					cout << "Decode : " << display_class_array[i]->decode << display_class_array[i]->status << "\n" ;
			}
			else
			{
					cout << "Decode : " << "(I" << code_memory[display_class_array[i]->decode_pc_no]->instruction_no << ") " << display_class_array[i]->decode << display_class_array[i]->decode_status << "\n" ;
			}

			if ( display_class_array[i]->execute == "Empty" || display_class_array[i]->execute == "" )
			{
					cout << "INTFU : " << display_class_array[i]->execute << "\n" ;
			}
			else
			{
					cout << "INTFU : " << "(I" << code_memory[display_class_array[i]->execute_pc_no]->instruction_no << ") " << display_class_array[i]->execute << display_class_array[i]->execute_status << "\n" ;
			}

			if ( display_class_array[i]->mul1 == "Empty" || display_class_array[i]->mul1 == "" )
			{
					cout << "Mul1 : " << display_class_array[i]->mul1 << "\n" ;
			}
			else
			{
					cout << "Mul1 : " << "(I" << code_memory[display_class_array[i]->mul1_pc_no]->instruction_no << ") " << display_class_array[i]->mul1 << display_class_array[i]->mul1_status << "\n" ;
			}

			if ( display_class_array[i]->mul2 == "Empty" || display_class_array[i]->mul2 == "")
			{
					cout << "Mul2 : " << display_class_array[i]->mul2 << "\n" ;
			}
			else
			{
					cout << "Mul2 : " << "(I" << code_memory[display_class_array[i]->mul2_pc_no]->instruction_no << ") " << display_class_array[i]->mul2 << display_class_array[i]->mul2_status << "\n" ;
			}

			if ( display_class_array[i]->div1 == "Empty" || display_class_array[i]->div1 == "")
			{
					cout << "DIV1 : " << display_class_array[i]->div1 << "\n" ;
			}
			else
			{
					cout << "DIV1 : " << "(I" << code_memory[display_class_array[i]->div1_pc_no]->instruction_no << ") " << display_class_array[i]->div1 << display_class_array[i]->div1_status << "\n" ;
			}

			if ( display_class_array[i]->div2 == "Empty" || display_class_array[i]->div2 == "")
			{
					cout << "DIV2 : " << display_class_array[i]->div2 << "\n" ;
			}
			else
			{
					cout << "DIV2 : " << "(I" << code_memory[display_class_array[i]->div2_pc_no]->instruction_no << ") " << display_class_array[i]->div2 << display_class_array[i]->div2_status << "\n" ;
			}


			if ( display_class_array[i]->div3 == "Empty" || display_class_array[i]->div3 == "")
			{
					cout << "DIV3 : " << display_class_array[i]->div3 << "\n" ;
			}
			else
			{
					cout << "DIV3 : " << "(I" << code_memory[display_class_array[i]->div3_pc_no]->instruction_no << ") " << display_class_array[i]->div3 << display_class_array[i]->div3_status << "\n" ;
			}

			if ( display_class_array[i]->div4 == "Empty" || display_class_array[i]->div4 == "")
			{
					cout << "DIV4 : " << display_class_array[i]->div4 << "\n" ;
			}
			else
			{
					cout << "DIV4 : " << "(I" << code_memory[display_class_array[i]->div4_pc_no]->instruction_no << ") " << display_class_array[i]->div4 << display_class_array[i]->div4_status << "\n" ;
			}


			if ( display_class_array[i]->memory == "Empty" || display_class_array[i]->memory == "")
			{
					cout << "Memory : " << display_class_array[i]->memory << "\n" ;
			}
			else
			{
					cout << "Memory : " << "(I" << code_memory[display_class_array[i]->memory_pc_no]->instruction_no << ") " << display_class_array[i]->memory << "\n" ;
			}


			if ( display_class_array[i]->writeback == "Empty" || display_class_array[i]->writeback == "" )
			{
					cout << "Witeback : " << display_class_array[i]->writeback << "\n" ;
			}
			else
			{
					cout << "Witeback : " << "(I" << code_memory[display_class_array[i]->writeback_pc_no]->instruction_no << ") " << display_class_array[i]->writeback << "\n" ;
			}

			}
		}


		// display 16 register values
		for ( int i=0 ; i < 16 ; i++)
		{
					cout << "REGISTER [ " << i << " ] = " << R[i]->value << "\n";
		}

		int counter = 0;
		// first 100 memory locations
		for( int i=0; i < 4000; i+=4)
		{
				if ( counter <= 100 )
				{
					//data_mem_value
					cout << "Data memory Location [" << i << "] = " <<  data_memory[i]->data_mem_value << "\n";
					counter = counter + 1;
				}
		}

}

void simulate()
{

	bool flag = true;
	while ( no_of_cycle != MAX_CYCLES && flag == true )
	{
		writeback_stage();
		memory_stage();
		execute_stage();
		decode_rf();
		flag = fetch();
	}
}

bool init_flag = false;

bool initialize(std::string filename1)
{

	if ( init_flag == true )
	{
				for( int i=0; i < 16; i++)
		    	{
		    			// Constructor ( value, status, phy_register, phy register status)
		    			delete R[i];
		    	}

		    	// initialise data memory
		    	for( int i=0; i < 4000; i+=4)
		    	{
		            	// Constructor ( address, value)
		            	delete data_memory[i];
		    	}

		    	for( int i=1; i < 4000; i++)
		    	{
		    			// Constructor ( address, value)
		    			delete display_class_array[i] ;
		    	}

		    	for ( int i = 3996 ; i <= address_val; i+=4 )
		    	{
		    			delete code_memory[i];
		    	}

		    	for ( int i=0; i < execute_buffer_array_length; i++ )
		    	{
		    			delete execute_buffer_array[i];
		    	}

	}

	init_flag = true;

	// initially address is 0
	linenumber = 0;
	MAX_CYCLES = 220;
	no_of_cycle = 0;
	stall_var = 0;
	mull_stall = 0;
	zero_flag = "active";
	check_zero_flag = "acitve";
	jumping_flag = false;
	same_stage_execute = true;
	execute_buffer_array_length = 0;
	int instruction_no = 0;

	// fetch instruction buffer
	fetch_buffer = "";
	//decode instruction buffer
	decode_rf_buffer = "";
	// execute instruction buffer
	execute_buffer = "";
	// execute instruction buffer
	memory_buffer = "";
	// writeback instruction buffer
	writeback_buffer = "";

	program_counter = 3996;
	address_val = 3996;


	same_stage_execute = true;
	execute_buffer_array_length = 0;
	program_counter = 3996;
	address_val = 3996;

	fetch_PC = address_val, decode_PC = address_val,  execute_PC = address_val, memory_PC = address_val, writeback_PC = address_val;
	// fetch instruction buffer
	std::string fetch_buffer = "";
	//decode instruction buffer
	std::string decode_rf_buffer = "";
	// execute instruction buffer
	std::string execute_buffer = "";
	// execute instruction buffer
	std::string memory_buffer = "";
	// writeback instruction buffer
	std::string writeback_buffer = "";

	linenumber = 0;
	no_of_cycle = 0;
	stall_var = 0;
	mull_stall = 0;
	zero_flag = "active";
	check_zero_flag = "active";
	jumping_flag = false;
	jumping_flag_2 = false;
	check_bz_status_mem = true;
	check_bz_status_wb = true;
	check_if_jumping = false;



    	// enter filename here
    	std::string filename = filename1;

    	// to parse the file line by line
    	string line,command, operation;

    	ifstream myfile (filename.c_str());
    	if (myfile.is_open())
    	{
            	while ( getline (myfile,line) )
            	{
                    	if ( ! line.empty() )
                    	{
                    			linenumber = linenumber + 1;
                    			address_val = address_val + 4;
                    			code_memory[address_val] = new Code_Memory(linenumber,address_val,line, instruction_no);
                    			instruction_no = instruction_no + 1;
                    			//cout << address_val << " " << instruction_no << " " << line << '\n';
                    	}
            	}
            	myfile.close();
    	}
    	else
    	{
            	cout << "ABORT ! Unable to open file";
            	return false;
            	//exit(0);
    	}

    	//initialize an array of 16 registers
    	for( int i=0; i < 16; i++)
    	{
    			// Constructor ( value, status, phy_register, phy register status)
    			R[i] = new Register(0 , true, 0 , false);
    	}

    	// initialise data memory
    	for( int i=0; i < 4000; i+=4)
    	{
            	// Constructor ( address, value)
            	data_memory[i] = new Data_Memory(i , 0);
    	}

    	for( int i=1; i < 4000; i++)
    	{
    			// Constructor ( address, value)
    			display_class_array[i] = new display_class;
    	}
    	return true;
}

bool is_number(string str)
{
	for(int i = 0;i < ( str.length()  - 1) ; i++)
	{
			if((int)str[i] < 10)
			{
					return true;
			}
			else
			{
				return false;
			}
	}
	return false;
}


int main(int argc, char * argv[])
{

		std::string filename;
   	    filename = argv[1];

		std::string userinput ;

        cout << "1. initialize : This is to initialize Program Counter to point to first instruction \n" ;
	    cout << "2. simulate : This is simulate the no of cycles specified as <n> \n";
    	cout << "3. display : Displays the contents of each stage in the pipeline, all registers, and the contents of the first 100 memory locations \n" ;
	    cout << "4. Exit : To terminate simulator \n \n" ;

		while ( true )
		{
		    while ( std::getline(std::cin, userinput) )
		    {
				if ( userinput == "initialize" )
				{
						bool returnval = initialize(filename);
						if ( returnval == true )
						{
								cout << "Initialization success \n";
						}
						else
						{
								cout << "Initialization failed \n";
						}
				}
				else if ( userinput.find("simulate") != std::string::npos || is_number(userinput) == true )
				{
    					while (userinput.find("simulate") != string::npos)
	    				        userinput.replace(userinput.find("simulate"), 8, " ");
	    				int cycles = atoi( userinput.c_str() );
	    				MAX_CYCLES = cycles;
	    				simulate();
                        cout << "Simulation success \n";
				}
				else if ( userinput == "display" || userinput == "3" )
				{
						display();
				}
				else if ( userinput == "Exit" || userinput == "4" )
				{
						cout << "Simulator terminated normally \n";
						exit(0);
				}
				else
				{
						cout << "Invalid command entered \n";
				}
                
                cout << "\n1. initialize : This is to initialize Program Counter to point to first instruction \n" ;
    	    	cout << "2. simulate : This is simulate the no of cycles specified as <n> \n";
        		cout << "3. display : Displays the contents of each stage in the pipeline, all registers, and the contents of the first 100 memory locations \n" ;
	    	    cout << "4. Exit : To terminate simulator \n \n" ;

		    }        
		}
}
