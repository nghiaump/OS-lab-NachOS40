#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 32

int main()
{
	// KHAI BAO
	int flag_mutex, i, SLSV, readCode;
	int process[10];
	SpaceId si_input, si_output;
	char c_readFile;
	
	//-----------------------------------------------------------

	flag_mutex = CreateSemaphore("voinuoc", 0);
	if (flag_mutex == -1)
		return 1;

	// Mo file input.txt chi de doc so luong sinh vien
	PrintString("Opening the input file\n");
	si_input = Open("input.txt", 1);
	if (si_input == -1)
		return 1;
	PrintString("Opened successfully!\n");

	// Tao file output.txt de ghi ket qua
	PrintString("Creating output file\n");
	flag_mutex = CreateFile("output.txt");
	if (flag_mutex == -1)
		return 1;
	PrintString("Output file created!\n");

	// Doc cac ky tu trong input.txt va chuyen thanh so
	SLSV = 0; // Khoi tao gia tri so luong sinh vien
	while (1)
	{
		readCode = Read(&c_readFile, 1, si_input);

		// Doc toi cuoi file thi thoat
		if (readCode == -3)
			break;
		// Chuyen char sang int
		if (c_readFile >= '0' && c_readFile <= '9')
			SLSV = SLSV * 10 + (c_readFile - 48);
	}

	PrintString("\nSLSV = ");
	PrintNum(SLSV);
	PrintChar('\n');

	// Exec() cac tien trinh tuong ung so luong sinh vien
	for (i = 0; i < SLSV; ++i)
	{
		process[i] = Exec("../test/sinhvien");
		if (process[i] == -1)
		{
			Close(si_input);
			Close(si_output);
			return 1;
		}
		PrintString("ID = ");
		PrintNum(process[i]);
		PrintString("\n");
	}

	// Signal semaphore voinuoc de cac tien trinh con duoc lap lich chay
	Signal("voinuoc");

	// Join() cac tien trinh con vao tien trinh cha la "main"
	for (i = 0; i < SLSV; ++i)
	{
		Join(process[i]);
	}

	Exit(0);
}
