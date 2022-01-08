#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 32


int main()
{
	// KHAI BAO
	int f_Success, flagSV, flagVN; // Bien co dung de kiem tra thanh cong
	SpaceId si_input, si_output, si_sinhvien, si_result;	// Bien id cho file
	int SLSV;	// Luu so luong thoi diem xet
	char c_readFile;	// Bien ki tu luu ki tu doc tu file
	int tryToWrite;

	//-----------------------------------------------------------


	f_Success = CreateSemaphore("voinuoc", 0);
	if(f_Success == -1)
		return 1;
	
	
	// Tao file output.txt de ghi ket qua cuoi cung	
	PrintString("Creating output file\n");
	f_Success = CreateFile("output.txt");
	if(f_Success == -1)
		return 1;
	//PrintString("Output file created!\n");
	
	
	// Mo file input.txt chi de doc
	//PrintString("Opening the input file\n");
	si_input = Open("input.txt", 1);
	if(si_input == -1)
		return 1;
	PrintString("Open input.txt successfully at address: ");
	PrintNum(si_input);
	PrintString(" (main)\n");
	
	// Mo file output.txt de doc va ghi
	si_output = Open("output.txt", 0);
	if(si_output == -1)
	{
		Close(si_input);
		return 1;
	}
	PrintString("Open output.txt successfully at address: ");
	PrintNum(si_output);
	PrintString(" (main)\n");

	// Doc so luong thoi diem xet o file input.txt
	//**** Thuc hien xong doan lenh duoi thi con tro file o input.txt o dong 1
	SLSV = 0;
	//PrintNum(si_input);
	
	//PrintString("Begin loop\n");
	while(1)
	{
		Read(&c_readFile, 1, si_input);
		if(c_readFile != '\n')
		{
			if(c_readFile >= '0' && c_readFile <= '9')
				SLSV = SLSV * 10 + (c_readFile - 48);
		}
		else
			break;
	}
	PrintString("Finished reading input.txt\n");
	PrintString("SLSV = ");
	PrintNum(SLSV);
	PrintChar('\n');
	


	// Goi thuc thi tien trinh sinhvien.c
	for(int i = 0; i < SLSV; ++i){
		flagSV = Exec("../test/sinhvien");
		if(flagSV == -1)
	{
		Close(si_input);
		Close(si_output);
		return 1;
	}
		Join(flagSV);
		
	}
	
	
	
	PrintString("Executed all sinhvien\n");


	

	
	return 0;	
	
}
