#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 32


int main()
{
	// KHAI BAO
	int f_Success, flagSV, flagVN; // Bien co dung de kiem tra thanh cong
	SpaceId si_input, si_output, si_sinhvien, si_result;	// Bien id cho file
	int SLTD;	// Luu so luong thoi diem xet
	char c_readFile;	// Bien ki tu luu ki tu doc tu file
	int tryToWrite;

	//-----------------------------------------------------------


	// Khoi tao 4 Semaphore de quan ly 3 tien trinh
	f_Success = CreateSemaphore("main",0);
	if(f_Success == -1)
		return 1;
	f_Success = CreateSemaphore("sinhvien", 0);
	if(f_Success == -1)
		return 1;
	f_Success = CreateSemaphore("voinuoc", 0);
	if(f_Success == -1)
		return 1;
	f_Success = CreateSemaphore("m_vn", 0);
	if(f_Success == -1)
		return 1;
	f_Success = CreateSemaphore("LA", 1);
	if(f_Success == -1)
		return 1;
	
	/*
	// Tao file output.txt de ghi ket qua cuoi cung	
	PrintString("Creating output file\n");
	f_Success = CreateFile("output.txt");
	if(f_Success == -1)
		return 1;
	PrintString("Output file created!\n");
	*/
	
	// Mo file input.txt chi de doc
	//PrintString("Opening the input file\n");
	si_input = Open("input.txt", 1);
	if(si_input == -1)
		return 1;
	PrintString("Open input.txt successfully\n");
	
	// Mo file output.txt de doc va ghi
	si_output = Open("output.txt", 0);
	if(si_output == -1)
	{
		Close(si_input);
		return 1;
	}
	PrintString("Open output.txt successfully\n");

	// Doc so luong thoi diem xet o file input.txt
	//**** Thuc hien xong doan lenh duoi thi con tro file o input.txt o dong 1
	SLTD = 0;
	//PrintNum(si_input);
	
	//PrintString("Begin loop\n");
	while(1)
	{
		Read(&c_readFile, 1, si_input);
		if(c_readFile != '\n')
		{
			if(c_readFile >= '0' && c_readFile <= '9')
				SLTD = SLTD * 10 + (c_readFile - 48);
		}
		else
			break;
	}
	PrintString("Finished reading input.txt\n");
	PrintString("SLTD = ");
	PrintNum(SLTD);
	PrintChar('\n');
	


	// Goi thuc thi tien trinh sinhvien.c
	flagSV = Exec("../test/sinhvien");
	//Signal("sinhvien");
	//Join(f_Success);	
	if(f_Success == -1)
	{
		Close(si_input);
		Close(si_output);
		return 1;
	}
	PrintString("Executed sinhvien\n");


	// Goi thuc thi tien trinh voinuoc.c
	flagVN = Exec("../test/voinuoc");
	// Join(flagVN);

	if(f_Success == -1)
	{
		Close(si_input);
		Close(si_output);
		return 1;
	}
	PrintString("Executed voinuoc\n");

	// Thuc hien xu ly khi nao het thoi diem xet thi thoi
	while(SLTD--)
	{
		// Tao file sinhvien.txt
		PrintString("creating sinhvien.txt\n");
		f_Success = CreateFile("sinhvien.txt");
		if(f_Success == -1)
		{
			Close(si_input);
			Close(si_output);
			return 1;
		}
		PrintString("Created sinhvien.txt successfully!\n");
		
		// Mo file sinhvien.txt de ghi tung dong sinhvien tu file input.txt
		PrintString("Opening sinhvien.txt\n");
		si_sinhvien = Open("sinhvien.txt", 0);
		if(si_sinhvien == -1)
		{
			Close(si_input);
			Close(si_output);
			return 1;
		}
		PrintString("Opened sinhvien.txt successfully\n");
		while(1)
		{
			if(Read(&c_readFile, 1, si_input) < 1)
			{
				// Doc toi cuoi file
				break;
			}
			if(c_readFile != '\n')
			{				
				tryToWrite = Write(&c_readFile, 1, si_sinhvien);	
				PrintString("Current char: ");
				PrintChar(c_readFile);
				//PrintNum(tryToWrite); -3
				if(tryToWrite > 0)
					PrintString(" write OK\n");
				else
					PrintString(" but cannot be writen\n");	
			}
			else
				break;
						
		}
		// Dong file sinhvien.txt lai
		Close(si_sinhvien);
			
		// Goi tien trinh sinhvien hoat dong
		Signal("sinhvien");
		Join(flagSV);
		Join(flagVN);
		

		// Tien trinh chinh phai cho 
		Wait("main");	
		PrintString("Main waiting finished\n");
		// Thuc hien doc file tu result va ghi vao ket qua o output.txt
		si_result = Open("result.txt", 1);
		if(si_result == -1)
		{
			Close(si_input);
			Close(si_output);
			return 1;
		}

		PrintString("\n Lan thu: ");
		PrintNum(SLTD);
		PrintString("\n");	

		// Doc cac voi vao output.txt		
		while(1)
		{
			if(Read(&c_readFile, 1, si_result)  < 1)
			{
				Write("\r\n", 2, si_output);
				Close(si_result);
				Signal("m_vn");
				break;
			}
			Write(&c_readFile, 1, si_output);
			Write(" ", 1, si_output);
			
		}
		
	}
	
	
	Close(si_input);
	Close(si_output);

	
	return 0;	
	
}
