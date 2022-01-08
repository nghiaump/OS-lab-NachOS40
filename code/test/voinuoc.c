#include "syscall.h"
#include "copyright.h"

void main()
{
	// Khai bao
	int f_Success; // Bien co dung de kiem tra thanh cong
	SpaceId  si_voinuoc, si_result;	// Bien id cho file
	char c_readFile;	// Bien ki tu luu ki tu doc tu file
	int v1, v2;		// Voi 1, voi 2
	int v;			// Dung tich nuoc cua sinh vien
	int flag_done_result;	// Bien co luu dau hieu doc xong file result

	//-----------------------------------------------------------
	
	PrintString("Started voinuoc successfully\n");
	v1 = v2 = 0;
	// Xu ly voi nuoc
	// WHILE(11111111111111111111111111111111111111)
	
	while(1)
	{
		PrintString("Waiting m_vn in the head (voinuoc)\n");
		Wait("m_vn");

		// Mo file result.txt de ghi voi nao su dung
		PrintString("Opening result.txt in voinuoc\n");
		si_result = Open("result.txt", 0);
		if(si_result == -1)
		{		//?
			Signal("sinhvien");
			return;
		}
		PrintString("Opened result.txt successfully at address: ");
		PrintNum(si_result);
		PrintString(" (voinuoc)\n");

		while(1)
		{
			Wait("voinuoc");
			c_readFile = 0;			
			// Mo file voi nuoc .txt de doc dung tich
			si_voinuoc = Open("voinuoc.txt", 1);
			if(si_voinuoc == -1)
			{
				PrintString("Finished 1 line of sinhvien\n");
				PrintString("Return to main\n");
				Close(si_result);
				Signal("sinhvien");			
				return;
			}
			PrintString("Address of si_voinuoc = ");
			PrintNum(si_voinuoc);
			PrintString("\nReading the water volume:\n");
			v = 0;
			flag_done_result = 0;
			
			while(1)
			{			
				if(Read(&c_readFile, 1, si_voinuoc)  == -3)
				{	
					PrintString("Read char return code -3\n");
					Close(si_voinuoc);			
					break;
				}
				PrintString("Current char: ");
				PrintChar(c_readFile);
				PrintString("\n");

				if(c_readFile != '*')
				{
					PrintString("Read char continuously\n");
					v = v* 10 + (c_readFile - 48);
				}

				else
				{
					PrintString("Finished reading the line\n");
					flag_done_result = 1;				
					Close(si_voinuoc);
					break;			
				}
			
			}
			
			
			PrintString("\r\nv = ");
			PrintNum(v);
			PrintChar('\n');
			if(v!= 0)
			{
				PrintString("Decide which faucet to be used: ");
				// Dung voi 1
				if(v1 <= v2)
				{
					v1 += v;
					Write("1", 1, si_result);
					PrintString("============>Faucet 1");
				}
				else	// Dung voi 2
				{					
					v2 += v;
					Write("2", 1, si_result);
					PrintString("============>Faucet 2");
					
				}
			}
			
			if(flag_done_result == 1)
			{
				PrintString(" ==> Flag end of line triggered!\n");
				v1 = v2 = 0;
				Close(si_result);				
				//Signal("sinhvien");
				break;				
			}	
			
			PrintString("\nvoinuoc signals sinhvien to get the next volume\n");
			Signal("sinhvien");
			//Wait("voinuoc");
		}

		PrintString("\nNow here (still in while loop)\n");
		//Close(si_result);
		Signal("m_vn"); //7735
		Signal("sinhvien");

	}
	Exit(0);		
}
